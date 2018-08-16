#pragma once

#define TFTP_RRQ   1   /*读请求 (RRQ)*/
#define TFTP_WRQ   2   /*写请求 (WRQ)*/
#define TFTP_DATA  3   /*数据  (DATA)*/
#define TFTP_ACK   4   /*ACK    (ACK)*/
#define TFTP_ERROR 5   /*Error(ERROR)*/


#define TFTP_WSTAT_FIRSTACK		0
#define TFTP_WSTAT_NEXTACK		1
#define TFTP_WSTAT_LASTACK		2

#define MAX_RETRY		3             	
#define TFTP_NOTEND_DATALEN		512+2+2	

#define Not_defined					0
#define File_not_found				1
#define Access_violation			2
#define Disk_full					3
#define Illegal_TFTP_operation		4
#define Unknown_port				5
#define File_already_exists			6
#define No_such_user				7
#define Time_out					8
//#define Read_file_Error			9
//#define Cannot_create_file		10
//#define Sender_Abort				11
#define MIN_TIMEOUT         1
#define MAX_TIMEOUT         255
#define DEF_TIMEOUT         5


#define IS_TIMEOUT(t) ((MIN_TIMEOUT < t)  && (t < MAX_TIMEOUT))

#define MIN_BLKSIZE			8
#define MAX_BLKSIZE			65464
#define DEF_BLKSIZE			1428


#define IS_BLKSIZE(t) ((MIN_BLKSIZE < t)  && (t < MAX_BLKSIZE))


typedef struct tftp_info
{
	char            filename[256];
	unsigned int    filelen;
	unsigned int    fileoffset;
	unsigned int    rlen;

	unsigned char	opcode;
	unsigned char	state;
	unsigned short	blocknum;
	unsigned short	lastblocknum;

	char		    msg[1024];
	unsigned int	msglen;

	DWORD			tc;
}TftpInfo;

//RRQ/WRQ packet
// 2 bytes		string		 1 byte    string    1 byte
//|	opcode	|	filename	|	0	|	mode	|	0	|
static int makereq(char *buffer, char opcode, char *filename, char * mode)
{
	int pos = 0;
	unsigned int i = 0;
	char *s = "octet";

	buffer[pos] = 0;
	pos++;
	buffer[pos] = opcode;
	pos++;

	for(i=0;i<strlen(filename);i++){  
		buffer[pos] = filename[i];
		pos++;
	}
	buffer[pos] = 0;   
	pos++;
	for(i=0;i<strlen(s);i++){  
		buffer[pos] = s[i];
		pos++;
	}
	buffer[pos] = 0;
	pos++;
	return pos;  
}

static int makeextreq(char *buffer, char opcode, char *filename, char * mode,int timeout,int blksize)
{
	int pos = 0;
	unsigned int i = 0;
	char *s = "octet";

	buffer[pos] = 0;
	pos++;
	buffer[pos] = opcode;
	pos++;

	for(i=0;i<strlen(filename);i++){  
		buffer[pos] = filename[i];
		pos++;
	}
	buffer[pos] = 0;   
	pos++;
	for(i=0;i<strlen(s);i++){  
		buffer[pos] = s[i];
		pos++;
	}
	buffer[pos] = 0;
	pos++;
	pos += sprintf((char *)(buffer + pos),"timeout%c%d%c",0,timeout,0);
	pos += sprintf((char *)(buffer + pos),"blksize%c%d%c",0,blksize,0);
	pos++;
	return pos;  
} 

//data packet
// 2 bytes     2 bytes	   n bytes
//|	opcode	|	block	|	data	|
static int makedata( char *buffer, unsigned short block, char *data, int datasize)
{
	int pos = 0;
	unsigned int i = 0;
	buffer[pos] = 0;
	pos++;
	buffer[pos] = TFTP_DATA;//opcode=3
	pos++;
	buffer[pos] = (char)(block>>8);
	pos++;
	buffer[pos] = (char)block;
	pos++;
	for(i=0;i<datasize;i++){  
		buffer[pos] = data[i];
		pos++;
	}

	return pos;
}

//ack packet
// 2 bytes		2bytes
//|	opcode	|	block	|
static unsigned int makeack( char *buffer, unsigned short block)
{
	int pos = 0;
	buffer[pos] = 0;
	pos++;
	buffer[pos] = TFTP_ACK;    
	pos++;
	buffer[pos] = (char)(block >> 8);
	pos++;
	buffer[pos] = (char)block;
	pos++;
	return pos;
}

//ack packet
// 2 bytes		2bytes
//|	opcode	|	block	|
static unsigned int makeoack( char *buffer, unsigned short block)
{
	int pos = 0;
	buffer[pos] = 0;
	pos++;
	buffer[pos] = TFTP_ACK;    
	pos++;
	buffer[pos] = (char)(block >> 8);
	pos++;
	buffer[pos] = (char)block;
	pos++;
	return pos;
}

//error pakcet 
// 2 bytes      2 bytes			string       1 byte
//|	opcode	|	errorcode	|	errormsg	|	0	|
static int makeerr(char * buffer,unsigned short errorcode,char *errormsg)
{
	int pos=0;
	unsigned int i = 0;
	buffer[pos]=0;
	pos++;
	buffer[pos]=TFTP_ERROR;  //5
	pos++;
	buffer[pos] = (char)(errorcode>>8);
	pos++;
	buffer[pos] = (char)(errorcode);
	pos++;
	for(i=0;i<strlen(errormsg);i++){  
		buffer[pos] = errormsg[i];
		pos++;
	}
	buffer[pos] = 0;
	pos++;
	return pos;
}

static int makewaitmsg(char * buffer,int wait_time)
{
	char errormsg[16] = "";
	sprintf(errormsg,"WAIT;%d",wait_time);
	return makeerr(buffer,TFTP_ERROR,errormsg);
}

static int makeabortmsg(char * buffer,int abort_id)
{
	char errormsg[16] = "";
	sprintf(errormsg,"ABORT;%04x",abort_id);
	return makeerr(buffer,TFTP_ERROR,errormsg);
}