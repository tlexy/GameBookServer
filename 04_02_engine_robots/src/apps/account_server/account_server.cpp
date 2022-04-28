#include "account_server.h"
#include "http_handler.h"

void AccountServerApp::InitApp()
{
	AddListenerToThread("127.0.0.1", 8088);

	HttpHandler* http_handler = new HttpHandler();
	_pThreadMgr->AddObjToThread(http_handler);
}