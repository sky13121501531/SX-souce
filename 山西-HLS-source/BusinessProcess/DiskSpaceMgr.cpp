///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����DiskSpaceMgr.cpp
// �����ߣ�jiangcheng
// ����ʱ�䣺2009-06-18
// �������������̿ռ������
///////////////////////////////////////////////////////////////////////////////////////////
#include "DiskSpaceMgr.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/StrUtil.h"
#include "../FileSysAccess/DiskOperation.h"

DiskSpaceMgr::DiskSpaceMgr()
{
	bFlag = false;
	sysCleanTime = PROPMANAGER::instance()->GetSystemCleanTime();//���ÿ�������ʱ��
	dbCleanInterval = StrUtil::Str2Int(PROPMANAGER::instance()->GetDBCleanInterval());
}

DiskSpaceMgr::~DiskSpaceMgr()
{

}

int DiskSpaceMgr::open(void *)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ���̿ռ�����߳̿�ʼִ�� !\n"));
	this->activate();
	return 0;
}

int DiskSpaceMgr::svc()
{
	bFlag = true;

	while (bFlag)
	{
		OSFunction::Sleep(0,20);

		if (!sysCleanTime.empty())
		{
			time_t currentTime = time(0);//��ǰʱ��
			std::string strCurTime = TimeUtil::DateTimeToStr(currentTime);
			std::string strDay = TimeUtil::GetCurDate();
			strDay += " ";
			strDay += sysCleanTime;

			if (abs(TimeUtil::DiffMinute(strCurTime,strDay)) < 3)//������ʱ�����²�����3����
			{
				if (lastRunDate.compare(TimeUtil::GetCurDate()) != 0)
					DISKOPERATION::instance()->ProcessInprireFiles();//��������ļ�
				lastRunDate= TimeUtil::GetCurDate();//�����ϴ�����ʱ��
			}

		}

		DISKOPERATION::instance()->CheckDiskIsFullForRegular(dbCleanInterval*60);//���̿ռ���

	}
	
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ���̿ռ�����߳�ִֹͣ�� !\n"));

	return 0;
}

int DiskSpaceMgr::Stop()
{
	bFlag = false;
	this->wait();
	return 0;
}