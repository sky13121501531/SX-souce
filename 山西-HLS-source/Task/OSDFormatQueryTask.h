///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����OSDFormatQueryTask.h
// �����ߣ�jsp
// ����ʱ�䣺2017-08-1
// ����������OSD�������ò�ѯ������
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceIndependentTask.h"

class OSDFormatQueryTask : public DeviceIndependentTask
{
public:
	OSDFormatQueryTask();
	OSDFormatQueryTask(std::string strXML);
	virtual ~OSDFormatQueryTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();

private:
	std::string m_strObjectName;
};