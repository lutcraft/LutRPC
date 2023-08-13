
#include <string>
#include <sstream>
#include <time.h> //c库的time
#include <sys/time.h>
#include "code/comm/log.h"

namespace lutrpc
{
    std::string LogLevelToString(LogLevel level);
    LogLevel StringToLogLevel(const std::string &log_level);

    static Logger *g_logger = NULL;

    Logger *Logger::GetGlobalLogger()
    {
        if (g_logger == NULL)
        {
            Logger::InitGlobalLogger();
        }
        return g_logger;
    }

    void Logger::InitGlobalLogger()
    {

        LogLevel global_log_level = lutrpc::Info;
        printf("Init log level [%s]\n", LogLevelToString(global_log_level).c_str());
        g_logger = new Logger(global_log_level);
    }

    std::string LogEvent::toString()
    {
        struct timeval now_time;

        gettimeofday(&now_time, nullptr);

        struct tm now_time_t;
        localtime_r(&(now_time.tv_sec), &now_time_t);

        char buf[128];
        strftime(&buf[0], 128, "%y-%m-%d %H:%M:%S", &now_time_t);
        std::string time_str(buf);
        int ms = now_time.tv_usec / 1000;
        time_str = time_str + "." + std::to_string(ms);

        std::stringstream ss;

        ss << "[" << LogLevelToString(m_level) << "]\t"
           << "[" << time_str << "]\t"
           << "[" << m_pid << ":" << m_thread_id << "]\t";

        return ss.str();
    }

    std::string LogLevelToString(LogLevel level)
    {
        switch (level)
        {
        case Debug:
            return "DEBUG";

        case Info:
            return "INFO";

        case Error:
            return "ERROR";
        default:
            return "UNKNOWN";
        }
    }

    LogLevel StringToLogLevel(const std::string &log_level)
    {
        if (log_level == "DEBUG")
        {
            return Debug;
        }
        else if (log_level == "INFO")
        {
            return Info;
        }
        else if (log_level == "ERROR")
        {
            return Error;
        }
        else
        {
            return Unknown;
        }
    }

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
}