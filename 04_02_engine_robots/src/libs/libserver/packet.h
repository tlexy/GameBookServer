#pragma once

#include "base_buffer.h"
#include "common.h"

#pragma pack(push)
#pragma pack(4)

struct PacketHead {
    unsigned short MsgId;
};

#pragma pack(pop)

#if TestNetwork
#define DEFAULT_PACKET_BUFFER_SIZE	10
#else
// Ĭ�ϴ�С 10KB
#define DEFAULT_PACKET_BUFFER_SIZE	1024 * 10
#endif

class Packet : public Buffer {
public:
    Packet(const Proto::MsgId msgId, SOCKET socket);
    ~Packet();

    template<class ProtoClass>
    ProtoClass ParseToProto()
    {
        ProtoClass proto;
        proto.ParsePartialFromArray(GetBuffer(), GetDataLength());
        return proto;
    }

    template<class ProtoClass>
    void SerializeToBuffer(ProtoClass& protoClase)
    {
        auto total = protoClase.ByteSizeLong();
        if (GetEmptySize() < total)
        {
            ReAllocBuffer(total);
        }

        protoClase.SerializePartialToArray(GetBuffer(), total);
        FillData(total);
    }

    void Dispose() override;
    void CleanBuffer();

    char* GetBuffer() const;
    unsigned short GetDataLength() const;
    int GetMsgId() const;
    void SetMsgId(Proto::MsgId id);
    //void FillData(unsigned int size);
    //void ReAllocBuffer();
    SOCKET GetSocket() const;

private:
    Proto::MsgId _msgId;
    SOCKET _socket;
};
