#include <thread>

#include "Reactor.h"
#include "AsyncOperProcessor.h"

namespace andre
{

Reactor::Reactor() : m_exit(false)
{
	m_events.setMaxSize(maxQueueSize);
}

Reactor::~Reactor(){}

void Reactor::handleEvents()
{
	ReactorEvent re;
	
	while (!m_exit) {
		
		re.message = nullptr;
		m_events.waitAndPop(re);
		
		if (!m_exit) { 
			handleEvent(re.handler,re.message);
		}
	}
}

std::shared_ptr<MessageData> Reactor::waitInLoop(EventHandler *handler,
												 const Handle &handle)
{
	if ( ! AsyncOperProcessor::instance().
		 isHandlerRegistered(handler, handle) ) {
		
		return nullptr;
	}
	
	ReactorEvent re;
	
	while (!m_exit) {

		re.message = nullptr;
		m_events.waitAndPop(re);
		
		if (!m_exit) {
			const auto &searchingHandle = re.message->getData()->handle;
			const auto &deregistrationHandle = handler->getDeregisterHandleNonConst();

			if (re.handler == handler) {

				if ( searchingHandle == handle ) {
					return re.message; 
				}

				if (!re.handler->isDeregistering()) {
					m_events.push(re);
				}
				else {// сюда попадет дерегистрируемый handler
					if ( searchingHandle == deregistrationHandle ) {
						m_events.push(re);
						return nullptr;
					}
				}
			}
			else {
				if (!re.handler->isDeregistering()) {
					m_events.push(re);
				}
				else {
					if ( searchingHandle == deregistrationHandle ) {
						m_events.push(re);
					}
				}
			}

			std::this_thread::yield();
		}
	}
	
	return nullptr;
}

bool Reactor::addEvent(EventHandler *handler, const std::shared_ptr<MessageData> &message)
{
	ReactorEvent re = {handler, message};
	return m_events.push(re);
}

void Reactor::exit()
{
	m_exit = true;
}

} // namespace andre
