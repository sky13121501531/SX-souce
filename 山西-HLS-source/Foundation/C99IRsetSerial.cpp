#include "C99IRsetSerial.h"

C99IRsetSerial::C99IRsetSerial(void)
{
	m_hCom = 0;
}

C99IRsetSerial::~C99IRsetSerial(void)
{
	CloseSerial();
}
void C99IRsetSerial::CloseSerial(void)
{
	CloseHandle(m_hCom);
	m_hCom=INVALID_HANDLE_VALUE;
	return;
}

bool C99IRsetSerial::InitSerialDevice(string str)
{
	DCB dcb;
	COMMTIMEOUTS timeOuts; //���ڲ�����ʱ�ṹ��
	DWORD dwError = 0;
	m_hCom = CreateFile(str.c_str(), GENERIC_READ | GENERIC_WRITE,
		0, NULL, OPEN_EXISTING, 0, NULL);

	if (m_hCom == INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hCom);
		m_hCom=INVALID_HANDLE_VALUE;
		return false;
	}
	ZeroMemory(&dcb, sizeof(DCB));
	ZeroMemory(&timeOuts, sizeof(COMMTIMEOUTS));

	//���ý��ջ������ͷ��ͻ������Ĵ�С
	if (!SetupComm(m_hCom, 2048, 2048)) 
	{
		CloseHandle(m_hCom);
		m_hCom=INVALID_HANDLE_VALUE;
		return false;
	}

	dcb.BaudRate	= 9600;         //�趨������Ϊ9600 bps
	dcb.ByteSize	= 8;              //�趨����λΪ8
	dcb.Parity		= NOPARITY;       //�趨����żУ��
	dcb.StopBits	= ONESTOPBIT;     //�趨һ��ֹͣλ
	dcb.DCBlength = sizeof(DCB);

	////���ô����豸���Ʋ���
	if (!SetCommState(m_hCom, &dcb)) 
	{
		CloseHandle(m_hCom);
		m_hCom=INVALID_HANDLE_VALUE;
		return false;
	}

	timeOuts.ReadIntervalTimeout = 20;
	timeOuts.ReadTotalTimeoutConstant = 30;
	timeOuts.ReadTotalTimeoutMultiplier = 2;
	timeOuts.WriteTotalTimeoutConstant = 0;
	timeOuts.WriteTotalTimeoutMultiplier = 0;

	if (!SetCommTimeouts(m_hCom, &timeOuts)) 
	{
		CloseHandle(m_hCom);
		m_hCom=INVALID_HANDLE_VALUE;
		return false;
	}

	return true;
}


bool C99IRsetSerial::SendSerial(unsigned char* cSend, int iSendLen, 
								unsigned char*& cRecv, int& iRecvLen)
{
	bool bRet = false;
	bRet = WriteCom(cSend, iSendLen);
	if (!bRet)
		return false;

	unsigned char buf[iBufSize] = {0};
	bRet = ReadCom(buf, iBufSize);

	if (!bRet )
		return false;
	int ibuflen = ReadFrameLen(buf);
	if (ibuflen <= 0)
		return false;
	unsigned char* p = new unsigned char[ibuflen+1];
	memset(p, 0, ibuflen+1);
	memcpy(p, buf, ibuflen);
	cRecv = p;
	iRecvLen = ibuflen;
	
	return true;
}

bool C99IRsetSerial::WriteCom(unsigned char* pBuf, int iBufLen)
{

	DWORD tmp;
	//ֹͣ���ͺͽ������ݣ���������ͺͽ��ջ����� 
	PurgeComm(m_hCom, PURGE_TXCLEAR | PURGE_RXCLEAR);
	if (!WriteFile(m_hCom, pBuf, iBufLen, &tmp, NULL))
	{
		return false;
	}

	return true;
}

bool C99IRsetSerial::ReadCom(unsigned char* pBuf, int iBufLen)
{
	DWORD tmp ;
	if (!ReadFile(m_hCom, pBuf, iBufLen, &tmp, NULL))
	{
		return false;
	}

	return true;
}

unsigned char* C99IRsetSerial::GetSendBuf(unsigned char* cCmdCode,
										  unsigned char* buf, int buflen)
{
	unsigned char cFrameHead[2] = {0xF7, 0x7F};
	unsigned char cFrameLen[2] = {0};
	unsigned char cCheckCode = 0;
	int  i1 = 0;
	unsigned char* bfl= NULL;
	bfl = GetFrameLength(buflen,i1);
	memcpy(cFrameLen, bfl, 2);
	delete bfl;
	bfl = NULL;

	unsigned char* pbuf = new unsigned char[i1];
	memset(pbuf, 0, i1);
	int i2 = 0;
	memcpy(pbuf+i2,	cFrameHead, 2);
	i2 += 2;
	memcpy(pbuf+i2, cFrameLen, 2);
	i2 += 2;
	memcpy(pbuf+i2, cCmdCode, 2);
	i2 += 2;
	memcpy(pbuf+i2, buf, buflen);
	i2 += buflen;
	cCheckCode = GetCheckCode(pbuf, i1);
	memcpy(pbuf+i2, &cCheckCode, 1);

	return pbuf;
}



//��ȡ У����
unsigned char C99IRsetSerial::GetCheckCode(unsigned char* pBuf, int iBufLen)
{
	unsigned char iNum = 0;
	for (int i = 0; i < iBufLen; i++)
	{
		iNum += (unsigned char)(pBuf[i]);
	}
	iNum += 1;
	unsigned char ch = iNum & 0xFF;
	return ch;
}


//��ȡ ֡����
unsigned char* C99IRsetSerial::GetFrameLength(int iContentsLen, int& iFrameLen)
{
	unsigned char* p = new unsigned char[2];
	memset(p, 0, 2);
	int inum = 7 + iContentsLen; //7 = 2+2+2+1;
	int i1 = inum >> 8;
	int i2 = inum & 0xFF;
	*p = (unsigned char)i1;
	*(p+1) = (unsigned char)i2;
	iFrameLen = inum;
	return p;
}


//��ȡ�ļ�����
int C99IRsetSerial::ReadFrameLen(unsigned char* pBuf)
{
	if (pBuf == NULL)
		return -1;
	int iLen = -1;
	iLen = (*(pBuf+2))*256 + *(pBuf+3);
	return iLen;
}

unsigned char* C99IRsetSerial::char2mem(unsigned char* p, int len, int &iOutlen)
{
	if (len <= 1)
		return NULL;
	int i1 = len >> 1;
	unsigned char* p1 = new unsigned char[i1+1];
	memset(p1, 0, i1+1);
	for (int i = 0; i < i1; i++)
	{
		char b[3] = {0};
		memcpy(b, p+2*i, 2);
		int inum = 0;
		sscanf(b, "%02x", &inum);
		*(p1+i) = inum;
	}
	iOutlen = i1;
	return p1;
}

unsigned char* C99IRsetSerial::mem2char(unsigned char* p, int len, int &iOutlen)
{
	if (len == 0)
		return NULL;
	unsigned char buf[iBufSize] = {0};
	memcpy(buf, p, len);
	int i1 = len*2;
	unsigned char* pb1 = new unsigned  char[i1+1];
	memset(pb1, 0, i1+1);
	for (int i = 0; i < len; i++)
	{
		char b11[3] = {0};
		sprintf(b11, "%02x", buf[i]);
		memcpy(pb1+2*i, b11, 2);
	}
	iOutlen = i1;
	return pb1;
}



//״̬�� ����
string C99IRsetSerial::StateParser(unsigned char cState)
{
	string buf = "";
	switch(cState)
	{
	case 0x00:
		buf = "�������ɹ�";
		break;
	case 0x01:
		buf = "�洢����";
		break;
	case 0x02:
		buf = "�ð���δѧϰ";
		break;
	case 0x03:
		buf = "֡���ݲ������� ";
		break;
	case 0x04:
		buf = "��֧�ִ���ͨѶ";
		break;
	case 0x05:
		buf = "ͨѶ֡���ݳ��Ȳ���";
		break;
	case 0x06:
		buf = "��Ӧ��ʱ";
		break;
	case 0x07:
		buf = "У��ͳ���";
		break;
	case 0x08:
		buf = "�汾����";
		break;
	case 0x09:
		buf = "��ǰģ��æ";
		break;
	case 0x0A:
		buf = "��������";
		break;
	default:
		break;
	}
	return buf;
}

//������ ����
unsigned char C99IRsetSerial::CmdCodeParser(unsigned char* pCmdCode)
{
	if (pCmdCode == NULL)
		return NULL;
	unsigned char cmdcode = *(pCmdCode+1);

	return cmdcode;
}

string C99IRsetSerial::ContestsParser(unsigned char CmdCodeLow,
					  unsigned char* cContests, int iContestsLen)
{
	if (cContests == NULL)
		return NULL;
	string str;
	switch(CmdCodeLow)
	{
	case 0x81:
		{
			str = "ѧϰ����ָ�� Ӧ��֡: ";
			unsigned char cState = *(cContests);
			string str1 = StateParser(cState);
			str += str1;
		}
		break;
	case 0x82:
		{
			str = "�������ⷢ�� Ӧ��֡: ";
			unsigned char cState = *(cContests);
			string str1 = StateParser(cState);
			str += str1;
		}
		break;
	case 0x83:
		{
			str = "�������ⷢ�� Ӧ��֡: ";
			unsigned char cState = *(cContests);
			string str1 = StateParser(cState);
			str += str1;
		}
		break;
	case 0x84:
		{
			str = "�������ⷢ�� Ӧ��֡: ";
			unsigned char cState = *(cContests);
			string str1 = StateParser(cState);
			str += str1;
		}
		break;
	case 0x85:
		{
			str = "���ú��ⷢ�ͼ��ʱ�� Ӧ��֡: ";
			unsigned char cState = *(cContests);
			string str1 = StateParser(cState);
			str += str1;
		}
		break;
	case 0x86:
		{
			str = "��ȡ�������ⰴ���洢�� Ӧ��֡: ";
			unsigned char cState = *(cContests);
			string str1 = StateParser(cState);
			str += str1;
		}
		break;
	case 0x87:
		{
			str = "д�뵥�����ⰴ���洢�� Ӧ��֡: ";
			unsigned char cState = *(cContests);
			string str1 = StateParser(cState);
			str += str1;
		}
		break;
	case 0x88:
		{
			str = "����д�������ָ�� Ӧ��֡: ";
			unsigned char cState = *(cContests);
			string str1 = StateParser(cState);
			str += str1;
		}
		break;
	case 0x89:
		{
			str = "������ȡ������ָ�� Ӧ��֡: ";
			unsigned char cState = *(cContests);
			string str1 = StateParser(cState);
			str += str1;
		}
		break;
	case 0x8A:
		{
			str = "����LED2����״̬�� Ӧ��֡: ";
			unsigned char cState = *(cContests);
			string str1 = StateParser(cState);
			str += str1;
		}
		break;
	case 0x8B:
		{
			str = "���ò����� Ӧ��֡: ";
			unsigned char cState = *(cContests);
			string str1 = StateParser(cState);
			str += str1;
		}
		break;
	case 0x8C:
		{
			str = "��ȡ����汾�� Ӧ��֡: ";
			unsigned char cState = *(cContests);
			string str1 = StateParser(cState);
			str += str1;
		}
		break;
	case 0x8D:
		{
			str = "У���������� Ӧ��֡: ";
			unsigned char cState = *(cContests);
			string str1 = StateParser(cState);
			str += str1;
		}
		break;
	case 0x8E:
		{
			str = "��ѯУ����״̬ Ӧ��֡: ";
			unsigned char cState = *(cContests);
			string str1 = StateParser(cState);
			str += str1;
		}
		break;
	case 0x91:
		{
			str = "�ָ��������� Ӧ��֡: ";
			unsigned char cState = *(cContests);
			string str1 = StateParser(cState);
			str += str1;
		}
		break;
	case 0x92:
		{
			str = "��ѯLED2�ƹ���״̬ Ӧ��֡: ";
			unsigned char cState = *(cContests);
			string str1 = StateParser(cState);
			str += str1;
		}
		break;
	case 0x93:
		{
			str = "��ѯ���ʱ�� Ӧ��֡: ";
			unsigned char cState = *(cContests);
			string str1 = StateParser(cState);
			str += str1;
		}
		break;
	case 0x94:
		{
			str = "����ģ��ID�� Ӧ��֡: ";
			unsigned char cState = *(cContests);
			string str1 = StateParser(cState);
			str += str1;
		}
		break;
	case 0x95:
		{
			str = "��ȡģ��ID�� Ӧ��֡: ";
			unsigned char cState = *(cContests);
			string str1 = StateParser(cState);
			str += str1;
		}
		break;
	}

	return str;
}


//��ȡģ��ID�� ֡
bool C99IRsetSerial::ReadIDCode(unsigned char& ucIDCode)
{
	unsigned char cIDNum[10] = {0xF7, 0x7F, 0x00, 0x07, 0x00, 0x15, 0x93};
	unsigned char* pSend = new unsigned char[8];
	memset(pSend, 0, 8);
	memcpy(pSend, cIDNum, 7);
	unsigned char* pRecv = NULL;
	int iOutLen = 0;
	bool bRet = SendSerial(pSend, 7, pRecv, iOutLen);
	delete []pSend;
	pSend = NULL;
	if (bRet && (pRecv != NULL) )
	{
		int ilen = ReadFrameLen(pRecv);
		if (ilen == iOutLen)
		{
			unsigned char cIDCode = *(pRecv+7);
			ucIDCode = cIDCode;
		}
		delete []pRecv;
		pRecv = NULL;
		return true;
	}
	return false;
}

//��ȡ���ʱ�� 
bool C99IRsetSerial::ReadCmdIntervalTime(unsigned char ucIDCode, int& iIntervalTime)
{
	unsigned char cCmdCode[2] = {0};
	cCmdCode[0] = ucIDCode;
	cCmdCode[1] = 0x13;		//0x13 ��ѯ���ʱ��ָ��
	unsigned char* pFrames = GetSendBuf(cCmdCode, NULL, 0);
	int iFramesLen = ReadFrameLen(pFrames);
	unsigned char* pRecv = NULL;
	int iOutLen = 0;
	bool bRet = SendSerial(pFrames, iFramesLen, pRecv, iOutLen);
	delete []pFrames;
	pFrames = NULL;
	if (bRet && (pRecv != NULL) )
	{
		int ilen = ReadFrameLen(pRecv);
		if (ilen == iOutLen)
		{
			unsigned char cIntervalTime = *(pRecv+7);
			iIntervalTime = (int)cIntervalTime;
		}
		delete []pRecv;
		pRecv = NULL;
		return true;
	}
	return false;
}


//���յ������� ���н���
unsigned char C99IRsetSerial::RecvBufParser(unsigned char* pRecvBuf, int iBufLen,
											 int& iContestsLen, unsigned char* pContests )
{
	if (pRecvBuf == NULL)
		return 0;
	
	int iFrameLen = ReadFrameLen(pRecvBuf);
	if (iFrameLen != iBufLen)
		return 0;
	if (iFrameLen > 7)
	{
		int iContestsLen = iFrameLen -7;
		unsigned char cStateCode = *(pRecvBuf+6);
		if (iContestsLen > 1)
		{
			int ilen = iContestsLen - 1;
			unsigned char* pFrameContests = new unsigned char[ilen+1];
			memset(pFrameContests, 0, ilen+1);
			memcpy(pFrameContests, pRecvBuf+7, ilen);
			pContests = pFrameContests;
			iContestsLen = ilen;
		}
		return cStateCode;		
	}


	return 0;
}
