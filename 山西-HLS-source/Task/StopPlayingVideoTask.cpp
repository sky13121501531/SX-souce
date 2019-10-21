
#include "StopPlayingVideoTask.h"
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
#include <list>
#include <iostream>

using namespace std;

#ifdef ZONG_JU_ZHAO_BIAO


StopPlayingVideoTask::StopPlayingVideoTask() : DeviceIndependentTask()
{

}

StopPlayingVideoTask::StopPlayingVideoTask(std::string strXML) : DeviceIndependentTask(strXML)
{
	if(this->GetVersion() == "3.0")
	{
		XmlParser parser(strXML.c_str());
		pXMLNODE rootNode = parser.GetNodeFromPath("Msg");
		parser.GetAttrNode(rootNode,"CoderList",m_CoderIdList);         
	}
}

StopPlayingVideoTask::~StopPlayingVideoTask()
{
}
void StopPlayingVideoTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]��Ƶ����ֹͣ����ִ�� !\n",DeviceID));
	bRun = true;
	SetRunning(); 

	std::list<int> videoDevList;
	if(this->GetVersion() == "3.0")
	{
		PROPMANAGER::instance()->SeparateStr(m_CoderIdList,videoDevList);
	}
	else
	{
		int devnum = PROPMANAGER::instance()->GetCoderDevNum();
		for(int devi = 0;devi < devnum;devi++)
		{
			videoDevList.push_back(devi);
		}
		PROPMANAGER::instance()->GetVirDevList( videoDevList );
	}
	//ֹͣvideoDevList��ͨ������Ƶ����
	std::list<int>::iterator tempIter = videoDevList.begin();
	for (; tempIter!=videoDevList.end(); tempIter++)
	{
		bool ret = TSSENDERMGR::instance()->StopAllClient( *tempIter );	
		if (!ret)
			ACE_DEBUG((LM_DEBUG, "(%T| %t) ͨ��[%d]ֹͣ��������ʧ�� !\n", *tempIter));
	}
	
	RetValue = RUN_SUCCESS;

	SendXML(TranslateUpXML::TranslateStopPlayingVideo(this));	//����xml

	SetFinised(); 

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]��Ƶ����ֹͣ����ֹͣ !\n",DeviceID));
}

string StopPlayingVideoTask::GetTaskName()
{
	return std::string("��Ƶ����ֹͣ����");
}
std::string StopPlayingVideoTask::GetObjectName()
{
	return std::string("StopPlayingVideoTask");
}

#endif