///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����DeviceStatusQueryTask.h
// �����ߣ�gaoxd
// ����ʱ�䣺2009-06-22
// ����������ͨ��״̬��ѯ
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once 

#include "DeviceIndependentTask.h"

class DeviceStatusQueryTask : public DeviceIndependentTask
{
public:
	DeviceStatusQueryTask();
	DeviceStatusQueryTask(std::string strXML);
	virtual ~DeviceStatusQueryTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
};