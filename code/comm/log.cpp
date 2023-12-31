
#include <string>
#include <sstream>
#include <time.h> //c库的time
#include <sys/time.h>
#include "code/comm/log.h"
#include "code/comm/config.h"

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

        LogLevel global_log_level = StringToLogLevel(Config::GetGlobalConfig()->m_log_level);
        printf("Init log level [%s]\n", LogLevelToString(global_log_level).c_str());
        g_logger = new Logger(global_log_level);
    }

    void Logger::pushLog(const lutrpc::LogEvent &logEvt)
    {
        ScopeMutext<Mutex> lock(m_mutex);
        m_log_evnet_queue.push(logEvt);
    }

    void Logger::log()
    {
        //尽快换出临界区数据，防止锁冲突
        ScopeMutext<Mutex> lock(m_mutex);
            std::queue<LogEvent> tmp;
            m_log_evnet_queue.swap(tmp);

        while (!tmp.empty()) {
            LogEvent logEvt = tmp.front();
            printf(logEvt.toString().c_str());
            tmp.pop();
        }
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
           << "[" << m_pid << ":" << m_thread_id << "]\t"
           << "[" << m_file_name << ":" << m_file_line << "]\t"
           << m_log_msg << "\n"
           ;

        return ss.str();
    }

    std::string LogLevelToString(LogLevel level)
    {
        switch (level)
        {
        case DEBUG:
            return "DEBUG";

        case INFO:
            return "INFO";

        case ERROR:
            return "ERROR";
        default:
            return "UNKNOWN";
        }
    }

    LogLevel StringToLogLevel(const std::string &log_level)
    {
        if (log_level == "DEBUG")
        {
            return DEBUG;
        }
        else if (log_level == "INFO")
        {
            return INFO;
        }
        else if (log_level == "ERROR")
        {
            return ERROR;
        }
        else
        {
            return UNKNOWN;
        }
    }


}