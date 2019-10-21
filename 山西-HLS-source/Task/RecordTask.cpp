///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����RecordTask.cpp
// �����ߣ�jiangcheng
// ����ʱ�䣺2009-06-01
// ����������¼��������
///////////////////////////////////////////////////////////////////////////////////////////
#include "RecordTask.h"
#include "TranslateDownXML.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/PropManager.h"
#include <vector>
#include <iostream>
#include "ace/Task.h"
#include "ace/OS.h"
#include "../Foundation/TypeDef.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../FileSysAccess/TSRecorder.h"
#include "Scheduler.h"
#include "../Foundation/XmlParser.h"
#include "../DeviceAccess/TsFetcherMgr.h"
#include "../DeviceAccess/DeviceAccessMgr.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include "../Foundation/AppLog.h"
#include "../Task/TranslateXMLForDevice.h"
#include "../DBAccess/DBManager.h"
using namespace std;
extern int g_realstreamusedev;
class Record_Task : public ACE_Task<ACE_MT_SYNCH>
{
public:
	Record_Task();
	~Record_Task(){};

};

Record_Task::Record_Task()
{
	msg_queue()->high_water_mark(188*10000);
	msg_queue()->low_water_mark(188*1000);
}

RecordTask::RecordTask() : DeviceRelatedTask()
{

}

RecordTask::RecordTask(std::string strXML) : DeviceRelatedTask(strXML)
{
	m_bIsHDProgram = false;
	XmlParser parser;
	parser.Set_xml(strStandardXML);
	//
	std::string version;
	pXMLNODE rootNode = parser.GetNodeFromPath("Msg");
	parser.GetAttrNode(rootNode,"Version",version);            //��ñ�׼xml��RealtimeQueryTask�ڵ��Freq����ֵ
	//
	pXMLNODE recordNode;
	if(strStandardXML.find("AutoRecord")!=string::npos)
	{
		recordNode=parser.GetNodeFromPath("Msg/AutoRecord/Record");
	}
	else if(strStandardXML.find("TaskRecord")!=string::npos)
	{
		recordNode=parser.GetNodeFromPath("Msg/TaskRecord/Record");
	}
	string OrgNetID, TsID, ServiceID,VideoPID,AudioPID,Code,Bps;
	string dayofweek,starttime,endtime,startdatetime,enddatetime,sWidth,sHeight,cdevid,tdevid;
	string strTChassisID="",strTPosition="",strTChanId="",strCChassisID="",strCPosition="",strCModuleID="",strCChanId="";
	if(recordNode)
	{
		parser.GetAttrNode(recordNode,"TaskID",TaskID);
		parser.GetAttrNode(recordNode,"DeviceID",DeviceID);
		parser.GetAttrNode(recordNode,"Freq",Freq);
		parser.GetAttrNode(recordNode,"OrgNetID",OrgNetID);
		parser.GetAttrNode(recordNode,"TsID",TsID);
		parser.GetAttrNode(recordNode,"ServiceID",ServiceID);
		parser.GetAttrNode(recordNode,"VideoPID",VideoPID);
		parser.GetAttrNode(recordNode,"AudioPID",AudioPID);
		parser.GetAttrNode(recordNode,"Code",Code);
		parser.GetAttrNode(recordNode,"DayofWeek",dayofweek);
		parser.GetAttrNode(recordNode,"StartTime",starttime);
		parser.GetAttrNode(recordNode,"EndTime",endtime);
		parser.GetAttrNode(recordNode,"StartDateTime",startdatetime);
		parser.GetAttrNode(recordNode,"EndDateTime",enddatetime);
		parser.GetAttrNode(recordNode,"Width",sWidth);
		parser.GetAttrNode(recordNode,"Height",sHeight);
		parser.GetAttrNode(recordNode,"CDeviceID",cdevid);  
		parser.GetAttrNode(recordNode,"TDeviceID",tdevid);  
		parser.GetAttrNode(recordNode,"CChassisID",strCChassisID);
		parser.GetAttrNode(recordNode,"CPosition",strCPosition);
		parser.GetAttrNode(recordNode,"CModuleID",strCModuleID);
		parser.GetAttrNode(recordNode,"CTranscode",strCChanId);
		parser.GetAttrNode(recordNode,"TChassisID",strTChassisID);
		parser.GetAttrNode(recordNode,"TPosition",strTPosition);
		parser.GetAttrNode(recordNode,"TDemodulate",strTChanId);
		parser.GetAttrNode(recordNode,"Bps",Bps);
		//
		strServiceID = ServiceID;
		strVideoPID = VideoPID;
		strAudioPID = AudioPID;
		//
	}
	CHANNELINFOMGR::instance()->GetChannelID(DVBType,OrgNetID,TsID,ServiceID,VideoPID,AudioPID,Code,ChannelID);

//#ifdef ZONG_JU_ZHAO_BIAO
	m_bIsHDProgram =  CHANNELINFOMGR::instance()->IsHDTV(DVBType, OrgNetID, TsID, ServiceID);
//#endif
	if(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")
	{
		if (StrUtil::Str2Int(sWidth)>720||StrUtil::Str2Int(sHeight)>576)
		{
			m_bIsHDProgram = true;
		}
	}
	else
	{
		if (StrUtil::Str2Int(sWidth)>640||StrUtil::Str2Int(sHeight)>480)
		{
			m_bIsHDProgram = true;
		}
	}
	if(ChannelID.length()<1)
	{
		ChannelID=Freq;
	}
	CHANNELINFOMGR::instance()->GetChanCodeByChanId(DVBType, ChannelID, m_strChannelName);

	TaskScheduler=new Scheduler();//�����ִ��ʱ�����
	//��������ִ������ʱ��
	if (dayofweek.length()==0)
	{
		if (TaskID=="0")//�Զ�¼��
		{
			TaskScheduler->SetRunTime(TimeUtil::GetCurDateTime());
		}
		else//�������е�����¼��
		{
			TaskScheduler->SetRunTime(startdatetime,enddatetime,enddatetime);
		}
	}
	else
	{
		long diffSec=TimeUtil::StrToSecondTime(endtime)-TimeUtil::StrToSecondTime(starttime);
		//�������
		
		if(dayofweek=="ALL"||dayofweek=="All")//ÿ������
		{
			std::string StartDateTime=TimeUtil::GetCurDate()+" "+starttime;
			std::string EndDateTime=TimeUtil::GetCurDate()+" "+endtime;
			long cycleSec=ONEDAYLONG-diffSec;
			TaskScheduler->SetRunTime(StartDateTime,EndDateTime,TimeUtil::GetEndLessTime(),cycleSec);//���õ�һ������ʱ��
		}
		else//һ�ܵ�ĳ������
		{
			std::string dateTime,startDate;
			long today=TimeUtil::DateIsWeekDay(time(0));//�������ڼ�
			int diffDay=StrUtil::Str2Int(dayofweek)-today;//����֮�����������
			if (diffDay<0)
			{
				diffDay+=7;
			}
			dateTime=TimeUtil::CalDay(time(0),diffDay);
			startDate=TimeUtil::GetDateFromDatetime(dateTime);
			std::string StartDateTime=startDate+" "+starttime;
			std::string EndDateTime=startDate+" "+endtime;
			long cycleSec=ONEDAYLONG*7-diffSec;
			TaskScheduler->SetRunTime(StartDateTime,EndDateTime,TimeUtil::GetEndLessTime(),cycleSec);//���õ�һ������ʱ��
		}
		
	}
	//if (DeviceID == 0)		//��ʼ��ͨ��
	//{
	//	DeviceID = -1;// ����Ϊ-1�����Ȼ����һ��ͨ��
	//}
	//
	if(version == "3.0")
	{
		//
		if(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")//���»�ȡ�߼�ͨ��
		{
			if(cdevid.size()<=0)
			{
				tdevid = StrUtil::Int2Str(PROPMANAGER::instance()->GetTunerDeviceID(DVBC,StrUtil::Str2Int(strTChassisID),StrUtil::Str2Int(strTPosition),StrUtil::Str2Int(strTChanId)));
				vector<int> coddevlis;
				PROPMANAGER::instance()->SeparateStrVec(strCChanId,coddevlis);
				cdevid = "";
				for(int i=0;i<coddevlis.size();i++)
				{
					if(cdevid.size() == 0)
					{
						cdevid = StrUtil::Int2Str(PROPMANAGER::instance()->GetCoderDeviceID(StrUtil::Str2Int(strCChassisID),StrUtil::Str2Int(strCPosition),StrUtil::Str2Int(strCModuleID),coddevlis[i]));
						parser.SetAttrNode( "TDeviceID",tdevid,recordNode );	    //����TsQuery�ڵ��OrgNetID����
						parser.SetAttrNode( "CDeviceID",cdevid,recordNode);			    //����TsQuery�ڵ��TsID����
						parser.SaveToString(strStandardXML);
					}
					else
					{
						cdevid += ";";
						cdevid += StrUtil::Int2Str(PROPMANAGER::instance()->GetCoderDeviceID(StrUtil::Str2Int(strCChassisID),StrUtil::Str2Int(strCPosition),StrUtil::Str2Int(strCModuleID),coddevlis[i]));
					}
				}
			}
		}
		//
		vector<int> codlis;
		PROPMANAGER::instance()->SeparateStrVec(cdevid,codlis);
		for(int i=0;i<codlis.size();i++)
		{
			if(i == 0)
			{
				if(PROPMANAGER::instance()->SetCoderLinkTunerinfo(codlis[i],StrUtil::Str2Int(tdevid),DVBType,Freq,ServiceID,m_bIsHDProgram,Bps,"¼�������..."))
				{
					DeviceID = codlis[i];
				}
				else
				{
					break;
				}
			}
			else
			{
				PROPMANAGER::instance()->FlagCoderInfo(codlis[i],Freq,ServiceID,DVBType,m_bIsHDProgram,"¼�������...",StrUtil::Int2Str(DeviceID));
			}
		}
	}
}
RecordTask::~RecordTask()
{
}
void RecordTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]¼������ִ�� !\n",DeviceID));
	bRun = true;
	SetRunning();
	if(g_realstreamusedev == DeviceID)
	{
		g_realstreamusedev = -1;
	}
	strDeviceXML = TranslateXMLForDevice::TranslateRecordTask(strStandardXML );//�Զ����׼XML��Ӳ���ӿ�XMLת��
	
	ACE_Task<ACE_MT_SYNCH>* Task = new Record_Task;
	TSFETCHERMGR::instance()->SetTsDeviceXml(DeviceID,strDeviceXML);
	TSFETCHERMGR::instance()->SetRecordTask(DeviceID,Task);
	TSFETCHERMGR::instance()->IncreaseTaskNum(DeviceID);
	//����ģ�����ز�����ģ���¼����Ϣ
	sSignalCheck param;
	if(DVBType==ATV||DVBType==RADIO||DVBType==AM||DVBType==CTV)
	{	
		param.DeviceID	= StrUtil::Int2Str(DeviceID);
		param.dvbtype	= DVBType;
		param.Freq		= Freq;
		param.ChannelID	= ChannelID;
		//CHECKLEVELFORUNLOCK::instance()->AddRecordInfo(param);
	}

	TSRecorder record(DeviceID,strStandardXML);

	//int BufLen = 188*4096;

	//if (DVBType == RADIO  || DVBType==AM)
	//{
	//	BufLen   /= 16;
	//}

	//unsigned char* TsBuf = new unsigned char[BufLen];//������,UDPTsFetcher���Ѿ���188*800�Ļ���˴�������Ҫ����,����¼���ļ������ݿ�ļ�¼ʱ���OSDʱ��ƫ�����,������2��
	//memset(TsBuf,0,BufLen);//��ʼ��������
	//
	//size_t PacketLen = 0;

	int nNoDataCount = 0;
	bool bHasData = true;
	while(bRun == true && IsRunning())
	{
		try
		{
			ACE_Message_Block *mb = NULL;
			ACE_Time_Value OutTime(ACE_OS::gettimeofday()+ACE_Time_Value(5));
			if (Task->getq(mb,&OutTime) != -1 && mb != NULL)//�����ݽ����߳�ȡ����
			{
				if (!bHasData)
				{
					bHasData = true;
					std::string msg = "ͨ����";
					msg += StrUtil::Int2Str(DeviceID);
					msg += " �������ݳɹ�!!";
					APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,msg,LOG_OUTPUT_FILE);
				}

				if (mb->length() > 0)
				{
					record.SaveFile((unsigned char*)mb->rd_ptr(),mb->length());//����д���ļ�
					//if (PacketLen>=BufLen-mb->length())
					//{
					//	record.SaveFile(TsBuf,PacketLen);//����д���ļ�
					//	memset(TsBuf,0,BufLen);
					//	PacketLen = 0;
					//}
				
					//memcpy(TsBuf+PacketLen,(unsigned char*)mb->rd_ptr(),mb->length());//�������ڴ�
					//PacketLen += mb->length();
				}
				
				mb->release();
			}
			else
			{
				if (bHasData)	//���֮ǰ�������ݵ�
				{
					nNoDataCount ++;
					if (nNoDataCount >= 10)
					{
						nNoDataCount = 0;
						bHasData = false;
						std::string msg = "ͨ����";
						msg += StrUtil::Int2Str(DeviceID);
						msg += " ��������ʧ��!!";
						APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,msg,LOG_OUTPUT_FILE);
					}
				}
			}
		}
		catch(...)
		{
			string error = "¼�������쳣";
			APPLOG::instance()->WriteLog( OTHER,LOG_EVENT_ERROR,error,LOG_OUTPUT_BOTH );			
			OSFunction::Sleep(0,100);
		}
	}

	TSFETCHERMGR::instance()->DelRecordTask(DeviceID,Task);
	
	//��ȡ������ʣ�µ�����
	//std::cout<<"msg queue length : "<<Task->msg_queue()->message_length()<<std::endl;
	ACE_Message_Block *mb = NULL;
	ACE_Time_Value OutTime(ACE_OS::gettimeofday()+ACE_Time_Value(1));
	while (Task->getq(mb,&OutTime) != -1 && mb != NULL)//�����ݽ����߳�ȡ����
	{
		if (mb->length() > 0)
		{
			record.SaveFile((unsigned char*)mb->rd_ptr(),mb->length());//����д���ļ�
			//if (PacketLen>=BufLen-mb->length())
			//{
			//	bool ret = record.SaveFile(TsBuf,PacketLen);//����д���ļ�
			//	memset(TsBuf,0,BufLen);
			//	PacketLen = 0;
			//}
			////std::cout<<"data in msg queue : "<<mb->length()<<std::endl;
			//memcpy(TsBuf+PacketLen,(unsigned char*)mb->rd_ptr(),mb->length());//�������ڴ�
			//PacketLen += mb->length();
		}
		mb->release();
	}

	//if (PacketLen > 0)
	//{
	//	//std::cout<<"packet len : "<<PacketLen<<std::endl;
	//	record.SaveFile(TsBuf,PacketLen);	//����д���ļ�
	//}
	
	delete Task;
	//delete[] TsBuf;//�ͷ��ڴ�

	if (TaskScheduler != NULL)
		TaskScheduler->SetNextRunTime();//����������´�ִ��ʱ��
	
	TSFETCHERMGR::instance()->DecreaseTaskNum(DeviceID);
	if(DVBType==ATV||DVBType==RADIO||DVBType==AM)
	{
		//CHECKLEVELFORUNLOCK::instance()->RemoveRecordInfo(param);
	}

	SetFinised();
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]¼������ֹͣ !\n",DeviceID));
}

std::string RecordTask::GetObjectName()
{
	if (TaskID == "0")
		return std::string("AutoRecord");
	else
		return std::string("TaskRecord");
}
string RecordTask::GetTaskName()
{
	if (TaskID == "0")
		return std::string("�Զ�¼��");
	else
		return std::string("����¼��");

}

void RecordTask::SetDeciveID( int deviceid )
{
	DeviceID=deviceid;
}

bool RecordTask::AddTaskXml( void )
{
	XmlParser psr;
	psr.Set_xml(strStandardXML);
	pXMLNODE node=NULL;
	if(TaskID == "0")
		node=psr.GetNodeFromPath("Msg/AutoRecord/Record");
	else
		node=psr.GetNodeFromPath("Msg/TaskRecord/Record");
	if (node)
	{
		psr.SetAttrNode("DeviceID",DeviceID,node); //���������deviceid
		psr.Get_xml(strStandardXML);
	}
	//return DBMANAGER::instance()->AddXmlTask(DVBType,strStandardXML);
	string taskxmltodb = strStandardXML;
	if(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")
	{
		XmlParser tmpparser(taskxmltodb.c_str());
		pXMLNODE TsNode = tmpparser.GetNodeFromPath( "Msg/AutoRecord/Record" );
		tmpparser.SetAttrNode("TDeviceID",string(""),TsNode);
		tmpparser.SetAttrNode("CDeviceID",string(""),TsNode);
		tmpparser.SetAttrNode("DeviceID",string(""),TsNode);
		tmpparser.SaveToString(taskxmltodb);
	}
	//������������ִ��5�Σ����ȫ��ʧ�ܣ��ͷ���false
	bool ret = false;
	int nAddCount = 5;
	while (nAddCount-- > 0)
	{
		ret = DBMANAGER::instance()->AddXmlTask(DVBType,taskxmltodb);
		if (ret)
		{
			break;
		}
	}

	return ret;
}

bool RecordTask::DelTaskXml( void )
{
	return DBMANAGER::instance()->DeleteTask(DVBType,strStandardXML);
}

std::string RecordTask::CreateSchedulerAlarm()
{
	std::string amxml ="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\" ?> ";//����xmlͷ
	amxml+="<Msg><RecordTaskAlarmReport></RecordTaskAlarmReport></Msg>";
	XmlParser amparser(amxml.c_str());

	std::string Uptype;
	if(DVBType==RADIO)
	{
		Uptype="RadioUp";
	}
	else if(DVBType == ATV)
	{
		Uptype="TVMonUp";
	}
	long alarmid=0;
	pXMLNODE amroot=amparser.GetRootNode();
	amparser.SetAttrNode( "Version",string("2"),amroot );//�汾

	string strDateTime = TimeUtil::GetCurDateTime();
	string  MsgID = OSFunction::GetXmlMsgID(strDateTime);
	amparser.SetAttrNode( "MsgID",MsgID,amroot );//��Ϣid����
	amparser.SetAttrNode( "DateTime",strDateTime/*TimeUtil::GetCurDateTime()*/,amroot );//��ǰʱ��
	amparser.SetAttrNode( "Type",Uptype,amroot );//��Ϣ����
	amparser.SetAttrNode( "SrcCode",PROPMANAGER::instance()->GetDefSrcCode(DVBType),amroot );//������ʶ����ͨ���ӿڻ��
	amparser.SetAttrNode( "DstCode",PROPMANAGER::instance()->GetDefDstCode(DVBType),amroot );//Ŀ�������ʶ
	amparser.SetAttrNode( "ReplyID",string("-1"),amroot );//�ظ�����Ϣid

	string code=PROPMANAGER::instance()->GetDefSrcCode(DVBType);
	pXMLNODE amreportnode=amparser.GetNodeFromPath("Msg/RecordTaskAlarmReport");
	pXMLNODE amalarmnode=amparser.CreateNodePtr(amreportnode,"RecordTaskAlarm ");
	amparser.SetAttrNode("EquCode",string(""),amalarmnode);
	PROPMANAGER::instance()->GetAlarmID(alarmid);
	amparser.SetAttrNode("AlarmID",alarmid,amalarmnode);	
	amparser.SetAttrNode("ChCode",GetChannelID(),amalarmnode);
	amparser.SetAttrNode("TaskID",GetTaskID(),amalarmnode);
	amparser.SetAttrNode("Desc",string("¼����Դ��ͻ����"),amalarmnode);
	amparser.SetAttrNode("Reason",string(""),amalarmnode);
	amparser.SetAttrNode("CheckDateTime",TimeUtil::DateTimeToStr(time(0)),amalarmnode);
	amparser.SetAttrNode("StartDateTime",TimeUtil::DateTimeToStr(GetStartDateTime()),amalarmnode);
	amparser.SetAttrNode("EndDateTime",TimeUtil::DateTimeToStr(GetEndDateTime()),amalarmnode);

	string TypeID="";
	if(TaskID=="0")
		TypeID = strMsgID;
	else
		TypeID = TaskID;

	std::string retXML;
	amparser.SaveToString(retXML);
	sCheckParam tmpParam;
	tmpParam.DVBType=DVBType;
	tmpParam.AlarmType = ALARM_RECORDSCHEDULER;
	tmpParam.Freq = Freq;
	tmpParam.DeviceID = StrUtil::Int2Str(DeviceID);
	tmpParam.ChannelID = ChannelID;
	tmpParam.TypeID = TypeID;
	tmpParam.TypeDesc =" ¼���ͻ����";
	tmpParam.CheckTime = time(0);
	tmpParam.AlarmID = StrUtil::Long2Str(alarmid);
	DBMANAGER::instance()->AddAlarmInfo(tmpParam,"1");
	return retXML;
}


#ifdef ZONG_JU_ZHAO_BIAO
bool RecordTask::IsHDProgram()
{
	return m_bIsHDProgram;
}
#endif