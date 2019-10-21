//����ѧϰ����
#ifndef _99IRSETSERIAL_H_
#define _99IRSETSERIAL_H_
#pragma once
#include <Windows.h>
#include <iostream>
using namespace std;

const int iBufSize = 2048;
class C99IRsetSerial
{
public:
	C99IRsetSerial(void);
	~C99IRsetSerial(void);
	bool InitSerialDevice(string str);
	void CloseSerial(void);
	bool SendSerial(unsigned char* cSend, int iSendLen, 
		unsigned char*& cRecv, int& iRecvLen);
	//��ȡ ���� ����
	unsigned char* GetSendBuf(unsigned char* cCmdCode,
		unsigned char* buf, int buflen);
	//���յ������� ���н���
	unsigned char RecvBufParser(unsigned char* pRecvBuf, int iBufLen,
		int& iContestsLen, unsigned char* pContests = NULL);

	//״̬�� ����
	string StateParser(unsigned char cState);

	//������ ����
	unsigned char CmdCodeParser(unsigned char* pCmdCode);
	//���� ����
	string ContestsParser(unsigned char CmdCodeLow,
		unsigned char* cContests, int iContestsLen);


	//��ȡ�ļ�����
	int ReadFrameLen(unsigned char* pBuf);
	//��ȡģ��ID�� ֡
	bool ReadIDCode(unsigned char& ucIDCode);
	//��ȡ���ʱ�� 
	bool ReadCmdIntervalTime(unsigned char ucIDCode, int& iIntervalTime);

	/*���ַ�����ֵ 2���ַ�һ�� ת������һ���ַ����ĵ�ַ
	**���� ֵΪ(�ĸ��ַ�) "0001" ������ת�ɣ����� 0x00, 0x01 (������ַ)
	*/
	unsigned char* char2mem(unsigned char* p, int len, int &iOutlen);

	/*���ַ����ĵ�ַ ת���� ��һ���ַ�����ֵ
	**���� 0x00, 0x01 (������ַ) ������ת�ɣ����� ֵΪ(�ĸ��ַ�) "0001"
	*/
	unsigned char* mem2char(unsigned char* p, int len, int &iOutlen);

private:
	bool WriteCom(unsigned char* pBuf, int iBufLen);
	bool ReadCom(unsigned char* pBuf, int iBufLen);
	//��ȡ У����
	unsigned char GetCheckCode(unsigned char* pBuf, int iBufLen);

	//��ȡ ֡����
	unsigned char* GetFrameLength(int iContentsLen, int& iFrameLen);

private:
	HANDLE m_hCom;
};

#endif
