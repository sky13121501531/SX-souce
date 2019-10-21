///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����Scheduler.h
// �����ߣ�jiangcheng
// ����ʱ�䣺2009-05-26
// ������������ʱ����ʱ�䴦����
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <string>
#include "../Foundation/TypeDef.h"

/************************************************************************
������SetRunTime()��ʼ��ʱ�����

************************************************************************/

class Scheduler
{
    //Constructors
public:
    Scheduler(void);
    ~Scheduler(void);
protected:
	 Scheduler(Scheduler &self);//������������

public:
	void SetRunTime(std::string startDateTime, std::string endDateTime=TimeUtil::GetEndLessTime(),std::string expiredDateTime=TimeUtil::GetEndLessTime(),long cycleInterval=0,long waitInterval=0,long runDuration=0);
	void SetNextRunTime(bool firstset = false);

	void SetNextStartDateTime();
	void ModifyExpiredTime(std::string expiredDateTime);

	bool IsRunNow();		//�ж��Ƿ�Ӧ������
	bool IsExpired();		//�ж��Ƿ�Ӧ�ù���

public:
	time_t GetStartDateTime(){return StartDateTime;};
	time_t GetEndDateTime(){return EndDateTime;};
	long GetCycleInterval(){return CycleInterval;};

private:
	time_t StartDateTime;		//��ʼʱ��
	time_t EndDateTime;			//����ʱ��

	time_t CurrentStartDateTime;//��ǰ��ʼʱ��	
	time_t CurrentEndDateTime;	//��ǰ����ʱ��
	time_t ExpiredDateTime;		//����ʱ��

	long CycleInterval;		//���ڼ��(���ν���ʱ�� ���´ο�ʼʱ��ļ��)
	long WaitInterval;		//�ȴ����
	long RunDuration;		//�������г���ʱ��
};
