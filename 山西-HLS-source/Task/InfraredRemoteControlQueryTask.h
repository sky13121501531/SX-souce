///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����InfraredRemoteControlQueryTask.h
// ����ʱ�䣺2016-03-01
// ��������������ң���� �����������
///////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DeviceIndependentTask.h"

class InfraredRemoteControlQuery : public DeviceIndependentTask
{
public:
	InfraredRemoteControlQuery();
	InfraredRemoteControlQuery(std::string strXML);
	virtual ~InfraredRemoteControlQuery();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();	
};