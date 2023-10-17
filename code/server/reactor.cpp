
#include "code/comm/util.h"
#include "code/comm/log.h"
#include "code/server/reactor.h"

namespace lutrpc
{

#define EPOLL_MAX_TIME_OUT 100000 // 100s
#define EPOLL_MAX_EVENTS_NUM 10

    // reactor 线程级单例，每个线程都有一个线程级别的reactor作为自身处理消息的驱动
    static thread_local Reactor *t_reactor = NULL;

    Reactor::Reactor()
    {
        if (t_reactor != NULL)
        {
            ERRORLOG("this thread has created event loop\n");
            return;
        }
        m_thread_id = lutGetThreadId();
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

    int Reactor::addEpollCtl(FdEvent *event)
    {
        // 获取迭代器对象
        auto it = m_listen_fds.find(event->getFd());
        int op = EPOLL_CTL_ADD;
        if (it != m_listen_fds.end()) // 要添加的已存在
        {
            op = EPOLL_CTL_MOD;
        }
        m_listen_fds.insert(event->getFd()); // 可以反复instert
        epoll_event epEvent = event->getEpollEvent();
        return epoll_ctl(m_epoll_fd, op, event->getFd(), &epEvent);
    }

    int Reactor::addEvent(FdEvent *event)
    {
        if (InReactorThread())
        {
            // 直接添加
            addEpollCtl(event);
        }
        else // 是其他线程调用了本reactor实例的add方法，将epollctrl方法作为回调，加入到本reactor实例的任务队列，等待下次线程唤醒时添加
        {
            addTask([this, event]() { // 参数通过lamda表达式的捕获传递
                addEpollCtl(event);
            });
            // 立刻唤醒（添加）
            //  wakeUpReactor();
        }
        return 0;
    }

    int Reactor::delEvent(FdEvent *event)
    {
        if (InReactorThread())
        {
            /* code */
        }
        return 0;
    }

    bool Reactor::InReactorThread()
    {
        return lutGetThreadId() == m_thread_id;
    }

    int Reactor::addTask(std::function<void()> cb)
    {
        ScopeMutext<Mutex> lock(m_mutex);
        m_pending_tasks.push(cb);
        return 0;
    }

    /**
     * 启动循环反应堆主循环，不断等待事件（epollwait）和执行订阅任务（task）
     */
    void Reactor::loop()
    {
        while (1)
        {
            ScopeMutext<Mutex> lock(m_mutex);
            std::queue<std::function<void()>> tmp_tasks;
            m_pending_tasks.swap(tmp_tasks);
            lock.UnlockScopeMutext();

            while (!tmp_tasks.empty())
            {
                std::function<void()> cb = tmp_tasks.front();
                tmp_tasks.pop();
                if (cb)
                {
                    cb();
                }
            }
            epoll_event result_events[EPOLL_MAX_EVENTS_NUM];
            int rt = epoll_wait(m_epoll_fd, result_events, EPOLL_MAX_EVENTS_NUM, EPOLL_MAX_TIME_OUT);
            INFOLOG("now end epoll_wait, rt = %d", rt);
            if (rt < 0)
            {
                ERRORLOG("epoll_wait error, errno=", errno);
            }
            else
            {
                for (int i = 0; i < rt; ++i)
                {
                    epoll_event triggering_event = result_events[i];
                    FdEvent *fd_event = static_cast<FdEvent *>(triggering_event.data.ptr);
                    if (fd_event == NULL)
                    {
                        ERRORLOG("fd_event = NULL, continue");
                        continue;
                    }
                    INFOLOG("fd %d trigger %d event", fd_event->getFd(), fd_event->getEpollEvent().events)
                    addTask(fd_event->handler());
                }
            }
        }
    }
}