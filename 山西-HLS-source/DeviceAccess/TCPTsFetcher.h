///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����TCPTsFetcher.h
// �����ߣ�gaoxd
// ����ʱ�䣺2010-07-07
// ������������Ƶ���ݻ�ȡ�߳� ����TCPЭ��
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "TsFetcher.h"
#include "ace/Task.h"
#include "ace/SOCK_Connector.h"
#include <vector>
#include <string>

class TsSender;

class TCPTsFetcher : public TsFetcher
{
public:
	TCPTsFetcher(int deviceid);
	~TCPTsFetcher();
private:
	TCPTsFetcher();
public:
	int Start();

	int open(void*);

	virtual int svc();

	int Stop();

	bool SetTsDeviceXml(std::string devicexml);

	bool SendTsDeviceXml();
	bool RebootCard(){return true;};	//��ʱ�Ȳ�ʵ��

	void IncreaseTaskNum();
	void DecreaseTaskNum();
	void StopHistoryTask();

	bool SetTsSendTask(TsSender* task);
	bool SetTsRoundTask(TsSender* task);
	void SetSendSwitch(bool sendswitch);		//�����Ƿ�������
	bool SetRecordTask(ACE_Task<ACE_MT_SYNCH>* task);
	bool DelRecordTask(ACE_Task<ACE_MT_SYNCH>* task);
	void SetReSendSwitch(bool sendswitch);
	//
	void SetMulti(bool bMulti,std::vector<UdpSendTsAddr> UdpSendTsAddrlist)
	{
		m_SendForMulti = bMulti;
	}
private:
	std::string DeviceTaskXml;			//Ӳ����ʼ��ָ��
	std::string CurDeviceXml;			//��ǰTSӲ������Ϣ

	bool URLChanged;
	ACE_Thread_Mutex TaskPointerMutex;
	ACE_Thread_Mutex TaskMutex;
	ACE_Thread_Mutex TaskNumMutex;
	static ACE_Thread_Mutex TaskRoundMutex;

	TsSender* TsSendTaskPointer;
	TsSender* TsSendRoundTaskPointer;
	std::vector<ACE_Task<ACE_MT_SYNCH>*> RecordTaskPointerVec;
	
	ACE_SOCK_Stream DeviceSock;
	std::string URL;
	bool bFlag;
	bool bSendSwtich;
	int TaskNum;

	bool NewPackageHead;		//����Ƶ����ͷ����
	bool m_SendForMulti;
};