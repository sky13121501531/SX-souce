#pragma once

#include "DeviceIndependentTask.h"
//������ʽ����
class AlarmReportSetTask : public DeviceIndependentTask
{
public:
	AlarmReportSetTask();
	AlarmReportSetTask(std::string strXML);
	virtual ~AlarmReportSetTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
};