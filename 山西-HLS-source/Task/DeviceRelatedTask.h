///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����DeviceRelatedTask.h
// �����ߣ�jiangcheng
// ����ʱ�䣺2009-05-26
// �������������豸������ص�������
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "XMLTask.h"

class DeviceRelatedTask : public XMLTask
{
public:
	DeviceRelatedTask();
	DeviceRelatedTask(std::string strXML);
	virtual ~DeviceRelatedTask();
public:
	std::string GetBaseObjectName() {return std::string("DeviceRelatedTask");}
	virtual bool Stop(void);			//����ֹͣ����
};