// 工具类是对系统调用和C库的封装

#pragma once

#include <unistd.h>

namespace lutrpc
{

#define INVALID_PID (0)
#define INVALID_PID (0)

    pid_t lutGetPid();

    pid_t lutGetThreadId();

}