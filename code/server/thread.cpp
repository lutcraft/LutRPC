
#include <pthread.h>
#include <assert.h>
#include "code/comm/log.h"
#include "code/comm/util.h"

#include "code/server/thread.h"
namespace lutrpc
{

    Thread::Thread()
    {

        int rt = sem_init(&m_init_semaphore, 0, 0);
        assert(rt == 0);

        rt = sem_init(&m_start_semaphore, 0, 0);
        assert(rt == 0);

        pthread_create(&m_thread, NULL, &Thread::Main, this);

        // wait, 直到新线程执行完 Main 函数的前置
        sem_wait(&m_init_semaphore);

        DEBUGLOG("Thread [%d] create success", m_thread_id);
    }

    Thread::~Thread()
    {

        m_reactor->stop();
        sem_destroy(&m_init_semaphore);
        sem_destroy(&m_start_semaphore);
        // 主线程等待从线程结束再退出
        pthread_join(m_thread, NULL);

        if (m_reactor)
        {
            delete m_reactor;
            m_reactor = NULL;
        }
    }

    void *Thread::Main(void *arg)
    {
        Thread *thread = static_cast<Thread *>(arg);

        thread->m_reactor = new Reactor();
        thread->m_thread_id = lutGetThreadId();

        // 唤醒等待的线程
        sem_post(&thread->m_init_semaphore);

        // 让IO 线程等待
        DEBUGLOG("Thread %d created, wait start semaphore", thread->m_thread_id);
        sem_wait(&thread->m_start_semaphore);

        DEBUGLOG("Thread %d start loop ", thread->m_thread_id);
        thread->m_reactor->loop();
        DEBUGLOG("Thread %d end loop ", thread->m_thread_id);

        return NULL;
    }

    Reactor *Thread::getReactor()
    {
        return m_reactor;
    }

    void Thread::start()
    {
        DEBUGLOG("Now invoke Thread %d", m_thread_id);
        sem_post(&m_start_semaphore);
    }

    void Thread::join()
    {
        pthread_join(m_thread, NULL);
    }

}