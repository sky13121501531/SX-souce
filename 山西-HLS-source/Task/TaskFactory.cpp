///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����TaskFactory.cpp
// �����ߣ�jiangcheng
// ����ʱ�䣺2009-05-25
// �������������񹤳�����
///////////////////////////////////////////////////////////////////////////////////////////
#pragma warning(disable:4996)
#include "TaskFactory.h"
#include "XMLTask.h"
#include "./StreamRealtimeQueryTask.h"
#include "./ChannelScanQueryTask.h"
#include "./RecordSetTask.h"
#include "./RecordSetQueryTask.h"
#include "../Foundation/AppLog.h"
#include "../Foundation/XmlParser.h"
#include "./RecordQueryTask.h"
#include "./StreamRealtimeRoundQueryTask.h"
#include "./ClientInfoQueryTask.h"
#include "./QualitySetTask.h"
#include "./QualityRealtimeQueryTask.h"
#include "./ChannelSetTask.h"
#include "./AlarmParamSetTask.h"
#include "./AlarmParamQueryTask.h"
#include "./DeviceStatusQueryTask.h"
#include "./EquipmentHeartBeatQueryTask.h"
#include "./RunPlanSetTask.h"
#include "./QualityQueryTask.h"
#include "./RecordTask.h"
#include "./RoundRecordSetTask.h"
#include "./RoundRecordTask.h"
#include "./AgentInfoQueryTask.h"
#include "./OSDFormatQueryTask.h"
#include "./QualityTask.h"
#include "./TableQueryTask.h"
#include "./EnvironmentQueryTask.h"
#include "./QualityCompensationSetTask.h"
#include "./ManualRecordSetTask.h"
#include "./EPGQueryTask.h"
#include "./MHPQueryTask.h"
#include "./AlarmParamSetTask.h"
#include "./OSDSetTask.h"
#include "./RecordCapabilityQueryTask.h"
#include "./HDDefAudioParamSetTask.h"
#include "./StreamRealtimeRateTask.h"
#include "./AlarmReportSetTask.h"
#include "./ATVChannelScanQueryTask.h"
#include "./StopClientTask.h"
#include "./ManualTaskSchedulerQuery.h"
#include "./TaskSchedulerQuery.h"
#include "./CleanAlarmStateTask.h"
#include "./MatrixQueryTask.h"
#include "./ChangeQAMQueryTask.h"
#include "./LoopAlarmInfoTask.h"
#include "./RecordParamSetTask.h"
#include "./AlarmQueryTask.h"
#include "./DeviceManageTask.h"
#include "./InfoCheckRequestTask.h"
#include "./RunPlanCheckTask.h"
#include "./AlarmExpireTask.h"
#include "./DeviceInfoQuery.h"
#ifdef ZONG_JU_ZHAO_BIAO
#include "./NvrStatusQueryTask.h"
#include "./NvrStatusSetTask.h"
#include "./ICInfoQueryTask.h"
#include "./AgentInfoSetTask.h"
#include "./RebootSetTask.h"
#include "./GetRecVideoTimeTask.h"
#include "./StopPlayingVideoTask.h"
#include "./ChanEncryptQueryTask.h"
#include "./EncryptCheckTask.h"
#endif
#include "./InfraredRemoteControlQueryTask.h"
#include "./OSDFormatSetTask.h"
#include "GetAgentInfoQueryTask.h"
#include "CameraQuery.h"
#include "LimitedBandwidth.h"
#include "AutoRecordUploadTask.h"
#include "CardDevStatus.h"
#include "ControlCardTimeQuery.h"
#include "SetCardStatusTask.h"

TaskFactory::TaskFactory()
{

}

TaskFactory::~TaskFactory()
{

}

XMLTask* TaskFactory::CreateTask(std::string strStandardXML)
{
	XMLTask *pXMLTask = NULL;
	string TaskName;

	if (strStandardXML.empty()) return NULL;
	ValidateXML(strStandardXML);
	std::string Type;
	std::string DVBType;
	try
	{		
		XmlParser pXP((const char *)strStandardXML.c_str());
		
		std::string RootName = pXP.GetRootNodeName();//���ڵ�����
		pXP.GetAttrNode(pXP.GetRootNode(),"Type",Type);
		pXP.GetAttrNode(pXP.GetRootNode(),"DVBType",DVBType);
		if (RootName == "msg" || RootName == "Msg")
		{
			pXP.GetAttrNode( pXP.GetRootNode(),"TaskType",TaskName);
			if(TaskName.size()<=0)
			{
				if(strStandardXML.find("TableScanQuery")!=-1)
				{
					TaskName = "TableScanQuery";
				}
			}
		}
		else 
		{
			string msg = string("TaskFactory::CreateTaskXML�ļ���ʽ�����޷������������XML�������£�") + strStandardXML;
			//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);
			return NULL;
		}
	}
	catch(...)
	{
		string msg = string("XML�ļ���ʽ�����޷������������XML�������£�") + strStandardXML;
		//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);
		return NULL;
	}

	if (!TaskName.empty() && TaskName.size() > 0)
	{
		if (TaskName == "StreamRealtimeQuery")								//ʵʱ��Ƶ
			pXMLTask = new StreamRealtimeQueryTask(strStandardXML);
		else if (TaskName == "StreamRoundQuery")							//��Ƶ�ֲ�
			pXMLTask = new StreamRealtimeRoundQueryTask(strStandardXML);
		else if (TaskName == "ChannelScanQuery"&&(DVBType=="DVBC"||DVBType=="CTTB"||DVBType=="DVBS"||DVBType=="ATV"||DVBType=="RADIO"||DVBType=="AM"||DVBType=="CTV"))		//Ƶ��ɨ��
			pXMLTask = new ChannelScanQueryTask(strStandardXML);
		else if (TaskName == "ChannelSet")									//Ƶ������
			pXMLTask = new ChannelSetTask(strStandardXML);
		else if(TaskName == "RecordTaskSet")								//��������\�Զ�¼��
			pXMLTask = new RecordSetTask(strStandardXML);	
		else if(TaskName == "RecordTask")									//����¼������
			pXMLTask = new RecordTask(strStandardXML);
		else if (TaskName == "RecordQuery")									//�Զ�\����¼���ѯ\��������
			pXMLTask = new RecordQueryTask(strStandardXML);	
		else if (TaskName == "RecordSetQuery")						//�Զ�¼�����ò�ѯ����
			pXMLTask = new RecordSetQueryTask(strStandardXML);	
		else if (TaskName == "RoundRecordSet")								//����¼����������
			pXMLTask = new RoundRecordSetTask(strStandardXML);
		else if (TaskName == "RoundRecord")									//����¼������
			pXMLTask = new RoundRecordTask(strStandardXML);
		else if (TaskName == "ClientInfoQuery")								//��Ƶ�û���ѯ
			pXMLTask=new ClientInfoQueryTask(strStandardXML);
		else if (TaskName == "QualityRealtimeQuery")				        //ָ��ʵʱ��ѯ
			pXMLTask = new QualityRealtimeQueryTask(strStandardXML);
		else if (TaskName == "QualityTaskSet")								//ָ����������
			pXMLTask=new QualitySetTask(strStandardXML);
		else if(TaskName == "QualityTask")						    		//ָ������
			pXMLTask = new QualityTask(strStandardXML);
		else if (TaskName == "QualityQuery")								//ָ������ѯ
			pXMLTask = new QualityQueryTask(strStandardXML);
		else if (TaskName == "AlarmParamSet")								//������������
			pXMLTask=new AlarmParamSetTask(strStandardXML);
		else if (TaskName == "AlarmParamQuery")								//����������ѯ
			pXMLTask=new AlarmParamQueryTask(strStandardXML);
		else if (TaskName == "DeviceStatusQuery")			                //ͨ��״̬��ѯ
			pXMLTask=new DeviceStatusQueryTask(strStandardXML);
		else if (TaskName == "RunPlanSet")									//����ͼ����
		    pXMLTask = new RunPlanSetTask(strStandardXML);
		else if ((TaskName == "TableQuery")||(TaskName == "TableScanQuery"))									//���ѯ
			pXMLTask = new TableQueryTask(strStandardXML); 
		else if (TaskName == "EnvironmentQuery")							//���л���ָ���ѯ
			pXMLTask = new EnvironmentQueryTask(strStandardXML);            
		else if (TaskName == "QualityCompensationSet")						//ָ�겹������
		    pXMLTask = new QualityCompensationSetTask(strStandardXML);     
		else if (TaskName == "ManualRecordSet")								//�ֶ�¼������
		    pXMLTask = new ManualRecordSetTask(strStandardXML);             
		else if (TaskName == "EPGQuery")									//EPG���ӽ�Ŀ��
		    pXMLTask = new EPGQuery(strStandardXML);                        
		else if (TaskName == "MHPQuery")									//MHP��ѯ
		    pXMLTask = new MHPQueryTask(strStandardXML); 
		else if(TaskName == "AlarmThresholdSet")                            //��������
			pXMLTask=new AlarmParamSetTask(strStandardXML);
		else if(TaskName == "AlarmTypeSet")                            //������ʽ
			pXMLTask=new AlarmReportSetTask(strStandardXML);
		else if (TaskName=="OSDSet")
			pXMLTask=new OSDSetTask(strStandardXML);
		else if(TaskName == "NVRSteamRateSet")
			pXMLTask = new StreamRealtimeRateTask(strStandardXML);
		else if(TaskName == "RecordCapabilityQuery")
			pXMLTask = new RecordCapabilityQueryTask(strStandardXML);
		else if(TaskName == "HDDefAudioParamSet")
			pXMLTask =new HDDefAudioParamSetTask(strStandardXML);
		else if(TaskName == "StopClient")
			pXMLTask = new StopClientTask(strStandardXML);
		else if(TaskName == "TaskSchedulerQuery")
			pXMLTask = new TaskSchedulerQuery(strStandardXML);
		else if(TaskName == "ManualTaskSchedulerQuery")
			pXMLTask = new ManualTaskSchedulerQuery(strStandardXML);
		else if(TaskName == "ClearAlarmState")									//����״̬������� 
			pXMLTask = new CleanAlarmStateTask(strStandardXML);
		else if(TaskName == "MatrixQuery")										//�����л����� 
			pXMLTask = new MatrixQueryTask(strStandardXML);
		else if(TaskName == "ChangeQAMQuery")									//QAM�������� 
			pXMLTask = new ChangeQAMQueryTask(strStandardXML);
		else if(TaskName == "LoopAlaInf")										//ѭ�б����������� 
			pXMLTask = new LoopAlarmInfoTask(strStandardXML);
		else if(TaskName == "RecordParamSet")									//��Ƶת��¼��Ĭ�ϲ������� 
			pXMLTask = new RecordParamSetTask(strStandardXML);
		else if(TaskName == "AlarmQuerySet")									//������ѯ 
			pXMLTask = new AlarmQueryTask(strStandardXML);
		else if(TaskName == "RoundRecordTask")
			pXMLTask = new RoundRecordTask(strStandardXML);
		else if(TaskName == "DeviceManage")
			pXMLTask = new DeviceManageTask(strStandardXML);
		else if (TaskName == "InfoCheckRequest")								//������Ϣ��ѯ����(�°���)
			pXMLTask = new InfoCheckRequestTask(strStandardXML);
		else if (TaskName == "RunPlanCheckRequest")
			pXMLTask = new RunPlanCheckTask(strStandardXML);
		else if(TaskName == "TaskAndAlarmExpireRequest")
			pXMLTask = new AlarmExpireTask(strStandardXML);
		else if(TaskName == "DeviceInfo")
			pXMLTask = new DeviceInfoQuery(strStandardXML);
#ifdef ZONG_JU_ZHAO_BIAO
		else if (TaskName=="NvrStatusQuery")
			pXMLTask = new NvrStatusQueryTask(strStandardXML);
		else if (TaskName=="NvrStatusSet")
			pXMLTask = new NvrStatusSetTask(strStandardXML);
		else if (TaskName=="ICInfoQuery")
			pXMLTask = new ICInfoQueryTask(strStandardXML);
		else if (TaskName=="AgentInfoSet")                                      //ǰ����������
			pXMLTask = new AgentInfoSetTask(strStandardXML);
		else if (TaskName=="AgentInfoQuery")                                    //ǰ���������ò�ѯ
			pXMLTask = new AgentInfoQueryTask(strStandardXML);
		else if (TaskName=="RebootSet")
			pXMLTask = new RebootSetTask(strStandardXML);
		else if (TaskName=="StopPlayingVideo")
			pXMLTask = new StopPlayingVideoTask(strStandardXML);
		else if (TaskName=="GetRecordVideoTime")
			pXMLTask = new GetRecVideoTimeTask(strStandardXML);
		else if (TaskName=="ICInfoChannelEncryptQuery")
			pXMLTask = new ChanEncryptQueryTask(strStandardXML);
		else if (TaskName=="EncryptCheckTask")
			pXMLTask = new EncryptCheckTask(strStandardXML);
#endif
		else if (TaskName == "InfraredRemoteControlQuery")
			pXMLTask= new InfraredRemoteControlQuery(strStandardXML);
		else if (TaskName == "OSDFormatSet")
			pXMLTask= new OSDFormatSetTask(strStandardXML);
		else if (TaskName == "OSDFormatQuery")
			pXMLTask= new OSDFormatQueryTask(strStandardXML);
		else if (TaskName == "GetAgentInfo")
			pXMLTask= new GetAgentInfoQueryTask(strStandardXML);
		else if (TaskName == "CameraQuery")
			pXMLTask= new CameraQueryTask(strStandardXML);
		else if (TaskName == "DevStatus")
			pXMLTask= new CardDevStatusTask(strStandardXML);
		else if (TaskName == "EquipmentHeartBeatQuery")
			pXMLTask= new EquipmentHeartBeatQueryTask(strStandardXML);
		else if (TaskName == "AutoRecordUploadTask")
			pXMLTask= new AutoRecordUploadTask(strStandardXML);
		else if (TaskName == "LimitedBandwidth")
			pXMLTask= new LimitedBandwidth(strStandardXML);
		else if (TaskName == "ControlCardTimeQuery")
			pXMLTask= new ControlCardTimeQuery(strStandardXML);
		else if (TaskName == "SetCardStatus")
			pXMLTask= new SetCardStatusTask(strStandardXML);
		else
			return NULL;
	}

	return pXMLTask;
}
bool TaskFactory::ValidateXML(string& xml)
{
	size_t pos = xml.find("<!--",0);
	while (pos >=0 && pos < xml.length())
	{
		size_t pos2 = xml.find("-->",pos);
		xml.erase(xml.begin()+pos, xml.begin()+pos2+3);
		pos = xml.find("<!--");
	}
	return true;
}

