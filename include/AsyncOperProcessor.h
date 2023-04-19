#ifndef ASYNCOPERPROCESSOR_H
#define ASYNCOPERPROCESSOR_H

#include <map>
#include <memory>
#include <vector>
#include <set>
#include <sstream>

#include "MessageData.h"
#include "Handle.hpp"

#include "EventHandler.h"
#include "Reactor.h"

#include "threadsafemap.hpp"
#include "atomiccounter.hpp"

#include "andre_global.h"

namespace andre
{


// Основной класс для межреакторного взаимодействия. 
// Процессор асинхронных операций.
// Не создает собственного потока, но предоставляет другим потокам свой
// функционал в потокобезопасном режиме. 
class ANDRESHARED_EXPORT AsyncOperProcessor
{
public:
	
	// Класс запускает реакторы и ведёт учет запущенных
	class ANDRESHARED_EXPORT StartReactorDispatcher
	{
		StartReactorDispatcher(const StartReactorDispatcher &) = delete;
		StartReactorDispatcher &operator=(const StartReactorDispatcher &) = delete;
		
	public:
		StartReactorDispatcher()
		{
			AsyncOperProcessor::instance().m_startedReactorNumbers ++;
			AsyncOperProcessor::instance().startReactorDispatcher();
		}
		
		~StartReactorDispatcher()
		{
			AsyncOperProcessor::instance().m_startedReactorNumbers --;
		}
	};

	friend class StartReactorDispatcher;

	static AsyncOperProcessor &instance()
	{
		static AsyncOperProcessor theSingleInstance;
		return theSingleInstance;
	}
	
	// останавливает и удаляет ReactorDispatcher из межреакторного взаимодействия
	void shutdownReactorDispatcher();
	
	// регистрирует класс(EventHandler), реагирующий на определенные сообщения
	template<typename ReactorType>
	bool registerHandler(EventHandler *handler)
	{
		static_assert( std::is_base_of<Reactor, ReactorType>::value,
					   "Need class derived from 'Reactor'");
		if (handler->m_deregistering)
		{
			return false;
		}
		multithread::AtomicCounter ac(handler->m_registeringThreadsCounter);
		if (handler->m_deregistering)
		{
			return false;
		}
		
		size_t reactId = registerReactor<ReactorType>();
		const std::vector<Handle> &handles = handler->getHandles();
		handler->onRegister();
		
		for ( const Handle &handle : handles ) {
			std::lock_guard<std::shared_mutex> lk(m_mainMapMutex);
			m_mainMap[handle][reactId].insert(handler);
		}
		
		return true;
	}

	// Регистрирует класс(EventHandler), реагирующий на определенные сообщения.
	bool registerHandler(EventHandler *handler, size_t threadID)
	{
		if (handler->m_deregistering)
		{
			return false;
		}
		multithread::AtomicCounter ac(handler->m_registeringThreadsCounter);
		if (handler->m_deregistering)
		{
			return false;
		}

		size_t reactId;
		if ( ! m_threadToReactor.read(threadID, reactId) ) {
			return false;
		}

		const std::vector<Handle> &handles = handler->getHandles();
		handler->onRegister();

		for ( const Handle &handle : handles ) {
			std::lock_guard<std::shared_mutex> lk(m_mainMapMutex);
			m_mainMap[handle][reactId].insert(handler);
		}

		return true;
	}
	
	// overflows 
	bool deregisterHandler(EventHandler *handler,
			bool isBlocking = false,
			const std::shared_ptr<MessageData> &marker = nullptr,
			std::map<unsigned long long, std::set<EventHandler *> > *overflows = nullptr);
	bool postMessage(const std::shared_ptr<MessageData> &msg,
					 std::map< unsigned long long,
					 std::set<EventHandler *> > *overflows = nullptr);
	
	bool isHandlerRegistered(EventHandler *handler, const Handle &handle);
	
	bool isAllReactorsStopped() {
		return 0 == m_startedReactorNumbers;
	}

	std::shared_ptr<MessageData> waitInLoop(EventHandler *handler,
											const Handle &handle) 
	{
		size_t reactorID;
		size_t threadID;
		
		if ( ! getReactorID(reactorID, threadID) ) {
			return nullptr;
		}

		std::shared_ptr<Reactor> reactor = getReactor(reactorID);
		
		return reactor->waitInLoop(handler, handle);
	}
	
	std::string getDebugInfo() {
		std::stringstream sstream;
		sstream << "m_mainMap: " << m_mainMap.size() << std::endl;
		
		std::shared_lock<std::shared_mutex> lk(m_reactorsMutex);
		
		sstream << "m_reactors: " << m_reactors.size() << std::endl;
		
		for ( auto &elem: m_reactors ) {
			sstream << std::boolalpha << (elem == nullptr) << std::endl;
		}
		sstream << "m_threadToReactor: " 
				<< m_threadToReactor.size() << std::endl;
		return sstream.str();
	}
	
private:
	mutable std::shared_mutex m_mainMapMutex;
	mutable std::shared_mutex m_reactorsMutex;
	
	// Количество запущенных реакторов
	std::atomic<int> m_startedReactorNumbers; 

	// запускает ReactorDispatcher принадлежащий запускающему потоку
	void startReactorDispatcher();
	
	// возвращает идентификатор класса-Реактора(reactorID),
	// который принадлежит вызывающему потоку(threadID)
	// bool - отвечает существует ли такой реактор
	bool getReactorID(size_t &reactorID, size_t &threadID);
	
	// пмещает сообщение в очередь реактора.
	// возвращает false если очередь переполнена или передан несуществующий reactId
	bool eventToReactor(size_t reactId, EventHandler * handler,
						const std::shared_ptr<MessageData> &message);

	// вспомогательная, потоко-не-безопасная функция
	inline bool unlockedPostMessage(const std::shared_ptr<MessageData> &msg,
					std::map< unsigned long long,
							  std::set<EventHandler *>
							> *overflows = nullptr);

	// удаляем Handle из главной map
	void unlockedRemoveHandle(const Handle &handle);
	
	// возвращает ID реактора
	template<typename ReactorType>
	size_t registerReactor()
	{
		size_t reactorID;
		
		size_t threadID;
		ReactorType *freshReactor = nullptr;
		
		if ( ! getReactorID(reactorID, threadID) ) {
			std::lock_guard<std::shared_mutex> lk(m_reactorsMutex);
			
			reactorID = m_reactors.size();
			std::shared_ptr<ReactorType> reactor =
					std::make_shared<ReactorType>();
			freshReactor = reactor.get();

			for ( size_t id = 0; id < m_reactors.size(); id++ ) {
				
				if ( nullptr == m_reactors[id] ) {
					reactorID = id;
					m_reactors[reactorID] = reactor;
					break;			
				}
			}
			
			if ( reactorID == m_reactors.size() ) {
				m_reactors.push_back(reactor);
			}
			
			m_threadToReactor.write(threadID, reactorID);
		}
		
		if ( nullptr != freshReactor ) {
			freshReactor->auxInit();
		}
		
		return reactorID;
	}	

	multithread::SimpleMap< size_t/*threadID(hash)*/,
								size_t/*reactorID*/ > m_threadToReactor;
	std::map<  Handle, 
				std::map< size_t /*reactorID*/,
							std::set<EventHandler* /*handler*/> >  > m_mainMap;
	std::vector< std::shared_ptr<Reactor> > m_reactors;
	inline std::shared_ptr<Reactor> getReactor(size_t reactorID)
	{
		std::shared_ptr<Reactor> reactorPtr;
				
		{
			std::shared_lock<std::shared_mutex> lk(m_reactorsMutex);
			reactorPtr = m_reactors[reactorID];
		}
		
		return reactorPtr;
	}
	
	AsyncOperProcessor(): m_startedReactorNumbers(0)
	{
		
	}
	AsyncOperProcessor(const AsyncOperProcessor &root) = delete;
	AsyncOperProcessor &operator=(const AsyncOperProcessor &) = delete;
};

} // namespace andre

#endif // ASYNCOPERPROCESSOR_H
