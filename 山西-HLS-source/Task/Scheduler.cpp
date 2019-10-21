///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����Scheduler.h
// �����ߣ�jiangcheng
// ����ʱ�䣺2009-05-26
// ������������ʱ����ʱ�䴦����
///////////////////////////////////////////////////////////////////////////////////////////
#include <time.h>
#include "Scheduler.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/AppLog.h"


Scheduler::Scheduler(void)
{
	ExpiredDateTime = time(0)-1;//����������Ϊ���ڣ��������SetRunTime()��ʼ��ʱ�䣬������ֱ�ӹ���
}

Scheduler::~Scheduler(void)
{
}

void Scheduler::SetRunTime(std::string startDateTime, std::string endDateTime,std::string expiredDateTime,long cycleInterval,long waitInterval,long runDuration)
{
	StartDateTime = TimeUtil::StrToDateTime(startDateTime);
	EndDateTime = TimeUtil::StrToDateTime(endDateTime);

	if (StartDateTime >= EndDateTime)
	{
		string msg = "����ʱ�����ô���,����Ϊ��������";
		//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);

		ExpiredDateTime = time(0)-1;//����������Ϊ����
		return;
	}

	ExpiredDateTime = TimeUtil::StrToDateTime(expiredDateTime);

	CycleInterval = cycleInterval;
	WaitInterval = waitInterval;
	RunDuration = runDuration;

	//���õ�ǰ����ʱ��
	CurrentStartDateTime = StartDateTime; //��һ������Ϊ���ÿ�ʼʱ����ȴ�ʱ��
	CurrentEndDateTime = CurrentStartDateTime + RunDuration;

	if (RunDuration == 0 || CurrentEndDateTime > EndDateTime)
	{
		CurrentEndDateTime = EndDateTime;
	}

	SetNextRunTime(true);
	return;
}
void Scheduler::SetNextRunTime(bool firstset)
{
	if (EndDateTime <= time(0) && CycleInterval > 0)//���񵥴ν���
	{
		do 
		{
			time_t TempRunDuration = EndDateTime - StartDateTime;
			StartDateTime = EndDateTime + CycleInterval;
			EndDateTime = EndDateTime + CycleInterval +TempRunDuration;
		} 
		while (EndDateTime <= time(0));

		CurrentStartDateTime = StartDateTime;
		CurrentEndDateTime = CurrentStartDateTime + RunDuration;

		if (RunDuration == 0 || CurrentEndDateTime > EndDateTime)
		{
			CurrentEndDateTime = EndDateTime;
		}
	}
	else
	{
		if (CurrentStartDateTime < time(0))
		{
			CurrentStartDateTime = time(0);
		}
		if (firstset == false)
		{
			CurrentStartDateTime = CurrentStartDateTime +  RunDuration  + WaitInterval;
		}

		CurrentEndDateTime = CurrentStartDateTime + RunDuration;

		if (RunDuration == 0 || CurrentEndDateTime > EndDateTime)
		{
			CurrentEndDateTime = EndDateTime;
		}
		if (CurrentStartDateTime >= EndDateTime)
		{
			CurrentStartDateTime = EndDateTime;
		}
	}
	string info = string("�´�ִ��ʱ�䣺") + TimeUtil::DateTimeToStr(CurrentStartDateTime) + string("--") + TimeUtil::DateTimeToStr(CurrentEndDateTime);
	//SYSMSGSENDER::instance()->SendMsg(info);
}
void Scheduler::SetNextStartDateTime()
{
	CurrentStartDateTime += CycleInterval;
}
void Scheduler::ModifyExpiredTime(std::string expiredDateTime)
{
	ExpiredDateTime = TimeUtil::StrToDateTime(expiredDateTime);
}
bool Scheduler::IsRunNow()
{
	return (CurrentStartDateTime <= time(0) && CurrentEndDateTime >= time(0));
}
bool Scheduler::IsExpired()
{
	return ExpiredDateTime <= time(0);
}