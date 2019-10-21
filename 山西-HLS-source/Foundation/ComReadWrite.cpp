#include "ComReadWrite.h"


#if  defined(WIN32) || defined(__WIN32__)

	HRESULT CComReadWrite::InitDevice(int comport, DWORD dwbaud)
	{
		DCB    dcb;
		COMMTIMEOUTS TimeOuts; //���ڲ�����ʱ�ṹ��
		DWORD dwError = 0;
		char name[16];
		sprintf(name,"COM%d",comport);
	   
		hCom = CreateFile( name,
						GENERIC_READ | GENERIC_WRITE,
						0,					// comm devices must be opened w/exclusive-access
						NULL,				// no security attributes
						OPEN_EXISTING,		// comm devices must use OPEN_EXISTING
						0,					// no overlapped I/O
						NULL			);		// hTemplate must be NULL for comm devices 

		if (hCom == INVALID_HANDLE_VALUE) 
		{
			return S_FALSE;
		}// handle error 
		ZeroMemory(&dcb,sizeof(DCB));
		ZeroMemory(&TimeOuts,sizeof(COMMTIMEOUTS));

		//���ý��ջ������ͷ��ͻ������Ĵ�С
		if (!SetupComm(hCom, 256, 256)) 
		{	goto ret0;		}			
		
		//GetCommState(hCom,&dcb);  //��ȡȱʡ��DCB�ṹ��ֵ
	    
		// Fill in the DCB: baud=9600, 8 data bits, even parity, 1 stop bit. 
		dcb.BaudRate	= dwbaud;         //�趨������Ϊ9600 bps
		dcb.ByteSize	= 8;              //�趨����λΪ8
		dcb.Parity		= NOPARITY;       //�趨����żУ��
		dcb.StopBits	= ONESTOPBIT;     //�趨һ��ֹͣλ
		dcb.DCBlength = sizeof(DCB);
		
		////���ô����豸���Ʋ���
		if (!SetCommState(hCom, &dcb)) 
		{	goto ret0;		}			
		
		TimeOuts.ReadIntervalTimeout = 10;
		TimeOuts.ReadTotalTimeoutConstant = 30;
		TimeOuts.ReadTotalTimeoutMultiplier = 2;
		TimeOuts.WriteTotalTimeoutConstant = 0;
		TimeOuts.WriteTotalTimeoutMultiplier = 0;

		if (!SetCommTimeouts(hCom, &TimeOuts)) 
		{	goto ret0;		}			

		/*
		//���Ӵ��ڵĴ���ͽ��յ��ַ������¼� 
��		SetCommMask(hCom,EV_ERR|EV_RXCHAR);
��		//�����˹����衢δ���źŵ��¼� 
��		hEvent=CreateEvent(NULL,FALSE,FALSE,"WatchEvent"); 
		//����һ���¼������߳������Ӵ����¼� 
��		AfxBeginThread(CommWatchProc,pParam); 
		*/
		return	S_OK;

	ret0:
		CloseHandle(hCom);
		hCom=INVALID_HANDLE_VALUE;
		return S_FALSE;
	}

	HRESULT CComReadWrite::CloseDevice()
	{
		if(hCom==INVALID_HANDLE_VALUE)
		{
			return S_OK;
		}
		CloseHandle(hCom);
		hCom=INVALID_HANDLE_VALUE;
		return S_OK;
	}

	HRESULT CComReadWrite::WriteCom(char* str,int count)
	{
		DWORD tmp;
		//Mutex.acquire();
		//ֹͣ���ͺͽ������ݣ���������ͺͽ��ջ����� 
		PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR);
		if (!WriteFile(hCom, str, count, &tmp, NULL))
		{
			//Mutex.release();
			return S_FALSE;
		}
	//	::FhtLog("д��COM��,���ݳ���Ϊ%d,����Ϊ:%s", count,(LPCTSTR)::Bin2HexString((BYTE*)str, count, 1));
		//Mutex.release();
		//printf("д�����ݳɹ���\n");
		return S_OK;
	}

	HRESULT CComReadWrite::ReadCom(char* str,int count,int nRetryCount)
	{
		DWORD dwTotalRead = 0;
		DWORD dwReadCount = 0;
		//Mutex.acquire();
		while(dwTotalRead < (DWORD)count && dwReadCount < (DWORD)nRetryCount)
		{
			DWORD tmp =0;
			ReadFile(hCom, str+dwTotalRead, count-dwTotalRead, &tmp, NULL);	
		    //printf("��COM�������ݳ���Ϊ%d,����Ϊ:%s", tmp,str);
			dwTotalRead += tmp;
			dwReadCount++;
		}
		//printf("�����ݳɹ���\n");
		//return S_OK;
		if((DWORD)count!=dwTotalRead)
		{
			//Mutex.release();
			return S_FALSE;
		}
		else 
		{
			//Mutex.release();
			return S_OK;
		}
	}
#else
	HRESULT CComReadWrite::InitDevice(int comport, unsigned long dwbaud, unsigned long dReadIntervalTimeout, unsigned long dReadTotalTimeoutConstant)
	{
		if (initbool)
			return S_OK;
		bool reval =S_FALSE;
		char dev[12]= {0};
		sprintf(dev,"/dev/ttyS%d",comport-1);
		hCom=open(dev,O_RDWR  |  O_NOCTTY);
		if(hCom==-1)
		{
			perror("can't open dev");
		}
		if(0 == set_opt(hCom,dwbaud,8,'N',1,2))
			{
			reval = S_OK;
			initbool = true;
		}
		else
			{
				close(hCom);
			}
		return reval;
	}
	HRESULT CComReadWrite::CloseDevice()
	{
		initbool = false;
		close(hCom);
		return S_OK;
	}
	HRESULT CComReadWrite::WriteCom(char* str,int count,int nRetryCount)
	{
		HRESULT reval = S_FALSE;
		_Syn->Lock();
		int sendByte = write(hCom,str ,count);

		_Syn->UnLock();
		if ( sendByte == count)
	 		reval = S_OK;
		return reval;
	}

	HRESULT CComReadWrite::ReadCom(char* str,int count,int nRetryCount)
	{	
		
		int dwTotalRead = 0;
		int dwReadCount = 0;

 		_Syn->Lock();
		while(dwTotalRead < count && dwReadCount < nRetryCount)
		{
			int readByte =0;
			readByte = read(hCom, str+dwTotalRead, count-dwTotalRead);	
	        
			dwTotalRead += readByte;
			dwReadCount++;
			//printf("readByte =%d  dwTotalRead = %d\n",readByte,dwTotalRead);
	       
		}
		_Syn->UnLock();
		if(count!=dwTotalRead)
		{
			return S_FALSE;
		}
		else 
		{
			return S_OK;
		}
		/*int reval;
		int  readByte = 1;
		while(readByte > 0)
		{
		readByte = read(hCom,str,8);
		}
		return reval;*/
	}

	int CComReadWrite::set_opt(int fd,int nSpeed, int nBits, char nEvent, int nStop,int nTimeout)
	{
		struct termios newtio,oldtio;
		/*����������д��ڲ������ã�������������ںŵȳ���������صĳ�����Ϣ*/
		if ( tcgetattr( fd,&oldtio) != 0)
		{
			perror("SetupSerial 1");
			return -1;
		}
		bzero( &newtio, sizeof( newtio ) );
		/*����һ�������ַ���С*/
		newtio.c_cflag |= CLOCAL | CREAD;
		newtio.c_cflag &= ~CSIZE;
		/*����ֹͣλ*/
		switch( nBits )
		{
			case 7:
				newtio.c_cflag |= CS7;
				break;
			case 8:
				newtio.c_cflag |= CS8;
				break;
		}
	/*������żУ��λ*/
		switch( nEvent )
		{
			case 'O': //����
				newtio.c_cflag |= PARENB;
				newtio.c_cflag |= PARODD;
				newtio.c_iflag |= (INPCK | ISTRIP);
			break;
			case 'E': //ż��
				newtio.c_iflag |= (INPCK | ISTRIP);
				newtio.c_cflag |= PARENB;
				newtio.c_cflag &= ~PARODD;
			break;
				case 'N': //����żУ��λ
				newtio.c_cflag &= ~PARENB;
			break;
		}
		/*���ò�����*/
		switch( nSpeed )
		{
			case 2400:
				cfsetispeed(&newtio, B2400);
				cfsetospeed(&newtio, B2400);
			break;
			case 4800:
				cfsetispeed(&newtio, B4800);
				cfsetospeed(&newtio, B4800);
			break;
			case 9600:
				cfsetispeed(&newtio, B9600);
				cfsetospeed(&newtio, B9600);
			break;
			case 115200:
				cfsetispeed(&newtio, B115200);
				cfsetospeed(&newtio, B115200);
			break;
			case 460800:
				cfsetispeed(&newtio, B460800);
				cfsetospeed(&newtio, B460800);
			break;
			default:
				cfsetispeed(&newtio, B9600);
				cfsetospeed(&newtio, B9600);
			break;
		}
		/*����ֹͣλ*/
		if( nStop == 1 )
			newtio.c_cflag &= ~CSTOPB;
		else if ( nStop == 2 )
			newtio.c_cflag |= CSTOPB;
		/*���õȴ�ʱ�����С�����ַ�*/
		/*nTimeout = nTimeout / 1000;
		if (nTimeout <= 0)
    		nTimeout = 1;*/
		newtio.c_cc[VTIME] = 10 ;
		newtio.c_cc[VMIN] = 0;
		/*����δ�����ַ�*/
		tcflush(fd,TCIFLUSH);
		/*����������*/
		if((tcsetattr(fd,TCSANOW,&newtio))!=0)
		{
			perror("com set error");
			return -1;
		}
		return 0;
	} 
#endif
