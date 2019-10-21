///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����DeviceAccessMgr.h
// �����ߣ�jiangcheng
// ����ʱ�䣺2009-06-09
// ����������Ӳ���豸���ʹ�����
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "ace/Synch.h"
#include "ace/Singleton.h"
#include "ace/Message_Queue.h"
#include "ace/SOCK_Stream.h"
#include <map>

class DeviceAccess;
class DeviceAccessMgr
{
public:
	DeviceAccessMgr();
	virtual ~DeviceAccessMgr();
public:
	//����ͨ���ź��·���XML����õ�Ӳ���豸�ظ���XML��Ϣ
	bool SendTaskMsg(int deviceID, const std::string& strCmdMsg,std::string& strRetMsg);
	bool SendMultiTask(int deviceID,std::string strtsIp,unsigned int tsport,int cardindex,int MultiIndex);
	//����ͨ���ź�Ts���ļ��ڵ��ַ���õ����ʸ�����socket�����Ϣ
	bool GetTsReceiveHandle(int deviceID,const std::string& strAddress,ACE_SOCK_Stream& streamHandle);
	//����Ts���ļ���ַ���õ����ʸ�����socket�����Ϣ
	bool GetTsReceiveHandle(const std::string& strAddress,ACE_SOCK_Stream& streamHandle);
	//�жϸ�ͨ���Ƿ�����
	bool CheckFreqLock(int deviceID);					
	//���ÿ�ʱ��
	bool SetSysTime(int deviceID);	

private:
	void UnInit();
private:
	std::map<int,DeviceAccess*> deviceAccessMap;
	ACE_Thread_Mutex getTsMutex;
};

typedef ACE_Singleton<DeviceAccessMgr,ACE_Mutex>  DEVICEACCESSMGR;