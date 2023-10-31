#include <string.h>
#include "code/server/fd_event.h"
namespace lutrpc
{

    FdEvent::FdEvent(int fd) : m_fd(fd)
    {
        memset(&m_event, 0, sizeof(m_event));
        memset(&m_callback, 0, sizeof(m_callback));
    }

    FdEvent::FdEvent()
    {
        memset(&m_event, 0, sizeof(m_event));
        memset(&m_callback, 0, sizeof(m_callback));
    }

    FdEvent::~FdEvent()
    {
    }

    std::function<void()> FdEvent::handler()
    {
        return m_callback;
    }

    void FdEvent::regCallBack(std::function<void()> callback)
    {
        m_callback = callback;
        m_event.data.ptr = this;
    }
} // namespace lutrpc
