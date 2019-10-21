///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����TsFetcher.h
// �����ߣ�gaoxd
// ����ʱ�䣺2009-06-16
// ������������Ƶ���ݻ�ȡ�߳�
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ace/Task.h"
#include "ace/SOCK_Connector.h"
#include <vector>
#include <string>
#include "../Foundation/TypeDef.h"
class TsSender;

class TsFetcher : public ACE_Task<ACE_MT_SYNCH>
{
public:
	TsFetcher(int deviceid);
	~TsFetcher();
private:
	TsFetcher();
public:
	virtual int Start() = 0;

	virtual int open(void*) = 0;

	virtual int svc() = 0;

	virtual int Stop() = 0;

	virtual bool SetTsDeviceXml(std::string devicexml) = 0;

	virtual bool SendTsDeviceXml() = 0;

	virtual void IncreaseTaskNum() = 0;
	virtual void DecreaseTaskNum() = 0;
	virtual void StopHistoryTask() = 0;

	virtual bool SetTsSendTask(TsSender* task) = 0;
	virtual bool SetTsRoundTask(TsSender* task) = 0;
	virtual void SetSendSwitch(bool sendswitch) = 0;		//�����Ƿ�������
	virtual bool SetRecordTask(ACE_Task<ACE_MT_SYNCH>* task) = 0;
	virtual bool DelRecordTask(ACE_Task<ACE_MT_SYNCH>* task) = 0;
	virtual void SetReSendSwitch(bool sendswitch) = 0;
	virtual bool RebootCard() = 0;	
	virtual void SetMulti(bool bMulti,std::vector<UdpSendTsAddr> UdpSendTsAddrlist) = 0;
	virtual void SetForDataToCodec(bool senddatatocodec) = 0;		
protected:
	int FindBegin(unsigned char* tsBuf,unsigned int tsLen);
protected:
	int DeviceId;
	int TunerID;
};