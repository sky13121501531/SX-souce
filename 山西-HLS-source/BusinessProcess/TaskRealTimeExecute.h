///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����TaskRealTimeExecute.h
// �����ߣ�gaoxd
// ����ʱ�䣺2009-10-27
// �����������豸�޹�������ִ����
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ace/Task.h"

class  XMLTask;
class TaskRealTimeExecute: public ACE_Task<ACE_MT_SYNCH>
{
public:
	TaskRealTimeExecute(void);
	~TaskRealTimeExecute(void);
public:
	virtual int open(void*);
	virtual int Stop();

	virtual int svc();

public:
	bool SetTask(XMLTask* task);
	bool CurTaskIsNull(){return curTask == NULL;};
private:
	XMLTask* curTask;	 
};
