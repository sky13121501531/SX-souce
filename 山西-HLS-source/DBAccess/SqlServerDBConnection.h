#pragma once
#include "Ado.h"
#include "DBConnection.h"
#include "DBParamTypeDef.h"

class SqlServerDBConnection : public DBConnection
{
public:
	SqlServerDBConnection(void);
	~SqlServerDBConnection(void);
public:
	bool InitConnect(void);//��ʼ��sqlserver���ݿ�����
	bool DisConnect(void);//�Ͽ�sqlserver���ݿ�����
	bool IsConnected(void);
public:
	bool AddXmlTask(eDVBType type,std::string xml);//���xml����
	bool DeleteTask(eDVBType type,std::string xml);//ɾ��xml����
	bool QueryTask(eDVBType type,std::vector<std::string>& vecXml);//��ѯ����
	bool QueryTask(std::vector<std::string>& vecXml);//��ѯ����
	//Ƶ��ɨ�����ݿ����
	bool AddChannel(eDVBType type,std::string channelid,std::string xml);//���Ƶ����Ϣ
	bool DeleteChannel(eDVBType type,std::string channelid);//ɾ��Ƶ����Ϣ
	bool DeleteChannelByType( eDVBType type);  //ɾ�������͵�Ƶ��
	bool QueryChannel(eDVBType type,std::string channelid,std::string& xml);//��ѯָ�����ͺ�Ƶ��id��Ƶ����Ϣ
	bool QueryAllChannel(std::vector<std::string>&vecXml);//��ѯ����Ƶ����Ϣ
	bool QueryTypeChannel(eDVBType type,std::vector<std::string>&vecXml);//��ѯָ�����͵�Ƶ����Ϣ
	bool QueryChannelID(eDVBType type,std::vector<std::string>&vecChannelID);//��ѯָ�����͵�Ƶ��id
	//¼����Ϣ���ݿ����
	bool AddRecord30V(sRecordInfo record);
	bool AddRecord(sRecordInfo record);//���¼����Ϣ
	//���ݼ�����ͺ�����id�����ļ�
    bool QueryRecordByTaskid(eDVBType type,std::string taskId,std::vector<sRecordInfo>& vecRecordInfo,std::string StartTime="",std::string EndTime="");
	//���ݼ�����ͺ�ts����Ϣ����¼���ļ�
	bool QueryRecordByChannelID(eDVBType type,std::string taskID,std::string channelID,std::vector<sRecordInfo>& vecRecordInfo,std::string StartTime="",std::string EndTime="",std::string DeviceID="");
	//���ݼ�����ͺ�ͨ����Ϣ����¼���ļ�
	bool QueryRecordByDeviceID(eDVBType type,std::string taskid,std::string DeviceID,std::vector<sRecordInfo>& vecRecordInfo,std::string StartTime="",std::string EndTime="");
	bool UpdateFileEndTime(std::string filename,std::string endtime);
	bool QueryLostRecordByChannelID(eDVBType type,std::string taskID,std::string channelID,std::vector<sRecordInfo>& vecRecordInfo,std::string StartTime="",std::string EndTime="",std::string DeviceID="");
	bool GetEarliestStartTimeRecordByChannelIDandDeviceID(eDVBType type,std::string DeviceID,std::string channelID,std::string &EarliestStartTime);//ͨ����ӦDeviceID  and ChanneID ��ȡ���ݿ��Ƶ������¼��ʱ��
	//���̹���ʹ�ýӿ�
	bool DeleteRecord(std::string filename);//�����ļ���ɾ��¼����Ϣ
	bool QueryRecordByDays(unsigned int days,std::vector<std::string>& vecFilename);//���ݹ���ʱ����¼����Ϣ
	bool QueryRecordTop(unsigned int count,std::vector<std::string>& vecFilename);//��ѯǰ����¼���¼
	bool QueryRecordExpire(std::vector<std::string>& vecFilename);//��ѯ���ڼ�¼
	bool QueryRecordTopInPath(unsigned int count, std::string recPath, std::vector<std::string>& vecFilename);	//��ȡrecPath·����count�����ϵ�¼���ļ�	add by jidushan 11.05.04
	//��������
	bool UpdateAlarmParam(sAlarmParam& alarmparam);//���±�������
	bool QueryAlarmParam(eDVBType type,std::vector<sAlarmParam>& AlarmParamVec);//��ѯ��������
	bool QueryAlarmParam(std::vector<sAlarmParam>& AlarmParamVec);//��ѯ��������
	//�������
	bool AddAlarmInfo(sCheckParam& alarminfo,std::string mode);
	bool UpdateAlarmInfo(eDVBType dvbtype,std::string Alarmid,string mode);
	bool UpdateAlarmInfo(eDVBType type,std::string Alarmid,string Alarmtime,string mode);
	bool QueryAlarmInfo(string mode,std::vector<sCheckParam>& vecAlarmInfo);
	bool QueryAlarmInfo(string deviceid,string channelid,string sttime,string entime,std::vector<sCheckParam>& vecAlarmInfo);
	bool QueryFreqAlarmID(eDVBType dvbtype,string deviceid,string freq,string typeID,string& alarmid);
	bool QueryProgramAlarmID(eDVBType dvbtype,string deviceid,string channelid,string typeID,string& alarmid);
	bool DeleteAlarmInfo(std::string strAlarmTime);
	bool DeleteAlarmfromAlarmid(std::string strAlarmid);
	bool UpdateAlarmRecordExpireTime(eDVBType dvbtype,std::string Alarmid);
	//����ͼ����
	bool AddRunPlanParam(sRunPlanParam runPlanParam);//�������ͼ����
	bool QueryRunPlanParam(eDVBType type,std::vector<sRunPlanParam>& runPlanParam);//��ѯָ�����͵�����ͼ����
	bool QueryAllRunPlanParam(std::vector<sRunPlanParam>& runPlanParam);//��ѯ���е�����ͼ����
	bool DeleteRunPlanParam(eDVBType type,std::string channelid);//ɾ��ָ�����͵�����ͼ����
	bool UpdateRunPlanParam(eDVBType dvbtype,std::string channelid,std::vector<sRunPlanParam> runPlanParam);//��������ͼ����
	//ָ����Ϣ
	bool AddQualityInfo(eDVBType dvbtype,std::string freq,std::string taskid,eQualityInfo quality);//���ָ����Ϣ
	bool AddQualityInfo(const std::string &strInsertSql);
	bool QueryQualityInfo(eDVBType dvbtype,std::string freq,std::string taskid,std::vector<eQualityInfo>& vecQuality,std::string startdatetime,std::string enddatetime);//��ѯָ����Ϣ
	bool DelQualityInfo(std::string strCheckTime);
	//Ƶ����Ϣ
	bool AddSpectrumInfo(eDVBType dvbtype,std::string freq,std::string taskid,eSpecInfo spec,std::string checktime);//���Ƶ����Ϣ
	bool QuerySpecInfo(eDVBType dvbtype,std::string taskid,std::vector<eSpecInfo>& vecSpec,std::string startdatetime,std::string enddatetime);//��ѯƵ����Ϣ
	//MsgID
	bool GetMsgID(std::string &MsgID);//���MsgID
	bool UpdateMsgID(std::string MsgID);//����MsgID
	//ָ�겹��
	bool QueryCompensationValu(std::vector<sQualityCompensation>& qualityCmpVec);//��ѯָ�겹��ֵ
	bool UpdateCompensationValu(sQualityCompensation& qualityCmp);//��ѯָ�겹��ֵ
	bool AddCompensationValu(sQualityCompensation& qualityCmp);//���ָ�겹��ֵ
	bool DelCompensationValu(sQualityCompensation& qualityCmp);//ɾ��ָ�겹��ֵ

	//wz_101229
	//��ʱ�ļ������غ���
	bool AddTempFile(string filename, string createtime, string expiredays);
	bool QueryTempFilesExpire( std::vector<std::string>& vecFilename );
	bool DeleteTempFile(std::string filename);
	bool QueryTempFileByDays(unsigned int days,std::vector<std::string>& vecFilename);
	bool QueryTempFileTop(unsigned int count,std::vector<std::string>& vecFilename);
	//wz_101129
	//������ѯ 2011-03-19
	bool AlarmInfoQuery(vector<sCheckParam> &vecAlarm);
	bool AlarmStateClear(string Freq);
	//������Ϣ��ѯ
	bool QueryFileConfig(eDVBType dvbtype,std::string NodeName,std::string& NodeValue);

	bool QuerySystemConfig(eDVBType dvbtype, sSysConfigParam &config);
	bool QueryDvbConfig( eDVBType type,vector<sDvbConfigParam> &VecConfig);
	bool QueryAlarmLevelThreshold( vector<ThresHold> &VecThresHold);
	bool SetAlarmLevelThreshold( eDVBType dvbtype,string freq,string alarmthreshold);
	bool QueryAlarmPriority(std::vector<sAlarmPriority> &vecAlarmPriority);
	bool QueryScheduleAlarmInfo( string taskid );

	bool QueryLastRecordDateTime(std::string strFilePath,std::string &strDateTime);

	bool TruncateTable(std::string strTableName);

#ifdef ZONG_JU_ZHAO_BIAO

public:
	//���ü�����͵�SrcCode
	bool SetGeneralSrcCode(eDVBType eType, std::string strSrcCode);
	//���ü�����͵�DestCode
	bool SetGeneralDstCode(eDVBType eType, std::string strSrcCode);
	//���ü�����͵�SrcCode
	bool SetGeneralUpUr(eDVBType eType, std::string strUpUr);

	//���ñ���url
	bool SetAlarmUrl(eDVBType eDvbtype, std::string strAlarmUrl);
	//�����������ֲ��ϱ�URL   //FRee_fanrong_20140117
	bool SetUpStreamRoundUrl(eDVBType eDvbtype, std::string strStreamRoundUrl);
	//����¼��·��
	bool SetRecordSavePath(std::string strRecordPath);
	//������־����·��
	bool SetLogSavePath(std::string strLogdPath);

	//�������ָ�ǰ����־����·��    //FRee_fanrong_20140106
	bool SetVIDEOLogSavePath(std::string strLogPath) ;	//FRee_fanrong_20140106
	bool SetSILogSavePath(std::string strLogPath) ;		//FRee_fanrong_20140106
	bool SetCASLogSavePath(std::string strLogPath);		//FRee_fanrong_20140106
	bool SetSMSLogSavePath(std::string strLogPath) ;	//FRee_fanrong_20140106

	//����cas sms si�ļ�����·��
	bool SetSmsFilePath(std::string strSmsPath);
	bool SetCasFilePath(std::string strCasPath);
	bool SetSIFilePath(std::string strCasPath);

	//����¼�������
	bool SetHDRecordBps(eDVBType eDvbtype, std::string strBps);
	bool SetSDRecordBps(eDVBType eDvbtype, std::string strBps);
	//����¼��ķֱ���
	bool SetHDRecordHW(eDVBType eDvbtype, std::string strHeight, std::string strWidth);
	bool SetSDRecordHW(eDVBType eDvbtype, std::string strHeight, std::string strWidth);

	//��ѯĳ��Ƶ��¼���ļ������������ʱ��
	bool QueryRecVideoTime(eDVBType eDvbtype, std::string strChanId, std::string strExpireDay, std::string& strStartTime, std::string& strEndTime,std::string DeviceID="");

	//
	bool SetAutoAnalysisTime(std::string strTime );
	bool GetAutoAnalysisTime(std::string& strTime );
	//
	bool AgentSetInfo(eDVBType eDvbtype,vector<sAgentSetInfo> &VecAgentSetInfo);
	bool GetAgentSetInfo(eDVBType eDvbtype,vector<sAgentSetInfo> &VecAgentSetInfo);
	bool AutoAnalysisSetInfo(string strStartTime,string strPSIType,string strScanType,string strDevID);
	bool GetAutoAnalysisInfo(string& strStartTime,string& strPSIType,string& strScanType,string& strDevID);
	bool OSDFormatSetInfo(eDVBType eDvbtype, vector<OSDFormat> &VecOSDFormat);	
	bool GetOSDFormatSetInfo(eDVBType eDvbtype, vector<OSDFormat> &VecOSDFormat);
	bool AddUploadTask( sRecordUploadTaskInfo uploadtask );
	bool UpdateUploadTask(string taskid,string curendtime,string newfilepathname,string taskstatus);
	bool QueryUploadTask( vector<sRecordUploadTaskInfo> &VecUploadtask );
#endif


private:
	CADODatabase* DBCon;
	CADODatabase* BandDBCon;
	bool BandDBOK;
};
