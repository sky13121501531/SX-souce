#ifndef __SERICAL_COM__
#define __SERICAL_COM__


#include "ace/Task.h"
#include     <stdio.h>      /*��׼�����������*/
#include     <stdlib.h>     /*��׼�����ⶨ��*/
//#include     <unistd.h>     /*Unix ��׼��������*/
#include     <sys/types.h>  
#include     <sys/stat.h>   
#include 		 <string.h>
#include     <fcntl.h>      /*�ļ����ƶ���*/
#include     <errno.h>      /*����Ŷ���*/
//#include "SystemImpl.h"
//#include "Thread.h"

#if  defined(WIN32) || defined(__WIN32__)
#include <Windows.h>
#include <time.h>    /*PPSIX �ն˿��ƶ���*/
#else
#define HRESULT 				long
#define DWORD   				unsigned long
#define S_OK                                   	((HRESULT)0x00000000L)
#define S_FALSE            											((HRESULT)0x00000001L)
#include     <termios.h>    /*PPSIX �ն˿��ƶ���*/
#endif        

#if  defined(WIN32) || defined(__WIN32__)
	class CComReadWrite
	{
	private:
		HANDLE hCom;  //�����豸���
		static ACE_Thread_Mutex Mutex;
	public:
		HRESULT InitDevice(int comport=1, DWORD dwbaud=9600);
		HRESULT CloseDevice();
		HRESULT WriteCom(char* str,int count);
		HRESULT ReadCom(char* str,int count,int nRetryCount=10);
	};
#else
	class CComReadWrite
	{
	private:
		int hCom;  //�����豸���
		bool initbool;
		int set_opt(int fd,int nSpeed, int nBits, char nEvent, int nStop,int nTimeout);
	public:
		static SynClass* _Syn;		
		static CComReadWrite* _Instance;
		static CComReadWrite* GetInstance();
	public:
		CComReadWrite(){ initbool = false; };
		virtual ~CComReadWrite(){};
		//interface
	public:
		HRESULT InitDevice(int comport=1, unsigned long dwbaud=9600, unsigned long dReadIntervalTimeout=100, unsigned long dReadTotalTimeoutConstant=30);
		HRESULT CloseDevice();
		HRESULT WriteCom(char* str,int count,int nRetryCount=10);
		HRESULT ReadCom(char* str,int count,int nRetryCount=10);
	};
#endif
#endif
