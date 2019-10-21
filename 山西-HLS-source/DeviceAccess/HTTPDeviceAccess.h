
///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����HTTPDeviceAccess.h
// �����ߣ�gaoxd
// ����ʱ�䣺2010-07-06
// ����������Ӳ���豸�����࣬����HTTPЭ��
///////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "DeviceAccess.h"

class HTTPDeviceAccess : public DeviceAccess
{
public:
	HTTPDeviceAccess(int deviceid,const std::string& strIP,int nPort);
	~HTTPDeviceAccess(void);
private:
	HTTPDeviceAccess(void);
public:
	bool SendMultiTask(std::string strtsIp,unsigned int tsport,int cardindex,int MultiIndex);
	bool SendTaskMsg(const std::string& strCmdMsg,std::string& strRetMsg);
	bool GetTsReceiveHandle(const std::string& strAddress,ACE_SOCK_Stream& streamHandle);
	bool CheckFreqLock(){return true;};
	bool SetSysTime(){return true;};
};
