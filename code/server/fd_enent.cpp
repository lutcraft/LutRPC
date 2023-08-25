#pragma once
#include <sys/epoll.h>

namespace lutrpc
{
    /**
     * FdEvnet是对fd事件的封装
     *  has a fd epoll监听的文件描述符
     *  has a epoll_evnet 标识在这个描述符上，epoll需要监听的事件类型
     *  has callback list 标识在指定描述符上，指定事件发生时，需要进行的操作（常常是激活线程池中的一个线程，进行IO操作）
     * 
    */
    class FdEvent
    {
    private:
        int m_fd{-1};
        epoll_event m_event;
    public:
        FdEvent(int fd){this->m_fd = fd;};
        ~FdEvent();
    };
    
} // namespace lutrpc
