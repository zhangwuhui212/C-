// c_read_hex.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

int _tmain(int argc, _TCHAR* argv[]) 
{ 
	char filename[] = "rtthread-stm32.hex";
	FILE *fp; 
	char buf[64];
	if((fp = fopen(filename,"r")) == NULL)
	{ 
		printf("error!"); 
		return -1; 
	} 
	
	while (!feof(fp)) 
	{
		memset(buf,0,sizeof(buf));
		fgets(buf,64,fp);
		printf("%s\n", buf);
	} 

// 	memset(buf,0,sizeof(buf));
// 	while(fscanf_s(fp,"%s",buf,64)!=EOF)
// 	{
// 		printf("%s\n", buf);
// 		memset(buf,0,sizeof(buf));
// 	}

	fclose(fp);
	getchar();
	return 0; 
}

