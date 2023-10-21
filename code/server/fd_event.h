#pragma once
#include <functional>
#include <sys/epoll.h>
namespace lutrpc
{
    /**
     * FdEvnet是对fd事件的封装
     *  has a fd epoll监听的文件描述符
     *  has a epoll_evnet 标识在这个描述符上，epoll需要监听的事件类型
     *  has callback list 标识在指定描述符上，指定事件发生时，需要进行的操作（常常是激活线程池中的一个线程，进行IO操作）
     * 
    */
    class FdEvent
    {
    private:
        epoll_event m_event;
        std::function<void()> m_callback;       //本事件注册的读写回调，在epoll抓到这个event事件发生时，会调用这个回调
    public:
        FdEvent(int fd){this->m_fd = fd;};
        ~FdEvent();
        int getFd(){return this->m_fd;};
        int setEpollEvent(uint32_t event){ this->m_event.events |= event; return 0;};
        epoll_event getEpollEvent(){return this->m_event;};
        std::function<void()> handler();     //获取event回调的接口
        void regCallBack(std::function<void()> callback);//注册event回调的接口，回调参数通过lamda表达式传入参数，但这导致传入的回调函数必须为lamda表达式了
    protected:
        int m_fd{-1};
    };
    
} // namespace lutrpc