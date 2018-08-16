// ToHello.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

int _tmain(int argc, TCHAR* argv[])
{
	int i;
	wprintf(L"argc = %d\n", argc);

	for (i = 0; i<argc; i++)
		wprintf(L"argv[%d] = %s\n", i, argv[i]);

	TCHAR* pch = L"123";
	wprintf(L"argv[%d] = %s\n", 111, pch);
	return 0;
}

