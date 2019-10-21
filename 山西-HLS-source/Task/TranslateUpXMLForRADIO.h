///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����TranslateUpXMLForRADIO.h
// �����ߣ�gaoxd
// ����ʱ�䣺2010-04-09
// ������������ԭ������ִ�н����XMLת��ΪӦ��ϵͳ�����XML
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <vector>
#include <string>
#include "./XMLTask.h"
#include "../Foundation/XmlParser.h"

class TranslateUpXMLForRADIO
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
	static std::string TranslateQualitySet( const XMLTask* task );
	static std::string TranslateSpectrumSet(const XMLTask* task);
	static std::string TranslateSpectrumQuery( const XMLTask* task,std::vector<eSpecInfo> vecSpecInfo);

	static std::string TranslateTaskRecordStream(const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime);
	//¼������ add by jidushan 11.04.19
	static std::string TranslateTaskRecordFile( const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime);

	static std::string TranslateQualityQuery( const XMLTask* task,std::map< string,vector<eQualityInfo> > FreqQualityInfo);
	static std::string TranslateClientInfoQuery(const XMLTask* task,std::vector<sVedioUserInfo>& vecClient);
	//ָֹͣ��ͨ���û�
	static std::string TranslateStopClient( const XMLTask* task );
	//��������ͼ����XML
	static std::string TranslateRunPlanSet(const XMLTask* task);
	//����״̬��ѯ
	static std::string TranslateTaskSchedulerQuery( const XMLTask* task,std::vector<sTaskInfo> &taskinfovec );
	//�˹�����״̬��ѯ
	static std::string TranslateManualTaskSchedulerQuery(const  XMLTask* task,std::vector<sTaskInfo> &taskinfovec );
	//������Ϣ��ѯ(�°���:��ѯƵ����)
	static std::string TranslateChannelCheck(const XMLTask* task, const std::vector<sChannelInfo>& vecChanInfo );
	//������Ϣ��ѯ(�°���:��ѯ�Զ�¼��/��)
	static std::string TranslateRecordCheck(const XMLTask* task, const std::map<int,sTaskInfo>& mapTaskInfo);
	//������Ϣ��ѯ(�°���:��ѯ��������)
	static std::string TranslateAlarmParamCheck(const XMLTask* task, const CodeAlarmParamMap& vecAlarmParam);
	//����ͼ��Ϣ��ѯ(�°���)
	static std::string TranslateRunPlanCheck(const XMLTask* task, std::string equCode, const RunPlanParamMap& mapRunplanParam);
	//����ʧЧ����
	static std::string TranslateAlarmExpireTask(const XMLTask* task);

	static std::string TranslateTaskRecordFile(  const XMLTask* task,std::vector<sRecordFileInfo> FileInfo);//�°���¼���ļ����ؽӿ�
private:
	//���췵��XMLͷ��Ϣ
	static std::string GetXmlHeader(const XMLTask* task,std::string retType);
};
