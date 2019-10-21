///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����ChannelSetTask.cpp
// �����ߣ�gaoxd
// ����ʱ�䣺2009-07-27
// ����������Ƶ������������
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceIndependentTask.h"

class ChannelSetTask : public DeviceIndependentTask
{
public:
	ChannelSetTask();
	ChannelSetTask(std::string strXML);
	virtual ~ChannelSetTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
};