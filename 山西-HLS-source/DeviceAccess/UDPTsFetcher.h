///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����UDPUDPTsFetcher.h
// �����ߣ�gaoxd
// ����ʱ�䣺2010-07-07
// ������������Ƶ���ݻ�ȡ�߳� ����UDPЭ��
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "TsFetcher.h"
#include "ace/Task.h"
#include "../Foundation/TypeDef.h"
#include <ace/SOCK_Dgram_Mcast.h>
#include <ace/INET_Addr.h>
#include <vector>
#include <string>
class TsSender;
class UdpTsSender;

class UDPTsFetcher : public TsFetcher
{
public:
	UDPTsFetcher(int deviceid);
	~UDPTsFetcher();
private:
	UDPTsFetcher();
public:
	int Start();

	int open(void*);

	virtual int svc();

	int Stop();

	bool SetTsDeviceXml(std::string devicexml);

	bool SendTsDeviceXml();
	bool SendTsDeviceXmlForDVBC(std::string devXml);
	bool SendTsDeviceXmlForCTTB(std::string devXml);
	bool SendTsDeviceXmlForDVBS(std::string devXml);
	bool SendTsDeviceXmlForFMAM(std::string devXml);

	bool RebootCard();

	void IncreaseTaskNum();
	void DecreaseTaskNum();
	void StopHistoryTask();

	bool SetTsSendTask(TsSender* task);
	bool SetTsRoundTask(TsSender* task);
	void SetSendSwitch(bool sendswitch);		//�����Ƿ�������
	void SetForDataToCodec(bool senddatatocodec);
	bool SetRecordTask(ACE_Task<ACE_MT_SYNCH>* task);
	bool DelRecordTask(ACE_Task<ACE_MT_SYNCH>* task);
	void SetReSendSwitch(bool sendswitch);
	//
	void SetMulti(bool bMulti,std::vector<UdpSendTsAddr> UdpSendTsAddrlist);
	
private:
	bool FlushHistoryDatum();
	void TimeConvert(time_t curtime,unsigned char *timebuf);//��ϵͳʱ����Ϣ���뵽¼��
	void PutSysTime(ACE_Task<ACE_MT_SYNCH>* task);
private:
	
	std::string CurDeviceXml;			//��ǰTSӲ������Ϣ

	ACE_Thread_Mutex TaskPointerMutex;
	ACE_Thread_Mutex TaskMutex;
	ACE_Thread_Mutex TaskNumMutex;
	ACE_Thread_Mutex RealStreamMutex;
	static ACE_Thread_Mutex TaskRoundMutex;

	TsSender* TsSendTaskPointer;
	TsSender* TsSendRoundTaskPointer;
	std::vector<ACE_Task<ACE_MT_SYNCH>*> RecordTaskPointerVec;
	std::vector<ACE_Message_Block *> StrRoundDataPointerVec;
	
	ACE_INET_Addr remote_addr;
	ACE_SOCK_Dgram_Mcast DeviceSock;
	//

	//
	bool m_SendForMulti;
	UdpTsSender* m_pUdpTsSender;
	//

	bool bFlag;
	bool bSendSwtich;
	bool m_bsenddatatocodec;
	bool bSendOrder;
	int TaskNum;
	bool ReSendSwitch;
	bool NewPackageHead;		//����Ƶ����ͷ����
	int comamdtype;
};