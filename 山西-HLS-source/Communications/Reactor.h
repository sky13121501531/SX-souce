///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����Reactor.h
// �����ߣ�jiangcheng
// ����ʱ�䣺2009-05-20
// �����������Զ���ķ�Ӧ����
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ace/Task.h"
#include "ace/Reactor.h"

class XMLReceiverServer;
class RealTimeVideoServer;
class RealTimeQualityServer;
class DeviceServer;

class MyReactor : public ACE_Task<ACE_MT_SYNCH>
{
public:
	MyReactor(void);
	virtual ~MyReactor(void);
public:
	int Init();

	int open(void*);

	virtual int svc();

	int Stop();

	ACE_Reactor* GetReactor();
private:
	RealTimeVideoServer *pRealTimeVideoServer;
	RealTimeQualityServer* pRealTimeQualityServer;
	DeviceServer* pDeviceServer;
	XMLReceiverServer* pXMLReceiverServer;
};
