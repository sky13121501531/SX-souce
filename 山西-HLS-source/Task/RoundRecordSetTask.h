///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����RoundRecordSetTask.h
// �����ߣ�gaoxd
// ����ʱ�䣺2011-04-16
// ��������������¼������������
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceIndependentTask.h"

class RoundRecordSetTask : public DeviceIndependentTask
{
public:
	RoundRecordSetTask();
	RoundRecordSetTask(std::string strXML);
	virtual ~RoundRecordSetTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
};