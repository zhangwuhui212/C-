#pragma once

typedef enum MyMsgTypeID
{
	My_Msg_Type_Msg,
	My_Msg_Type_Push_File,
	My_Msg_Type_Push_File_Ack,
	My_Msg_Type_Push_File_Progress,
	My_Msg_Type_Push_File_End,
	My_Msg_Type_Push_File_End_Ack,
};

typedef enum MyUploadFileTypeID
{
	My_UploadFile_Type_TXT,
	My_UploadFile_Type_PDF,
	My_UploadFile_Type_AUDIO,
	My_UploadFile_Type_VEDIO,
};

typedef struct MyNode
{
	UINT addr;
	UINT port;
}MYNODE;

typedef struct MyUploadFile
{
	UINT type;
	char name[256];
	ULONGLONG len;
}MYUPLOADFILE;

typedef struct MyMsg
{
	UINT id;
	ULONGLONG len;
	UINT error;
	char buf1[256];
	char buf2[256];
}MYMSG;


