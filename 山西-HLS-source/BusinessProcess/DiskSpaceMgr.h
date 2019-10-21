///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����DiskSpaceMgr.h
// �����ߣ�jiangcheng
// ����ʱ�䣺2009-06-18
// �������������̿ռ������
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ace/Task.h"
#include <string>

class DiskSpaceMgr : public ACE_Task<ACE_MT_SYNCH>
{
public:
	DiskSpaceMgr();
	virtual ~DiskSpaceMgr();
public:
	int open(void*);

	virtual int svc();

	int Stop();
private:
	bool bFlag;
	std::string sysCleanTime;	//ÿ���ļ�ϵͳ����ʱ��
	std::string lastRunDate;	//�ϴ����е�ʱ��
	int dbCleanInterval;		//ÿ�ΰ������ݿ�����¼����ʱ��(����)
};