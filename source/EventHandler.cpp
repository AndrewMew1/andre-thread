#include "EventHandler.h"


namespace andre
{

EventHandler::EventHandler() : 
	m_registeringThreadsCounter(0), m_deregistering(false), m_threadsCounter(0)
{
}

EventHandler::~EventHandler()
{
}

} // namespace andre
