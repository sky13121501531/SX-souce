///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����AlarmParamQueryTask.h
// �����ߣ�jsp
// ����ʱ�䣺2017-07-26
// ��������������������ѯ������
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceIndependentTask.h"

class AlarmParamQueryTask : public DeviceIndependentTask
{
public:
	AlarmParamQueryTask();
	AlarmParamQueryTask(std::string strXML);
	virtual ~AlarmParamQueryTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();

private:
	std::string m_strObjectName;
};