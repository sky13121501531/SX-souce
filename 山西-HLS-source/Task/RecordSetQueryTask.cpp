#include "RecordSetQueryTask.h"
#include "../DBAccess/DBManager.h"
#include "TranslateUpXML.h"
using namespace std;
RecordSetQueryTask::RecordSetQueryTask() : DeviceIndependentTask()
{

}

RecordSetQueryTask::RecordSetQueryTask(std::string strXML) : DeviceIndependentTask(strXML)
{
}

RecordSetQueryTask::~RecordSetQueryTask()
{

}
void RecordSetQueryTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]�Զ�¼�����ò�ѯ����ִ�� !\n",DeviceID));
	bRun = true;
	SetRunning();

	std::vector<std::string>  vctXML;
	if( DBMANAGER::instance()->QueryTask(DVBType,vctXML) ==false )
		RetValue = RUN_FAILED;
	else
		RetValue = RUN_SUCCESS;

	SendXML(TranslateUpXML::TranslateRecordSetQuery(this,vctXML)); //���͵�ǰ��
	
	SetFinised(); 
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]�Զ�¼�����ò�ѯ����ֹͣ !\n",DeviceID));
	return;
}

string RecordSetQueryTask::GetTaskName()
{
	return "�Զ�¼�����ò�ѯ����";
}
std::string RecordSetQueryTask::GetObjectName()
{
	return m_strObjectName;
}

