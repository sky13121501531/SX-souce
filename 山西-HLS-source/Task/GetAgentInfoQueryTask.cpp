#include "GetAgentInfoQueryTask.h"
#include "./TranslateUpXML.h"

GetAgentInfoQueryTask::GetAgentInfoQueryTask(void) : DeviceIndependentTask()
{
}

GetAgentInfoQueryTask::~GetAgentInfoQueryTask(void)
{
}

GetAgentInfoQueryTask::GetAgentInfoQueryTask(std::string strXML) : DeviceIndependentTask(strXML)
{
	m_strQueryLevel = "4";
	XmlParser parser(strXML.c_str());
	pXMLNODE queryNode = parser.GetNodeFromPath("Msg/GetAgentInfo");
	if(this->GetVersion() == "3.0")
	{
		parser.GetAttrNode(queryNode,"QueryDetail",m_strQueryLevel);  
	}
	else
	{
		parser.GetAttrNode(queryNode,"Level",m_strQueryLevel);  
	}
}
void GetAgentInfoQueryTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]ǰ����Ϣ��ѯ����ִ�� !\n",DeviceID));
	bRun = true;

	SetRunning(); 

	RetValue = RUN_SUCCESS;
	SendXML(TranslateUpXML::TranslateGetAgentInfoQuery(this,m_strQueryLevel));//����xml

	SetFinised(); 

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]ǰ����Ϣ��ѯ����ֹͣ !\n",DeviceID));

}
std::string GetAgentInfoQueryTask::GetObjectName()
{
	return string("GetAgentInfoQueryTask");
}
std::string GetAgentInfoQueryTask::GetTaskName()
{
	return string("ǰ����Ϣ��ѯ");
}
string GetQueryLevel();
