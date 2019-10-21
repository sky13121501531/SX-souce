#pragma warning(disable:4996)
#include "RunPlanSetTask.h"
#include "TranslateDownXML.h"
#include "TranslateUpXML.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/XmlParser.h"
#include <vector>
#include <iostream>
#include "ace/Task.h"
#include "../Foundation/TypeDef.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include "../BusinessProcess/RunPlanInfoMgr.h"
#include "../Foundation/AppLog.h"
using namespace std;

RunPlanSetTask::RunPlanSetTask() : DeviceIndependentTask()
{

}

RunPlanSetTask::RunPlanSetTask(std::string strXML) : DeviceIndependentTask(strXML)
{
}

RunPlanSetTask::~RunPlanSetTask()
{

}

void RunPlanSetTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]����ͼ��������ִ�� !\n",DeviceID));
	bRun = true;
    SetRunning();
    if ( TransRunplanSet( strStandardXML ) == false )
       RetValue = RUN_FAILED;
	else
       RetValue = RUN_SUCCESS;

	SendXML(TranslateUpXML::TranslateRunPlanSet(this));
	SetFinised();
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]����ͼ��������ֹͣ !\n",DeviceID));
}

string RunPlanSetTask::GetTaskName()
{
	return "����ͼ��������";
}
std::string RunPlanSetTask::GetObjectName()
{
	return std::string("RunPlanSetTask");
}
bool RunPlanSetTask::TransRunplanSet(std::string strXML)
{
	XmlParser parser(strXML.c_str());
	string channelID,OrgNetID,TsID,ServiceID,VideoPID,AudioPID,Code,Freq;
	string StartTime, EndTime, ValidStartDateTime, ValidEndDateTime, DayOfWeek,Month,DayOfMonth;
	string StartDateTime, EndDateTime,AlarmEndTime;
	int RunPlanType;
	string nodeName;
	pXMLNODE rootNode = NULL ;
	pXMLNODE childNode = NULL;
	rootNode = parser.GetNodeFromPath( "Msg" );
	childNode = parser.GetNodeFirstChild( rootNode );
	nodeName  = parser.GetNodeName( childNode );//��ȡ��Ԫ����
	pXMLNODE runPlanSetChannelNode = NULL;
	pXMLNODE runPlanSetNode = parser.GetNodeFromPath("Msg/RunPlanSet");
	//��ȡRunPlanSet���ӽڵ��б�
	pXMLNODELIST runPlanSetChannelList = parser.GetNodeList( runPlanSetNode );
	//��ȡRunPlanSet���ӽڵ����
	int count = parser.GetChildCount( runPlanSetNode );
	RunPlanParamVec mrunPlanParamVec;
	for (int i = 0;i<count;i++)
	{
		
		runPlanSetChannelNode = parser.GetNextNode( runPlanSetChannelList );
		
		parser.GetAttrNode( runPlanSetChannelNode,"OrgNetID",OrgNetID );
		parser.GetAttrNode( runPlanSetChannelNode,"TsID",TsID );
		parser.GetAttrNode( runPlanSetChannelNode,"ServiceID",ServiceID );
		parser.GetAttrNode( runPlanSetChannelNode,"VideoPID",VideoPID);
		parser.GetAttrNode( runPlanSetChannelNode,"AudioPID",AudioPID);
		parser.GetAttrNode( runPlanSetChannelNode,"ChannelCode",Code );
		parser.GetAttrNode( runPlanSetChannelNode,"Freq",Freq );
		parser.GetAttrNode( runPlanSetChannelNode,"Month",Month );
		parser.GetAttrNode( runPlanSetChannelNode,"DayOfMonth",DayOfMonth );
		parser.GetAttrNode( runPlanSetChannelNode,"DayOfWeek",DayOfWeek ); 
		parser.GetAttrNode( runPlanSetChannelNode,"StartTime",StartTime );
		parser.GetAttrNode( runPlanSetChannelNode,"EndTime",EndTime );
		parser.GetAttrNode( runPlanSetChannelNode,"StartDateTime",StartDateTime );
		parser.GetAttrNode( runPlanSetChannelNode,"EndDateTime",EndDateTime );
		parser.GetAttrNode( runPlanSetChannelNode,"ValidStartDateTime",ValidStartDateTime );
		parser.GetAttrNode( runPlanSetChannelNode,"ValidEndDateTime",ValidEndDateTime );
		parser.GetAttrNode( runPlanSetChannelNode,"Type",RunPlanType);
	
		if (DVBType==DVBC || DVBType==DVBS||DVBType==CTTB)
		{
			//��ȡ����ͼ������channelID
			bool isRight =CHANNELINFOMGR::instance()->GetChannelID( GetDVBType(),OrgNetID,TsID,ServiceID,VideoPID,AudioPID,Code,channelID );
			if (isRight == false)
			{
				string msg = "����ͼʧ��!OrgNetID:"+OrgNetID+"-TsID:"+TsID+"-ServiceID:"+ServiceID+"-VideoPID:"+VideoPID+"-AudioPID:"+AudioPID;
				APPLOG::instance()->WriteLog(TASK,LOG_EVENT_DEBUG,msg,LOG_OUTPUT_BOTH);
				return false;
			}
		}
		
		if(DayOfWeek=="7")  //DVBC������"7"��Ϊ"0"��ģ���ͳһ����
			DayOfWeek="0";
		//��������ͼ����
		sRunPlanParam mrunPlanParam;
		mrunPlanParam.dvbtype=GetDVBType();
		mrunPlanParam.DayOfWeek = DayOfWeek;
		mrunPlanParam.Month = Month;
		mrunPlanParam.DayOfMonth = DayOfMonth;
		mrunPlanParam.StartTime = StartTime;
		mrunPlanParam.EndTime = EndTime;

		if (ValidStartDateTime.length()<1)	//���ValidStartDateTimeΪ�գ��͸�ֵΪ��ǰʱ��
			mrunPlanParam.ValidStartDateTime = TimeUtil::GetCurDateTime();
		else
			mrunPlanParam.ValidStartDateTime = ValidStartDateTime;
		
		if (ValidEndDateTime.length()<1)		//���ValidEndDateTimeΪ�գ��͸�ֵΪ�ܳ�ʱ���Ժ�
			mrunPlanParam.ValidEndDateTime = TimeUtil::GetEndLessTime();
		else
			mrunPlanParam.ValidEndDateTime = ValidEndDateTime;
		
		mrunPlanParam.StartDateTime = StartDateTime;
		mrunPlanParam.EndDateTime = EndDateTime;
		
		if (DVBType==DVBC || DVBType==DVBS)
		{
			mrunPlanParam.ChannelID = channelID;
			mrunPlanParam.Type = RunPlanType;
		}
		else if (DVBType==ATV || DVBType==RADIO || DVBType==AM || DVBType==CTV)
		{
			string tempOrgnetId,tempTsId,tempServiceId,tempVideoPid,tempAudioPid;
			//��ȡchannelId
			bool ret = CHANNELINFOMGR::instance()->GetChannelID(DVBType,tempOrgnetId,tempTsId,tempServiceId,
													tempVideoPid,tempAudioPid,Code,channelID);
			if (!ret)
				mrunPlanParam.ChannelID = Freq;
			else
				mrunPlanParam.ChannelID=channelID;

			mrunPlanParam.Type = 0;		//ģ��ֻ��ͣ��
		}
		else if(DVBType==CTTB)	//�����AMû��������
		{
			mrunPlanParam.ChannelID = channelID;
			mrunPlanParam.Type = 0;
		}
		mrunPlanParamVec.push_back( mrunPlanParam );
	}
	if( RUNPLANINFOMGR::instance()->UpdatePlanInfo(GetDVBType(),channelID,mrunPlanParamVec) == false )
	{
		string msg = "����ͼʧ��!UpdatePlanInfo";
		APPLOG::instance()->WriteLog(TASK,LOG_EVENT_DEBUG,msg,LOG_OUTPUT_BOTH);
		return false;
	}
	return true;
}