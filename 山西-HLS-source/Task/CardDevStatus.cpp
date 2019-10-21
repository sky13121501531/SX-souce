#include "CardDevStatus.h"
#include "./TranslateUpXML.h"

CardDevStatusTask::CardDevStatusTask(void) : DeviceIndependentTask()
{
}

CardDevStatusTask::~CardDevStatusTask(void)
{
}

CardDevStatusTask::CardDevStatusTask(std::string strXML) : DeviceIndependentTask(strXML)
{

}
void CardDevStatusTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]����忨����״̬��ѯ����ִ�� !\n",DeviceID));
	bRun = true;

	SetRunning(); 

	RetValue = RUN_SUCCESS;

	SendXML(TranslateUpXML::TranslateCardDevStatusQuery(this));//����xml

	SetFinised(); 

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]����忨����״̬��ѯ����ֹͣ !\n",DeviceID));

}
std::string CardDevStatusTask::GetObjectName()
{
	return string("CardDevStatusTask");
}
std::string CardDevStatusTask::GetTaskName()
{
	return string("����忨����״̬��ѯ����");
}
