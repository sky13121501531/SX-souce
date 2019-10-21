
#include "DeviceManager.h"
#include "OS_Environment.h"
#include "../DeviceAccess/TsFetcherMgr.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/StrUtil.h"

DeviceManager::DeviceManager()
{
	bFlag = false;
	ManageType = "week";				//Ĭ��Ϊÿ������ִ��
	ManageWeekday = "2";				//Ĭ��Ϊÿ�����ڶ�
	ManageSingleday = "2010-05-01";		//Ĭ��...
	ManageTime = "00:00:00";			//Ĭ��Ϊ�賿

	ManageType = PROPMANAGER::instance()->GetDeviceManagetype();
	ManageWeekday = PROPMANAGER::instance()->GetDeviceManageweekday();
	ManageSingleday = PROPMANAGER::instance()->GetDeviceManagesingleday();
	ManageTime = PROPMANAGER::instance()->GetDeviceManagetime();

	if (ManageType == "week")
	{
		meCheckType = PERWEEK;
	}
	else if (ManageType == "day")
	{
		meCheckType = PERDAY;
	}
	else if (ManageType == "single")
	{
		meCheckType = PERSINGLE;
	}
	else
	{
		meCheckType = PERWEEK;
	}
}

DeviceManager::~DeviceManager()
{

}

int DeviceManager::open(void *)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) �豸�����߳̿�ʼִ�� !\n"));
	this->activate();
	return 0;
}

int DeviceManager::svc()
{
	bFlag = true;	
	SetNextRunTime();	//�����´�����ʱ��

	while (bFlag)
	{
		time_t currentTime = time(0);//��ǰʱ��
		std::string strCurTime = TimeUtil::DateTimeToStr(currentTime);
		
		if (abs(TimeUtil::DiffMinute(strCurTime,mNextRunTime)) < 3)//������ʱ�����²�����3����
		{
			RebootDevice();					//�������а忨			
			SetNextRunTime();				//�����´�����ʱ��
			OSFunction::Sleep(60*30);		//ÿ�����һ��ֹͣ30����
		}
		OSFunction::Sleep(60);				// ÿ���Ӽ��һ��
	}

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) �豸�����߳��߳�ִֹͣ�� !\n"));

	return 0;
}

int DeviceManager::Start()
{
	open(0);
	return 0;
};
int DeviceManager::Stop()
{
	bFlag = false;
	this->wait();
	return 0;
}
bool DeviceManager::RebootDevice()
{
	//std::list<int> devicedlist;
	//PROPMANAGER::instance()->GetAllDeviceList(devicedlist);	
	int devnum = PROPMANAGER::instance()->GetCoderDevNum();
	//��ȡȫ��IP
	//for(std::list<int>::iterator ptr=devicedlist.begin();ptr!=devicedlist.end();ptr++)
	//{
	//	TSFETCHERMGR::instance()->RebootCard(*ptr);
	//	OSFunction::Sleep(0,50);
	//}
	return true;
}

bool DeviceManager::SetNextRunTime()
{
	if (meCheckType == PERSINGLE)		//����
	{
		mNextRunTime = ManageSingleday + std::string(" ") + ManageTime;
	}
	else if (meCheckType == PERWEEK)	//ÿ����
	{
		time_t currentTime = time(0);		//��ǰʱ��
		std::string strCurTime = TimeUtil::DateTimeToStr(currentTime);
		long CurWeekday = TimeUtil::DateIsWeekDay(strCurTime);						//��ȡ��ǰ����
		long diffday = StrUtil::Str2Long(ManageWeekday) - CurWeekday;				//���ڵĲ�ֵ

		std::string strCurDate = TimeUtil::CalDay(TimeUtil::GetCurDate(),diffday);
		mNextRunTime = TimeUtil::GetDateFromDatetime(strCurDate) + std::string(" ") + ManageTime;					//������Ӧ�ü���ʱ��
		if (TimeUtil::DiffSecond(mNextRunTime,strCurTime) < 0)						//�����ڼ��ʱ����ڵ�ǰʱ��
		{
			mNextRunTime = TimeUtil::CalDay(mNextRunTime,7);										//�´μ��ʱ��˳��Ϊ������
		}
	}
	else if (meCheckType == PERDAY)		//ÿ��
	{
		time_t currentTime = time(0);		//��ǰʱ��
		std::string strCurTime = TimeUtil::DateTimeToStr(currentTime);
		mNextRunTime = TimeUtil::GetCurDate() + std::string(" ") + ManageTime;	//����Ӧ�ü���ʱ��
		if (TimeUtil::DiffSecond(mNextRunTime,strCurTime) < 0)					//������ʱ����ڵ�ǰʱ��
		{
			mNextRunTime = TimeUtil::CalDay(mNextRunTime,1);									//�´μ��ʱ��˳��Ϊ��һ��
		}
	}
	return true;
}