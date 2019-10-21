#include "OSDSetTask.h"
#include "../Foundation/PropManager.h"
#include "./TranslateUpXML.h"
OSDSetTask::OSDSetTask(void)
{
}

OSDSetTask::OSDSetTask( std::string strXML ):DeviceIndependentTask(strXML)
{
	strStandardXML=strXML;
}
OSDSetTask::~OSDSetTask(void)
{
}

void OSDSetTask::Run( void )
{	
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]OSD��������ʼ !\n",DeviceID));
	std::string info,infox,infoy,timetype,timex,timey;
	OSDInfo osdinfo;
	XmlParser parser(strStandardXML.c_str());
	pXMLNODE OSDSetNode=parser.GetNodeFromPath("Msg/OSDSet");
	parser.GetAttrNode(OSDSetNode,"InfoOSD",info);
	parser.GetAttrNode(OSDSetNode,"InfoOSDX",infox);
	parser.GetAttrNode(OSDSetNode,"InfoOSDY",infoy);
	parser.GetAttrNode(OSDSetNode,"TimeOSDType",timetype);
	parser.GetAttrNode(OSDSetNode,"TimeOSDX",timex);
	parser.GetAttrNode(OSDSetNode,"TimeOSDY",timey);
	osdinfo.Info=info;
	osdinfo.InfoX=infox;
	osdinfo.InfoY=infoy;
	osdinfo.TimeType=timetype;
	osdinfo.TimeX=timex;
	osdinfo.TimeY=timey;
	PROPMANAGER::instance()->SetOSDInfo(DVBType,osdinfo);
	RetValue = RUN_SUCCESS;
	SendXML(TranslateUpXML::TranslateOSDSet(this));
	SetFinised();
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]OSD��������ֹͣ !\n",DeviceID));

}

std::string OSDSetTask::GetObjectName()
{
	return "OSDSetTask";
}

std::string OSDSetTask::GetTaskName()
{
	return "OSD��Ϣ����";
}