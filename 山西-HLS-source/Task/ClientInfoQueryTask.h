///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����ClientInfoQueryTask.cpp
// �����ߣ�gaoxd
// ����ʱ�䣺2009-06-25
// ������������Ƶ�û���ѯ������
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceIndependentTask.h"

class ClientInfoQueryTask : public DeviceIndependentTask
{
public:
	ClientInfoQueryTask();
	ClientInfoQueryTask(std::string strXML);
	virtual ~ClientInfoQueryTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
};