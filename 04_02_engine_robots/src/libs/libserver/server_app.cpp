#include "common.h"
#include "server_app.h"
#include "network_listen.h"

ServerApp::ServerApp(APP_TYPE  appType)
{
    _appType = appType;

    Global::Instance();
    ThreadMgr::Instance();
    _pThreadMgr = ThreadMgr::GetInstance();
    UpdateTime();

    // �����߳�
    for (int i = 0; i < 3; i++)
    {
        _pThreadMgr->NewThread();
    }
}

ServerApp::~ServerApp()
{
    _pThreadMgr->DestroyInstance();
}

void ServerApp::Dispose()
{
    _pThreadMgr->Dispose();
}

void ServerApp::StartAllThread() const
{
    _pThreadMgr->StartAllThread();
}

void ServerApp::Run() const
{
    bool isRun = true;
    while (isRun)
    {
        UpdateTime();        
        _pThreadMgr->Update();
        isRun = _pThreadMgr->IsGameLoop();
    }
}

void ServerApp::UpdateTime() const
{
    auto timeValue = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    Global::GetInstance()->TimeTick = timeValue.time_since_epoch().count();

#if ENGINE_PLATFORM != PLATFORM_WIN32
    auto tt = std::chrono::system_clock::to_time_t(timeValue);
    struct tm* ptm = localtime(&tt);
    Global::GetInstance()->YearDay = ptm->tm_yday;
#else
    auto tt = std::chrono::system_clock::to_time_t(timeValue);
    struct tm tm;
    localtime_s(&tm, &tt);
    Global::GetInstance()->YearDay = tm.tm_yday;
#endif
}

bool ServerApp::AddListenerToThread(std::string ip, int port) const
{
    NETWORK_TYPE ntype = NETWORK_BASE;
    if (_appType == APP_ACCOUNT)
    {
        ntype = NETWORK_HTTP;
    }
    NetworkListen* pListener = new NetworkListen(ntype);
    if (!pListener->Listen(ip, port))
    {
        delete pListener;
        return false;
    }

    _pThreadMgr->AddNetworkToThread(APP_Listen, pListener);
    return true;
}
