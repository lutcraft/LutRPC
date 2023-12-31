#pragma once

#include <set>
#include "code/server/fd_event.h"
#include "code/server/fd_event_wkup.h"
#include "code/server/fd_event_timer.h"

namespace lutrpc
{
    /**
     * 反应堆对象，每个线程一个，是RPCServer的事件调度中心
     */
    class Reactor
    {
    private:
        int m_thread_id;                                   // 运行本reactor实例的线程号
        int m_epoll_fd{0};                                 // epoll句柄
        std::set<int> m_listen_fds;                        // reactr监听的所有hd
        Mutex m_mutex;                                     // m_pending_tasks 的写锁
        std::queue<std::function<void()>> m_pending_tasks; // 本reactor对象在循环中需要执行的任务
        // 向本reactor对象的epoll添加 在fd上的event事件监听，如果对应fd已经被epoll监听，则对event事件列表进行modify
        int addEpollCtl(FdEvent *event);
        int delEpollCtl(FdEvent *event);

        TimerFdEvent * m_fd_timer{NULL};

        WakeUpFdEvent *m_wakeUpEvent{NULL};
        bool m_stop_flag {false};
    protected:
        int addTask(std::function<void()> cb);
    public:
        Reactor();
        ~Reactor();

        int GetEpollFd() { return this->m_epoll_fd; };
        // 向epool句柄新增Event，内部封装epoll_ctl
        int addEvent(FdEvent *event);
        // 向epool句柄删除Event，内部封装epoll_ctl
        int delEvent(FdEvent *event);
        //向epoll 添加一个定时器
        void addTimer(Timer::TIMER_SMT_P timer);

        void showListeningFds();

        bool InReactorThread();
        void initWakeUp();
        void initTimer();
        void wakeUpReactor();       //向wakeup fd写入，使得epoll_wait可以返回
        void loop();
        void stop();
    };

} // namespace lutrpc
