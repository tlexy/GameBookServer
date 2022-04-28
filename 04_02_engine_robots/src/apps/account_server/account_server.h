#pragma once
#include "libserver/server_app.h"

class AccountServerApp : public ServerApp {
public:
    explicit AccountServerApp() : ServerApp(APP_TYPE::APP_ACCOUNT) { }
    void InitApp() override;
};