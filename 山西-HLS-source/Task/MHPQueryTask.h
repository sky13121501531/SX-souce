///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����MHPQueryTask.h
// �����ߣ�zhangyc
// ����ʱ�䣺2009-11-27
// ����������MHP��ѯ������
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceIndependentTask.h"

class MHPQueryTask : public DeviceIndependentTask
{
public:
	MHPQueryTask();
	MHPQueryTask(std::string strXML);
	virtual ~MHPQueryTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
};