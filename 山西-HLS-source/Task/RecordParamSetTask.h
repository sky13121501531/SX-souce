#pragma once

#include "DeviceIndependentTask.h"

//��Ƶת��¼��Ĭ�ϲ�������
class RecordParamSetTask : public DeviceIndependentTask
{
public:
	RecordParamSetTask();
	RecordParamSetTask(std::string strXML);
	virtual ~RecordParamSetTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
};