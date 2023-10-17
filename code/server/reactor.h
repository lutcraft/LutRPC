#pragma once

#include <set>
#include "code/server/fd_event.h"

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
        Mutex m_mutex;                                     // m_pending_tasks的写锁
        std::queue<std::function<void()>> m_pending_tasks; // 本reactor对象在循环中需要执行的任务
        // 向本reactor对象的epoll添加 在fd上的event事件监听，如果对应fd已经被epoll监听，则对event事件列表进行modify
        int addEpollCtl(FdEvent *event);

    public:
        Reactor();
        ~Reactor();

        int GetEpollFd() { return this->m_epoll_fd; };
        // 向epool句柄新增Event，内部封装epoll_ctl
        int addEvent(FdEvent *event);
        // 向epool句柄删除Event，内部封装epoll_ctl
        int delEvent(FdEvent *event);
        bool InReactorThread();
        int addTask(std::function<void()> cb);
        void loop();
    };

} // namespace lutrpc
