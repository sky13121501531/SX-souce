
#pragma once
#include <vector>
#include <string>
#include "./XMLTask.h"
#include "../Foundation/XmlParser.h"

class TranslateUpXMLForCTV
{
public:
	//����ʵʱ��ƵXML
	static std::string TranslateStreamRealtimeQuery(const XMLTask* task);
	//����¼������XML
	static std::string TranslateRecord(const XMLTask* task,const std::vector<XMLTask*>& resultVec);
	//�Զ�¼����Ƶ����ѯXML
	static std::string TranslateAutoRecordStream(const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime);
	//Ƶ��ɨ��
	static std::string TranslateChannelScanQuery(const XMLTask* task,std::string strxml);
	//�ֲ�
	static std::string TranslateStreamRound(const XMLTask* task,std::string deviceID);
	//Ƶ������
	static std::string TranslateChannelSet(const XMLTask* task);
	//������������
	static std::string TranslateAlarmParamSet(const XMLTask* task);
	static std::string TranslateSpectrumSet(const XMLTask* task);
	static std::string TranslateQualitySet( const XMLTask* task );
	static std::string TranslateSpectrumQuery( const XMLTask* task,std::vector<eSpecInfo> vecSpecInfo);
	//��ʱ¼��鿴
	static std::string TranslateTaskRecordStream(const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime);
	//¼������ add by jidushan 11.04.19
	static std::string TranslateTaskRecordFile( const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime);
	//ָ���ѯ
	static std::string TranslateQualityQuery( const XMLTask* task,std::map< string,vector<eQualityInfo> > FreqQualityInfo);
	static std::string TranslateClientInfoQuery(const XMLTask* task,std::vector<sVedioUserInfo>& vecClient);
	//ָֹͣ��ͨ���û�
	static std::string TranslateStopClient( const XMLTask* task );
	//��������ͼ����XML
	static std::string TranslateRunPlanSet(const XMLTask* task);
	//����״̬��ѯ
	static std::string TranslateTaskSchedulerQuery( const XMLTask* task,std::vector<sTaskInfo> &taskinfovec );
	//�˹�����״̬��ѯ
	static std::string TranslateManualTaskSchedulerQuery( const XMLTask* task,std::vector<sTaskInfo> &taskinfovec );
private:
	//���췵��XMLͷ��Ϣ
	static std::string GetXmlHeader(const XMLTask* task,std::string retType);
};