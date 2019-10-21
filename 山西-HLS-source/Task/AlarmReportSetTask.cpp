
#include "AlarmReportSetTask.h"
#include "TranslateDownXML.h"
#include "TranslateUpXML.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/TypeDef.h"
#include "../Communications/TsSenderMgr.h"
#include <vector>
#include <iostream>

using namespace std;

AlarmReportSetTask::AlarmReportSetTask() : DeviceIndependentTask()
{

}

AlarmReportSetTask::AlarmReportSetTask(std::string strXML) : DeviceIndependentTask(strXML)
{
}

AlarmReportSetTask::~AlarmReportSetTask()
{
}
void AlarmReportSetTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]������ʽ��������ִ�� !\n",DeviceID));
	bRun = true;

	SetRunning(); 
	UpAlarmInfo alarminfo;
	XmlParser parser;
	parser.Set_xml(strStandardXML);
	pXMLNODE SetNode=parser.GetNodeFromPath("Msg/AlarmTypeSet");
	parser.GetAttrNode(SetNode,"Type",alarminfo.type);
	parser.GetAttrNode(SetNode,"URL",alarminfo.alarmurl);
	parser.GetAttrNode(SetNode,"TimeInterval",alarminfo.interval);
	PROPMANAGER::instance()->SetUpAlarmInfo(DVBType,alarminfo);
	RetValue = RUN_SUCCESS;
	
	SendXML(TranslateUpXML::TranslateAlarmReport(this));//����xml

	SetFinised(); 

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]������ʽ��������ֹͣ !\n",DeviceID));
}

string AlarmReportSetTask::GetTaskName()
{
	return std::string("������ʽ��������");
}
std::string AlarmReportSetTask::GetObjectName()
{
	return std::string("AlarmReportSetTask");
}

