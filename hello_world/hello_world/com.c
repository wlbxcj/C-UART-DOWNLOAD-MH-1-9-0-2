#include <stdlib.h>
#include <stdio.h>
#include <io.h>
//#include <alloc.h>
#include <fcntl.h>
#include <process.h>
#include <sys/stat.h>
#include <Windows.h>



HANDLE hCom; //全局变量，串口句柄

int config_com(void);
int open_com(void)
{
	hCom=CreateFile(TEXT("COM1"),//COM1口 
					GENERIC_READ|GENERIC_WRITE, //允许读和写 
					0, //独占方式 
					NULL, 
					OPEN_EXISTING, //打开而不是创建 
					 0, //同步方式 FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED, //异步方式打开  
					NULL); 

	printf("hCom(%d)!\r\n", hCom);
	if(hCom == INVALID_HANDLE_VALUE)
	{
		printf("打开COM失败(%d)!", GetLastError());
		return FALSE;
	}
	config_com();
	return TRUE;
}

int config_com(void)
{
	DCB dcb;
	COMMTIMEOUTS TimeOuts;//设定读超时
	
	SetupComm(hCom, 4096, 4096);//输入缓冲区和输出缓冲区的大小都是1024

	TimeOuts.ReadIntervalTimeout=1;
	TimeOuts.ReadTotalTimeoutMultiplier=1;
	TimeOuts.ReadTotalTimeoutConstant=1;//设定写超时
	TimeOuts.WriteTotalTimeoutMultiplier=500;
	TimeOuts.WriteTotalTimeoutConstant=2000;
	SetCommTimeouts(hCom,&TimeOuts);//设置超时/

	GetCommState(hCom,&dcb); 
	dcb.BaudRate=115200; //波特率为9600 
	dcb.ByteSize=8; //每个字节有8位 
	dcb.Parity=NOPARITY; //无奇偶校验位 
	dcb.StopBits=ONESTOPBIT; //1个停止位 
	SetCommState(hCom,&dcb); 
	
	PurgeComm(hCom,	PURGE_TXCLEAR|PURGE_RXCLEAR);
}

int read_com(unsigned char *buf, unsigned int read_len)
{
	int file_size = 0;
	DWORD dwBytesRead = 0;

	ReadFile(hCom, buf ,read_len, &dwBytesRead, NULL);

	return dwBytesRead;
}

int write_com(unsigned char *buf, unsigned int write_len)
{
	DWORD dwBytesWrite,dwBytesToWrite;

	dwBytesToWrite = write_len;
	do
	{                                       //循环写文件，确保完整的文件被写入  
		WriteFile(hCom ,buf, dwBytesToWrite, &dwBytesWrite, NULL);
		dwBytesToWrite -= dwBytesWrite;
		buf += dwBytesWrite;
	} while (dwBytesToWrite > 0);

	return write_len;
}

void clear_com(void)
{
	PurgeComm(hCom,	PURGE_TXCLEAR|PURGE_RXCLEAR);
}

int close_com(void)
{
	return CloseHandle(hCom);
}