///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����ATVChannelScanQueryTask.cpp
// �����ߣ�gaoxd
// ����ʱ�䣺2009-06-08
// ����������Ƶ��ɨ��������
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceRelatedTask.h"
using namespace std;
class ATVChannelScanQueryTask : public DeviceRelatedTask
{
public:
	ATVChannelScanQueryTask();
	ATVChannelScanQueryTask(std::string strXML);
	virtual ~ATVChannelScanQueryTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
};