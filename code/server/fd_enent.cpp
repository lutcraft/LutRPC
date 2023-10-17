#include "code/server/fd_event.h"
namespace lutrpc
{
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
