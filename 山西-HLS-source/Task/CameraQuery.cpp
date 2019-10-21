#include "CameraQuery.h"
#include "./TranslateUpXML.h"

CameraQueryTask::CameraQueryTask(void) : DeviceIndependentTask()
{
}

CameraQueryTask::~CameraQueryTask(void)
{
}

CameraQueryTask::CameraQueryTask(std::string strXML) : DeviceIndependentTask(strXML)
{

}
void CameraQueryTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]����ͷ��Ƶ��ѯ����ִ�� !\n",DeviceID));
	bRun = true;

	SetRunning(); 

	RetValue = RUN_FAILED;

	SendXML(TranslateUpXML::TranslateCameraQuery(this,strStandardXML));//����xml

	SetFinised(); 

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]����ͷ��Ƶ��ѯ����ֹͣ !\n",DeviceID));

}
std::string CameraQueryTask::GetObjectName()
{
	return string("CameraQueryTask");
}
std::string CameraQueryTask::GetTaskName()
{
	return string("����ͷ��Ƶ��ѯ����");
}
