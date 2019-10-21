#pragma once

#include "DeviceIndependentTask.h"
//QAM����
class ChangeQAMQueryTask : public DeviceIndependentTask
{
public:
	ChangeQAMQueryTask();
	ChangeQAMQueryTask(std::string strXML);
	virtual ~ChangeQAMQueryTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
};