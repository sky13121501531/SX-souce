///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����DeviceAccess.h
// �����ߣ�gaoxd
// ����ʱ�䣺2010-07-06
// ����������Ӳ���豸���ʻ���
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ace/Synch.h"
#include "ace/SOCK_Stream.h"
#include <string>

class DeviceAccess
{
public:
	DeviceAccess(int deviceid, std::string strIP,int nPort);
	virtual ~DeviceAccess(void);
protected:
	DeviceAccess(void);
public:
	virtual bool SendTaskMsg(const std::string& strCmdMsg,std::string& strRetMsg) = 0;
	virtual bool SendMultiTask(std::string strtsIp,unsigned int tsport,int cardindex,int MultiIndex) = 0;
	virtual bool GetTsReceiveHandle(const std::string& strAddress,ACE_SOCK_Stream& streamHandle) = 0;	
	virtual bool CheckFreqLock() = 0;					//�жϸ�ͨ���Ƿ�����
	virtual bool SetSysTime() = 0;					//�жϸ�ͨ���Ƿ�����
protected:
	ACE_Thread_Mutex sendTaskMsgMutex;
	int DeviceId;

	time_t m_tChanFixKeepTime; //�ϴμ��ʱ��,�����ж��Ƿ����ͨ������״̬ Ĭ��3���ڲ��ظ����
	bool m_bIsFreqLock; //��ǰƵ������״̬

	std::string strIPAddress;
	int port;
};

