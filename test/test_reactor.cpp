
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "code/comm/essential.h"
#include "code/comm/log.h"
#include "code/comm/config.h"
#include "code/server/reactor.h"

int main(int argc, const char **argv)
{
    lutrpc::Config::GetGlobalConfig();
    lutrpc::Logger::InitGlobalLogger();

    lutrpc::Reactor *reactor = new lutrpc::Reactor();


    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    ERRORLOG("listenfd = %d", listenfd);
    if (listenfd == -1)
    {
        ERRORLOG("listenfd = -1");
        exit(0);
    }
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    addr.sin_port = htons(12310);
    addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &addr.sin_addr);

    int rt = bind(listenfd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr));
    if (rt != 0)
    {
        ERRORLOG("bind error");
        exit(1);
    }

    rt = listen(listenfd, 100);
    if (rt != 0)
    {
        ERRORLOG("listen error");
        exit(1);
    }

    lutrpc::FdEvent *event = new lutrpc::FdEvent(listenfd);
    event->setEpollEvent(EPOLLIN);
    INFOLOG("event %d\n", event->getEpollEvent().events);
    event->regCallBack([listenfd]()
            {
                sockaddr_in peer_addr;
                socklen_t addr_len = sizeof(peer_addr);
                memset(&peer_addr, 0, sizeof(peer_addr));
                int clientfd = accept(listenfd, reinterpret_cast<sockaddr*>(&peer_addr), &addr_len);

                INFOLOG("success get client fd[%d], peer addr: [%s:%d]", clientfd, inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port));
                return; 
            });
    reactor->addEvent(event);
    reactor->delEvent(event);
    std::cout << "Epoll fdid:" << reactor->GetEpollFd() << std::endl;

    int i = 0;
    lutrpc::Timer::TIMER_SMT_P pTimer = std::make_shared<lutrpc::Timer>(1000 ,true, [&i]()
    {
        INFOLOG("timer callbacking!%d", i++);
    });

    reactor->addTimer(pTimer);
    reactor->showListeningFds();

    reactor->loop();
    return LUT_OK;
}