///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����BusinessLayoutMgr.cpp
// �����ߣ�jiangcheng
// ����ʱ�䣺2009-05-27
// ����������ҵ����ȹ滮��ӿڹ�����
///////////////////////////////////////////////////////////////////////////////////////////
#include "BusinessLayoutMgr.h"
#include "../Foundation/PropManager.h"
#include "../Task/XMLTask.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/AppLog.h"
#include "../Foundation/StrUtil.h"
#include "RecordUploadProcessMgr.h"
#include "ChannelInfoMgr.h"
int g_iCurRealChan = -1;
RecordUploadProcessMgr* g_pRecordUploadProcessMgr;
BusinessLayoutMgr::BusinessLayoutMgr()
{
	CreateInstance();
}

BusinessLayoutMgr::~BusinessLayoutMgr()
{
	DestoryInstance();
}

void BusinessLayoutMgr::CreateInstance()
{
	pTaskMonitor = new TaskMonitor;
	pTaskRealTimeMonitor = new TaskRealTimeMonitor;
	pDiskSpaceMgr = new DiskSpaceMgr;
	pClientInfoMgr = new ClientInfoMgr;
	pDeviceManager = new DeviceManager;
	g_pRecordUploadProcessMgr = new RecordUploadProcessMgr;
}

void BusinessLayoutMgr::DestoryInstance()
{
	if (NULL != pTaskMonitor)
	{
		delete pTaskMonitor;
		pTaskMonitor = NULL;
	}

	if (NULL != pTaskRealTimeMonitor)
	{
		delete pTaskRealTimeMonitor;
		pTaskRealTimeMonitor = NULL;
	}

	if (NULL != pDiskSpaceMgr)
	{
		delete pDiskSpaceMgr;
		pDiskSpaceMgr = NULL;
	}

	if (NULL != pClientInfoMgr)
	{
		delete pClientInfoMgr;
		pClientInfoMgr = NULL;
	}

	if (NULL != pDeviceManager)
	{
		delete pDeviceManager;
		pDeviceManager = NULL;
	}
	if (NULL != g_pRecordUploadProcessMgr)
	{
		delete g_pRecordUploadProcessMgr;
		g_pRecordUploadProcessMgr = NULL;
	}
}

int BusinessLayoutMgr::Start()
{
	//�豸����������ִ��ģ���ʼ��
	if (pTaskMonitor->Init() == -1)
	{
		ACE_DEBUG((LM_ERROR,"(%T| %t)����ִ��ģ���ʼ������!\n"));
		return -1;
	}
	else
	{
		pTaskMonitor->open(0);
	}
	
	if (pTaskRealTimeMonitor != NULL)
		pTaskRealTimeMonitor->open(0); //�豸�޹��������ִ��ģ���ʼ��
	
	if (pDiskSpaceMgr != NULL)	
		pDiskSpaceMgr->open(0);			//���̹���ģ���ʼ��ִ��
	
	if (pClientInfoMgr != NULL)
		pClientInfoMgr->open(0);		//�û���Ϣ�����߳�����
	
	if (pDeviceManager != NULL)
		pDeviceManager->open(0);		//�豸���������̣߳�
	
	if (g_pRecordUploadProcessMgr != NULL)
		g_pRecordUploadProcessMgr->open(0);		

	return 0;
}

int BusinessLayoutMgr::Stop()
{
	if (NULL != pTaskMonitor)
		pTaskMonitor->Stop();

	if (NULL != pTaskRealTimeMonitor)
		pTaskRealTimeMonitor->Stop();

	if (NULL != pDiskSpaceMgr)
		pDiskSpaceMgr->Stop();

	if (NULL != pClientInfoMgr)
		pClientInfoMgr->Stop();

	if (NULL != pDeviceManager)
		pDeviceManager->Stop();

	if (NULL != g_pRecordUploadProcessMgr)
		g_pRecordUploadProcessMgr->Stop();

	return 0;
}
bool BusinessLayoutMgr::AddDeviceSocket(ACE_SOCK_Stream socket)
{
	return false;
}
///////////////////���������ؽӿ�/////////////////////////

bool BusinessLayoutMgr::AddTask(XMLTask* task)
{
	if(task == NULL)
		return false;

	if (task->GetBaseObjectName() == "DeviceIndependentTask")
	{
		return pTaskRealTimeMonitor->AddTask(task);	//�������ͨ���޹�ִ�ж�����
	}
	//��ͨ�������Ҫ���ȵ�����
	if(	TaskChannleIsValid(task)		&&		//�ж�����Ƶ����ͨ���Ƿ�Ϸ�
		ProcessTaskDeviceID(task)		&&		//��������ͨ�����жϺϷ��Ի����Զ�����ͨ����
		ProcessTaskRunPriority(task)	&&		//���������������ȼ�
		ProcessRealtimeTask(task)		&&		//����ʵʱ�����ж��Ƿ�ɹ���ִ�У��Ƿ��и������ȼ�ռ��
		ProcessAutorecordTask(task))			//�����Զ�¼������
	{
		return pTaskMonitor->AddTask(task);
	}
	else
	{
		return pTaskRealTimeMonitor->AddTask(task);			//�����ȼ�����ֱ�ӽ���ʵʩִ�ж������У�������Ӧ��XML
	}
	return false;
}
bool BusinessLayoutMgr::AddRecTask(XMLTask* task)
{
	if(task == NULL)
		return false;

	if (task->GetBaseObjectName() == "DeviceIndependentTask")
	{
		return pTaskRealTimeMonitor->AddTask(task);	//�������ͨ���޹�ִ�ж�����
	}
	//��ͨ�������Ҫ���ȵ�����
	if(	TaskChannleIsValid(task)		&&		//�ж�����Ƶ����ͨ���Ƿ�Ϸ�
		ProcessTaskDeviceID(task)		&&		//��������ͨ�����жϺϷ��Ի����Զ�����ͨ����
		ProcessTaskRunPriority(task)	&&		//���������������ȼ�
		ProcessRealtimeTask(task)		&&		//����ʵʱ�����ж��Ƿ�ɹ���ִ�У��Ƿ��и������ȼ�ռ��
		ProcessAutorecordTask(task))			//�����Զ�¼������
	{
		return pTaskMonitor->AddRecTaskInfo(task);
	}
	else if((task->GetObjectName()!="AutoRecord")&& (task->GetObjectName()!="TaskRecord"))//¼��û����Դ����ִ��
	{
		return pTaskRealTimeMonitor->AddTask(task);			//�����ȼ�����ֱ�ӽ���ʵʩִ�ж������У�������Ӧ��XML
	}
	return false;
}

bool BusinessLayoutMgr::TaskChannleIsValid(XMLTask* task)
{
	if(task == NULL)
		return false;

	if (task->IsVedioTask() && task->GetChannelID().length()<1)
	{
		string msg = "��Ƶ����Ƶ���Ƿ���Ƶ��IDΪ��";
		//SYSMSGSENDER::instance()->SendMsg(msg,task->GetDVBType(),VS_MSG_SYSALARM);

		task->SetRetValue(CHANNELID_UNAVAIABLE);
		return false;
	}

	return true;
}
bool BusinessLayoutMgr::ProcessTaskDeviceID(XMLTask* task)
{
	if(task == NULL)
		return false;

	//����ҳ������ʵʱ��Ƶͨ��
	if(task->GetObjectName()=="StreamRealtimeQueryTask"||task->GetObjectName()=="StreamRealtimeRoundQueryTask")
	{
		list<int> DevList;
		int i_index = 0;
		if(task->GetVersion() == "3.0")
		{
			if(task->GetDeviceID() < 0)
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		else if(PROPMANAGER::instance()->GetCurWorkVersion() == "3.9"||PROPMANAGER::instance()->GetCurWorkVersion() == "4.0")
		{
			PROPMANAGER::instance()->GetTaskDeviceList("StreamRealtimeRoundQueryTask", task->GetDVBType(), DevList);
			std::list<int>::iterator pt=DevList.begin();
			for(;pt!=DevList.end();pt++)
			{
				if(g_iCurRealChan != *pt)
				{
					i_index = *pt;
					g_iCurRealChan = i_index;
					break;
				}
			}
		}
		else
		{
			PROPMANAGER::instance()->GetTaskDeviceList(task->GetObjectName(), task->GetDVBType(), DevList);
			list<int>::iterator iter_list = DevList.begin();
			if(DevList.size()>0)
			{
				i_index = *iter_list;
			}
		}
		if(task->GetDVBType() == DVBC)//�ܾ���������ǰ��ͨ�����ýӿ���ͣ��
		{
			if (task->GetDeviceID() < 0)
			{
				task->SetDeciveID(i_index);
			}
			return true;
		}
		task->SetDeciveID(i_index);
	}
	else if(task->GetObjectName()=="QualityRealtimeQueryTask" || task->GetObjectName()=="QualityTask")
	{
		if(task->GetVersion() == "3.0")
		{
			if(task->GetDeviceID() < 0)
			{
				return false;
			}
			else
			{
				int i_index = PROPMANAGER::instance()->GetCoderDevNum();//���������TaskMonitorMap��ָ��ͨ������ת���豸����,������0,1�Ȼ����ͨ������,��ִ��ʱ����ȡͨ�����ȥGetCoderDevNum,MER-BER�϶���Ҫ����ȡ
				task->SetDeciveID(i_index+task->GetDeviceID());
				return true;
			}
		}
		else
		{
			list<int> DevList;
			int i_index = 0;
			PROPMANAGER::instance()->GetTaskDeviceList(task->GetObjectName(), task->GetDVBType(), DevList);
			list<int>::iterator iter_list = DevList.begin();
			if((task->GetDVBType() == DVBC)||(task->GetDVBType() == CTTB))//�����ź�ȡ��Mer-Ber�豸,��ģ���ź�һ��
			{
				for(iter_list; iter_list != DevList.end(); iter_list++)
				{
					i_index = *iter_list;
					Quality_DevInfo* p = NULL;
					p = PROPMANAGER::instance()->GetQuaDevInfo(i_index);
					if (p->s_devType != "Mer-Ber")
					{
						break;
					}
				}
			}
			else
			{
				if(DevList.size()>0)
				{
					i_index = *iter_list;
				}
			}
			i_index += PROPMANAGER::instance()->GetCoderDevNum();//���������TaskMonitorMap��ָ��ͨ������ת���豸����,������0,1�Ȼ����ͨ������,��ִ��ʱ����ȡͨ�����ȥGetCoderDevNum,MER-BER�϶���Ҫ����ȡ
			task->SetDeciveID(i_index);
			return true;
		}
	}
	else if(task->GetObjectName()=="SpectrumScanTask" || task->GetObjectName()=="SpectrumTask")
	{
		list<int> DevList;
		int i_index = 0;
		PROPMANAGER::instance()->GetTaskDeviceList(task->GetObjectName(), task->GetDVBType(), DevList);
		list<int>::iterator iter_list = DevList.begin();
		i_index = *iter_list;
		i_index += PROPMANAGER::instance()->GetCoderDevNum();//���������TaskMonitorMap��Ƶ��ͨ������ת���豸+ָ��ͨ������,������0,1�Ȼ����ͨ������,��ִ��ʱ����ȡͨ��
		i_index += PROPMANAGER::instance()->GetQuaDevInfoNum();
		task->SetDeciveID(i_index);
		return true;
	}
	else if(task->GetObjectName()=="ChannelScanQueryTask")
	{//�̴߳���ʹ�õ�Ƶͨ��1
		return true;
	}
	else if(task->GetObjectName()=="AutoRecord"||task->GetObjectName()=="TaskRecord")
	{
		if(task->GetVersion() == "3.0")
		{
			if(task->GetDeviceID() < 0)
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		else
		{
			if (task->GetDeviceID() >= 0)		//��ͨ���ж�
			{
				if (false == PROPMANAGER::instance()->IsDeviceAvaiable(task->GetDeviceID()))
				{
					task->SetRetValue(DEVICEID_UNAVAIABLE);
					string errstr = task->GetTaskName() + string("ͨ��[") + StrUtil::Int2Str(task->GetDeviceID()) + string("]�����ڻ��޷��������");
					//SYSMSGSENDER::instance()->SendMsg(errstr,task->GetDVBType(),VS_MSG_SYSALARM);

					errstr =task->GetTaskName() + string("����ʧ��");
					//SYSMSGSENDER::instance()->SendMsg(errstr,task->GetDVBType(),VS_MSG_SYSALARM);
					return false;
				}
				int devid = task->GetDeviceID();
				if(false == PROPMANAGER::instance()->GetDevID(devid,task->GetDVBType(),StrUtil::Str2Int(task->GetPriority()),task->GetTaskFreq(),task->GetTaskServiceID(),task->GetTaskID(),task->IsHDProgram()))
				{
					string errstr = task->GetTaskName()+task->GetTaskFreq()+ string("����û����Դ,����û��ִ��!");
					//SYSMSGSENDER::instance()->SendMsg(errstr,task->GetDVBType(),VS_MSG_SYSALARM);
					return false;
				}
				task->SetDeciveID(task->GetDeviceID());
				return true;
			}
			else	//��ͨ������
			{
				int devid = -1;
				if(false == PROPMANAGER::instance()->GetDevID(devid,task->GetDVBType(),StrUtil::Str2Int(task->GetPriority()),task->GetTaskFreq(),task->GetTaskServiceID(),task->GetTaskID(),task->IsHDProgram()))
				{
					string errstr = task->GetTaskName()+task->GetTaskFreq()+ string("]û����Դʹ��,����û��ִ��!");
					//SYSMSGSENDER::instance()->SendMsg(errstr,task->GetDVBType(),VS_MSG_SYSALARM);
					return false;
				}
				task->SetDeciveID(devid);
				return true;
			}
		}
	}

	return true;
}
bool BusinessLayoutMgr::ProcessTaskRunPriority(XMLTask* task)
{
	if(task == NULL)
		return false;

	return true;
}
bool BusinessLayoutMgr::ProcessRealtimeTask(XMLTask* task)
{
	if(task == NULL)
		return false;

	if (task->IsRealTimeTask() == false) //��ʵʱ����
		return true;

	if(pTaskMonitor->ShareDevice(task) == true)
		return true;

	vector<sTaskInfo> taskinfovec;
	pTaskMonitor->QueryTaskInfo(task->GetDeviceID(),taskinfovec);

	vector<sTaskInfo>::iterator ptr = taskinfovec.begin();
	for (;ptr!=taskinfovec.end();++ptr)
	{
		if ((*ptr).status == RUNNING)
			break;
	}
	if (ptr!=taskinfovec.end() && StrUtil::Str2Int((*ptr).priority) > StrUtil::Str2Int(task->GetPriority()))	//�����ȼ�����ռ��
	{
		task->SetRetValue(PREFERENTIALTASK_USE);
		return false;
	}
	return true;
}
bool BusinessLayoutMgr::ProcessAutorecordTask(XMLTask* task)
{
	if(task == NULL)
		return false;

	std::string tasktype = task->GetObjectName();
	if (tasktype!="AutoRecord")
		return true;

	std::vector<sTaskInfo> taskinfovec;
	pTaskMonitor->QueryTaskInfo(task->GetDeviceID(),taskinfovec);

	std::vector<sTaskInfo>::iterator Ptr=taskinfovec.begin();
	for (;Ptr!=taskinfovec.end();++Ptr)
	{
		if ((*Ptr).taskname=="AutoRecord")
			break;
	}
	if (Ptr != taskinfovec.end())
	{
		if (StrUtil::Str2Int(task->GetPriority()) >= StrUtil::Str2Int((*Ptr).priority)) //Ҫ���õ��Զ�¼���������ȼ�����ڵ��������Զ�¼����������ȼ���
		{
			return pTaskMonitor->DelTask(task->GetDeviceID(),"0");	//��ʷ�������ù���
		}
		else
		{
			task->SetRetValue(PREFERENTIALTASK_USE);
			return false;
		}
	}
	//���û���ҵ��Զ�¼������ֱ�����ü���
	return true;
}
bool BusinessLayoutMgr::DeviceStatusQuery( int deviceid,int& DeviceStatus,std::string& TaskName )
{
	//�ж�Ĭ��ͨ�����Ƿ�Ϸ�
	//list<int> DeviceList;
	int devnum = PROPMANAGER::instance()->GetCoderDevNum();
	//list<int>::iterator pDeviceList = DeviceList.begin();
	//for (;pDeviceList!=DeviceList.end();++pDeviceList)
	int devi = 0;
	for(;devi < devnum;devi++)
	{
		if (deviceid == devi)
			break;
	}
	if (devi == devnum)//��Ĭ��ͨ�����ڸõ�����������ͨ������
	{
		DeviceStatus = 1;
		TaskName = string("�ް忨");
		return true;
	}
	std::vector<sTaskInfo> taskinfo;
	pTaskMonitor->QueryTaskInfo(deviceid,taskinfo);    //��ѯ��ͨ����������Ϣ
	if (taskinfo.size()>0) 
	{
		size_t i=0;
		for (;i!=taskinfo.size();++i)
		{
			if(taskinfo[i].status==RUNNING)
			{
				DeviceStatus = 2;
				TaskName = taskinfo[i].taskname;      //�����������
				break;
			}
		}
		if (i==taskinfo.size())
		{
			DeviceStatus = 0;
			TaskName = string("����״̬");
		}	
	}
	else
	{
		DeviceStatus = 0;//�豸���ڿ���״̬
        TaskName = string("����״̬");
	}
	return true;
}

bool BusinessLayoutMgr::DelTask( std::string taskid )
{
#ifdef ZONG_JU_ZHAO_BIAO
	//�ܾ�����
	//���taskidΪ�գ������ɾ�������е��Զ�¼����Ϊ�Զ�¼���taskid��Ϊ��
	if (taskid.length()<1)
		return false;
#endif
	
	if (pTaskMonitor == NULL)
		return false;

	return pTaskMonitor->DelTask(taskid);
}

bool BusinessLayoutMgr::DelTask( int deviceid,std::string taskid )
{
	if (pTaskMonitor == NULL)
		return false;

	return pTaskMonitor->DelTask(deviceid,taskid);
}

bool BusinessLayoutMgr::DelTask(eDVBType dvbtype,std::string taskid)
{
	if (pTaskMonitor == NULL)
		return false;

	return pTaskMonitor->DelTask(dvbtype,taskid);
}

bool BusinessLayoutMgr::DelRecInfo(eDVBType dvbtype,std::string taskid)
{
	if (pTaskMonitor == NULL)
		return false;

	return pTaskMonitor->DelRecInfo(dvbtype,taskid);
}

bool BusinessLayoutMgr::SetManualRecord(int deviceid,enumDVBType dvbtype,std::string& Taskxml)
{
	if (pTaskMonitor == NULL)
		return false;

	return pTaskMonitor->SetManualRecord(deviceid,dvbtype,Taskxml);
}
bool BusinessLayoutMgr::QueryRunTaskInfo(int deviceid,sTaskInfo& taskinfo)
{
	if (pTaskMonitor == NULL)
		return false;

	return pTaskMonitor->QueryRunTaskInfo(deviceid,taskinfo);
}
bool BusinessLayoutMgr::QueryRecRunTaskInfo(int deviceid,sTaskInfo& taskinfo)
{
	if (pTaskMonitor == NULL)
		return false;

	return pTaskMonitor->QueryRecRunTaskInfo(deviceid,taskinfo);
}
bool BusinessLayoutMgr::QueryAllTaskInfo(eDVBType dvbtype,std::vector<sTaskInfo>& taskinfovec)
{
	if (pTaskMonitor == NULL)
		return false;

	return pTaskMonitor->QueryAllTaskInfo(dvbtype,taskinfovec);
}
bool BusinessLayoutMgr::GetAutoRecordDeviceIdByFreq(eDVBType dvbtype,std::string freq,int& deviceid,std::string serviceid,std::string videopid,std::string audiopid)
{
	if (pTaskMonitor == NULL)
		return false;

	return pTaskMonitor->GetAutoRecordDeviceIdByFreq(dvbtype,freq,deviceid,serviceid,videopid,audiopid);
}

bool BusinessLayoutMgr::GetAutoRecordDeviceIdByFreqEx(eDVBType dvbtype,std::string freq,int& deviceid,bool isHD,std::string serviceid,std::string videopid,std::string audiopid)
{
	if (pTaskMonitor == NULL)
		return false;

	return pTaskMonitor->GetAutoRecordDeviceIdByFreqEx(dvbtype,freq,deviceid,isHD,serviceid,videopid,audiopid);
}

bool BusinessLayoutMgr::GetAutoRecordDeviceIdByChannelID(eDVBType dvbtype,std::string channelid,int& deviceid)
{
	if (pTaskMonitor == NULL)
		return false;

	return pTaskMonitor->GetAutoRecordDeviceIdByChannelID(dvbtype,channelid,deviceid);
}

//����Ƶ�û�����
bool BusinessLayoutMgr::AddUser(ACE_SOCK_Stream client,string& xml)
{
	if (pClientInfoMgr == NULL)
		return false;

	return pClientInfoMgr->AddUser(client,xml);
}
bool BusinessLayoutMgr::GetUser(enumDVBType DvbType,std::vector<sVedioUserInfo>& uservec)
{
	if (pClientInfoMgr == NULL)
		return false;

	return pClientInfoMgr->GetUser(DvbType,uservec);
}
bool BusinessLayoutMgr::StopUser(int deviceid)
{
	if (pClientInfoMgr == NULL)
		return false;

	return pClientInfoMgr->StopUser(deviceid);
}


#ifdef ZONG_JU_ZHAO_BIAO

bool BusinessLayoutMgr::QueryTaskInfo( enumDVBType eDvbType, int deviceId, std::vector<sTaskInfo>& taskInfoVec )
{
	if (pTaskMonitor == NULL)
		return false;

	return pTaskMonitor->QueryTaskInfo(deviceId, taskInfoVec);
}

bool BusinessLayoutMgr::IsExistTaskByTaskid(eDVBType dvbtype,std::string taskid)
{
	if (pTaskMonitor == NULL)
		return false;

	return pTaskMonitor->IsExistTaskByTaskid(dvbtype,taskid);
}
bool BusinessLayoutMgr::IsExistRecTaskByFreq(eDVBType dvbtype,std::string freq)
{
	if (pTaskMonitor == NULL)
		return false;

	return pTaskMonitor->IsExistRecTaskByFreq(dvbtype,freq);
}

#endif