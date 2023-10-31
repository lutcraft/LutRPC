// 工具类是对系统调用和C库的封装

#pragma once

#include <sys/types.h>
#include <unistd.h>

namespace lutrpc
{

#define INVALID_PID (0)

    pid_t lutGetPid();

    pid_t lutGetThreadId();
    int64_t getNowMs();

}