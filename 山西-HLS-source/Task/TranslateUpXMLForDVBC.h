///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：TranslateUpXMLForDVBC.h
// 创建者：jiangcheng
// 创建时间：2009-06-04
// 内容描述：将原子任务执行结果的XML转化为应用系统所需的XML
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
	
	//返回实时视频XML
	static std::string TranslateStreamRealtimeQuery(const XMLTask* task);
	//返回轮播查询XML
	static std::string TranslateStreamRoundQuery( const XMLTask* task ,std::string deviceID = "",bool multi = false);
	//返回频道扫描XML
	static std::string TranslateChannelScan(const XMLTask* task,const std::string& strRtnXML);
	//返回频道设置xml
	static std::string TranslateChannelSet(const XMLTask* task);
	//返回视频用户查询XML
	static std::string TranslateClientInfoQuery(const XMLTask* task,std::vector<sVedioUserInfo>& vecClient);
	//PSI/SI查询XML
	static std::string TranslateTableQuery( const XMLTask* task,std::string strFileName);
	//返回录像设置XML
	static std::string TranslateRecord(const XMLTask* task,const std::vector<XMLTask*>& resultVec);
	//返回录像设置XML
	static std::string TranslateRecord(const XMLTask* task,const std::vector<sRecordSetInfo>& recSetInfoVec);
	//返回录像设置查询XML
	static std::string TranslateRecordSetQuery(const XMLTask* task,const std::vector<string>& recSetQueryInfoVec);
	//返回偱切录像设置XML
	static std::string TranslateRoundRecord(const XMLTask* task,const std::vector<XMLTask*>& resultVec);
	//返回指标任务设置xml
	static std::string TranslateQualitySet(const XMLTask* task);
	//返回指标查询XML
	static std::string TranslateQualityQuery( const XMLTask* task,std::map< string,vector<eQualityInfo> > FreqQualityInfo);
	//实时指标查询
	static std::string TranslateQualityRealtimeQuery(const XMLTask* task,std::string retxml);

	//返回报警参数设置xml
	static std::string TranslateAlarmParamSet(const XMLTask* task);
	//返回报警参数查询xml
	static std::string TranslateAlarmParamQuery(const XMLTask* task, const AlarmParamVec& vecAlarmParam);
	//报警方式
	static std::string TranslateAlarmReport(const XMLTask* task);
	//频点报警结果查询
	static std::string TranslateFreqAlarmQuery(const XMLTask* task,std::vector<sCheckParam> vecAlarmResult);
	//节目报警结果查询
	static std::string TranslateProgramAlarmQuery(const XMLTask* task,std::vector<sCheckParam> vecAlarmResult);
	//环境报警结果查询
	static std::string TranslateEnvAlarmQuery(const XMLTask* task,std::vector<sCheckParam> vecAlarmResult);
	//返回通道状态查询XML
	static std::string TranslateDeviceStatusQuery(const XMLTask* task,std::vector<sDeviceStatus> & VecDeviceStatus);
	//返回运行图设置XML
	static std::string TranslateRunPlanSet(const XMLTask* task);
	//返回运行图查询XML
	static std::string TranslateRunPlanCheck(const XMLTask* task, std::string equCode, const RunPlanParamMap& mapRunplanParam);

	//自动录像视频流查询XML
	static std::string TranslateAutoRecordStream(const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime);
	//自动录像文件下载XML
	static std::string TranslateAutoRecordFile(const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime,std::vector<sRecordInfo> vecRecLostTime);
	//任务录像视频流查询XML
	static std::string TranslateTaskRecordStream(const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime);
	//任务录像文件下载XML
	static std::string TranslateTaskRecordFile(const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime);
	//MHP查询XML
	static std::string TranslateMHPQuery(const XMLTask *task, std::string ftp, std::string userName, std::string passWord);
	//指标补偿设置XML
	static std::string TranslateQualityCompensationSet(const XMLTask *task);
	//数据业务分析时间设置XML
	static std::string TranslateAutoAnalysisTimeSet(const XMLTask* task);
	//数据业务分析时间设置查询XML
	static std::string TranslateAutoAnalysisTimeQuery(const XMLTask* task);
	//环境指标查询XML
	static std::string TranslateEnvironmentQuery(const XMLTask* task,EnvMapInfo& EnvMap,std::vector<sCheckParam>& paramVec);
	//手动录像设置XML
	static std::string TranslateManualRecordSet(const XMLTask* task);
	//手动录像主动上报
	static std::string TranslateManualRecordReport(const XMLTask *task,const std::string& strStandXML,std::string URL);
	//EPG电子节目单查询XML
	static std::string TranslateEPGQuery(const XMLTask* task,std::string URL);
	//OSD设置
	static std::string TranslateOSDSet(const XMLTask* task);
	//实时视频流率任务xml
	static std::string TranslateStreamRealtimeRate(const XMLTask* task);
	//录像路数查询
	static std::string TranslateRecordCapabilityQuery(const XMLTask* task);
	//高清音频默认参数设置
	static std::string TranslateHDDefAudioParamSet(const XMLTask* task);
	//音频参数设置
	static std::string TranslateAudioParamSet(const XMLTask* task);

	//报警状态清除 wz_110107
	static std::string TranslateCleanAlarmStateTask(const XMLTask* task);
	//矩阵切换 wz_110107
	static std::string TranslateMatrixQuery(const XMLTask* task);
	//QAM设置 wz_110107
	static std::string TranslateChangeQAMQuery(const XMLTask* task);
	//循切报警设置 wz_110107
	static std::string TranslateLoopAlarmInfo(const XMLTask* task);
	//视频转码录像默认参数设置 wz_110107
	static std::string TranslateRecordParamSet(const XMLTask* task);

	static std::string TranslateAlarmQuery( const XMLTask* task,std::vector<sCheckParam> vecAlarmResult );
	//用户连接停止
	static std::string TranslateStopClient(const XMLTask* task);
 
#ifdef ZONG_JU_ZHAO_BIAO
	//录像路数查询		总局入网：修改接口，添加容器
	static std::string TranslateRecordCapabilityQuery(const XMLTask* task, int newIndexCount, int pessCount, std::string strRecordType, const std::vector<RecChanInfo>& vecRecChanInfo);

	//视频播放停止任务
	static std::string TranslateStopPlayingVideo( const XMLTask* task );
	//小卡卡号查询任务
	static std::string TranslateICInfoQuery( const XMLTask* task,std::vector<CACardInfo> cardinfo );
	//板卡(通道)状态查看任务
	static std::string TranslateNvrStatusQuery(const XMLTask* task, const std::map<int,sTaskInfo>& mapTaskInfo);
	//板卡(通道)设置任务
	static std::string TranslateNvrStatusSet(const XMLTask* task);
	//前端属性配置任务
	static std::string TranslateAgentInfoSet( const XMLTask* task );
	//前端属性配置查询任务
	static std::string TranslateAgentInfoQuery( const XMLTask* task,const std::vector<sAgentSetInfo> &VecAgentSetInfo);
	//前端重起任务
	static std::string TranslateRebootSet( const XMLTask* task, const std::string& strStandXML );
	static std::string TranslateRebootSetToOther( const XMLTask* task, const std::string& strStandXML );

	static std::string TranslateRecVideoTime(const XMLTask* task, const VIDEOTIMEVEC& vecRecVideoTime);

	static std::string TranslateRecordFileLost(const XMLTask* task, const std::vector<sRecordInfo>& vecRecLostTime);

	static std::string TranslateChanEncryptQuery(const XMLTask* task);
	static std::string TranslateEncryptResult(const XMLTask* task);
#endif
	//返回 红外遥控器 虚拟键 XML
	static std::string TranslateInfraredRemoteControlQuery(const XMLTask* task);
	//构造返回XML头信息
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
