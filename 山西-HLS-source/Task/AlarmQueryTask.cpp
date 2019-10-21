
#include "AlarmQueryTask.h"
#include "TranslateDownXML.h"
#include "TranslateUpXML.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/XmlParser.h"
#include <vector>
#include <iostream>
#include "ace/Task.h"
#include "../DBAccess/DBManager.h"
#include "../BusinessProcess/AlarmParamInfoMgr.h"
#include "../Foundation/TimeUtil.h"
#include "./TranslateDownXML.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
using namespace std;

AlarmQueryTask::AlarmQueryTask() : DeviceIndependentTask()
{

}

AlarmQueryTask::AlarmQueryTask(std::string strXML) : DeviceIndependentTask(strXML)
{
}

AlarmQueryTask::~AlarmQueryTask()
{

}
void AlarmQueryTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]������ѯ����ִ�� !\n",DeviceID));
	bRun = true;
	SetRunning();
	XmlParser parser;
	parser.Set_xml(strStandardXML);
	pXMLNODE paramSetNode=parser.GetNodeFromPath("Msg/AlarmQuerySet");
	std::string Index,Freq,ServiceID,VideoPID,AudioPID;
	parser.GetAttrNode(paramSetNode,string("Index"),Index);//ͨ��
	parser.GetAttrNode(paramSetNode,string("Freq"),Freq); //���Freq����
	parser.GetAttrNode(paramSetNode,string("ServiceID"),ServiceID); //���ServiceID����
	parser.GetAttrNode(paramSetNode,string("VideoPID"),VideoPID); //���VideoPID����
	parser.GetAttrNode(paramSetNode,string("AudioPID"),AudioPID); //���AudioPID����
	pXMLNODELIST paramSetNodeList=parser.GetNodeList(paramSetNode);//������ѯ�ڵ��б�
	string OrgNetID,TsID;
	vector<sCheckParam> vevAlarmParam;
	for (int i=0;i!=paramSetNodeList->Size();++i)
	{
		sCheckParam tempAlarmParam;
		tempAlarmParam.Freq=Freq;
		tempAlarmParam.DeviceID=Index;
		tempAlarmParam.DVBType = GetDVBType();//�����������
		CHANNELINFOMGR::instance()->GetTsID(DVBType,Freq,AudioPID,VideoPID,ServiceID,TsID);
		CHANNELINFOMGR::instance()->GetOrgNetID(DVBType,Freq,AudioPID,VideoPID,ServiceID,OrgNetID);
		tempAlarmParam.ChannelID=OrgNetID+TsID+Freq+ServiceID+VideoPID+AudioPID;
		tempAlarmParam.ServiceID=ServiceID;
		tempAlarmParam.VideoID=VideoPID;
		tempAlarmParam.AudioID=AudioPID;
		pXMLNODE childNode=paramSetNodeList->GetNextNode();//�ӽڵ�
		std::string TypeID,Desc;
		parser.GetAttrNode(childNode,string("Type"),TypeID);//���Type����
		tempAlarmParam.TypeID=TypeID;
		int alarmType=atoi(TypeID.c_str());
		if (DVBType==DVBC||DVBType==CTTB||DVBType==DVBS)
		{
			if (alarmType == 1)
				tempAlarmParam.AlarmType=ALARM_PROGRAM;
			else if(alarmType<=30)
				tempAlarmParam.AlarmType=ALARM_TR101_290;
			else if(alarmType<=40)
				tempAlarmParam.AlarmType=ALARM_PROGRAM;
			else if(alarmType<=50)
				tempAlarmParam.AlarmType=ALARM_FREQ;
			else
				tempAlarmParam.AlarmType=ALARM_ENVIRONMENT;
		}
		else
		{
			if(alarmType<10)
				tempAlarmParam.AlarmType=ALARM_FREQ;
			else 
				tempAlarmParam.AlarmType=ALARM_PROGRAM;
		}
		parser.GetAttrNode(childNode,string("Desc"),Desc);
		tempAlarmParam.TypeDesc=Desc;
		vevAlarmParam.push_back(tempAlarmParam);

	}
	if(DBMANAGER::instance()->AlarmInfoQuery(vevAlarmParam))
		RetValue = RUN_SUCCESS;
	else
		RetValue = RUN_FAILED;
	SendXML(TranslateUpXML::TranslateAlarmQuery(this,vevAlarmParam));//���ͻظ�xml
	SetFinised();
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]������ѯ����ֹͣ !\n",DeviceID));
}
string AlarmQueryTask::GetTaskName()
{
	return "������ѯ����";
}
std::string AlarmQueryTask::GetObjectName()
{
	return std::string("AlarmQueryTask");
}
