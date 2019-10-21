#include "EquipmentHeartBeatQueryTask.h"
#include "./TranslateUpXML.h"

EquipmentHeartBeatQueryTask::EquipmentHeartBeatQueryTask(void) : DeviceIndependentTask()
{
}

EquipmentHeartBeatQueryTask::~EquipmentHeartBeatQueryTask(void)
{
}

EquipmentHeartBeatQueryTask::EquipmentHeartBeatQueryTask(std::string strXML) : DeviceIndependentTask(strXML)
{

}
void EquipmentHeartBeatQueryTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]�豸��ͨ״̬��ѯ����ִ�� !\n",DeviceID));
	bRun = true;

	SetRunning(); 

	RetValue = RUN_SUCCESS;

	SendXML(TranslateUpXML::TranslateEquipmentHeartBeatQuery(this));//����xml

	SetFinised(); 

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]�豸��ͨ״̬��ѯ����ֹͣ !\n",DeviceID));

}
std::string EquipmentHeartBeatQueryTask::GetObjectName()
{
	return string("EquipmentHeartBeatQueryTask");
}
std::string EquipmentHeartBeatQueryTask::GetTaskName()
{
	return string("�豸��ͨ״̬��ѯ����");
}
