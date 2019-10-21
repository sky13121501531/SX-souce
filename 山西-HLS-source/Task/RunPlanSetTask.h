///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����RunPlanSetTask.h
// �����ߣ�gaoxd
// ����ʱ�䣺2009-06-25
// ��������������ͼ����������
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceIndependentTask.h"
#include "../Foundation/XmlParser.h"
class RunPlanSetTask : public DeviceIndependentTask
{
public:
	RunPlanSetTask();
	RunPlanSetTask(std::string strXML);
	virtual ~RunPlanSetTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
protected:
	bool TransRunplanSet(std::string strXML);
};