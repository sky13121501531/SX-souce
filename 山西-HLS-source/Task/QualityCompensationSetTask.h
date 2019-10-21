///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����QualityCompensationSetTask.h
// �����ߣ�zhangyc
// ����ʱ�䣺2009-11-27
// ����������ָ�겹������������
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceIndependentTask.h"

class QualityCompensationSetTask : public DeviceIndependentTask
{
public:
	QualityCompensationSetTask();
	QualityCompensationSetTask(std::string strXML);
	virtual ~QualityCompensationSetTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
};