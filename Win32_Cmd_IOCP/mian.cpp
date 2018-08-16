// IOCPTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "IOCP_TCP.h"
#include <iostream>

int _tmain(int argc, _TCHAR* argv[])
{

    CCriticalSection *p = new CCriticalSection;
    delete p;

    CompletionPort * cp = new CompletionPort();

    cp->Init("192.168.31.27", 4567);           //需要修改为自己的ip
    cp->Active();

    char buff[1024]={0};
    char recBuff[1024]={0};
    int len;
    while (true)
    {
        scanf_s("%s", buff, 1023);
        len = strlen(buff);
        cp->SendData(buff, len, "192.168.31.27");
        ZeroMemory(recBuff,1024);
        cp->ReceiveData(recBuff, 1024, "192.168.31.27");     //客户端的ip
        printf("\n%s\n", recBuff);
    }
    delete cp;
    return 0;
}
