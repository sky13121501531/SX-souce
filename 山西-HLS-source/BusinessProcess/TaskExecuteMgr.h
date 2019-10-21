///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����TaskExecuteMgr.h
// �����ߣ�gaoxd
// ����ʱ�䣺2009-11-18
// ��������������ִ�й�����
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ace/Task.h"
#include <list>

class TaskExecute;
class XMLTask;
class TaskExecuteMgr: public ACE_Task<ACE_MT_SYNCH>
{
public:
	TaskExecuteMgr(void);
	~TaskExecuteMgr(void);
public:
	int Init();

	int open(void*);

	virtual int svc();

	int Stop();
public:
	bool SetTask(XMLTask* task);		//����һ������
	bool StopTask();					//ֹͣ��������
	bool HasRunTask();					//�ж��Ƿ������е�����
	bool TaskInExectute(XMLTask* task);	//�жϸ������Ƿ���ִ�ж�����
	bool ShareDevice(XMLTask* task);	//�ж��������ܷ���Ӳ��
	std::string GetExecutePriority();	//�õ�����ͨ���������������ȼ�
	std::string GetExecuteMsgID();	//�õ�����ͨ��������������MsgID
private:
	bool bFlag;
	ACE_Thread_Mutex TaskExecuteMutex;
	std::list<TaskExecute*> TaskExecuteList;
};
