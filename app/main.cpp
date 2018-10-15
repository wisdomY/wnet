#ifdef WIN32
#include <WinSock2.h>
#include <stdio.h>
#include "MiniDumper.h"
#endif
#include <stdio.h>

int main(int argc, char * argv[] ) {
#ifdef WIN32
    WORD wVersionRequested;
    WSADATA wsaData;
    CMiniDumper dumper(true);
    int err;
    wVersionRequested = MAKEWORD(2, 2);
    err = WSAStartup(wVersionRequested, &wsaData);
#endif

    uint32_t threadNum = 4;
    CAppManager app();
    
    if (!app.Init()) {
        std::cout << "CLightControlApp Init failed." << std::endl;
        return -1;
    }
    else if (!app.Start()) {    //loop
        std::cout << "CLightControlApp Start failed." << std::endl;
        return -1;
    }

    if (!app.Stop()) {
        std::cout << "CLightControlApp Stop unexpected." << std::endl;
        return -1;
    }

    std::cout << "CLightControlApp exits successfully." << std::endl;
    return 0;
}
