///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����StopClientTask.h
// �����ߣ�gaoxd
// ����ʱ�䣺2010-08-05
// ����������ֹͣ��Ƶ�û�������
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceIndependentTask.h"

class StopClientTask : public DeviceIndependentTask
{
public:
	StopClientTask();
	StopClientTask(std::string strXML);
	virtual ~StopClientTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
private:
	int UserDeviceID;
};