#pragma once
#include "libserver/thread_obj.h"

class HttpHandler :public ThreadObject
{
public:
	bool Init() override;
	void RegisterMsgFunction() override;
	void Update() override;

private:
	void HandleHttpRequest(Packet* pPacket);

private:
};