#ifndef HANDLE_HPP
#define HANDLE_HPP
#include <functional>

#include "andre_global.h"

namespace andre
{

// Идентифицирует событие
struct ANDRESHARED_EXPORT Handle
{
	unsigned long long commandID;		
	unsigned long long messageParam;	
	
	bool operator<(const Handle &right) const
	{
		if (this->commandID < right.commandID) {
			return true;
		}

		if (this->commandID > right.commandID) {
			return false;
		}
		
		if (this->messageParam < right.messageParam) {
			return true;
		}
		
		return false;
	}
	
	bool operator==(const Handle &right) const
	{
		return ( (this->commandID == right.commandID) &&
				 (this->messageParam == right.messageParam) );
	}

	bool operator!=(const Handle &right) const
	{
		return ( ! operator==(right) );
	}
};

} // namespace andre

#endif // HANDLE_HPP
