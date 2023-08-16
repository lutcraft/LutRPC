#pragma once
#include <3rd/json.hpp>
#include <string>

using json = nlohmann::json;

namespace lutrpc
{

    class Config
    {
    private:
        Config(std::string);
        int InitConfigData(json);
    public:
        std::string m_log_level;

        ~Config();

        static Config * GetGlobalConfig();
    };

}