#include <string.h>
#include <sys/timerfd.h>
#include "code/comm/log.h"
#include "code/server/fd_event_timer.h"

namespace lutrpc
{
    TimerFdEvent::TimerFdEvent() : FdEvent()
    {
        m_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
        DEBUGLOG("timer fd=%d", m_fd);

        this->setEpollEvent(EPOLLIN);
        // 注册定时器到期回调到timerEvent
        // 这里也可以用bind，为了统一全部使用lamda表达式
        this->regCallBack([this]()
                          {
                             std::vector<Timer::TIMER_SMT_P> cb_vec;
                             DEBUGLOG("timer:%d time up! Running CBs!");
                             // 读取文件hd中的内容，防止定时器反复被触发
                             char buf[8];
                             while (1)
                             {
                                 if ((read(m_fd, buf, 8) == -1) && errno == EAGAIN)
                                 {
                                     break;
                                 }
                             }
                             int64_t now = getNowMs();
                             // 选出已经到期的任务，加入本次运行列表，并在原map中删除
                             ScopeMutext<Mutex> lock(m_mutex);
                             auto it = m_time_up_cbs.begin();
                             for (it = m_time_up_cbs.begin(); it != m_time_up_cbs.end(); ++it)
                             {
                                 // 找出已经到期，且没有被取消的定时器
                                 if ((*it).first <= now && !(*it).second->isCancled())
                                 {
                                     cb_vec.push_back((*it).second);
                                 }
                                 else
                                 {
                                     break;
                                 }
                             }
                             // 删除已经到期的定时器
                             m_time_up_cbs.erase(m_time_up_cbs.begin(), it);
                             lock.UnlockScopeMutext();
                             // 已经到期定时器中有repeat属性的，再次添加
                             for (auto i = cb_vec.begin(); i != cb_vec.end(); ++i)
                             {
                                 if ((*i)->isRepeated())
                                 {
                                     // 根据现在时间刷新 arriveTime
                                     (*i)->setTimeUpTime();
                                     //再次将timer注册到timerevent
                                     addTimerEvent(*i);
                                 }
                             }
                             // 执行本次运行列表
                            for (auto i: cb_vec) 
                            {
                                if (i->getCallBack()) 
                                {
                                    i->getCallBack()();
                                }
                            } });
    }

    TimerFdEvent::~TimerFdEvent()
    {
    }

    void TimerFdEvent::addTimerEvent(Timer::TIMER_SMT_P pTimer)
    {
        bool newerTimer = false;
        // 回调加入map
        ScopeMutext<Mutex> lock(m_mutex);
        if (m_time_up_cbs.empty())
        {
            newerTimer = true;
        }
        else
        {
            auto it = m_time_up_cbs.begin();
            // 如果当前定时器，比最早到的定时器早，则需要重置为当前定时器的时间
            if ((*it).second->getArriveTime() > pTimer->getArriveTime())
            {
                newerTimer = true;
            }
        }
        m_time_up_cbs.emplace(pTimer->getArriveTime(), pTimer);
        lock.UnlockScopeMutext();
        // 如果到期时间比第一个还要早，则刷新timerfd为第一个定时器时间
        if (newerTimer)
        {
            resetFdTimer();
        }
    }

    /**
     * 根据map的第一个定时器的时间重置timerfd的到期时间
     *
     */
    void TimerFdEvent::resetFdTimer()
    {
        ScopeMutext<Mutex> lock(m_mutex);
        auto tmp = m_time_up_cbs;
        lock.UnlockScopeMutext();

        if (tmp.size() == 0)
        {
            return;
        }

        int64_t now = getNowMs();

        auto it = tmp.begin();
        int64_t inteval = 0;
        if (it->second->getArriveTime() > now) // 第一个定时器在未来
        {
            inteval = it->second->getArriveTime() - now;
        }
        else // 防止设定负值
        {
            inteval = 100;
        }

        timespec ts;
        memset(&ts, 0, sizeof(ts));
        ts.tv_sec = inteval / 1000;
        ts.tv_nsec = (inteval % 1000) * 1000000;

        itimerspec value;
        memset(&value, 0, sizeof(value));
        value.it_value = ts;

        int rt = timerfd_settime(m_fd, 0, &value, NULL);
        if (rt != 0)
        {
            ERRORLOG("timerfd_settime error, errno=%d, error=%s", errno, strerror(errno));
        }
        DEBUGLOG("timer reset to %lld", now + inteval);
    }

} // namespace lutrpc
