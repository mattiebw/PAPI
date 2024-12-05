#include "papipch.h"
#include "Network/Server.h"

Server::Server()
{
    PAPI_INFO("Creating Server");
    m_IP.Clear();
}

Server::~Server()
{
    PAPI_INFO("Destroying Server");
}
