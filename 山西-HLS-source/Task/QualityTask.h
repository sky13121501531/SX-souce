///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����QualityTask.h
// �����ߣ�gaoxd
// ����ʱ�䣺2009-06-25
// ����������ָ��������
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceRelatedTask.h"

class QualityTask : public DeviceRelatedTask
{
public:
	QualityTask();
	QualityTask(std::string strXML);
	virtual ~QualityTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
	virtual void SetDeciveID(int deviceid);
	virtual bool AddTaskXml();
	virtual bool DelTaskXml();
protected:
    bool  GetQualityInfo(std::string orgXML,std::string rtnXML, std::string &Freq,std::string &taskID,std::vector<eQualityInfo> & vecQualityInfo);
	bool ReadyForAlarm(eQualityInfo eQinfo,sCheckParam& alarmVec);
	bool GetQualityInfoXmlFromDevice(int iDevID, std::string strFreq, std::string& strRetXml);
	bool GetBerInfo2ScientificNotation(string sInBer, string sFreq, string sLevel, string& sOutBer);//�� Ber ��Ϣ ת���� ��ѧ������
	bool GetLevelFromOtherDevice(Quality_DevInfo* pQua, std::string strFreq, std::string& sLevel);//�������豸(��ǿ��,Ƶ����) ��ȡ��ǿ��Ϣ
	bool GetLevelFromOtherDevice(Quality_DevInfo* pQua, vector<std::string> vecFreqs, map<std::string, std::string>& mapFreqLevels);//�������豸(��ǿ��,Ƶ����) ��ȡ��ǿ��Ϣ


private:
	std::vector<std::string> mVecStdStr;
	bool m_alarmlimit;//����¼��ָ��¼�Ƶı������޼�¼���
};