
#include <iostream>
#include <string.h>
#include <sys/eventfd.h>
#include "code/comm/essential.h"
#include "code/comm/util.h"
#include "code/comm/log.h"
#include "code/server/reactor.h"
#include "code/server/fd_event_wkup.h"

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
        this->initWakeUp();
        this->initTimer();
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
        int ret = epoll_ctl(m_epoll_fd, op, event->getFd(), &epEvent);
        if (ret == LUT_FAIL)
        {
            ERRORLOG("failed epoll_ctl when add fd, errno=%d, error=%s", errno, strerror(errno));
            ERRORLOG("op=%d,getFd=%u epEvent=%#x", op, event->getFd(), epEvent);
        }
        else
        {
            DEBUGLOG("add event success, fd[%d]", event->getFd());
        }

        return ret;
    }

    int Reactor::delEpollCtl(FdEvent *event)
    {
        // 获取迭代器对象
        auto it = m_listen_fds.find(event->getFd());
        int op = EPOLL_CTL_DEL;
        if (it == m_listen_fds.end()) // 要删除的本就不存在
        {
            return LUT_OK;
        }
        m_listen_fds.erase(event->getFd()); // 删除set元素
        epoll_event epEvent = event->getEpollEvent();
        DEBUGLOG("del event success, fd[%d]", event->getFd());
        int ret = epoll_ctl(m_epoll_fd, op, event->getFd(), &epEvent);
        if (ret == LUT_FAIL)
        {
            ERRORLOG("failed epoll_ctl when del fd, errno=%d, error=%s", errno, strerror(errno));
        }
        else
        {
            DEBUGLOG("add event success, fd[%d]", event->getFd());
        }
        return ret;
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
            wakeUpReactor();
        }
        return LUT_OK;
    }

    int Reactor::delEvent(FdEvent *event)
    {
        if (InReactorThread())
        {
            delEpollCtl(event);
        }
        else
        {
            addTask([this, event]() { // 参数通过lamda表达式的捕获传递
                delEpollCtl(event);
            });
            // 立刻唤醒（添加）
            wakeUpReactor();
        }
        return LUT_OK;
    }

    void Reactor::showListeningFds()
    {
        INFOLOG("Listening fd num = %d", m_listen_fds.size());
        for (auto it = m_listen_fds.begin(); it != m_listen_fds.end(); ++it)
        {
            INFOLOG("%d ", *it);
        }
        return;
    }

    bool Reactor::InReactorThread()
    {
        return lutGetThreadId() == m_thread_id;
    }

    int Reactor::addTask(std::function<void()> cb)
    {
        ScopeMutext<Mutex> lock(m_mutex);
        m_pending_tasks.push(cb);
        return LUT_OK;
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
                    INFOLOG("fd %d trigger 0x%x event", fd_event->getFd(), fd_event->getEpollEvent().events)
                    addTask(fd_event->handler());
                }
            }
        }
    }

    void Reactor::initWakeUp()
    {
        int wakeUpFd = eventfd(0, EFD_NONBLOCK);
        ERRORLOG("Wake up fd[%d]", wakeUpFd);
        if (wakeUpFd < 0)
        {
            ERRORLOG("failed to create event loop, eventfd create error, error info[%d]", errno);
            exit(0);
        }
        m_wakeUpEvent = new WakeUpFdEvent(wakeUpFd);
        m_wakeUpEvent->regCallBack([this]()
                                   {
                            char buf[8];
                            while(read(m_wakeUpEvent->getFd(), buf, 8) != -1 && errno != EAGAIN) {
                            }
                            DEBUGLOG("read full bytes from wakeup fd[%d]", m_wakeUpEvent->getFd()); });
        Reactor::addEvent(m_wakeUpEvent);
    }

    void Reactor::initTimer()
    {
        m_fd_timer = new TimerFdEvent();
        addEvent(m_fd_timer);
    }

    void Reactor::addTimer(Timer::TIMER_SMT_P timer)
    {
        m_fd_timer->addTimerEvent(timer);
    }

    void Reactor::wakeUpReactor()
    {
        INFOLOG("WAKE UP");
        m_wakeUpEvent->wakeup();
    }
}