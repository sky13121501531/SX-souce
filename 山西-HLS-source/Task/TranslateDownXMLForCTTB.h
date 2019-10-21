///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����TranslateDownXMLForCTTB.h
// �����ߣ�jiangcheng
// ����ʱ�䣺2009-06-04
// �������������·�XMLת��Ϊ���������XML
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <vector>
#include <string>
#include "../Foundation/XmlParser.h"
//���нӿڴ��ⲿ������ʲô���ļ�����ͣ��ӿ��ڲ��ж���Ӧ�ļ�����Ͳ���ӵ���Ӧ��ͳһ�ӿڵ� DownType ������

class TranslateDownXMLForCTTB
{
public:
	//���ر�׼��Ƶ��ɨ������XML
	static bool TranslateChannelScan(const std::string& strOriginalXML,std::string& strStandardXML);
	//���ر�׼��ʵʱ��Ƶ����XML
	static bool TranslateStreamRealtimeQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//���ر�׼����Ƶ�ֲ�����XML
	static bool TranslateStreamRoundQuery(const std::string& strOriginalXML,std::string& strStandardXML,bool multi = false);
	//���ر�׼��¼����������XML
	static bool TranslateRecordSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//���ر�׼��¼��ִ������XML
	static bool TranslateRecordTask(const std::string& strOriginalXML,std::vector<std::string>& strStandardXMLVec);
	//���ر�׼��¼���ѯ������XML
	static bool TranslateRecordQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//���ر�׼��ʵʱָ���ѯXML
	static bool TranslateQualityRealtimeQuery( const std::string& strOriginalXML,std::string& strStandardXML);
	//���ر�׼�Ķ�ʱָ����������xml
	static bool TranslateQualityTaskSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//���ر�׼�Ķ�ʱָ��ִ������xml
	static bool TranslateQualityTask(const std::string& strOriginalXML,std::vector<std::string>& strStandardXMLVec);
	//���ر�׼�Ķ�ʱָ������ѯ����xml
	static bool TranslateQualityQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//���ر�׼��Ƶ����������xml
	static bool TranslateChannelSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//���ر�׼�Ŀͻ���Ϣ��ѯ����xml
	static bool TranslateClientInfoQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//���ر�׼��ͨ��״̬��ѯ����xml
	static bool TranslateDeviceStatusQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//���ر�׼��PSI/SI��ѯ����xml
	static bool TranslateTableQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//���ر�׼������ͼ��������xml
	static bool TranslateRunPlanSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//���ر�׼���ֶ�¼������xml
	static bool TranslateManualRecordSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//���ر�׼�ı�����������xml
	static bool TranslateAlarmParamSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//���ر�׼��EPG��ѯxml
	static bool TranslateEPGQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//����ָ�겹����׼xml
	static bool TranslateQualityCompensationSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//����ָ���ѯ
	static bool TranslateEnvironmentQualityQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//���ر�׼������ҵ�����ʱ����������xml
	static bool TranslateAutoAnalysisTimeSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//MHP���ݲ�ѯ
	static bool TranslateMHPQuery(const std::string& strOriginalXML,std::string& strStandardXML);
};