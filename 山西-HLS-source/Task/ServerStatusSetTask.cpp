
#include "ServerStatusSetTask.h"
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
#include "../Foundation/AlarmPropManager.h"
#include "../DeviceMgr/ServerManager.h"
using namespace std;

ServerStatusSetTask::ServerStatusSetTask() : DeviceIndependentTask()
{

}

ServerStatusSetTask::ServerStatusSetTask(std::string strXML) : DeviceIndependentTask(strXML)
{
}

ServerStatusSetTask::~ServerStatusSetTask()
{

}
void ServerStatusSetTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]�豸����״̬��������ִ�� !\n",DeviceID));
	bRun = true;
	SetRunning();
	RetValue = RUN_SUCCESS;
	XmlParser parser;
	parser.Set_xml(strStandardXML);
	pXMLNODE SetNode=parser.GetNodeFromPath("Msg/ServerStatusSet");
	std::string EquCode,Action;
	parser.GetAttrNode(SetNode,"EquCode",EquCode);
	parser.GetAttrNode(SetNode,"Action",Action);
	pXMLNODELIST NodeList=parser.GetNodeList(SetNode);
	int count=parser.GetChildCount(SetNode);

	std::string DayOfWeek,StartTime,StartDateTime,URL;
	if(count==0)
	{
		pXMLNODE chilNode = parser.CreateNodePtr(SetNode,"SingleReportTime");
		parser.SetAttrNode("StartDateTime",TimeUtil::GetCurDateTime(),chilNode);
		parser.SaveToString(strStandardXML);
	}
	for(int i=0;i<count;i++)
	{
		pXMLNODE ChildNode=parser.GetNextNode(NodeList);

		parser.GetAttrNode(ChildNode,"DayOfWeek",DayOfWeek);//DayofWeek ��StartTime��ѭ���������������
		parser.GetAttrNode(ChildNode,"StartTime",StartTime);
		parser.GetAttrNode(ChildNode,"StartDateTime",StartDateTime);//StartDateTime Ϊ�����������������
		parser.GetAttrNode(ChildNode,"URL",URL);
	}
	std::string strTime;
	int Type=0;
	if(count==0)
	{
		Type = -1;
		strTime = TimeUtil::GetCurDateTime();
	}
	else if(DayOfWeek.length()<1)
	{
		Type = -1;
		strTime = StartDateTime;
	}
	else 
	{
		Type = StrUtil::Str2Int(DayOfWeek);
		strTime = StartTime;
	}

	SEVERMANAGER::instance()->SetStatus(Type,Action,strTime);
	DelTaskXml();
	AddTaskXml();
	SendXML(TranslateUpXML::TranslateServerStatusSet(this));//���ͻظ�xml
	SetFinised();
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]�豸����״̬��������ֹͣ !\n",DeviceID));
}
string ServerStatusSetTask::GetTaskName()
{
	return "�豸����״̬��������";
}
std::string ServerStatusSetTask::GetObjectName()
{
	return std::string("ServerStatusSetTask");
}
bool ServerStatusSetTask::DelTaskXml()
{
	return DBMANAGER::instance()->DeleteTask(UNKNOWN,strStandardXML);
}
bool ServerStatusSetTask::AddTaskXml()
{
	//return DBMANAGER::instance()->AddXmlTask(UNKNOWN,strStandardXML);
	//������������ִ��5�Σ����ȫ��ʧ�ܣ��ͷ���false
	bool ret = false;
	int nAddCount = 5;
	while (nAddCount-- > 0)
	{
		ret = DBMANAGER::instance()->AddXmlTask(DVBType,strStandardXML);
		if (ret)
		{
			break;
		}
	}

	return ret;
}