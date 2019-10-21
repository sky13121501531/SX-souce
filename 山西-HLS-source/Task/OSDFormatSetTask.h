#pragma once
#ifndef OSDFORMATSETTASK_H_H
#define OSDFORMATSETTASK_H_H

#include "DeviceIndependentTask.h"
//OSD���� ����
class OSDFormatSetTask : public DeviceIndependentTask
{
public:
	OSDFormatSetTask(void);
	OSDFormatSetTask(std::string strXML);
	virtual ~OSDFormatSetTask(void);
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();

};

#endif