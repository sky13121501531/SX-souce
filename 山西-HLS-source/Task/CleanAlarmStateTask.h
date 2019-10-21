#pragma once

#include "DeviceIndependentTask.h"
//����״̬���
class CleanAlarmStateTask : public DeviceIndependentTask
{
public:
	CleanAlarmStateTask();
	CleanAlarmStateTask(std::string strXML);
	virtual ~CleanAlarmStateTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
};