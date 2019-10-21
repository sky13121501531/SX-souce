///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����AgentInfoQueryTask.h
// �����ߣ�jsp
// ����ʱ�䣺2017-08-1
// ����������ǰ���������ò�ѯ������
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceIndependentTask.h"

class AgentInfoQueryTask : public DeviceIndependentTask
{
public:
	AgentInfoQueryTask();
	AgentInfoQueryTask(std::string strXML);
	virtual ~AgentInfoQueryTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();

private:
	std::string m_strObjectName;
};