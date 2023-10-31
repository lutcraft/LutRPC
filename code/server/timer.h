#pragma once
#include <memory>
#include <functional>

namespace lutrpc
{
    /**
     * 一个定时器对象，被注册到fd_event_timer have 的multimap中
     */
    class Timer
    {
    public:
        //定义智能指针类型，指向一个Timer类型对象
        typedef std::shared_ptr<Timer> TIMER_SMT_P;

        Timer(int interval, bool is_repeated, std::function<void()> cb);

        int64_t getArriveTime()
        {
            return m_arrive_time;
        }

        void setCancled(bool value)
        {
            m_is_cancled = value;
        }

        bool isCancled()
        {
            return m_is_cancled;
        }

        bool isRepeated()
        {
            return m_is_repeated;
        }

        std::function<void()> getCallBack()
        {
            return m_cb;
        }

        void setTimeUpTime();

    private:
        int64_t m_arrive_time; // ms
        int64_t m_interval;    // ms
        bool m_is_repeated{false};
        bool m_is_cancled{false};

        std::function<void()> m_cb;
    };
}