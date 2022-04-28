#pragma once
#include "libserver/thread_obj.h"
#include <json/json.h>

class HttpHandler :public ThreadObject
{
public:
	bool Init() override;
	void RegisterMsgFunction() override;
	void Update() override;

private:
	void HandleHttpRequest(Packet* pPacket);

	void SendHttpAns(const Json::Value& json, int socket);

private:
};