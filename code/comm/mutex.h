#pragma once

#include <pthread.h>

namespace lutrpc
{

    // 自动锁，通过构造和析构进行加锁和解锁，锁范围和对象作用域范围相同
    template <class T>
    class ScopeMutext
    {

    public:
        ScopeMutext(T &mutex) : m_mutex(mutex)
        {
            m_mutex.lock();
        }

        ~ScopeMutext()
        {
            m_mutex.unlock();
        }

    private:
        T &m_mutex;
    };

    // 通过构造和析构进行锁对象的创建，用户进行锁定和解锁的控制
    class Mutex
    {
    public:
        Mutex()
        {
            pthread_mutex_init(&m_mutex, NULL);
        }

        ~Mutex()
        {
            pthread_mutex_destroy(&m_mutex);
        }

        void lock()
        {
            pthread_mutex_lock(&m_mutex);
        }

        void unlock()
        {
            pthread_mutex_unlock(&m_mutex);
        }

    private:
        pthread_mutex_t m_mutex;
    };

}
