#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include "MessageData.h"
#include "Handle.hpp"
#include <vector>
#include <atomic>

#include "andre_global.h"

namespace andre
{

class AsyncOperProcessor;
class Reactor;

class ANDRESHARED_EXPORT EventHandler
{
	friend class AsyncOperProcessor;
	friend class Reactor;
	
public:
	EventHandler();
	virtual ~EventHandler();
	
private:
	std::vector<Handle> m_handles;
	
	// Количество потоков, пытающихся зарегистрировать данный 'EventHandler'.
	std::atomic<int> m_registeringThreadsCounter;

	// Признак того, что начался процесс дерегистрации EventHandler'а.
	std::atomic<bool> m_deregistering;

	// Для реализации последующей логики дерегистрируемых EventHandler'в
	Handle m_deregisterHandle;

protected:
	// функция обработчик сообщений.
	virtual void handleEvent(const std::shared_ptr<MessageData> &msg) = 0;

	// Подписываем наш класс на сообщение, которое хотим обработать 
	void addHandle(const Handle &handle)
	{
		m_handles.push_back(handle);
	}
	
	// То что от нас запросит Процессор асинхронных операций
	//  (AsyncOperProcessor) во время регистрации класса
	const std::vector<Handle> &getHandles()
	{
		return m_handles;
	}
	
	// Вызывается при регистрации EventHandler'а
	virtual void onRegister() {}

	inline bool isDeregistering()
	{
		return m_deregistering;
	}

	// Количество потоков в onHandleEvent
	std::atomic<int> m_threadsCounter;

	virtual Handle &getDeregisterHandleNonConst()
	{
		return m_deregisterHandle;
	}
};

} // namespace andre

#endif // EVENTHANDLER_H
