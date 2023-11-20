#include "code/comm/essential.h"
#include "code/comm/log.h"
#include "code/comm/config.h"
#include "code/server/thread_group.h"
namespace lutrpc
{

    ThreadGroup::ThreadGroup(int size) : m_size(size)
    {
        m_thread_group.resize(size);
        for (int i = 0; i < size; ++i)
        {
            m_thread_group[i] = new Thread();
        }
    }

    ThreadGroup::~ThreadGroup()
    {
    }

    void ThreadGroup::start()
    {
        for (size_t i = 0; i < m_thread_group.size(); ++i)
        {
            m_thread_group[i]->start();
        }
    }

    void ThreadGroup::join()
    {
        for (size_t i = 0; i < m_thread_group.size(); ++i)
        {
            m_thread_group[i]->join();
        }
    }

    Thread *ThreadGroup::getThread()
    {
        // 满或空返回第一个
        if (m_index == (int)m_thread_group.size() || m_index == -1)
        {
            m_index = 0;
        }
        // 递增
        return m_thread_group[m_index++];
    }

}