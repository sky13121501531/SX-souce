///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����EnvironmentChecher.h
// �����ߣ�gaoxd
// ����ʱ�䣺2009-09-15
// �������������������
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ace/Task.h"
#include "ace/Singleton.h"

class EnvironmentChecher : public ACE_Task<ACE_MT_SYNCH>
{
public:
	EnvironmentChecher();
	~EnvironmentChecher();
public:
	int Start();

	int Stop();

	int open(void*);

	virtual int svc();
	
private:
	bool bFlag;
};

typedef  ACE_Singleton<EnvironmentChecher,ACE_Mutex>  ENVIRONMENTCHECHER;