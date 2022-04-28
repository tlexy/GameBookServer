#include "http_handler.h"
#include "libserver/common.h"
#include "libserver/packet.h"
#include "libserver/thread_mgr.h"
#include <string>
#include <iostream>

bool HttpHandler::Init()
{
    return true;
}

void HttpHandler::RegisterMsgFunction()
{
    auto pMsgCallBack = new MessageCallBackFunction();
    AttachCallBackHandler(pMsgCallBack);

    pMsgCallBack->RegisterFunction(Proto::MsgId::L2HTTP_ACCOUNT_VERIFY, BindFunP1(this, &HttpHandler::HandleHttpRequest));
}

void HttpHandler::Update()
{

}

void HttpHandler::HandleHttpRequest(Packet* pPacket)
{
    int len = pPacket->GetDataLength();
    std::string html(pPacket->GetBuffer(), pPacket->GetReadSize());
    //std::string html(len, '\0');
    //pPacket->read((char*)html.c_str(), len);
    std::cout << "html: " << html.c_str() << std::endl;
}