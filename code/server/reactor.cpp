#include <sys/epoll.h>

#include "code/comm/util.h"
#include "code/comm/log.h"
#include "code/server/reactor.h"


namespace lutrpc
{

    // reactor 线程级单例
    static thread_local Reactor *t_reactor = NULL;

    Reactor::Reactor()
    {
        if (t_reactor != NULL)
        {
            ERRORLOG("this thread has created event loop\n");
            return;
        }

        m_epoll_fd = epoll_create(233);
        if (m_epoll_fd == -1)
        {
            ERRORLOG("failed to create event loop, epoll_create error, error info[%d]\n", errno);
            exit(0);
        }
        INFOLOG("succ create event loop in thread %d\n", lutGetThreadId());
        t_reactor = this;
    }
    Reactor::~Reactor()
    {
        close(m_epoll_fd);
    }

    Reactor::addEvent()
    {

    }
    
}