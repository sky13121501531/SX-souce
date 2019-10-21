///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����TaskRealTimeMonitor.h
// �����ߣ�gaoxd
// ����ʱ�䣺2009-10-27
// �����������豸�޹������������
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <list>
#include "ace/Task.h"

class XMLTask;
class TaskRealTimeExecute;

class TaskRealTimeMonitor: public ACE_Task<ACE_MT_SYNCH>
{
public:
	TaskRealTimeMonitor(void);
	~TaskRealTimeMonitor(void);

public:
	int open(void*);

	virtual int svc();

	int Stop();

public:
	bool AddTask(XMLTask* task);
private:
	bool bFlag;
	ACE_Thread_Mutex MonitorMapMutex;

	std::list<TaskRealTimeExecute*> TaskRealTimeExecuteList;
};
