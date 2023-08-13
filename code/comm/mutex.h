#pragma once

#include <pthread.h>

namespace lutrpc
{

    // 通过构造和析构进行加锁和解锁，锁范围和对象作用域范围相同
    template <class T>
    class ScopeMutext
    {

    public:
        ScopeMutext(T &mutex) : m_mutex(mutex)
        {
            m_mutex.lock();
            m_is_lock = true;
        }

        ~ScopeMutext()
        {
            m_mutex.unlock();
            m_is_lock = false;
        }

        void lock()
        {
            if (!m_is_lock)
            {
                m_mutex.lock();
            }
        }

        void unlock()
        {
            if (m_is_lock)
            {
                m_mutex.unlock();
            }
        }

    private:
        T &m_mutex;

        bool m_is_lock{false};
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
