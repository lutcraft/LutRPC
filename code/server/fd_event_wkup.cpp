#include <string>
#include "code/comm/log.h"
#include "code/server/fd_event_wkup.h"

using namespace std;

namespace lutrpc
{
    WakeUpFdEvent::WakeUpFdEvent(int fd) : FdEvent(fd)
    {
    }

    WakeUpFdEvent::~WakeUpFdEvent()
    {
    }

    void WakeUpFdEvent::wakeup()
    {
        string buf = "wakeup!";
        int rt = write(m_fd, buf.c_str(), buf.length());
        if (rt != (int)buf.length())
        {
            ERRORLOG("write to wakeup fd less than %u bytes, fd[%d]", m_fd, buf.length());
        }
        DEBUGLOG("success read %u bytes", buf.length());
    }
} // namespace lutrpc
