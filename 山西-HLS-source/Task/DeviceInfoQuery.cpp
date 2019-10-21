
#include "DeviceInfoQuery.h"
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
#include <vector>
#include <iostream>

using namespace std;

DeviceInfoQuery::DeviceInfoQuery() : DeviceIndependentTask()
{
	
}

DeviceInfoQuery::DeviceInfoQuery(std::string strXML) : DeviceIndependentTask(strXML)
{
	
}

DeviceInfoQuery::~DeviceInfoQuery()
{
}
void DeviceInfoQuery::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]�豸��Ϣ��ѯ����ִ�� !\n",DeviceID));
	bRun = true;	
	SetRunning(); 
	RetValue = RUN_SUCCESS;
	SendXML(TranslateUpXML::TranslateDeviceInfoQuery(this));//����xml

	SetFinised(); 

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]�豸��Ϣ��ѯ����ֹͣ !\n",DeviceID));
}

string DeviceInfoQuery::GetTaskName()
{
	return std::string("�豸��Ϣ��ѯ����");
}
std::string DeviceInfoQuery::GetObjectName()
{
	return std::string("DeviceInfoQuery");
}

