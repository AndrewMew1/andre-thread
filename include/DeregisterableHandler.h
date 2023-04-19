#ifndef DEREGISTERABLEHANDLER_H
#define DEREGISTERABLEHANDLER_H

#include "MessageData.h"
#include "Handle.hpp"
#include "EventHandler.h"
#include <atomic>

#include "andre_global.h"
namespace andre
{

// EventHandler с поддержкой функционала по дерегистрации
class ANDRESHARED_EXPORT DeregisterableHandler: public EventHandler
{

protected:
	inline std::string getMarkerCommand() const
    {
        return "marker_deregister";
    }

	unsigned long long getMarkerParam() const;
	
	// Используется для переопределения вместо handleEvent.
	virtual void onHandleEvent(const std::shared_ptr<MessageData> &) {}
	
	// Вызывается, когда DeregisterableHandler готов к уничтожению.
	virtual void onDestroyable(const std::shared_ptr<MessageData> &) {}

	// Вызывается при регистрации EventHandler'а
	virtual void onRegisterContinuation() {}

public:
	DeregisterableHandler();
	DeregisterableHandler(const std::string &endmarker);
	~DeregisterableHandler() override;
	
	// handle, на который подписываемся, для ожидания дерегистрации
	const Handle &getDeregistrationHandle();
	
	// дерегистрируем сами себя.
	// если возвращает 'false', handler не зарегистрирован.
	bool deregister();

	// то же, что и deregister(), но блокируется до тех пор, пока Handler
	// не закончит обрабатывать сообщения, которые успел начать к этому моменту.
	bool deregisterBlocking();

private:
	std::atomic<int> m_registrationCounter;

	inline bool deregister(bool isBlocking);

	// вместо неё - onHandleEvent().
	virtual void handleEvent(const std::shared_ptr<MessageData> &msg) override final;

	// ведётся подсчет количества регистраций
	virtual void onRegister() override final;

	Handle &getDeregisterHandleNonConst() override final
	{
		return EventHandler::getDeregisterHandleNonConst();
	}

};

} // namespace andre

#endif // DEREGISTERABLEHANDLER_H
