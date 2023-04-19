#ifndef DISPATCHREACTORSTOPPABLE_H
#define DISPATCHREACTORSTOPPABLE_H

#include "Reactor.h"
#include "andre_global.h"


namespace andre
{

class StoppingHandler;

class ANDRESHARED_EXPORT DispatchReactorStoppable : public Reactor
{
public:
	DispatchReactorStoppable();
	~DispatchReactorStoppable() override;

	virtual void auxInit() override;
	
private:
	std::shared_ptr<StoppingHandler> m_stoppingHandlerPtr;
	
};

} // namespace andre

#endif // DISPATCHREACTORSTOPPABLE_H
