///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����RecordSetTask.h
// �����ߣ�jiangcheng
// ����ʱ�䣺2009-06-01
// �����������Զ�¼������������
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceIndependentTask.h"

class RecordSetTask : public DeviceIndependentTask
{
public:
	RecordSetTask();
	RecordSetTask(std::string strXML);
	virtual ~RecordSetTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
private:
	bool CreateQualityTask(string freq,string CheckInterval,string TunerIndex);
	bool TestTaskIsEffective(std::vector<std::string> vecTaskXml);
};