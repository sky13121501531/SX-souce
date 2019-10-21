///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����DownOrderReceive.h
// �����ߣ�jiangcheng
// ����ʱ�䣺2009-05-20
// ��������������Ӧ��ϵͳ�´�����Ļ��࣬�����ʵ�ֲ�ͬ���շ�ʽ
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ace/Task.h"

class DownOrderReceive : public ACE_Task<ACE_MT_SYNCH>
{
public:
	DownOrderReceive();
	virtual ~DownOrderReceive();
public:
	virtual int Init();

	virtual int open(void*);

	virtual int svc();

	virtual int Stop();
protected:
	bool bFlag;
};