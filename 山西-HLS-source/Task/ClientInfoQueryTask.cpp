
#include "ClientInfoQueryTask.h"
#include "TranslateDownXML.h"
#include "TranslateUpXML.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/TypeDef.h"
#include "../BusinessProcess/BusinessLayoutMgr.h"
#include <vector>
#include <iostream>

using namespace std;

ClientInfoQueryTask::ClientInfoQueryTask() : DeviceIndependentTask()
{
	
}

ClientInfoQueryTask::ClientInfoQueryTask(std::string strXML) : DeviceIndependentTask(strXML)
{
}

ClientInfoQueryTask::~ClientInfoQueryTask()
{
}
void ClientInfoQueryTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]��Ƶ�û���ѯ����ִ�� !\n",DeviceID));
	bRun = true;
	
	SetRunning(); 

	std::vector<sVedioUserInfo> vecClient;
	if( BUSINESSLAYOUTMGR::instance()->GetUser(DVBType,vecClient) ==false )
       RetValue = RUN_FAILED;
	else
	   RetValue = RUN_SUCCESS;

    SendXML(TranslateUpXML::TranslateClientInfoQuery(this,vecClient));//����xml

	SetFinised(); 

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]��Ƶ�û���ѯ����ֹͣ !\n",DeviceID));
}

string ClientInfoQueryTask::GetTaskName()
{
	return std::string("��Ƶ�û���ѯ����");
}
std::string ClientInfoQueryTask::GetObjectName()
{
	return std::string("ClientInfoQueryTask");
}

