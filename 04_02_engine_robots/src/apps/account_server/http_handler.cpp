#include "http_handler.h"
#include "libserver/common.h"
#include "libserver/packet.h"
#include "libserver/thread_mgr.h"
#include <string>
#include <iostream>

#include "httpparser/request.h"
#include "httpparser/response.h"

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

void HttpHandler::SendHttpAns(const Json::Value& json, int socket)
{
	Json::StreamWriterBuilder wbuilder;
	wbuilder["emitUTF8"] = 1;

	httpparser::Response resp;
	resp.status = "OK";
	resp.statusCode = 200;
	resp.content = Json::writeString(wbuilder, json);
	resp.headers["Connection"] = "Close";
	resp.headers["Host"] = "wjhd.com";
	resp.headers["Content-Type"] = "application/json";

	std::string text = resp.inspect();

	Packet* packet = new Packet((Proto::MsgId)0, socket);
	if (packet->GetWriteSize() < text.size())
	{
		packet->ReAllocBuffer(text.size());
	}
	packet->write(text.c_str(), text.size());
	SendPacket(packet);
}

void HttpHandler::HandleHttpRequest(Packet* pPacket)
{
    int len = pPacket->GetDataLength();
    std::string html(pPacket->GetBuffer(), pPacket->GetReadSize());
    //std::string html(len, '\0');
    //pPacket->read((char*)html.c_str(), len);
    std::cout << "html: " << html.c_str() << std::endl;

	Json::Value json = Json::nullValue;
	json["code"] = 200;
	SendHttpAns(json, pPacket->GetSocket());
}