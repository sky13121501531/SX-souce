
#include "DeviceStatusQueryTask.h"
#include "TranslateDownXML.h"
#include "TranslateUpXML.h"
#include "ace/Log_Msg.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/OSFunction.h"
#include "../BusinessProcess/BusinessLayoutMgr.h"

DeviceStatusQueryTask::DeviceStatusQueryTask() : DeviceIndependentTask()
{

}

DeviceStatusQueryTask::DeviceStatusQueryTask(std::string strXML) : DeviceIndependentTask(strXML)
{
}

DeviceStatusQueryTask::~DeviceStatusQueryTask()
{

}

void DeviceStatusQueryTask::Run()
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]ͨ��״̬��ѯ����ִ�� !\n",DeviceID));
	SetRunning();

	RetValue = RUN_SUCCESS;
	XmlParser psr;
	psr.Set_xml(strStandardXML);

	pXMLNODE rootnode=psr.GetRootNode();

	//���Ҫ��ѯ��ͨ����
	pXMLNODE node=psr.GetNodeFromPath("Msg/DeviceStatusQuery");
	pXMLNODELIST nodelist=psr.GetNodeList(node);
	std::vector<sDeviceStatus> VecDeviceStatus;
	eDVBType dvbtype = UNKNOWN;
	
	try
	{
		for(int i=0;i<nodelist->Size();++i)
		{
			node=psr.GetNextNode(nodelist);
			std::string LogicDeviceId,DeviceId;
			psr.GetAttrNode(node,"DeviceID",LogicDeviceId);
			sDeviceStatus curDeviceStatus;
			std::string desc;
			int status=-1;
			PROPMANAGER::instance()->GetDeviceIndex(DeviceId,LogicDeviceId,OSFunction::GetStrDVBType(GetDVBType()));
			PROPMANAGER::instance()->GetDeviceType(atoi(DeviceId.c_str()),dvbtype);
			if (PROPMANAGER::instance()->IsDeviceAvaiable(StrUtil::Str2Int(DeviceId)) == false|| dvbtype != GetDVBType())  //ͨ�����Ϸ���������Ͳ���
			{ 
				curDeviceStatus.DevicID=StrUtil::Str2Int(LogicDeviceId);
				curDeviceStatus.Statuc=1;
				curDeviceStatus.Desc=string("�ް忨");
				VecDeviceStatus.push_back(curDeviceStatus);
			}
			else
			{
				BUSINESSLAYOUTMGR::instance()->DeviceStatusQuery(StrUtil::Str2Int(DeviceId),status,desc);//���ͨ��״̬

				curDeviceStatus.DevicID=StrUtil::Str2Int(LogicDeviceId);
				curDeviceStatus.Statuc=status;
				curDeviceStatus.Desc=desc;
				VecDeviceStatus.push_back(curDeviceStatus);
			}
		}
	}
	catch(...)
	{
		RetValue = RUN_FAILED;
	}

	SendXML(TranslateUpXML::TranslateDeviceStatusQuery(this,VecDeviceStatus));//����xml

	SetFinised();
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]ͨ��״̬��ѯ����ֹͣ !\n",DeviceID));
}

std::string DeviceStatusQueryTask::GetTaskName()
{
	return "ͨ��״̬��ѯ����";
}

std::string DeviceStatusQueryTask::GetObjectName()
{
	return std::string("DeviceStatusQueryTask");
}