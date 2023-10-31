#include "code/comm/util.h"
#include "code/server/timer.h"
namespace lutrpc
{
    Timer::Timer(int interval, bool is_repeated, std::function<void()> cb)
        : m_interval(interval), m_is_repeated(is_repeated), m_cb(cb)
    {
        setTimeUpTime();
    }

    //用于初始化和重置定时器
    void Timer::setTimeUpTime()
    {
        m_arrive_time = getNowMs() + m_interval;
    }
}
