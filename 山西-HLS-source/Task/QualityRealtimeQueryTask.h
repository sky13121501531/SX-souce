///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����QualityRealtimeQueryTask.cpp
// �����ߣ�gaoxd
// ����ʱ�䣺2009-06-25
// ����������ʵʱָ��������
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceRelatedTask.h"

class QualityRealtimeQueryTask : public DeviceRelatedTask
{
public:
	QualityRealtimeQueryTask();
	QualityRealtimeQueryTask(std::string strXML);
	virtual ~QualityRealtimeQueryTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
	virtual bool IsRealTimeTask(){return true;};	//ʵʱ����

private:
	int CreateTcpDataForRadio(std::string strRtnXML,unsigned char* TcpData, int size_n);
	int CreateTcpDataForATV(std::string strRtnXML,unsigned char* TcpData, int size_n);
	int CreateTcpDataForCTTB(std::string strRtnXML,unsigned char* TcpData,int size_n);

	bool ReadyForAlarm(std::string strXML,std::vector<sCheckParam>& alarmVec);
	string Periodicity;

	//�������豸(��ǿ��,Ƶ����) ��ȡ��ǿ��Ϣ
	bool GetLevelFromOtherDevice(Quality_DevInfo* pQua, std::string strFreq, std::string& sLevel);

private:
	string m_lastLevel;
	int m_tunDeviceID;
};