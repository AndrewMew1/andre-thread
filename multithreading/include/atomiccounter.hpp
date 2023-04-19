#ifndef ATOMICCOUNTER_HPP
#define ATOMICCOUNTER_HPP

#include <atomic>

namespace multithread
{
// Класс показывает, сколько потоков находится, в текущий момент, внутри скопа

class AtomicCounter
{
public:
	AtomicCounter(std::atomic<int> &count)
		:m_count(count)
	{
		m_count++;
	}
	
	~AtomicCounter()
	{
		m_count--;
	}

	AtomicCounter(const AtomicCounter&) = delete;
	AtomicCounter& operator=(const AtomicCounter&) = delete;
	
private:
	std::atomic<int> &m_count;
};

} //namespace multithread
#endif // ATOMICCOUNTER_HPP
