
#include "LoopAlarmInfoTask.h"
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

LoopAlarmInfoTask::LoopAlarmInfoTask() : DeviceIndependentTask()
{

}

LoopAlarmInfoTask::LoopAlarmInfoTask(std::string strXML) : DeviceIndependentTask(strXML)
{
}

LoopAlarmInfoTask::~LoopAlarmInfoTask()
{
}
void LoopAlarmInfoTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]ѭ�б�����������ִ�� !\n",DeviceID));
	bRun = true;

	SetRunning(); 

	RetValue = RUN_SUCCESS;

	SendXML(TranslateUpXML::TranslateLoopAlarmInfo(this));//����xml

	SetFinised(); 

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]ѭ�б�����������ֹͣ !\n",DeviceID));
}

string LoopAlarmInfoTask::GetTaskName()
{
	return std::string("ѭ�б�����������");
}
std::string LoopAlarmInfoTask::GetObjectName()
{
	return std::string("LoopAlarmInfoTask");
}

