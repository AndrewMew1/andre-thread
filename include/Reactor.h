#ifndef REACTOR_H
#define REACTOR_H

#include "EventHandler.h"
#include "threadsafequeue.hpp"

#include "andre_global.h"

namespace andre
{


struct ANDRESHARED_EXPORT ReactorEvent
{
	EventHandler *handler;
	std::shared_ptr<MessageData> message;
};

class ANDRESHARED_EXPORT Reactor
{
public:
	Reactor();	
	virtual ~Reactor();
	
	// Дополнительная инициализация. Вызывается после конструирования реактора
	virtual void auxInit(){}
	
	// Цикл очереди сообщений
	virtual void handleEvents();
	
	// Цикл очереди сообщений
	virtual std::shared_ptr<MessageData> waitInLoop(EventHandler *handler,
													const Handle &handle);	
	
	// Добавляем в очередь сообщение
	bool addEvent(EventHandler *handler,
				  const std::shared_ptr<MessageData> &message);
	
	void exit();
	
protected:
	std::atomic<bool> m_exit; 
	
	// Очередь сообщений
	multithread::SimpleQueue<ReactorEvent> m_events;
	
	// Максимальное количество event'ов-сообщений
	//,одновременно ждущих в очереди на обработку
	const int maxQueueSize = 100000;
	
	// Позволяет получить доступ к вызову функции 'EventHandler::handleEvent()'
	// из классов-наследников Reactor'а
	inline void handleEvent(EventHandler *handler,
							const std::shared_ptr<MessageData> &msg)
	{
		handler->handleEvent(msg);
	}
	
};

} // namespace andre

#endif // REACTOR_H
