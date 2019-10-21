///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����ChannelScanQueryTask.cpp
// �����ߣ�gaoxd
// ����ʱ�䣺2009-06-08
// ����������Ƶ��ɨ��������
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceIndependentTask.h"
using namespace std;
class ChannelScanQueryTask : public DeviceIndependentTask
{
public:
	ChannelScanQueryTask();
	ChannelScanQueryTask(std::string strXML);
	virtual ~ChannelScanQueryTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
	virtual bool IsRealTimeTask(){return true;};	//ʵʱ����
	string m_strScanTime;
	string m_strFreq;
	string m_strDevID;
	string CreatePreChanScanRet();
};