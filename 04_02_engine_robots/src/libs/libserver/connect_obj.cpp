#include "connect_obj.h"

#include "network.h"
#include "network_buffer.h"

#include "packet.h"
#include "thread_mgr.h"

ConnectObj::ConnectObj(Network* pNetWork, SOCKET socket) :_pNetWork(pNetWork), _socket(socket)
{
    _recvBuffer = new RecvNetworkBuffer(DEFAULT_RECV_BUFFER_SIZE, this);
    _sendBuffer = new SendNetworkBuffer(DEFAULT_SEND_BUFFER_SIZE, this);
}

ConnectObj::~ConnectObj()
{
    if (_recvBuffer != nullptr)
        delete _recvBuffer;

    if (_sendBuffer != nullptr)
        delete _sendBuffer;
}

void ConnectObj::Dispose()
{
    //std::cout << "close socket:" << _socket << std::endl;
    _sock_close(_socket);

    _recvBuffer->Dispose();
    _sendBuffer->Dispose();
}

void ConnectObj::Close()
{
    _isClose = true;
}

bool ConnectObj::IsClose() const
{
    return _isClose;
}

bool ConnectObj::HasRecvData() const
{
    return _recvBuffer->HasData();
}

Packet* ConnectObj::GetRecvPacket() const
{
    return _recvBuffer->GetPacket();
}

bool ConnectObj::Recv() const
{
    bool isRs = false;
    char *pBuffer = nullptr;
    while (true)
    {
        // 总空间数据不足一个头的大小，扩容
        if (_recvBuffer->GetEmptySize() < (sizeof(PacketHead) + sizeof(TotalSizeType)))
        {
            _recvBuffer->ReAllocBuffer(_recvBuffer->GetTotalSize() + 1024);
        }

        const int emptySize = _recvBuffer->GetBuffer(pBuffer);
        const int dataSize = ::recv(_socket, pBuffer, emptySize, 0);
        if (dataSize > 0)
        {
            //std::cout << "recv size:" << size << std::endl;
            _recvBuffer->FillData(dataSize);
        }
        else if (dataSize == 0)
        {
            //std::cout << "recv size:" << dataSize << " error:" << _sock_err() << std::endl;
            break;
        }
        else
        {
            const auto socketError = _sock_err();
#ifndef WIN32
            if (socketError == EINTR || socketError == EWOULDBLOCK || socketError == EAGAIN)
            {
                isRs = true;
            }
#else
            if (socketError == WSAEINTR || socketError == WSAEWOULDBLOCK)
            {
                isRs = true;
            }
#endif

            //std::cout << "recv size:" << dataSize << " error:" << socketError << std::endl;
            break;
        }
    }

    if (isRs)
    {
        while (true)
        {
            const auto pPacket = _recvBuffer->GetPacket();
            if (pPacket == nullptr)
                break;

            //const google::protobuf::EnumDescriptor *descriptor = Proto::MsgId_descriptor();
            //auto name = descriptor->FindValueByNumber(pPacket->GetMsgId())->name();
            //std::cout << "recv msg:" << name.c_str() << std::endl;

            if (pPacket->GetMsgId() == Proto::MsgId::MI_Ping)
            {
                //RecvPing();
            }
            else
            {
                if (_pNetWork->IsBroadcast() && _pNetWork->GetThread() != nullptr)
                {
                    ThreadMgr::GetInstance()->DispatchPacket(pPacket);
                }
                else
                {
                    _pNetWork->GetThread()->AddPacketToList(pPacket);
                }
            }
        }
    }

    return isRs;
}

bool ConnectObj::HasSendData() const
{
    return _sendBuffer->HasData();
}

void ConnectObj::SendPacket(Packet* pPacket) const
{
    //const google::protobuf::EnumDescriptor *descriptor = Proto::MsgId_descriptor();
    //auto name = descriptor->FindValueByNumber(pPacket->GetMsgId())->name();
    //std::cout << "send msg:" << name.c_str() << std::endl;

    _sendBuffer->AddPacket(pPacket);
}

bool ConnectObj::Send() const
{
    while (true) {
        char *pBuffer = nullptr;
        const int needSendSize = _sendBuffer->GetBuffer(pBuffer);

        // 没有数据可发送
        if (needSendSize <= 0)
        {
            return true;
        }

        const int size = ::send(_socket, pBuffer, needSendSize, 0);
        if (size > 0)
        {
            _sendBuffer->RemoveData(size);

            // 下一帧再发送
            if (size < needSendSize)
            {
                return true;
            }
        }

        if (size <= 0)
        {
            const auto socketError = _sock_err();
            std::cout << "needSendSize:" << needSendSize << " error:" << socketError << std::endl;
            return false;
        }
    }
}


///////////////////////////////////////////HttpConnectObj

HttpConnectObj::HttpConnectObj(Network* pNetWork, SOCKET socket)
    :ConnectObj(pNetWork, socket)
{}

Packet* HttpConnectObj::GetRecvPacket() const
{
    return _recvBuffer->GetRawPacket();
}

bool HttpConnectObj::Recv() const
{
    bool isRs = false;
    char* pBuffer = nullptr;
    while (true)
    {
        // 总空间数据不足一个头的大小，扩容
        if (_recvBuffer->GetEmptySize() < (sizeof(PacketHead) + sizeof(TotalSizeType)))
        {
            _recvBuffer->ReAllocBuffer(_recvBuffer->GetTotalSize() + 1024);
        }

        const int emptySize = _recvBuffer->GetBuffer(pBuffer);
        const int dataSize = ::recv(_socket, pBuffer, emptySize, 0);
        if (dataSize > 0)
        {
            //std::cout << "recv size:" << size << std::endl;
            _recvBuffer->FillData(dataSize);
        }
        else if (dataSize == 0)
        {
            //std::cout << "recv size:" << dataSize << " error:" << _sock_err() << std::endl;
            break;
        }
        else
        {
            const auto socketError = _sock_err();
#ifndef WIN32
            if (socketError == EINTR || socketError == EWOULDBLOCK || socketError == EAGAIN)
            {
                isRs = true;
            }
#else
            if (socketError == WSAEINTR || socketError == WSAEWOULDBLOCK)
            {
                isRs = true;
            }
#endif

            //std::cout << "recv size:" << dataSize << " error:" << socketError << std::endl;
            break;
        }
    }

    if (isRs)
    {
        while (true)
        {
            auto pPacket = _recvBuffer->GetRawPacket();
            if (pPacket == nullptr)
            {
                break;
            }
            pPacket->SetMsgId(Proto::MsgId::L2HTTP_ACCOUNT_VERIFY);

            if (_pNetWork->IsBroadcast() && _pNetWork->GetThread() != nullptr)
            {
                ThreadMgr::GetInstance()->DispatchPacket(pPacket);
            }
            else
            {
                _pNetWork->GetThread()->AddPacketToList(pPacket);
            }
        }
    }

    return isRs;
}

void HttpConnectObj::SendPacket(Packet* pPacket) const
{
    _sendBuffer->write(pPacket->GetBuffer(), pPacket->GetDataLength());
}

//bool HttpConnectObj::Send() const
//{
//    return false;
//}
