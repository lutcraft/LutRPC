#pragma once

#include <string>
#include <queue>
#include <memory>
#include "code/comm/mutex.h"
#include "code/comm/util.h"

namespace lutrpc
{

    //模板函数写在.h中
    template <typename... Args>
    std::string formatString(const char *str, Args &&...args)
    {

        int size = snprintf(nullptr, 0, str, args...);

        std::string result;
        if (size > 0)
        {
            result.resize(size);
            snprintf(&result[0], size + 1, str, args...);
        }

        return result;
    }

#define DEBUGLOG(str, ...)                                                                                                                                                                                  \
    if (lutrpc::Logger::GetGlobalLogger()->getLogLevel() <= lutrpc::DEBUG)                                                                                                                                  \
    {                                                                                                                                                                                                       \
        lutrpc::LogEvent tmpEvent(lutrpc::lutGetPid(), lutrpc::lutGetThreadId(), std::string(__FILE__), __LINE__, lutrpc::formatString(str, ##__VA_ARGS__), lutrpc::LogLevel::DEBUG); \
        lutrpc::Logger::GetGlobalLogger()->pushLog(tmpEvent);                                                                                                                                             \
        lutrpc::Logger::GetGlobalLogger()->log();                                                                                                                                                           \
    }

#define INFOLOG(str, ...)                                                                                                                                                                                   \
    if (lutrpc::Logger::GetGlobalLogger()->getLogLevel() <= lutrpc::INFO)                                                                                                                                   \
    {                                                                                                                                                                                                       \
        lutrpc::LogEvent tmpEvent(lutrpc::lutGetPid(), lutrpc::lutGetThreadId(), std::string(__FILE__), __LINE__, lutrpc::formatString(str, ##__VA_ARGS__), lutrpc::LogLevel::INFO); \
        lutrpc::Logger::GetGlobalLogger()->pushLog(tmpEvent);                                                                                                                                             \
        lutrpc::Logger::GetGlobalLogger()->log();                                                                                                                                                           \
    }

#define ERRORLOG(str, ...)                                                                                                                                                                                  \
    if (lutrpc::Logger::GetGlobalLogger()->getLogLevel() <= lutrpc::ERROR)                                                                                                                                  \
    {                                                                                                                                                                                                       \
        lutrpc::LogEvent tmpEvent(lutrpc::lutGetPid(), lutrpc::lutGetThreadId(), std::string(__FILE__), __LINE__, lutrpc::formatString(str, ##__VA_ARGS__), lutrpc::LogLevel::ERROR); \
        lutrpc::Logger::GetGlobalLogger()->pushLog(tmpEvent);                                                                                                                                             \
        lutrpc::Logger::GetGlobalLogger()->log();                                                                                                                                                           \
    }

    enum LogLevel
    {
        UNKNOWN = 0,
        DEBUG = 1,
        INFO = 2,
        ERROR = 3
    };

    class LogEvent
    {
    public:
        LogEvent(int32_t pid, int32_t thread_id, std::string file_name, int32_t file_line, std::string log_msg, LogLevel level) : 
                                            m_pid(pid),
                                            m_thread_id(thread_id),
                                            m_file_name(file_name),
                                            m_file_line(file_line),
                                            m_log_msg(log_msg),
                                            m_level(level) {}   //初始化列表顺序，需要与成员变量顺序一致

        std::string toString();

    private:
        int32_t m_pid;           // 进程号
        int32_t m_thread_id;     // 线程号
        std::string m_file_name; // 文件名
        int32_t m_file_line;     // 行号

        std::string m_log_msg;
        LogLevel m_level; // 日志级别
    };

    class Logger
    {
    public:
        typedef std::shared_ptr<Logger> s_ptr; // 智能指针，目前没用

        // 构造
        Logger(LogLevel level) : m_set_level(level) {}
        // 将一条logEventpush到logger的队列中
        void pushLog(const LogEvent &logEvt);
        // 将logger中队列里的日志逐条打印，并清空队列
        void log();

        LogLevel getLogLevel() const
        {
            return m_set_level;
        }

        static Logger *GetGlobalLogger();

        static void InitGlobalLogger();

    private:
        LogLevel m_set_level;
        std::queue<LogEvent> m_log_evnet_queue;

        Mutex m_mutex;
    };
}