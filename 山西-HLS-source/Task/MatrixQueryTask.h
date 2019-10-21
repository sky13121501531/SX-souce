#pragma once

#include "DeviceIndependentTask.h"

//�����л�
class MatrixQueryTask : public DeviceIndependentTask
{
public:
	MatrixQueryTask();
	MatrixQueryTask(std::string strXML);
	virtual ~MatrixQueryTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
};