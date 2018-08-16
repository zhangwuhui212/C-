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
	//��CreatePipe��CreateProcess��Createϵ�к����У�
	//ͨ������һ��SECURITY_ATTRIBUTES���͵Ĳ���
	SECURITY_ATTRIBUTES saAttr = {0};
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	//���Ѹò�����bInheritHandle����ΪTRUE��
	//�������������ľ���ɱ��ӽ��̼̳С�
	//���磬��CreatePipe�����Ĺܵ�������CreateProcess�����Ľ���
	saAttr.bInheritHandle = TRUE; 
	saAttr.lpSecurityDescriptor = NULL;

	//ChildIn_Write���ӽ��̵���������ChildIn_Read�Ǹ���������д���ӽ�������ľ��
	HANDLE ChildIn_Read, ChildIn_Write;
	CreatePipe(&ChildIn_Read, &ChildIn_Write, &saAttr, 0);
	//�����ӽ��̲��ܼ̳н�������ܵ�����һ�ˣ�ChildIn_Write
	SetHandleInformation(ChildIn_Write, HANDLE_FLAG_INHERIT, 0);
	//ChildOut_Write���ӽ��̵���������ChildOut_Read�Ǹ��������ڶ�ȡ�ӽ�������ľ��
	HANDLE ChildOut_Read, ChildOut_Write;
	CreatePipe(&ChildOut_Read, &ChildOut_Write, &saAttr, 0);
	//�����ӽ��̲��ܼ̳з�������ܵ�����һ�ˣ�ChildOut_Read
	SetHandleInformation(ChildOut_Read, HANDLE_FLAG_INHERIT, 0);

	//CreateProcess�ĵ�һ������
	STARTUPINFO StartupInfo = {0};
	GetStartupInfo(&StartupInfo);
	StartupInfo.cb = sizeof(STARTUPINFO);
	StartupInfo.dwFlags = STARTF_USESTDHANDLES   |   STARTF_USESHOWWINDOW;
	StartupInfo.wShowWindow = SW_HIDE;
	//����׼����ʹ�������������ǽ�����ChildOut_Write��
	StartupInfo.hStdError = ChildOut_Write; 
	StartupInfo.hStdOutput = ChildOut_Write;
	//����׼���붨�����ǽ�����ChildIn_Read��
	StartupInfo.hStdInput = ChildIn_Read;
	//�����ӽ��̽���StdIn�Լ�StdOut���ض���
	StartupInfo.dwFlags |= STARTF_USESTDHANDLES;

	//CreateProcess�ĵڶ�������
	PROCESS_INFORMATION  pi;   
	ZeroMemory(&pi, sizeof(pi)); 
	wchar_t cmd[20] = {0};
	wcscpy_s(cmd,L"hello.exe");
	if( !::CreateProcess(NULL, cmd, NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, NULL, &StartupInfo, &pi) )   
	{   
		int errorNo = ::GetLastError();  
		return ;   
	}
	//�����ļ����룬�����䴫�ݸ��ӽ��̵ı�׼����
	HANDLE InputFile = CreateFile(fin_in, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
	BOOL flag = FALSE;
	while (true)
	{
		char buffer[BUFSIZE] = {0};
		DWORD BytesRead, BytesWritten;
		//���ļ�����
		flag = ReadFile(InputFile, buffer, BUFSIZE, &BytesRead, NULL);
		if (!flag || (BytesRead == 0)) break;
		//������ӽ���
		flag = WriteFile(ChildIn_Write, buffer, BytesRead, &BytesWritten, NULL);
		if (!flag) break;
	}
	CloseHandle(InputFile);
	//�رո����̵�����ܵ���������������û�����룬�ӽ��̻�һֱ������
	CloseHandle(ChildIn_Write);

	//���ﲻ��Ҫ�ȴ��ӽ��̽���

	//�ر��ӽ��̵�����ܵ����������ӽ���û������������̻�һֱ������
	CloseHandle(ChildOut_Write);
	//��ȡ�ӽ��̵ı�׼����������䴫�ݸ��ļ����
	HANDLE OutputFile = CreateFile(fin_out, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	while (true)
	{
		char buffer[BUFSIZE] = {0};
		DWORD BytesRead, BytesWritten;
		//���ӽ��̶���
		flag = ReadFile(ChildOut_Read, buffer, BUFSIZE, &BytesRead, NULL);
		if (!flag || (BytesRead == 0)) break;
		//������ļ�
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

