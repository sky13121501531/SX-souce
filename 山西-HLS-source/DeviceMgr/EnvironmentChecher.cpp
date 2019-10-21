
#include "EnvironmentChecher.h"
#include "OS_Environment.h"
//#include "../Alarm/TR290AlarmMgr.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/StrUtil.h"
#include "../Alarm/AlarmMgr.h"

EnvironmentChecher::EnvironmentChecher()
{
	bFlag = false;
}

EnvironmentChecher::~EnvironmentChecher()
{

}

int EnvironmentChecher::open(void *)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ��������߳̿�ʼִ�� !\n"));
	this->activate();
	return 0;
}

int EnvironmentChecher::svc()
{
	bFlag = true;
	OS_Environment::CPURealtimeUsage();//������һ��CPUʹ����ֵ
	while (bFlag)
	{
	   OSFunction::Sleep(1,0);
	   //���CPU
	   sCheckParam checkparam;
	   checkparam.AlarmType=ALARM_TR101_290;
	   checkparam.DVBType=CTTB;
	   checkparam.TypeDesc="TSͬ����ʧ";
	   checkparam.TypeID="2";
	   checkparam.Freq="666";
	   checkparam.STD="DMB-T";
	   checkparam.SymbolRate="6875";
	   checkparam.TypedValue=StrUtil::Long2Str(OS_Environment::CPURealtimeUsage());
	   std::string xml;
	  // TR290ALARMMGR::instance()->CheckAlarm(checkparam,xml);
	  
	   //����ڴ�
	   /*checkparam.AlarmType=ALARM_ENVIRONMENT;
	   checkparam.DVBType=CTTB;
	   checkparam.TypeDesc="Memoryʹ����";
	   checkparam.TypeID="51";
	   checkparam.TypedValue=StrUtil::Long2Str( OS_Environment::MemoryRealtimeUsage());

	   ALARMMGR::instance()->CheckAlarm(checkparam);
	  */
	}

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ��������߳�ִֹͣ�� !\n"));

	return 0;
}

int EnvironmentChecher::Start()
{
	open(0);
	return 0;
};
int EnvironmentChecher::Stop()
{
	bFlag = false;
	this->wait();
	return 0;
}