#include "AgentInfoQueryTask.h"
#include "../DBAccess/DBManager.h"
#include "TranslateUpXML.h"
using namespace std;
vector<sAgentSetInfo> g_VecAgentSetInfo;
AgentInfoQueryTask::AgentInfoQueryTask() : DeviceIndependentTask()
{

}

AgentInfoQueryTask::AgentInfoQueryTask(std::string strXML) : DeviceIndependentTask(strXML)
{
}

AgentInfoQueryTask::~AgentInfoQueryTask()
{

}
void AgentInfoQueryTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]ǰ����������������Ϣ����ִ�� !\n",DeviceID));
	bRun = true;
	SetRunning();

	RetValue = RUN_SUCCESS;
	if(g_VecAgentSetInfo.size()==0)
	{
		if(DBMANAGER::instance()->GetAgentSetInfo(DVBC,g_VecAgentSetInfo) == false)
		{
			for(int i = 0; i < g_VecAgentSetInfo.size(); i++)
			{
				g_VecAgentSetInfo[i].s_strType.erase(g_VecAgentSetInfo[i].s_strType.find_last_not_of(" ")+1);
				g_VecAgentSetInfo[i].s_strDesc.erase(g_VecAgentSetInfo[i].s_strDesc.find_last_not_of(" ")+1);
				g_VecAgentSetInfo[i].s_strValue.erase(g_VecAgentSetInfo[i].s_strValue.find_last_not_of(" ")+1);
			}
			RetValue = RUN_FAILED;
		}
		else
			RetValue = RUN_SUCCESS;
	}

	SendXML(TranslateUpXML::TranslateAgentInfoQuery(this,g_VecAgentSetInfo)); //���͵�ǰ��
	
	SetFinised(); 
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]ǰ����������������Ϣ����ֹͣ !\n",DeviceID));
	return;
}

string AgentInfoQueryTask::GetTaskName()
{
	return "ǰ���������ò�ѯ����";
}
std::string AgentInfoQueryTask::GetObjectName()
{
	return m_strObjectName;
}

