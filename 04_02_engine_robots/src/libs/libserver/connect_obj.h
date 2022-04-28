#pragma once
#include <memory>

#include "disposable.h"
#include "network.h"
#include "time.h"

class RecvNetworkBuffer;
class SendNetworkBuffer;
class Packet;

#define PingTime 1000 // 1秒
#define PingDelayTime  10 * 1000 // 10秒

class ConnectObj : public IDisposable
{
public:
    ConnectObj(Network* pNetWork, SOCKET socket);
    ~ConnectObj() override;

    void Dispose() override;
    void Close();	// 逻辑层发起的关闭
    bool IsClose() const;

    SOCKET GetSocket() const { return _socket; }
    bool HasRecvData() const;
    virtual Packet* GetRecvPacket() const;
    virtual bool Recv() const;

    bool HasSendData() const;
    virtual void SendPacket(Packet* pPacket) const;
    virtual bool Send() const;

protected:
    Network* _pNetWork{ nullptr };
    const SOCKET _socket;
    RecvNetworkBuffer* _recvBuffer{ nullptr };
    SendNetworkBuffer* _sendBuffer{ nullptr };
    bool _isClose{ false };
};

class HttpConnectObj : public ConnectObj
{
public:
    HttpConnectObj(Network* pNetWork, SOCKET socket);

    virtual Packet* GetRecvPacket() const;
    virtual bool Recv() const;

    virtual void SendPacket(Packet* pPacket) const;
    //virtual bool Send() const;
};

