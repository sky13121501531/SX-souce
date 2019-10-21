///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����QualityQueryTask.h
// �����ߣ�gaoxd
// ����ʱ�䣺2009-06-22
// ����������ָ������ѯ
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once 

#include "DeviceIndependentTask.h"

class QualityQueryTask : public DeviceIndependentTask
{
public:
	QualityQueryTask();
	QualityQueryTask(std::string strXML);
	virtual ~QualityQueryTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
};