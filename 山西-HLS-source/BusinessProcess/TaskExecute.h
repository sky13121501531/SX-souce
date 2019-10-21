///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����TaskExecute.h
// �����ߣ�gaoxd
// ����ʱ�䣺2009-05-27
// ��������������ִ����
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ace/Task.h"
#include <string>

class  XMLTask;
class TaskExecute: public ACE_Task<ACE_MT_SYNCH>
{
public:
	TaskExecute(void);
	~TaskExecute(void);
public:
	virtual int open(void*);
	virtual int Stop();

	virtual int svc();

public:
	bool SetTask(XMLTask* task);
	bool TaskIsStop();

	XMLTask* GetCurTask();

	bool StopTask();
	bool IsShareTask();
	std::string GetTaskFreq();
	std::string GetDeviceXml();
	std::string GetPriority();
	std::string GetMsgID();

private:
	ACE_Thread_Mutex TasMutex;
	XMLTask* curTask;	 
};
