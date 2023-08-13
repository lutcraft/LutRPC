#pragma once

#include <string>
#include <queue>
#include <memory>
#include "code/comm/mutex.h"
#include "code/comm/util.h"

namespace lutrpc
{

template <typename... Args> std::string formatString(const char *str, Args &&...args);
#define DEBUGLOG(str, ...)                                                                                                                                                                                                                \
    if (lutrpc::Logger::GetGlobalLogger()->getLogLevel() <= lutrpc::Debug)                                                                                                                                                                \
    {                                                                                                                                                                                                                                     \
        lutrpc::Logger::GetGlobalLogger()->pushLog((new lutrpc::LogEvent(lutrpc::lutGetPid(), lutrpc::lutGetThreadId(), lutrpc::LogLevel::Debug))->toString() + "[" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "]\t" + lutrpc::formatString(str, ##__VA_ARGS__) + "\n"); \
        lutrpc::Logger::GetGlobalLogger()->log();                                                                                                                                                                                         \
    }

#define INFOLOG(str, ...)                                                                                                                                                                                                                \
    if (lutrpc::Logger::GetGlobalLogger()->getLogLevel() <= lutrpc::Info)                                                                                                                                                                \
    {                                                                                                                                                                                                                                    \
        lutrpc::Logger::GetGlobalLogger()->pushLog((new lutrpc::LogEvent(lutrpc::lutGetPid(), lutrpc::lutGetThreadId(), lutrpc::LogLevel::Info))->toString() + "[" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "]\t" + lutrpc::formatString(str, ##__VA_ARGS__) + "\n"); \
        lutrpc::Logger::GetGlobalLogger()->log();                                                                                                                                                                                        \
    }

#define ERRORLOG(str, ...)                                                                                                                                                                                                                \
    if (lutrpc::Logger::GetGlobalLogger()->getLogLevel() <= lutrpc::Error)                                                                                                                                                                \
    {                                                                                                                                                                                                                                     \
        lutrpc::Logger::GetGlobalLogger()->pushLog((new lutrpc::LogEvent(lutrpc::lutGetPid(), lutrpc::lutGetThreadId(), lutrpc::LogLevel::Error))->toString() + "[" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "]\t" + lutrpc::formatString(str, ##__VA_ARGS__) + "\n"); \
        lutrpc::Logger::GetGlobalLogger()->log();                                                                                                                                                                                         \
    }

    enum LogLevel
    {
        Unknown = 0,
        Debug = 1,
        Info = 2,
        Error = 3
    };

    class LogEvent
    {
    public:
        LogEvent(int32_t pid, int32_t thread_id, LogLevel level) : m_pid(pid),
                                                                   m_thread_id(thread_id),
                                                                   m_level(level) {}

        std::string toString();

    private:
        int32_t m_pid;       // 进程号
        int32_t m_thread_id; // 线程号

        LogLevel m_level; // 日志级别
        std::string m_log_msg;
    };

    class Logger
    {
    public:
        typedef std::shared_ptr<Logger> s_ptr;

        Logger(LogLevel level) : m_set_level(level) {}

        void pushLog(const std::string &msg);

        void log();

        LogLevel getLogLevel() const
        {
            return m_set_level;
        }

    public:
        static Logger *GetGlobalLogger();

        static void InitGlobalLogger();

    private:
        LogLevel m_set_level;
        std::queue<std::string> m_buffer;

        Mutex m_mutex;
    };

}