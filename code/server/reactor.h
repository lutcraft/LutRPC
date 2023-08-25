#pragma once

namespace lutrpc
{
    /**
     * 反应堆对象，每个线程一个，是RPCServer的事件调度中心
    */
    class Reactor
    {
    private:
        int m_epoll_fd {0};     //epoll句柄
    public:
        Reactor();
        ~Reactor();

        int GetEpollFd(){return this->m_epoll_fd;};
        //向epool句柄新增Event
        int addEvent();
    };
    
} // namespace lutrpc
