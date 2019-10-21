#include "LimitedBandwidth.h"
#include "./TranslateUpXML.h"

LimitedBandwidth::LimitedBandwidth(void) : DeviceIndependentTask()
{
}

LimitedBandwidth::~LimitedBandwidth(void)
{
}

LimitedBandwidth::LimitedBandwidth(std::string strXML) : DeviceIndependentTask(strXML)
{

}
void LimitedBandwidth::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ¼���ϴ������������ִ�� !\n"));
	bRun = true;

	SetRunning(); 

	RetValue = RUN_SUCCESS;

	SendXML(TranslateUpXML::TranslateLimitedBandwidth(this));//����xml

	SetFinised(); 

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ¼���ϴ������������ֹͣ !\n"));

}
std::string LimitedBandwidth::GetObjectName()
{
	return string("LimitedBandwidth");
}
std::string LimitedBandwidth::GetTaskName()
{
	return string("¼���ϴ��������");
}
