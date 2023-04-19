#ifndef MESSAGEDATA_H
#define MESSAGEDATA_H

#include <string>
#include <memory>
#include "Handle.hpp"

#include "andre_global.h"

namespace andre
{
struct ANDRESHARED_EXPORT ConstData
{
	Handle handle;// 
	std::string strCommandID;	
	std::string strMessageParam;
	
	ConstData();
	ConstData(const ConstData &data);
	virtual ~ConstData();
	ConstData &operator=(const ConstData &data);
};

// Смысл этого класса в том, что бы вернуть константные данные,
// которые могут быть обработаны в один момент в разных обработчиках(EventHandler),
//    находящихся в разных потоках и принадлежащих разным Реакторам
class ANDRESHARED_EXPORT MessageData final
{
public:
	explicit MessageData(std::unique_ptr<ConstData> &data)
	{
		m_data = std::move(data);
	}

	const std::shared_ptr<const ConstData> &getData()
	{
		return m_data;
	}
	
private:
	std::shared_ptr<const ConstData> m_data;
};

} // namespace andre

#endif // MESSAGEDATA_H
