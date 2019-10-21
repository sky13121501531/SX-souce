///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����DeviceManager.h
// �����ߣ�gaoxd
// ����ʱ�䣺2011-04-22
// �����������豸������������
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ace/Task.h"
#include "../Foundation/TypeDef.h"
#include <string>

class DeviceManager : public ACE_Task<ACE_MT_SYNCH>
{
public:
	DeviceManager();
	~DeviceManager();
public:
	int Start();
	int Stop();
	int open(void*);
	virtual int svc();

private:
	bool SetNextRunTime();
	bool RebootDevice();
	
private:
	bool bFlag;

	std::string ManageType;				//ManageType ȡֵ��Χ week,day,single;
	std::string ManageMonthday;
	std::string ManageWeekday;
	std::string ManageSingleday;
	std::string ManageTime;

	std::string mNextRunTime;	
	eCheckType meCheckType;
};