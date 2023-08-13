#include <sys/syscall.h>

#include "code/comm/util.h"

namespace lutrpc
{

    // 进程号
    static int g_pid = 0;

    // 线程号，线程存储周期
    static thread_local int g_thread_id = 0;

    pid_t lutGetPid()
    {
        if (g_pid != INVALID_PID)
        {
            return g_pid;
        }
        return getpid(); // 系统调用，获取进程号
    }

    // 每个线程号在全局变量进行缓存
    pid_t lutGetThreadId()
    {
        if (g_thread_id != 0)
        {
            return g_thread_id;
        }
        return syscall(SYS_gettid); // c标准库 -lc
    }

}