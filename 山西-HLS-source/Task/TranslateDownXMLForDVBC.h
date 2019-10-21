///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����TranslateDownXMLForDVBC.h
// �����ߣ�jiangcheng
// ����ʱ�䣺2009-06-04
// �������������·�XMLת��Ϊ���������XML
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <vector>
#include <string>
#include "../Foundation/XmlParser.h"
#include "../Foundation/Config.h"


class TranslateDownXMLForDVBC
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
	
	//���ر�׼�Ă���¼����������XML
	static bool TranslateRoundRecordSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//���ر�׼�Ă���¼��ִ������XML
	static bool TranslateRoundRecordTask(const std::string& strOriginalXML,std::vector<std::string>& strStandardXMLVec);
	
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
	//���ر�׼�ı��ѯ����xml
	static bool TranslateTableQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//���ر�׼������ͼ��������xml
	static bool TranslateRunPlanSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//���ر�׼������ͼ��ѯ����xml
	static bool TranslateRunPlanCheck(const std::string& strOriginalXML,std::string& strStandardXML);
	//���ر�׼���Զ�¼�����ò�ѯ����xml
	static bool TranslateRecordSetQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//���ر�׼���ֶ�¼������xml
	static bool TranslateManualRecordSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//���ر�׼������ҵ�����ʱ����������xml
	static bool TranslateAutoAnalysisTimeSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//���ر�׼������ҵ�����ʱ���ѯ����xml
	static bool TranslateAutoAnalysisTimeQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//EPG���ӽ�Ŀ����ѯ
	static bool TranslateEPGQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//MHP���ݲ�ѯ
	static bool TranslateMHPQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//����ָ���ѯ
	static bool TranslateEnvironmentQualityQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//���ر�׼��Ƶ��ɨ������xml
	static bool TranslateSpectrumScan(const std::string& strOriginalXML,std::string& strStandardXML);
	//����ָ�겹����׼xml
	static bool TranslateQualityCompensationSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//���ر����������ñ�׼xml
	static bool TranslateAlarmParamSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//���ر���������ѯ��׼xml
	static bool TranslateAlarmParamQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//�����ϱ���ʽ
	static bool TranslateAlarmReportSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//������ѯ
	static bool TranslateAlarmQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//OSD����
	static bool TranslateOSDSet(const std::string& strOriginalXML,std::string& strStandardXML);
	static bool TranslateStreamRealtimeRate(const std::string& strOriginalXML,std::string& strStandardXML);
	//¼��·����ѯ
	static bool TranslateRecordCapabilityQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//������ƵĬ�ϲ�������
	static bool TranslateHDDefAudioParamSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//��Ƶ��������
	static bool TranslateAudioParamSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//����״̬��� 
	static bool TranslateClearAlarmState(const std::string& strOriginalXML,std::string& strStandardXML);
	//�����л�
	static bool TranslateMatrixQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//QAM����
	static bool TranslateChangeQAMQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//ѭ�б�������  (ҳ��xml��ӿ��ĵ���ͬ-->AlaType������)
	static bool TranslateLoopAlarmInfo(const std::string& strOriginalXML,std::string& strStandardXML);
	//��Ƶת��¼��Ĭ�ϲ������� (ҳ��xml��ӿ��ĵ���ͬ-->xml�ṹ�Ͳ�һ��)
	static bool TranslateRecordParamSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//�û������ж�
	static bool TranslateStopClient(const std::string& strOriginalXML,std::string& strStandardXML);

#ifdef ZONG_JU_ZHAO_BIAO
	//��Ƶ����ֹͣ����
	static bool TranslateStopPlayingVideo(const std::string& strOriginalXML,std::string& strStandardXML);
	//С�����Ų�ѯ����
	static bool TranslateICInfoQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//�忨(ͨ��)��������
	static bool TranslateNvrStatusSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//ǰ��������������
	static bool TranslateAgentInfoSet(const std::string& strOriginalXML,std::string& strStandardXML);
	//ǰ���������ò�ѯ����
	static bool TranslateAgentInfoQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//ǰ����������
	static bool TranslateRebootSet(const std::string& strOriginalXML,std::string& strStandardXML);
	
	//�忨״̬�鿴
	static bool TranslateNvrStatusQuery(const std::string& strOriginalXML,std::string& strStandardXML);
	//ǰ��������������
	static bool TranslateGetRecVideoTime(const std::string& strOriginalXML,std::string& strStandardXML);
	//С����Ŀ��Ϣ��Ȩ��ѯ����
	static bool TranslateChanEncryptQuery(const std::string& strOriginalXML,std::string& strStandardXML);
#endif
	//���� ����ң���� ����� ��׼XML
	static bool TranslateInfraredRemoteControlQuery(
		const std::string& strOriginalXML, std::string& strStandardXML);
	//���� OSD���� ��׼XML
	static bool TranslateOSDFormatSetQuery(const std::string& strOriginalXML, std::string& strStandardXML);	
	//���� OSD���ò�ѯ ��׼XML
	static bool TranslateOSDFormatQuery(const std::string& strOriginalXML, std::string& strStandardXML);	
	//���� ǰ����Ϣ��ѯ ��׼XML
	static bool TranslateGetAgentInfoQuery(const std::string& strOriginalXML, std::string& strStandardXML);
	//
	static bool TranslateCardDevStatusQuery(const std::string& strOriginalXML, std::string& strStandardXML);
	//
	static bool TranslateCameraQuery(const std::string& strOriginalXML, std::string& strStandardXML);
	//
	static bool TranslateEquipmentHeartBeatQuery(const std::string& strOriginalXML, std::string& strStandardXML);
	//
	static bool TranslateAutoRecordUploadTask(const std::string& strOriginalXML, std::string& strStandardXML);
	//
	static bool TranslateLimitedBandwidth(const std::string& strOriginalXML, std::string& strStandardXML);
	//
	static bool TranslateControlCardTimeQueryTask(const std::string& strOriginalXML, std::string& strStandardXML);
	//
	static bool TranslateSetCardStatusTask(const std::string& strOriginalXML, std::string& strStandardXML);
};