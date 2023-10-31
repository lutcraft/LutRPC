#pragma once
#include <map>
#include "code/server/fd_event.h"
#include "code/server/timer.h"
namespace lutrpc
{
    /**
     * FdEvnet是对fd事件的封装
     *  has a fd epoll监听的文件描述符
     *  has a epoll_evnet 标识在这个描述符上，epoll需要监听的事件类型
     *  has callback list 标识在指定描述符上，指定事件发生时，需要进行的操作（常常是激活线程池中的一个线程，进行IO操作）
     *
     */
    class TimerFdEvent : public FdEvent
    {
    public:
        TimerFdEvent();
        ~TimerFdEvent();
        void resetFdTimer();
        void addTimerEvent(Timer::TIMER_SMT_P pTimer);

    private:
        Mutex m_mutex;                                            // m_time_up_cbs 的读写锁
        std::multimap<int64_t, Timer::TIMER_SMT_P> m_time_up_cbs; // 同一时间可能有多个定时器回调，所以需要使用一对多的multimap
    };

} // namespace lutrpc