#include <fstream>
#include "code/comm/essential.h"
#include "code/comm/config.h"
#include "code/comm/log.h"


namespace lutrpc
{
    static Config *g_config = NULL;


    Config * Config::GetGlobalConfig()
    {
        if (g_config == NULL)
        {
            g_config = new Config("./config/comm.json");
        }
        return g_config;
    }

    Config::Config(std::string configFile)
    {
        std::ifstream f(configFile);
        if (f.is_open())
        {
            json data = json::parse(f);
            InitConfigData(data);
        }
        else
        {
            ERRORLOG("config file %s not exist!\n", configFile);
            exit(1);
        }
        
    }

    Config::~Config()
    {
    }

    int Config::InitConfigData(json data)
    {
        data.at("LogLevel").get_to(this->m_log_level);
        // m_log_level = data[LogLevel];
        return LUT_OK;
    }

}