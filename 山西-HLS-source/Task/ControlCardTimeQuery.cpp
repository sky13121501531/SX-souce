#include "ControlCardTimeQuery.h"
#include "./TranslateUpXML.h"

ControlCardTimeQuery::ControlCardTimeQuery(void) : DeviceIndependentTask()
{
}

ControlCardTimeQuery::~ControlCardTimeQuery(void)
{
}

ControlCardTimeQuery::ControlCardTimeQuery(std::string strXML) : DeviceIndependentTask(strXML)
{

}
void ControlCardTimeQuery::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ���ؿ���ǰʱ���ѯ !\n"));
	bRun = true;

	SetRunning(); 

	RetValue = RUN_SUCCESS;

	SendXML(TranslateUpXML::TranslateControlCardTimeQuery(this));//����xml

	SetFinised(); 

	ACE_DEBUG ((LM_DEBUG,"(%T| %t)���ؿ���ǰʱ���ѯ !\n"));

}
std::string ControlCardTimeQuery::GetObjectName()
{
	return string("ControlCardTimeQuery");
}
std::string ControlCardTimeQuery::GetTaskName()
{
	return string("���ؿ���ǰʱ���ѯ");
}
