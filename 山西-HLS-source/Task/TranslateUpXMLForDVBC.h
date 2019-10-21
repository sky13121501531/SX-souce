///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����TranslateUpXMLForDVBC.h
// �����ߣ�jiangcheng
// ����ʱ�䣺2009-06-04
// ������������ԭ������ִ�н����XMLת��ΪӦ��ϵͳ�����XML
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <vector>
#include <string>
#include "./XMLTask.h"
#include "../Foundation/XmlParser.h"
#include "../Foundation/TypeDef.h"
class TranslateUpXMLForDVBC
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
	//����¼������XML
	static std::string TranslateRecord(const XMLTask* task,const std::vector<sRecordSetInfo>& recSetInfoVec);
	//����¼�����ò�ѯXML
	static std::string TranslateRecordSetQuery(const XMLTask* task,const std::vector<string>& recSetQueryInfoVec);
	//���؂���¼������XML
	static std::string TranslateRoundRecord(const XMLTask* task,const std::vector<XMLTask*>& resultVec);
	//����ָ����������xml
	static std::string TranslateQualitySet(const XMLTask* task);
	//����ָ���ѯXML
	static std::string TranslateQualityQuery( const XMLTask* task,std::map< string,vector<eQualityInfo> > FreqQualityInfo);
	//ʵʱָ���ѯ
	static std::string TranslateQualityRealtimeQuery(const XMLTask* task,std::string retxml);

	//���ر�����������xml
	static std::string TranslateAlarmParamSet(const XMLTask* task);
	//���ر���������ѯxml
	static std::string TranslateAlarmParamQuery(const XMLTask* task, const AlarmParamVec& vecAlarmParam);
	//������ʽ
	static std::string TranslateAlarmReport(const XMLTask* task);
	//Ƶ�㱨�������ѯ
	static std::string TranslateFreqAlarmQuery(const XMLTask* task,std::vector<sCheckParam> vecAlarmResult);
	//��Ŀ���������ѯ
	static std::string TranslateProgramAlarmQuery(const XMLTask* task,std::vector<sCheckParam> vecAlarmResult);
	//�������������ѯ
	static std::string TranslateEnvAlarmQuery(const XMLTask* task,std::vector<sCheckParam> vecAlarmResult);
	//����ͨ��״̬��ѯXML
	static std::string TranslateDeviceStatusQuery(const XMLTask* task,std::vector<sDeviceStatus> & VecDeviceStatus);
	//��������ͼ����XML
	static std::string TranslateRunPlanSet(const XMLTask* task);
	//��������ͼ��ѯXML
	static std::string TranslateRunPlanCheck(const XMLTask* task, std::string equCode, const RunPlanParamMap& mapRunplanParam);

	//�Զ�¼����Ƶ����ѯXML
	static std::string TranslateAutoRecordStream(const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime);
	//�Զ�¼���ļ�����XML
	static std::string TranslateAutoRecordFile(const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime,std::vector<sRecordInfo> vecRecLostTime);
	//����¼����Ƶ����ѯXML
	static std::string TranslateTaskRecordStream(const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime);
	//����¼���ļ�����XML
	static std::string TranslateTaskRecordFile(const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime);
	//MHP��ѯXML
	static std::string TranslateMHPQuery(const XMLTask *task, std::string ftp, std::string userName, std::string passWord);
	//ָ�겹������XML
	static std::string TranslateQualityCompensationSet(const XMLTask *task);
	//����ҵ�����ʱ������XML
	static std::string TranslateAutoAnalysisTimeSet(const XMLTask* task);
	//����ҵ�����ʱ�����ò�ѯXML
	static std::string TranslateAutoAnalysisTimeQuery(const XMLTask* task);
	//����ָ���ѯXML
	static std::string TranslateEnvironmentQuery(const XMLTask* task,EnvMapInfo& EnvMap,std::vector<sCheckParam>& paramVec);
	//�ֶ�¼������XML
	static std::string TranslateManualRecordSet(const XMLTask* task);
	//�ֶ�¼�������ϱ�
	static std::string TranslateManualRecordReport(const XMLTask *task,const std::string& strStandXML,std::string URL);
	//EPG���ӽ�Ŀ����ѯXML
	static std::string TranslateEPGQuery(const XMLTask* task,std::string URL);
	//OSD����
	static std::string TranslateOSDSet(const XMLTask* task);
	//ʵʱ��Ƶ��������xml
	static std::string TranslateStreamRealtimeRate(const XMLTask* task);
	//¼��·����ѯ
	static std::string TranslateRecordCapabilityQuery(const XMLTask* task);
	//������ƵĬ�ϲ�������
	static std::string TranslateHDDefAudioParamSet(const XMLTask* task);
	//��Ƶ��������
	static std::string TranslateAudioParamSet(const XMLTask* task);

	//����״̬��� wz_110107
	static std::string TranslateCleanAlarmStateTask(const XMLTask* task);
	//�����л� wz_110107
	static std::string TranslateMatrixQuery(const XMLTask* task);
	//QAM���� wz_110107
	static std::string TranslateChangeQAMQuery(const XMLTask* task);
	//ѭ�б������� wz_110107
	static std::string TranslateLoopAlarmInfo(const XMLTask* task);
	//��Ƶת��¼��Ĭ�ϲ������� wz_110107
	static std::string TranslateRecordParamSet(const XMLTask* task);

	static std::string TranslateAlarmQuery( const XMLTask* task,std::vector<sCheckParam> vecAlarmResult );
	//�û�����ֹͣ
	static std::string TranslateStopClient(const XMLTask* task);
 
#ifdef ZONG_JU_ZHAO_BIAO
	//¼��·����ѯ		�ܾ��������޸Ľӿڣ��������
	static std::string TranslateRecordCapabilityQuery(const XMLTask* task, int newIndexCount, int pessCount, std::string strRecordType, const std::vector<RecChanInfo>& vecRecChanInfo);

	//��Ƶ����ֹͣ����
	static std::string TranslateStopPlayingVideo( const XMLTask* task );
	//С�����Ų�ѯ����
	static std::string TranslateICInfoQuery( const XMLTask* task,std::vector<CACardInfo> cardinfo );
	//�忨(ͨ��)״̬�鿴����
	static std::string TranslateNvrStatusQuery(const XMLTask* task, const std::map<int,sTaskInfo>& mapTaskInfo);
	//�忨(ͨ��)��������
	static std::string TranslateNvrStatusSet(const XMLTask* task);
	//ǰ��������������
	static std::string TranslateAgentInfoSet( const XMLTask* task );
	//ǰ���������ò�ѯ����
	static std::string TranslateAgentInfoQuery( const XMLTask* task,const std::vector<sAgentSetInfo> &VecAgentSetInfo);
	//ǰ����������
	static std::string TranslateRebootSet( const XMLTask* task, const std::string& strStandXML );
	static std::string TranslateRebootSetToOther( const XMLTask* task, const std::string& strStandXML );

	static std::string TranslateRecVideoTime(const XMLTask* task, const VIDEOTIMEVEC& vecRecVideoTime);

	static std::string TranslateRecordFileLost(const XMLTask* task, const std::vector<sRecordInfo>& vecRecLostTime);

	static std::string TranslateChanEncryptQuery(const XMLTask* task);
	static std::string TranslateEncryptResult(const XMLTask* task);
#endif
	//���� ����ң���� ����� XML
	static std::string TranslateInfraredRemoteControlQuery(const XMLTask* task);
	//���췵��XMLͷ��Ϣ
	static std::string GetXmlHeader(const XMLTask* task,std::string retType);

	static std::string TranslateOSDFormatSetQuery(const XMLTask* task);

	static std::string TranslateOSDFormatQuery(const XMLTask* task,const std::vector<OSDFormat> &VecOSDFormat);

	static std::string TranslateGetAgentInfoQuery(const XMLTask* task,string QueryLevel);
	//
	static std::string TranslateCardDevStatusQuery(const XMLTask* task);
	//
	static std::string TranslateCameraQuery(const XMLTask* task,const std::string& strStandXML);
	//
	static std::string TranslateEquipmentHeartBeatQuery(const XMLTask* task);
	//
	static std::string TranslateAutoRecordUploadTask(const XMLTask* task);
	//
	static std::string TranslateLimitedBandwidth(const XMLTask* task);
	//
	static std::string TranslateControlCardTimeQuery(const XMLTask* task);
	//
	static std::string TranslateSetCardStatusTask(const XMLTask* task);
private:
};
