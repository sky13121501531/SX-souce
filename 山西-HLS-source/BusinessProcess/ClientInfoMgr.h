#pragma once

#include "ace/Task.h"
#include "ace/Synch.h"
#include "ace/Singleton.h"
#include "../Foundation/TypeDef.h"
#include <vector>

class ClientInfoMgr : public ACE_Task<ACE_MT_SYNCH>
{
public:
	ClientInfoMgr();
	~ClientInfoMgr();
public:
	int Start();

	int open(void*);

	virtual int svc();

	int Stop(); 

public:
	bool AddUser(ACE_SOCK_Stream client,string& xml);
	bool GetUser(enumDVBType DvbType,std::vector<sVedioUserInfo>& uservec);//���������Ƶ�����û�
	bool StopUser(int deviceid);											//ֹͣĳͨ���û�
private:
	std::vector<sVedioUserInfo> ClientVec;
	ACE_Thread_Mutex ClientMutex;
	bool bFlag;
};