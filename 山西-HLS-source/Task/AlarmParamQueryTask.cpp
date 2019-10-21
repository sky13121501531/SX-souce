
#include "AlarmParamQueryTask.h"
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
#include "../BusinessProcess/ChannelInfoMgr.h"
using namespace std;

AlarmParamQueryTask::AlarmParamQueryTask() : DeviceIndependentTask()
{

}

AlarmParamQueryTask::AlarmParamQueryTask(std::string strXML) : DeviceIndependentTask(strXML)
{
}

AlarmParamQueryTask::~AlarmParamQueryTask()
{

}
void AlarmParamQueryTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]����������ѯ����ִ�� !\n",DeviceID));
	bRun = true;
	SetRunning();
	XmlParser parser;
	parser.Set_xml(strStandardXML);
	pXMLNODE paramQueryNode=parser.GetNodeFromPath("Msg/AlarmParamQuery");

	RetValue = RUN_SUCCESS;
	string taskName;
	parser.GetAttrNode(paramQueryNode,string("ReturnTaskName"),taskName);//���STD����
	m_strObjectName = taskName;
	//��ѯĳһ����������б�������
	AlarmParamVec vecAllAlarmParam,vecAlarmParam;
	ALARMPARAMINFOMGR::instance()->GetAlarmParamByDvbtype(GetDVBType(), vecAllAlarmParam);
	AlarmParamVec ::const_iterator alarmParamIter= vecAllAlarmParam.begin();
	for (;alarmParamIter!=vecAllAlarmParam.end();alarmParamIter++)
	{
		if (taskName == "AlarmRFInfoQuery" && (*alarmParamIter).AlarmType == ALARM_FREQ)
		{
			vecAlarmParam.push_back(*alarmParamIter);
		}
		else if (taskName == "AlarmChannelInfoQuery" && (*alarmParamIter).AlarmType == ALARM_PROGRAM)
		{
			vecAlarmParam.push_back(*alarmParamIter);
		}
		else if (taskName == "AlarmStreamInfoQuery" &&(*alarmParamIter).AlarmType == ALARM_TR101_290)
		{
			vecAlarmParam.push_back(*alarmParamIter);
		}
	}
	SendXML( TranslateUpXML::TranslateAlarmParamQuery(this, vecAlarmParam) );
}

string AlarmParamQueryTask::GetTaskName()
{
	return "����������ѯ����";
}
std::string AlarmParamQueryTask::GetObjectName()
{
	return m_strObjectName;
}

