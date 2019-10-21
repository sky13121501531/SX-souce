///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����EPGQuery.h
// �����ߣ�gaoxd
// ����ʱ�䣺2009-06-25
// ����������EPG��ѯ������
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceIndependentTask.h"

class EPGQuery : public DeviceIndependentTask
{
public:
	EPGQuery();
	EPGQuery(std::string strXML);
	virtual ~EPGQuery();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();	
	string CreateEpgScanRet();

public:
	std::string m_strScanTime;
	string m_strDevID;
	string m_strFreq;
};