///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����RecordSetQueryTask.h
// �����ߣ�jsp
// ����ʱ�䣺2017-07-28
// �����������Զ�¼�����ò�ѯ������
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceIndependentTask.h"

class RecordSetQueryTask : public DeviceIndependentTask
{
public:
	RecordSetQueryTask();
	RecordSetQueryTask(std::string strXML);
	virtual ~RecordSetQueryTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();

private:
	std::string m_strObjectName;
};