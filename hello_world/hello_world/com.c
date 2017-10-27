#include <stdlib.h>
#include <stdio.h>
#include <io.h>
//#include <alloc.h>
#include <fcntl.h>
#include <process.h>
#include <sys/stat.h>
#include <Windows.h>



HANDLE hCom; //ȫ�ֱ��������ھ��

int config_com(void);
int open_com(void)
{
	hCom=CreateFile(TEXT("COM1"),//COM1�� 
					GENERIC_READ|GENERIC_WRITE, //�������д 
					0, //��ռ��ʽ 
					NULL, 
					OPEN_EXISTING, //�򿪶����Ǵ��� 
					 0, //ͬ����ʽ FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED, //�첽��ʽ��  
					NULL); 

	printf("hCom(%d)!\r\n", hCom);
	if(hCom == INVALID_HANDLE_VALUE)
	{
		printf("��COMʧ��(%d)!", GetLastError());
		return FALSE;
	}
	config_com();
	return TRUE;
}

int config_com(void)
{
	DCB dcb;
	COMMTIMEOUTS TimeOuts;//�趨����ʱ
	
	SetupComm(hCom, 4096, 4096);//���뻺����������������Ĵ�С����1024

	TimeOuts.ReadIntervalTimeout=1;
	TimeOuts.ReadTotalTimeoutMultiplier=1;
	TimeOuts.ReadTotalTimeoutConstant=1;//�趨д��ʱ
	TimeOuts.WriteTotalTimeoutMultiplier=500;
	TimeOuts.WriteTotalTimeoutConstant=2000;
	SetCommTimeouts(hCom,&TimeOuts);//���ó�ʱ/

	GetCommState(hCom,&dcb); 
	dcb.BaudRate=115200; //������Ϊ9600 
	dcb.ByteSize=8; //ÿ���ֽ���8λ 
	dcb.Parity=NOPARITY; //����żУ��λ 
	dcb.StopBits=ONESTOPBIT; //1��ֹͣλ 
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
	{                                       //ѭ��д�ļ���ȷ���������ļ���д��  
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