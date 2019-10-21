
#include "EnvironmentQueryTask.h"
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
#include "../Foundation/StrUtil.h"
#include "../DeviceMgr/OS_Environment.h"
#include "../Foundation/TypeDef.h"
#include "../Alarm/AlarmMgr.h"
using namespace std;

EnvironmentQueryTask::EnvironmentQueryTask() : DeviceIndependentTask()
{

}

EnvironmentQueryTask::EnvironmentQueryTask(std::string strXML) : DeviceIndependentTask(strXML)
{
}

EnvironmentQueryTask::~EnvironmentQueryTask()
{

}
void EnvironmentQueryTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]���л���ָ���ѯ����ִ�� !\n",DeviceID));
	bRun = true;
	SetRunning();
	EnvMapInfo mapInfo;
	XmlParser parser(strStandardXML.c_str());
	pXMLNODE queryNode = parser.GetNodeFromPath("Msg/EnvironmentQuery");
	pXMLNODELIST environmentNodeList = parser.GetNodeList(queryNode);
	int count = parser.GetChildCount(queryNode);
	for (int i = 0;i<count;i++)
	{
		pXMLNODE environmentNode = parser.GetNextNode(environmentNodeList);
		EnvInfo env;
		parser.GetAttrNode(environmentNode,"Type",env.Type);
		parser.GetAttrNode(environmentNode,"Desc",env.Desc);
			
		if (env.Type =="54")		//cpuʹ����
		{
			long cpuUsage = OS_Environment::CPURealtimeUsage();//������һ�μ��ֵ
			OSFunction::Sleep(0,100);
			cpuUsage = OS_Environment::CPURealtimeUsage();      //cpuʹ����
			std::string strCPU = StrUtil::Long2Str(cpuUsage);
			mapInfo.insert(make_pair(env,strCPU));
		}
		else if (env.Type =="55")
		{
			std::string strHardDisk;
			mapInfo.insert(make_pair(env,strHardDisk));
		}
		else if (env.Type =="56")	// �ڴ�ʹ����
		{
			long memUsage = OS_Environment::MemoryRealtimeUsage();   // �ڴ�ʹ����
			std::string strMemory = StrUtil::Long2Str(memUsage);
			mapInfo.insert(make_pair(env,strMemory));   
		}
		else		//����(�¶ȡ���ѹ��ʪ�ȡ�״̬��Ӳ��)
		{
			std::string strOtherVal = "";
			mapInfo.insert(make_pair(env,strOtherVal));
		}	   
	}
	std::vector<sCheckParam> paramVec;	
	SetRetValue(RUN_SUCCESS);
	SendXML(TranslateUpXML::TranslateEnvironmentQuery(this,mapInfo,paramVec));
	std::vector<sCheckParam>::iterator ptr;
	for(ptr=paramVec.begin();ptr!=paramVec.end();ptr++)
	{
			ALARMMGR::instance()->CheckAlarm(*ptr,true);
	}

	SetFinised();
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]���л���ָ���ѯ����ֹͣ !\n",DeviceID));
}
string EnvironmentQueryTask::GetTaskName()
{
	return "���л���ָ���ѯ";
}
std::string EnvironmentQueryTask::GetObjectName()
{
	return std::string("EnvironmentQueryTask");
}
