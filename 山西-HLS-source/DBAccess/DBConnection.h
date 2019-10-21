
#ifndef __DBCONNECTION__
#define __DBCONNECTION__

///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����DBConnection.h
// �����ߣ�gaoxd
// ����ʱ�䣺2009-05-27
// �������������ݿ����������࣬���岻ͬ��������жԲ�ͬ��ĸ��ֲ���
///////////////////////////////////////////////////////////////////////////////////////////

#include "DBParamTypeDef.h"
#include "../Foundation/TypeDef.h"
#include <string>
#include <vector>
class DBConnection
{
public:    
    DBConnection(void);
    virtual ~DBConnection(void) = 0;
public:
	virtual bool InitConnect(void){return true;};//��ʼ�����ݿ�����
	 virtual bool DisConnect(void){return true;};//�Ͽ����ݿ�����
	 virtual bool IsConnected(void){return true;};
public:
	//�������ݿ����
	virtual bool AddXmlTask(eDVBType type,std::string xml){return true;};//���xml����
	virtual bool DeleteTask(eDVBType type,std::string xml){return true;};//ɾ������
	virtual bool QueryTask(eDVBType type,std::vector<std::string>&  vecXml){return true;};//��ѯ����
	virtual bool QueryTask(std::vector<std::string>&  vecXml){return true;};//��ѯ����
	//Ƶ��ɨ�����ݿ����
	virtual bool AddChannel(eDVBType type,std::string channelid,std::string xml){return true;};//���Ƶ����Ϣ
	virtual bool DeleteChannel(eDVBType type,std::string channelid){return true;};//ɾ��Ƶ����Ϣ
	virtual bool DeleteChannelByType(eDVBType type){return true;};//��������ɾ��Ƶ��
	virtual bool QueryChannel(eDVBType type,std::string channelid,std::string& xml){return true;};//�������ͺ�Ƶ��id��ѯƵ����Ϣ
	virtual bool QueryTypeChannel(eDVBType type,std::vector<std::string>&vecXml){return true;};//�������Ͳ�ѯƵ����Ϣ
	virtual bool QueryAllChannel(std::vector<std::string>&vecXml){return true;};//��ѯ����Ƶ����Ϣ
	virtual bool QueryChannelID(eDVBType type,std::vector<std::string>&vecChannelID){return true;};//��ѯ�ض����͵�Ƶ��id
	//¼����Ϣ���ݿ����
	virtual bool AddRecord30V(sRecordInfo record){return true;};//���¼����Ϣ
	virtual bool AddRecord(sRecordInfo record){return true;};//���¼����Ϣ
	virtual bool QueryRecordByTaskid(eDVBType type,std::string taskId,std::vector<sRecordInfo>& vecRecordInfo,std::string StartTime="",std::string EndTime=""){return true;};//���ݼ�����ͺ�����id�����ļ�
	virtual bool QueryRecordByChannelID(eDVBType type,std::string taskid,std::string ChannelID,std::vector<sRecordInfo>& vecRecordInfo,std::string StartTime="",std::string EndTime="",std::string DeviceID=""){return true;};//���ݼ�����ͺ�Ƶ����Ϣ����¼���ļ�
	virtual	bool QueryRecordByDeviceID(eDVBType type,std::string taskid,std::string DeviceID,std::vector<sRecordInfo>& vecRecordInfo,std::string StartTime="",std::string EndTime=""){return true;};  //���ݼ�����ͺ�ͨ����Ϣ����¼���ļ�
	virtual bool UpdateFileEndTime(std::string filename,std::string endtime){return true;};
	virtual bool QueryLostRecordByChannelID(eDVBType type,std::string taskid,std::string ChannelID,std::vector<sRecordInfo>& vecRecordInfo,std::string StartTime="",std::string EndTime="",std::string DeviceID=""){return true;};//���ݼ�����ͺ�Ƶ����Ϣ����¼���ļ�
	virtual	bool GetEarliestStartTimeRecordByChannelIDandDeviceID(eDVBType type,std::string DeviceID,std::string channelID,std::string &EarliestStartTime){return true;};
	//���̹���ʹ�ýӿ�
	virtual bool DeleteRecord(std::string filename){return true;};//�����ļ���ɾ��¼����Ϣ
	virtual bool QueryRecordByDays(unsigned int days,std::vector<std::string>& vecFilename){return true;};//���ݹ���ʱ��ɾ��¼����Ϣ
	virtual bool QueryRecordTop(unsigned int count,std::vector<std::string>& vecFilename){return true;};//��ѯǰ����¼���¼
	virtual bool QueryRecordExpire(std::vector<std::string>& vecFilename){return true;};//��ѯ���ڼ�¼
	virtual bool QueryRecordTopInPath(unsigned int count, std::string recPath, std::vector<std::string>& vecFilename) {return true;}	//��ȡrecPath·����count�����ϵ�¼���ļ�	add by jidushan 11.05.04
	//��������
	virtual bool UpdateAlarmParam(sAlarmParam& alarmparam){return true;};//���±�������
	virtual bool QueryAlarmParam(eDVBType type,std::vector<sAlarmParam>& AlarmParamVec){return true;};//��ѯ��������
	virtual bool QueryAlarmParam(std::vector<sAlarmParam>& AlarmParamVec){return true;};//��ѯ��������
	//������ʽ
	virtual bool AddAlarmInfo(AlarmInfo& alarminfo){return true;};
	virtual bool QueryAlarmInfo(eDVBType type,std::string freq,std::string channelid,std::string typeId,vector<AlarmInfo>& vecAlarmInfo){return true;};
	virtual bool DeleteAlarmInfo(std::string strAlarmTime)	{ return true ; }
	virtual bool DeleteAlarmfromAlarmid(std::string strAlarmid)	{ return true ; }
	//����ͼ����
	virtual bool QueryRunPlanParam(eDVBType type,std::vector<sRunPlanParam>& runPlanParam){return true;};//��ѯָ�����͵�����ͼ����
	virtual bool QueryAllRunPlanParam(std::vector<sRunPlanParam>& runPlanParam){return true;};//��ѯ���е�����ͼ����
	virtual bool UpdateRunPlanParam(eDVBType dvbtype,std::string channelid,std::vector<sRunPlanParam> runPlanParam){return true;};//��������ͼ����
	//ָ����Ϣ
	virtual bool AddQualityInfo(eDVBType dvbtype,std::string freq,std::string taskid,eQualityInfo quality){return true;};//���ָ����Ϣ
	virtual bool AddQualityInfo(const std::string &strInsertSql)	{ return true ; }
	virtual bool QueryQualityInfo(eDVBType dvbtype,std::string freq,std::string taskid,std::vector<eQualityInfo>& vecQuality,std::string startdatetime,std::string enddatetime){return true;};//��ѯָ����Ϣ
	virtual bool DelQualityInfo(std::string strCheckTime)	{return true ; }
	//Ƶ����Ϣ
	virtual bool AddSpectrumInfo(eDVBType dvbtype,std::string freq,std::string taskid,eSpecInfo spec,std::string checktime){return true;};//���Ƶ����Ϣ
	virtual bool QuerySpecInfo(eDVBType dvbtype,std::string taskid,std::vector<eSpecInfo>& vecSpec,std::string startdatetime,std::string enddatetime){return true;};//��ѯƵ����Ϣ
	//MsgID
	virtual bool GetMsgID(std::string &MsgID){return true;};//���MsgID
	virtual bool UpdateMsgID(std::string MsgID){return true;};//����MsgID
	//AnalysisTime
	virtual bool GetAutoAnalysisTime(std::string &AnalysisTime){return true;};//���MsgID
	virtual bool SetAutoAnalysisTime(std::string AnalysisTime){return true;};//����MsgID
	//ָ�겹��
	virtual bool QueryCompensationValu(std::vector<sQualityCompensation>& qualityCmpVec){return true;};//��ѯָ�겹��ֵ
	virtual bool UpdateCompensationValu(sQualityCompensation& qualityCmp){return true;};//��ѯָ�겹��ֵ
	//AlarmID
	virtual bool AddAlarmID(string type,string alarmid){return true;};
	virtual bool QueryAlarmID(string type,string&  alarmid){return true;};
	virtual bool DeleteAlarmID(string type){return true;};
	//�������
	virtual bool AddAlarmInfo(sCheckParam& alarminfo,std::string mode){return true;};
	virtual bool UpdateAlarmInfo(eDVBType type,std::string Alarmid,string mode){return true;};
	virtual bool UpdateAlarmInfo(eDVBType type,std::string Alarmid,string Alarmtime,string mode){return true;};
	virtual bool QueryAlarmInfo(string mode,std::vector<sCheckParam>& vecAlarmInfo){return true;};
	virtual bool QueryAlarmInfo(string deviceid,string channelid,string sttime,string entime,std::vector<sCheckParam>& vecAlarmInfo){return true;};
	virtual bool QueryFreqAlarmID(eDVBType dvbtype,string deviceid,string freq,string typeID,string& alarmid){return true;};
	virtual bool QueryProgramAlarmID(eDVBType dvbtype,string deviceid,string channelid,string typeID,string& alarmid){return true;};
	virtual bool UpdateAlarmRecordExpireTime(eDVBType dvbtype,std::string Alarmid){return true;};
	//wz_101229
	//��ʱ�ļ����
	virtual bool AddTempFile(string filename, string createtime, string expiredays){return true;};
	virtual bool QueryTempFilesExpire( std::vector<std::string>& vecFilename ){return true;};
	virtual bool DeleteTempFile(std::string filename){return true;};
	virtual bool QueryTempFileByDays(unsigned int days,std::vector<std::string>& vecFilename){return true;};
	virtual bool QueryTempFileTop(unsigned int count,std::vector<std::string>& vecFilename){return true;};
	//wz_101229
	//������ѯ2011-03-19
	virtual bool AlarmInfoQuery(vector<sCheckParam> &vecAlarm){return true;};
	//����״̬��� 2011-03-19
	virtual bool AlarmStateClear(string Freq){return true;};

	virtual bool QueryFileConfig(eDVBType dvbtype,std::string NodeName,std::string& NodeValue){return true;};

	virtual bool QuerySystemConfig(eDVBType type, sSysConfigParam &config){return true;};
	virtual bool QueryDvbConfig(eDVBType type, vector<sDvbConfigParam> &VecConfig){return true;};
	virtual bool QueryAlarmLevelThreshold( vector<ThresHold> &VecThresHold){return true;};
	virtual bool SetAlarmLevelThreshold( eDVBType dvbtype,string freq,string alarmthreshold){return true;};
	virtual bool QueryAlarmPriority(std::vector<sAlarmPriority> &vecAlarmPriority){return true;};
	virtual bool QueryScheduleAlarmInfo( string taskid ){return true;};

	virtual bool QueryLastRecordDateTime(std::string strPath, std::string &strDateTime)	{ return true ; }

	virtual bool TruncateTable(std::string strTableName)	{return true ; }

#ifdef ZONG_JU_ZHAO_BIAO
	//���ü�����͵�SrcCode
	virtual bool SetGeneralSrcCode(eDVBType eType, std::string strSrcCode){ return true ; }

	virtual bool SetGeneralUpUr(eDVBType eType, std::string strUpUr){ return true ; }
	//���ü�����͵�SrcCode
	virtual bool SetGeneralDstCode(eDVBType eType, std::string strDstCode){ return true ; }
	//���ñ���url
	virtual bool SetAlarmUrl(eDVBType eDvbtype, std::string strAlarmUrl) {return true;};

	//�����������ֲ��ϱ�URL   //FRee_fanrong_20140117
	virtual bool SetUpStreamRoundUrl(eDVBType eDvbtype, std::string strAlarmUrl) {return true;};

	//����¼�񱣴�·��
	virtual bool SetRecordSavePath(std::string strRecordPath) {return true;};
	//���÷�������־����·��
	virtual bool SetLogSavePath(std::string strLogPath) {return true;};
	//�������ָ�ǰ����־����·��					//FRee_fanrong_20140106
	virtual bool SetVideoLogSavePath(std::string strLogPath) {return true;};    //FRee_fanrong_20140107
	virtual bool SetSILogSavePath(std::string strLogPath) {return true;};		//FRee_fanrong_20140107
	virtual bool SetCASLogSavePath(std::string strLogPath) {return true;};		//FRee_fanrong_20140107
	virtual bool SetSMSLogSavePath(std::string strLogPath) {return true;};		//FRee_fanrong_20140107
	
	//����sms cas�ļ�����·��
	virtual bool SetSmsFilePath(std::string strSmsPath) {return true;};
	virtual bool SetCasFilePath(std::string strCasPath) {return true;};

	//����SI �ļ�����·��
	virtual bool SetSIFilePath(std::string strSIPath) {return true;};

	virtual bool SetHDRecordBps(eDVBType eDvbtype, std::string strBps)	{ return true; }
	virtual bool SetSDRecordBps(eDVBType eDvbtype, std::string strBps)	{ return true; }

	//��ѯĳ��Ƶ��¼���ļ������������ʱ��
	virtual bool QueryRecVideoTime(eDVBType eDvbtype, std::string strChanId, std::string strExpireDay, std::string& strStartTime, std::string& strEndTime,std::string DeviceID=""){return true;}

	//����¼��ķֱ���
	virtual bool SetHDRecordHW(eDVBType eDvbtype, std::string strHeight, std::string strWidth)	{return true ; }
	virtual bool SetSDRecordHW(eDVBType eDvbtype, std::string strHeight, std::string strWidth)	{return true ; }

	//
	virtual bool AgentSetInfo(eDVBType eDvbtype, vector<sAgentSetInfo> &VecAgentSetInfo)	{return true ; }
	virtual bool GetAgentSetInfo(eDVBType eDvbtype, vector<sAgentSetInfo> &VecAgentSetInfo)	{return true ; }
	virtual bool AutoAnalysisSetInfo(string strStartTime,string strPSIType,string strScanType,string strDevID)	{return true ; }
	virtual bool GetAutoAnalysisInfo(string& strStartTime,string& strPSIType,string& strScanType,string& strDevID)	{return true ; }
	virtual bool OSDFormatSetInfo(eDVBType eDvbtype, vector<OSDFormat> &VecOSDFormat)	{return true ; }
	virtual bool GetOSDFormatSetInfo(eDVBType eDvbtype, vector<OSDFormat> &VecOSDFormat)	{return true ; }
	virtual bool AddUploadTask( sRecordUploadTaskInfo uploadtask )	{return true ; }
	virtual bool UpdateUploadTask(string taskid,string curendtime,string newfilepathname,string taskstatus)	{return true ; }
	virtual bool QueryUploadTask( vector<sRecordUploadTaskInfo> &VecUploadtask )	{return true ; }

#endif

	//virtual bool DeleteQualityInfo() {return true ; }
};

#endif
