// TestMD5.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "md5.h"

int _tmain(int argc, _TCHAR* argv[])
{
	unsigned char hexMd5[64] = {0};
	unsigned char md5[32] = {0};

	MDString("nihao!Œ“Ω–’≈Œ‰ª‘£°",md5);
	MD5ToString(md5,hexMd5);

	MD5File("D:\\text.txt",md5); 
	MD5ToString(md5,hexMd5);
	return 0;
}

