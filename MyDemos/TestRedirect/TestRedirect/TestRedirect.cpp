// TestRedirect.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <windows.h>
#define BUFSIZE 4096

using namespace std;
void Redirect(ostream &);
BOOL RunCMD(TCHAR *pLog,TCHAR *pCmd,BOOL bReconstruct);
void CreateRedirect(
	);

int _tmain(int argc, _TCHAR* argv[])
{

	CreateRedirect();
	return 0;
} 


void CreateRedirect(
	)
{
	LPWSTR fin_in = L"my_in.txt";
	LPWSTR fin_out = L"my_out.txt";
// 	cout << "start....." << endl;
// 	wcout <<fin_in<< endl;
// 	wcout <<fin_out<< endl;
	// 	wcout <<argv[1]<< endl;
	// 	wcout <<argv[2]<< endl;
	//在CreatePipe、CreateProcess等Create系列函数中，
	//通常都有一个SECURITY_ATTRIBUTES类型的参数
	SECURITY_ATTRIBUTES saAttr = {0};
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	//若把该参数的bInheritHandle项设为TRUE，
	//则它创建出来的句柄可被子进程继承。
	//例如，用CreatePipe创建的管道可用于CreateProcess创建的进程
	saAttr.bInheritHandle = TRUE; 
	saAttr.lpSecurityDescriptor = NULL;

	//ChildIn_Write是子进程的输入句柄，ChildIn_Read是父进程用于写入子进程输入的句柄
	HANDLE ChildIn_Read, ChildIn_Write;
	CreatePipe(&ChildIn_Read, &ChildIn_Write, &saAttr, 0);
	//设置子进程不能继承接收输入管道的另一端：ChildIn_Write
	SetHandleInformation(ChildIn_Write, HANDLE_FLAG_INHERIT, 0);
	//ChildOut_Write是子进程的输出句柄，ChildOut_Read是父进程用于读取子进程输出的句柄
	HANDLE ChildOut_Read, ChildOut_Write;
	CreatePipe(&ChildOut_Read, &ChildOut_Write, &saAttr, 0);
	//设置子进程不能继承发送输出管道的另一端：ChildOut_Read
	SetHandleInformation(ChildOut_Read, HANDLE_FLAG_INHERIT, 0);

	//CreateProcess的第一个参数
	STARTUPINFO StartupInfo = {0};
	GetStartupInfo(&StartupInfo);
	StartupInfo.cb = sizeof(STARTUPINFO);
	StartupInfo.dwFlags = STARTF_USESTDHANDLES   |   STARTF_USESHOWWINDOW;
	StartupInfo.wShowWindow = SW_HIDE;
	//将标准输出和错误输出定向到我们建立的ChildOut_Write上
	StartupInfo.hStdError = ChildOut_Write; 
	StartupInfo.hStdOutput = ChildOut_Write;
	//将标准输入定向到我们建立的ChildIn_Read上
	StartupInfo.hStdInput = ChildIn_Read;
	//设置子进程接受StdIn以及StdOut的重定向
	StartupInfo.dwFlags |= STARTF_USESTDHANDLES;

	//CreateProcess的第二个参数
	PROCESS_INFORMATION  pi;   
	ZeroMemory(&pi, sizeof(pi)); 
	wchar_t cmd[20] = {0};
	wcscpy_s(cmd,L"hello.exe");
	if( !::CreateProcess(NULL, cmd, NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, NULL, &StartupInfo, &pi) )   
	{   
		int errorNo = ::GetLastError();  
		return ;   
	}
	//读入文件输入，并将其传递给子进程的标准输入
	HANDLE InputFile = CreateFile(fin_in, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
	BOOL flag = FALSE;
	while (true)
	{
		char buffer[BUFSIZE] = {0};
		DWORD BytesRead, BytesWritten;
		//从文件读入
		flag = ReadFile(InputFile, buffer, BUFSIZE, &BytesRead, NULL);
		if (!flag || (BytesRead == 0)) break;
		//输出到子进程
		flag = WriteFile(ChildIn_Write, buffer, BytesRead, &BytesWritten, NULL);
		if (!flag) break;
	}
	CloseHandle(InputFile);
	//关闭父进程的输入管道，否则若父进程没有输入，子进程会一直被挂起
	CloseHandle(ChildIn_Write);

	//这里不需要等待子进程结束

	//关闭子进程的输出管道，否则若子进程没有输出，父进程会一直被挂起
	CloseHandle(ChildOut_Write);
	//读取子进程的标准输出，并将其传递给文件输出
	HANDLE OutputFile = CreateFile(fin_out, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	while (true)
	{
		char buffer[BUFSIZE] = {0};
		DWORD BytesRead, BytesWritten;
		//从子进程读入
		flag = ReadFile(ChildOut_Read, buffer, BUFSIZE, &BytesRead, NULL);
		if (!flag || (BytesRead == 0)) break;
		//输出到文件
		flag = WriteFile(OutputFile, buffer, BytesRead, &BytesWritten, NULL);
		if (!flag) break;
	}
	CloseHandle(OutputFile);
}
// cout << "the first row" << endl;
// Redirect(cout);
// cout << "the last row" << endl;
void Redirect(ostream &strm)
{
	ofstream file("redirect.txt");
	streambuf *strm_buffer = strm.rdbuf();

	strm.rdbuf(file.rdbuf());

	file << "one row for the file" << endl;
	strm << "one row for the stream" << endl;

	strm.rdbuf(strm_buffer);
}

