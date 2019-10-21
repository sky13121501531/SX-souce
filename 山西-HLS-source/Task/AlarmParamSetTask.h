///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����AlarmParamSetTask.h
// �����ߣ�gaoxd
// ����ʱ�䣺2009-06-25
// ����������������������������
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceIndependentTask.h"

class AlarmParamSetTask : public DeviceIndependentTask
{
public:
	AlarmParamSetTask();
	AlarmParamSetTask(std::string strXML);
	virtual ~AlarmParamSetTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();

private:
	std::string m_strObjectName;
};