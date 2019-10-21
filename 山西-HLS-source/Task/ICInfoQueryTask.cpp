
#include "ICInfoQueryTask.h"
#include "TranslateDownXML.h"
#include "TranslateUpXML.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/TypeDef.h"
#include "../Communications/TsSenderMgr.h"
#include "../Foundation/ParamSetManager.h"
#include <vector>
#include <iostream>

using namespace std;

#ifdef ZONG_JU_ZHAO_BIAO


ICInfoQueryTask::ICInfoQueryTask() : DeviceIndependentTask()
{

}

ICInfoQueryTask::ICInfoQueryTask(std::string strXML) : DeviceIndependentTask(strXML)
{
}

ICInfoQueryTask::~ICInfoQueryTask()
{
}
void ICInfoQueryTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]С�����Ų�ѯ����ִ�� !\n",DeviceID));
	bRun = true;

	SetRunning(); 

	RetValue = RUN_SUCCESS;
	std::vector<CACardInfo> cardinfo;
	PARAMSETMANAGER::instance()->GetCACardInfo(cardinfo);
	SendXML(TranslateUpXML::TranslateICInfoQuery(this,cardinfo));//����xml

	SetFinised(); 

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]С�����Ų�ѯ����ֹͣ !\n",DeviceID));
}

string ICInfoQueryTask::GetTaskName()
{
	return std::string("С�����Ų�ѯ����");
}
std::string ICInfoQueryTask::GetObjectName()
{
	return std::string("ICInfoQueryTask");
}



#endif