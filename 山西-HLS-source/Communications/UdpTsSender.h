///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����TsSender.h
// �����ߣ�gaoxd
// ����ʱ�䣺2009-06-16
// ������������Ƶ���ݷ����߳�
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ace/Task.h"
#include "../Foundation/TypeDef.h"
#include <vector>

class UdpTsSender : public ACE_Task<ACE_MT_SYNCH>
{
public:
	UdpTsSender();
	~UdpTsSender();
public:
	int Start();

	int open(void*);

	virtual int svc();

	int Stop();

	void SetClient(std::vector<UdpSendTsAddr> UdpSendTsAddrlist);		
	bool ClearClient();
private:
	std::vector<UdpSendTsAddr> m_UdpSendTsAddrlist;
	std::vector<UdpSendTsAddr> m_TempUdpSendTsAddrlist;
	ACE_Thread_Mutex QueueMutex;
	bool bFlag;
	bool m_bSetAddr;
	bool m_bClealAddr;
};
