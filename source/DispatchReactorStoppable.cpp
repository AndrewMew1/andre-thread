#include "DispatchReactorStoppable.h"
#include "AsyncOperProcessor.h"
#include "StoppingHandler.h"

namespace andre
{

DispatchReactorStoppable::DispatchReactorStoppable():Reactor()
{
	m_stoppingHandlerPtr = std::make_shared<StoppingHandler>();
}

DispatchReactorStoppable::~DispatchReactorStoppable()
{
	
}

void DispatchReactorStoppable::auxInit()
{
	AsyncOperProcessor::instance().registerHandler<DispatchReactorStoppable>(m_stoppingHandlerPtr.get());
}

} // namespace andre
