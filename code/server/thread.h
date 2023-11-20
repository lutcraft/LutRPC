#pragma once
#include <pthread.h> // 暂时先用pthread & semaphore
#include <semaphore.h>
#include "code/server/reactor.h"

namespace lutrpc
{
    class Thread
    {
    public:
        // 创建并初始化一个线程，这个现在在启动后会自旋，直到主线程唤醒信号量
        Thread();

        ~Thread();

        Reactor *getReactor();

        // 解除该线程自旋，开始启动
        void start();

        // 让主线程等待该子线程运行结束
        void join();

    public:
        //线程入口函数
        static void *Main(void *arg);

    private:
        pid_t m_thread_id{-1}; // 线程号
        pthread_t m_thread{0}; // 线程句柄

        Reactor *m_reactor{NULL}; // 当前 io 线程的反应堆对象

        sem_t m_init_semaphore; //初始化信号量，锁住主线程，直到子线程初始化完成，防止主线程对子线程的异常访问

        sem_t m_start_semaphore;//子线程启动信号量，在子线程初始化完成后，锁住子线程，直到主线程通知其继续执行
    };
}