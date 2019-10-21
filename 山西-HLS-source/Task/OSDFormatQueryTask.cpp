#include "OSDFormatQueryTask.h"
#include "../DBAccess/DBManager.h"
#include "TranslateUpXML.h"
using namespace std;
OSDFormatQueryTask::OSDFormatQueryTask() : DeviceIndependentTask()
{

}

OSDFormatQueryTask::OSDFormatQueryTask(std::string strXML) : DeviceIndependentTask(strXML)
{
}

OSDFormatQueryTask::~OSDFormatQueryTask()
{

}
void OSDFormatQueryTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]OSD����������Ϣ����ִ�� !\n",DeviceID));
	bRun = true;
	SetRunning();
	
	vector<OSDFormat> VecOSDFormat;
	if(DBMANAGER::instance()->GetOSDFormatSetInfo(DVBC,VecOSDFormat) == false)
		RetValue = RUN_FAILED;
	else
		RetValue = RUN_SUCCESS;

	for(int i = 0; i < VecOSDFormat.size(); i++)
	{
		VecOSDFormat[i].Freq.erase(VecOSDFormat[i].Freq.find_last_not_of(" ")+1);
		VecOSDFormat[i].ServiceID.erase(VecOSDFormat[i].ServiceID.find_last_not_of(" ")+1);
		VecOSDFormat[i].FontSize.erase(VecOSDFormat[i].FontSize.find_last_not_of(" ")+1);
		VecOSDFormat[i].Position.erase(VecOSDFormat[i].Position.find_last_not_of(" ")+1);
		VecOSDFormat[i].AntiColor.erase(VecOSDFormat[i].AntiColor.find_last_not_of(" ")+1);
		VecOSDFormat[i].Type.erase(VecOSDFormat[i].Type.find_last_not_of(" ")+1);
		VecOSDFormat[i].Align.erase(VecOSDFormat[i].Align.find_last_not_of(" ")+1);
	}
	SendXML(TranslateUpXML::TranslateOSDFormatQuery(this,VecOSDFormat)); //���͵�ǰ��
	
	SetFinised(); 
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]OSD����������Ϣ����ֹͣ !\n",DeviceID));
	return;
}

string OSDFormatQueryTask::GetTaskName()
{
	return "ǰ���������ò�ѯ����";
}
std::string OSDFormatQueryTask::GetObjectName()
{
	return m_strObjectName;
}

