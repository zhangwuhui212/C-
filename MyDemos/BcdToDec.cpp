// TestWin32.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <list>
#include <iostream>
using namespace std;

void DecToBCD(char *dec,char * bcd)
{
	int m = 0;
	char *p = dec;
	//cout << "输入一个十进制数:\n";
	//cin >> p;
	//cout << "8421码:";
	while(*p != '\0')
	{
		m = *p - 48;
		switch(m)
		{
		case 9:
			strcat(bcd,"1001");
			break;
		case 8:
			strcat(bcd,"1000");
			break;
		case 7:
			strcat(bcd,"0111");
			break;
		case 6:
			strcat(bcd,"0110");
			break;
		case 5:
			strcat(bcd,"0101");
			break;
		case 4:
			strcat(bcd,"0100");
			break;
		case 3:
			strcat(bcd,"0011");
			break;
		case 2:
			strcat(bcd,"0010");
			break;
		case 1:
			strcat(bcd,"0001");
			break;
		case 0:
			strcat(bcd,"0000");
			break;
		}
		p++;
	}
}

void BCDToDec(char * bcd,char *dec)
{
	char a[50],*p;
	int n;
	p=bcd;
	n=0;
	//cout<<"请输入8421码：\n";
	//cin>>a;
	while (*p != '\0')
	{
		n=(*p)-48;
		n=n*8;
		p++;
		n+=(*p-48)*4;
		p++;
		n+=(*p-48)*2;
		p++;
		n+=(*p-48)*1;
		p++;
		switch(n)
		{
		case 9:
			strcat(dec,"9");
			break;
		case 8:
			strcat(dec,"8");
			break;
		case 7:
			strcat(dec,"7");
			break;
		case 6:
			strcat(dec,"6");
			break;
		case 5:
			strcat(dec,"5");
			break;
		case 4:
			strcat(dec,"4");
			break;
		case 3:
			strcat(dec,"3");
			break;
		case 2:
			strcat(dec,"2");
			break;
		case 1:
			strcat(dec,"1");
			break;
		case 0:
			strcat(dec,"0");
			break;
		}
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
		char bcd[32] = {0};
		char dec[16] = "123";
		DecToBCD(dec,bcd);
		char dec1[16] = "";
		BCDToDec(bcd,dec1);
		return 0;
}

