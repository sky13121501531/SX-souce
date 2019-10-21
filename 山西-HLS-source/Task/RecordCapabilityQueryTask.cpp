
#include "RecordCapabilityQueryTask.h"
#include "TranslateDownXML.h"
#include "TranslateUpXML.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/TypeDef.h"
#include "../Communications/TsSenderMgr.h"
#include "../BusinessProcess/BusinessLayoutMgr.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include <vector>
#include <iostream>

using namespace std;

RecordCapabilityQueryTask::RecordCapabilityQueryTask() : DeviceIndependentTask()
{

}

RecordCapabilityQueryTask::RecordCapabilityQueryTask(std::string strXML) : DeviceIndependentTask(strXML)
{
#ifdef ZONG_JU_ZHAO_BIAO
	XmlParser tempParser( strXML.c_str() );
	pXMLNODE RecCapQueryNode = tempParser.GetNodeFromPath("Msg/RecordCapabilityQuery");
	std::string strRecordType;
	tempParser.GetAttrNode(RecCapQueryNode, "SetRecordType", m_strRecordType);
	int ChanCount = tempParser.GetChildCount(RecCapQueryNode);
	pXMLNODELIST ChanList = tempParser.GetNodeList(RecCapQueryNode);
	for (int i=0;i<ChanCount;i++)
	{
		RecChanInfo tempRecChanInfo;
		pXMLNODE ChanInfoNode = tempParser.GetNextNode(ChanList);
		tempParser.GetAttrNode(ChanInfoNode, "Freq", tempRecChanInfo.freq);
		tempParser.GetAttrNode(ChanInfoNode, "ServiceID", tempRecChanInfo.serviceId);
		tempParser.GetAttrNode(ChanInfoNode, "ProgramID", tempRecChanInfo.programId);
		tempRecChanInfo.canRecord = false;

		m_vecChanInfo.push_back(tempRecChanInfo);
	}
#endif
}

RecordCapabilityQueryTask::~RecordCapabilityQueryTask()
{
}
void RecordCapabilityQueryTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]¼��·����ѯ����ִ�� !\n",DeviceID));
	bRun = true;

	SetRunning(); 

#ifdef ZONG_JU_ZHAO_BIAO

	int newIndexCount = PROPMANAGER::instance()->GetUnUsedCoderDevNum();
	int pessCount = newIndexCount;

	if (newIndexCount == 0)	//�Ѿ�û�п���ͨ����
	{
		RetValue = RUN_SUCCESS;
		SendXML( TranslateUpXML::TranslateRecordCapabilityQuery(this, 0, 0, m_strRecordType, m_vecChanInfo) );//����xml

		SetFinised(); 
		ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]¼��·����ѯ����ֹͣ !\n",DeviceID));
		return ;
	}

	std::vector<RecChanInfo>::iterator chaninfoIter = m_vecChanInfo.begin();
	for (; chaninfoIter!=m_vecChanInfo.end(); chaninfoIter++)
	{
		if (pessCount>0)
		{
			(*chaninfoIter).canRecord = true;
			pessCount --;
		}
		else
		{
			(*chaninfoIter).canRecord = false;
		}
	}
#endif

	RetValue = RUN_SUCCESS;

#ifdef ZONG_JU_ZHAO_BIAO
	SendXML( TranslateUpXML::TranslateRecordCapabilityQuery(this, newIndexCount, pessCount, m_strRecordType, m_vecChanInfo) );//����xml
#else
	SendXML(TranslateUpXML::TranslateRecordCapabilityQuery(this));//����xml
#endif
	SetFinised(); 

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]¼��·����ѯ����ֹͣ !\n",DeviceID));
}

string RecordCapabilityQueryTask::GetTaskName()
{
	return std::string("¼��·����ѯ����");
}
std::string RecordCapabilityQueryTask::GetObjectName()
{
	return std::string("RecordCapabilityQueryTask");
}

