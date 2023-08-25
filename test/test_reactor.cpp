
#include <iostream>
#include "code/comm/log.h"
#include "code/comm/config.h"
#include "code/server/reactor.h"

int main(int argc, const char** argv) {
    lutrpc::Config::GetGlobalConfig();
    lutrpc::Logger::InitGlobalLogger();

    lutrpc::Reactor *reactor = new lutrpc::Reactor();
    std::cout << "Hello!" << reactor->GetEpollFd() << std::endl;
    return 0;
}