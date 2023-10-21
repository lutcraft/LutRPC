#pragma once
#include <functional>
#include <sys/epoll.h>
#include "code/server/fd_event.h"
namespace lutrpc
{
    /**
     * FdEvnet是对fd事件的封装
     *  has a fd epoll监听的文件描述符
     *  has a epoll_evnet 标识在这个描述符上，epoll需要监听的事件类型
     *  has callback list 标识在指定描述符上，指定事件发生时，需要进行的操作（常常是激活线程池中的一个线程，进行IO操作）
     *
     */
    class WakeUpFdEvent : public FdEvent
    {
    public:
        WakeUpFdEvent(int fd);

        ~WakeUpFdEvent();

        void wakeup();
    };

} // namespace lutrpc