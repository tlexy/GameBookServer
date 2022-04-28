#include "packet.h"
#include "network_buffer.h"
#include "connect_obj.h"

#include <cstdlib>
#include <cstring>

NetworkBuffer::NetworkBuffer(const unsigned size, ConnectObj* pConnectObj)
    :Buffer(size)
{
    _pConnectObj = pConnectObj;
}

NetworkBuffer::~NetworkBuffer()
{
}

bool NetworkBuffer::HasData() const
{
    return GetReadSize() > 0;
}

//unsigned int NetworkBuffer::GetEmptySize()
//{
//    return _bufferSize - _dataSize;
//}

//unsigned int NetworkBuffer::GetWriteSize() const
//{
//    if (_beginIndex <= _endIndex)
//    {
//        return _bufferSize - _endIndex;
//    }
//    else
//    {
//        return _beginIndex - _endIndex;
//    }
//}
//
//unsigned int NetworkBuffer::GetReadSize() const
//{
//    if (_dataSize <= 0)
//        return 0;
//
//    if (_beginIndex < _endIndex)
//    {
//        return _endIndex - _beginIndex;
//    }
//    else
//    {
//        return _bufferSize - _beginIndex;
//    }
//}

//void NetworkBuffer::FillData(unsigned int  size)
//{
//    _endIndex += size;
//    if (_endIndex >= _bufferSize)
//    {
//        _endIndex = _bufferSize - 1;//下标的最大值
//    }
//}

//void NetworkBuffer::RemoveDate(unsigned int size)
//{
//    _dataSize -= size;
//
//    if ((_beginIndex + size) >= _bufferSize)
//    {
//        size -= _bufferSize - _beginIndex;
//        _beginIndex = 0;
//    }
//
//    _beginIndex += size;
//}

//void NetworkBuffer::ReAllocBuffer()
//{
//    Buffer::ReAllocBuffer(_dataSize);
//}

Packet* NetworkBuffer::GetRawPacket()
{
    if (GetReadSize() > 0)
    {
        const auto socket = _pConnectObj->GetSocket();
        Packet* pPacket = new Packet((Proto::MsgId)0, socket);
        const auto dataLength = GetReadSize();
        while (pPacket->GetTotalSize() < dataLength)
        {
            pPacket->ReAllocBuffer(dataLength);
        }

        read(pPacket->GetBuffer(), dataLength);
        pPacket->FillData(dataLength);

        return pPacket;
    }
    else
    {
        return nullptr;
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////

RecvNetworkBuffer::RecvNetworkBuffer(const unsigned int size, ConnectObj* pConnectObj) : NetworkBuffer(size, pConnectObj) {

}

void RecvNetworkBuffer::Dispose() {

}

//int RecvNetworkBuffer::GetBuffer(char*& pBuffer) const
//{
//    pBuffer = _buffer + _endIndex;
//    return GetWriteSize();
//}

int RecvNetworkBuffer::GetBuffer(char*& pBuffer) const
{
    pBuffer = _buffer + _endIndex;
    return GetWriteSize();
}

Packet* RecvNetworkBuffer::GetPacket()
{
    // 数据长度不够
    if (GetReadSize() < sizeof(TotalSizeType))
    {
        return nullptr;
    }

    // 1.读出 整体长度
    unsigned short totalSize;
    MemcpyFromBuffer(reinterpret_cast<char*>(&totalSize), sizeof(TotalSizeType));

    // 协议体长度不够，等待
    if (GetReadSize() < totalSize)
    {
        return nullptr;
    }

    RemoveData(sizeof(TotalSizeType));

    // 2.读出 PacketHead
    PacketHead head;
    MemcpyFromBuffer(reinterpret_cast<char*>(&head), sizeof(PacketHead));
    RemoveData(sizeof(PacketHead));

    // 3.读出 协议
    // 检查一下id
    const google::protobuf::EnumDescriptor *descriptor = Proto::MsgId_descriptor();
    if (descriptor->FindValueByNumber(head.MsgId) == nullptr)
    {
        // 关闭网络
        _pConnectObj->Close();
        std::cout << "recv invalid msg." << std::endl;
        return nullptr;
    }

    const auto socket = _pConnectObj->GetSocket();
    Packet* pPacket = new Packet((Proto::MsgId)head.MsgId, socket);
    const auto dataLength = totalSize - sizeof(PacketHead) - sizeof(TotalSizeType);
    if (pPacket->GetTotalSize() < dataLength)
    {
        pPacket->ReAllocBuffer(dataLength);
    }

    MemcpyFromBuffer(pPacket->GetBuffer(), dataLength);
    pPacket->FillData(dataLength);
    RemoveData(dataLength);

    return pPacket;
}

void RecvNetworkBuffer::MemcpyFromBuffer(char* pVoid, const unsigned int size)
{
    read(pVoid, size);
}


///////////////////////////////////////////////////////////////////////////////////////////////

SendNetworkBuffer::SendNetworkBuffer(const unsigned int size, ConnectObj* pConnectObj) : NetworkBuffer(size, pConnectObj)
{

}

void SendNetworkBuffer::Dispose()
{

}

int SendNetworkBuffer::GetBuffer(char*& pBuffer) const
{
    pBuffer = _buffer + _beginIndex;
    return GetReadSize();
}

void SendNetworkBuffer::AddPacket(Packet* pPacket)
{
    const auto dataLength = pPacket->GetDataLength();
    TotalSizeType totalSize = dataLength + sizeof(PacketHead) + sizeof(TotalSizeType);

    // 长度不够，扩容
    if (GetEmptySize() < totalSize) 
    {
        ReAllocBuffer(totalSize);
    }
    if (GetWriteSize() < totalSize)
    {
        ReArrangeBuffer(totalSize);
    }
    if (GetWriteSize() < totalSize)
    {
        std::cout << "SendNetworkBuffer::AddPacket failed, reason 1" << std::endl;
        return;
    }
    //std::cout << "send buffer::Realloc. _bufferSize:" << _bufferSize << std::endl;

    // 1.整体长度
    MemcpyToBuffer(reinterpret_cast<char*>(&totalSize), sizeof(TotalSizeType));

    // 2.头部
    PacketHead head;
    head.MsgId = pPacket->GetMsgId();
    MemcpyToBuffer(reinterpret_cast<char*>(&head), sizeof(PacketHead));

    // 3.数据
    MemcpyToBuffer(pPacket->GetBuffer(), pPacket->GetDataLength());
}

void SendNetworkBuffer::MemcpyToBuffer(char* pVoid, const unsigned int size)
{
    write(pVoid, size);
}
