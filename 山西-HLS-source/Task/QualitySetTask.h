///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����QualitySetTask.h
// �����ߣ�gaoxd
// ����ʱ�䣺2009-06-25
// ����������ָ������������
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceIndependentTask.h"

class QualitySetTask : public DeviceIndependentTask
{
public:
	QualitySetTask();
	QualitySetTask(std::string strXML);
	virtual ~QualitySetTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
};