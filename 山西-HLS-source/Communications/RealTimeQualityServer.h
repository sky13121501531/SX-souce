///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����RealTimeQualityServer.h
// �����ߣ�gaoxd
// ����ʱ�䣺2009-09-09
// ����������ʵʱָ������������˴�����
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ace/Event_Handler.h"
#include "ace/SOCK_Acceptor.h"
#include <string>
#include <vector>

class QualitySender;

class RealTimeQualityServer : public ACE_Event_Handler
{
public:
	RealTimeQualityServer();
	virtual ~RealTimeQualityServer();
public:
	int Open(ACE_Addr &addr);

	int handle_input(ACE_HANDLE handle);

	ACE_HANDLE get_handle(void) const;

	int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask);

public:
	bool RecvTaskXml(ACE_SOCK_Stream& newclient,std::string& taskxml);
	int Stop();
private:
	ACE_SOCK_Acceptor peer_acceptor;
	ACE_Thread_Mutex videoServerMutex;
	bool bFlag;
};
