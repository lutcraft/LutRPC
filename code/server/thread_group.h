#pragma once
#include <vector>
#include "code/server/thread.h"

namespace lutrpc
{

    class ThreadGroup
    {

    public:
        ThreadGroup(int size);

        ~ThreadGroup();
        // 启动线程组所有线程
        void start();
        // join线程组所有线程
        void join();
        // 从进程组中，取一个进程
        Thread *getThread();

    private:
        int m_size{0};      // 进程组有几个线程实例
        std::vector<Thread *> m_thread_group;
        int m_index{0}; // 当前实例
    };

}
