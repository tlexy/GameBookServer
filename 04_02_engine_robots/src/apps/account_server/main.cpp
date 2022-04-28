#include "account_server.h"
#include "libserver/server_app.h"

#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "Iphlpapi.lib")
#pragma comment (lib, "Psapi.lib")
#pragma comment (lib, "Userenv.lib")

int main(int argc, char* argv[])
{
	return MainTemplate<AccountServerApp>();
}