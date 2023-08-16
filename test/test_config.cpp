#include <iostream>
#include "code/comm/config.h"


int main(int argc, char const *argv[])
{
    lutrpc::Config::GetGlobalConfig();

    std::cout << lutrpc::Config::GetGlobalConfig()->m_log_level << std::endl;

    return 0;
}
