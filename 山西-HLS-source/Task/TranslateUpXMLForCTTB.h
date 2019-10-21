///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����TranslateUpXMLForCTTB.h
// �����ߣ�jiangcheng
// ����ʱ�䣺2009-06-04
// ������������ԭ������ִ�н����XMLת��ΪӦ��ϵͳ�����XML
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <vector>
#include <string>
#include "./XMLTask.h"
#include "../Foundation/XmlParser.h"

class TranslateUpXMLForCTTB
{
public:
	
	//����ʵʱ��ƵXML
	static std::string TranslateStreamRealtimeQuery(const XMLTask* task);
	//�����ֲ���ѯXML
	static std::string TranslateStreamRoundQuery( const XMLTask* task ,std::string deviceID = "",bool multi = false);
	//����Ƶ��ɨ��XML
	static std::string TranslateChannelScan(const XMLTask* task,const std::string& strRtnXML);
	//����Ƶ������xml
	static std::string TranslateChannelSet(const XMLTask* task);
	//������Ƶ�û���ѯXML
	static std::string TranslateClientInfoQuery(const XMLTask* task,std::vector<sVedioUserInfo>& vecClient);
	//PSI/SI��ѯXML
	static std::string TranslateTableQuery( const XMLTask* task,std::string strFileName);
	//����¼������XML
	static std::string TranslateRecord(const XMLTask* task,const std::vector<XMLTask*>& resultVec);
	
	//����ָ����������xml
	static std::string TranslateQualitySet(const XMLTask* task);
	//����ָ���ѯXML
	static std::string TranslateQualityQuery( const XMLTask* task,std::map< string,vector<eQualityInfo> > FreqQualityInfo);

	//���ر�����������xml
	static std::string TranslateAlarmParamSet(const XMLTask* task);
	//����ͨ��״̬��ѯXML
	static std::string TranslateDeviceStatusQuery(const XMLTask* task,std::vector<sDeviceStatus> & VecDeviceStatus);
	//��������ͼ����XML
	static std::string TranslateRunPlanSet(const XMLTask* task);

	//�Զ�¼����Ƶ����ѯXML
	static std::string TranslateAutoRecordStream(const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime);
	//�Զ�¼���ļ�����XML
	static std::string TranslateAutoRecordFile(const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime);
	//����¼����Ƶ����ѯXML
	static std::string TranslateTaskRecordStream(const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime);
	//����¼���ļ�����XML
	static std::string TranslateTaskRecordFile(const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime);
	//EPG��ѯXML
	static std::string TranslateEPGQuery(const XMLTask* task,std::string URL);
	//ʵʱָ���ѯ
	static std::string TranslateQualityRealtimeQuery(const XMLTask* task,std::string strXml);
	//����ҵ�����ʱ������XML
	static std::string TranslateAutoAnalysisTimeSet(const XMLTask* task);
	//����ָ���ѯXML
	static std::string TranslateEnvironmentQuery(const XMLTask* task,EnvMapInfo& EnvMap,std::vector<sCheckParam>& paramVec);
	//MHP��ѯXML
	static std::string TranslateMHPQuery(const XMLTask *task, std::string ftp, std::string userName, std::string passWord);
	//ָ�겹������XML
	static std::string TranslateQualityCompensationSet(const XMLTask *task);
	//�ֶ�¼������
	static std::string TranslateManualRecordSet(const XMLTask* task);
	//�ֶ�¼�������ϱ�
	static std::string TranslateManualRecordReport(const XMLTask *task,const std::string& strStandXML,std::string URL);
private:
	//���췵��XMLͷ��Ϣ
	static std::string GetXmlHeader(const XMLTask* task,std::string retType);
};
