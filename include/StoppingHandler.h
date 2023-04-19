#ifndef STOPPINGHANDLER_H
#define STOPPINGHANDLER_H

#include "AsyncOperProcessor.h"

namespace andre
{

//'EventHandler', зарегистрированный на команду остановки диспетчера своего потока
class StoppingHandler : public EventHandler
{
public:
	StoppingHandler();

	static bool postStoppingMessage()
	{
		ConstData cd;
		cd.handle = getHandle();
		std::unique_ptr<ConstData> up = std::make_unique<ConstData>(cd);
		return AsyncOperProcessor::instance().postMessage(
			std::make_shared<MessageData>(up));
	}

private:
	virtual void handleEvent(const std::shared_ptr<MessageData> &) override
	{
		AsyncOperProcessor::instance().shutdownReactorDispatcher();
	}

	static Handle getHandle()
	{
		Handle handle;
		std::hash<std::string_view> hasher;
		handle.commandID = hasher(getCommand());
		handle.messageParam = hasher(getParam());
		return handle;		
	}

	static constexpr std::string_view getCommand()
	{
		return "ShutdownStoppableDispatchers";
	}

	static constexpr std::string_view getParam()
	{
		return "";
	}
};

} // namespace andre

#endif // STOPPINGHANDLER_H
