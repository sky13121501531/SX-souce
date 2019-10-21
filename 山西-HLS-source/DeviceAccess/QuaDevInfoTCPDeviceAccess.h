//////////////////////////////////////////////////////////////////////////
//����ʱ��: 20150922
//�������������� ��������Ƶ�ס���ǿ 
//////////////////////////////////////////////////////////////////////////
#ifndef QUADEVINFOTCPDEVICEACCESS_H_H_H
#define QUADEVINFOTCPDEVICEACCESS_H_H_H
#pragma once
#include "./TCPDeviceAccess.h"
#include "./TcpClt.h"
using namespace std;

const long lQuaRecvBufSize = 1048576;//1*1024*1024 = 1M

class CQuaDevInfoTCPDeviceAccess :
	public TCPDeviceAccess
{
public:
	CQuaDevInfoTCPDeviceAccess(int deviceid,std::string strIP,int nPort);
	virtual ~CQuaDevInfoTCPDeviceAccess(void);

public:
	//��ǿ ����
	bool SendQuaDevInfoCmd(string s_freq, string &s_out_cmd); 
	//Ƶ�� ����
	bool SendSpecDevInfoCmd(string s_freq_start, string s_freq_end,int i_step_len, string &s_out_cmd);
private:
	string m_strIP;
	int m_iPort;
	CTcpClt* m_pTcpClt;
};

#endif