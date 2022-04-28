#pragma once

#include "network.h"

class NetworkListen :public Network
{
public:
    NetworkListen(NETWORK_TYPE type = NETWORK_BASE);
    bool Init() override;
    bool Listen(std::string ip, int port);
    void Update() override;

protected:
    virtual int Accept();
};
