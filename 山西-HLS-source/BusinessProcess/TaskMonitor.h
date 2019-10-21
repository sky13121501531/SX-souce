///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����TaskMonitor.h
// �����ߣ�gaoxd
// ����ʱ�䣺2009-05-26
// �������������������
// ʹ��˵���������ཫִ������Ϊ��Ա����ʹ�ã�ʹ��ʱ��Ҫ��std::list<int/*ͨ����*/>������ʼ����
//			 ��ʼ��������list�е��豸�ų�ʼ����Ӧ��ִ�������
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <list>
#include <map>
#include <string>
#include "ace/Task.h"
#include <vector>
#include "../Foundation/TypeDef.h"
class XMLTask;
class TaskExecuteMgr;
struct DelTaskInfo
{
	eDVBType dvbtype;
	std::string taskid;
};
class TaskMonitor: public ACE_Task<ACE_MT_SYNCH>
{
public:
	TaskMonitor(void);
	~TaskMonitor(void);

public:
	int Init();

	int open(void*);

	virtual int svc();

	int Stop();

public:
	bool AddTask(XMLTask* task);											//�������
	bool AddRecTask(XMLTask* task);											//�������
	bool AddRecTaskInfo(XMLTask* task);									    //�������
	bool DelTask(std::string taskid);										//ɾ������ͨ���ڵ�ָ������
	bool DelTask(int deviceid,std::string taskid);							//ɾ��ָ��ͨ����ָ������
	bool DelTask(eDVBType dvbtype,std::string taskid);						//ɾ��ָ��ָ������ָ����������ɾ���������Զ�¼��
	//
	bool DelRecTask(eDVBType dvbtype,std::string taskid);						//
	bool DelRecInfo(eDVBType dvbtype,std::string taskid);						//
	//
	bool QueryTaskInfo(int deviceid,std::vector<sTaskInfo>& taskinfovec);		//��ȡָ��ͨ����ȫ��������Ϣ
	bool QueryRunTaskInfo(int deviceid,sTaskInfo& taskinfo);					//��ȡָ��ͨ��������������Ϣ
	bool QueryRecRunTaskInfo(int deviceid,sTaskInfo& taskinfo);					//
	bool QueryAllTaskInfo(eDVBType dvbtype,std::vector<sTaskInfo>& taskinfovec);//��ȡָ�����͵�ȫ��������Ϣ

	bool GetAutoRecordDeviceIdByFreq(eDVBType dvbtype,std::string freq,int& deviceid,std::string serviceid="",std::string videopid="",std::string audiopid="");//ͨ�����ͺ�Ƶ���ȡͨ����Ϣ
	bool GetAutoRecordDeviceIdByFreqEx(eDVBType dvbtype,std::string freq,int& deviceid,bool isHD,std::string serviceid="",std::string videopid="",std::string audiopid="");		//ͨ�����ͺ�Ƶ���ȡͨ����Ϣ
	bool GetAutoRecordDeviceIdByChannelID(eDVBType dvbtype,std::string channelid,int& deviceid); //ͨ��������ͣ�channelid��ȡͨ����Ϣ

	bool UpdateRunPriority(std::vector<sTaskInfo> taskinfovec);				//��������ִ�����ȼ�
	bool SetManualRecord(int deviceid,enumDVBType dvbtype,std::string& Taskxml);			//�����ֶ�¼��

	bool ShareDevice(XMLTask* task);	//�ж��������ܷ���Ӳ�� 
	int  UnGetDevIdSize();//���ڷ���ͨ��������ִ��ǰ,���δ����ͨ��������ͬʱ�·�,���ڶ��������ͬһͨ�����
	//
	bool IsExistTaskByTaskid(eDVBType dvbtype,std::string taskid);//¼��¼��ָ��ʹ��,���ָ������ʱ�ж�ָ�������Ƿ��Ѿ�����,����¼�����һ��Ƶ������Ŀ
	bool IsExistRecTaskByFreq(eDVBType dvbtype,std::string freq);//¼��¼��ָ��ʹ��,ɾ��¼������ʱ,�Ƿ�ɾ��ָ������,����Ƶ���¼�������Ƿ����,���ھͲ�ɾ��,�����ھ�ɾ��
private:
	bool bFlag;
	time_t t_TaskInfoMapUpdateTime;
	sTaskInfo TaskInfoList[500];
	bool UpDataTaskInfoMapShadow();

	ACE_Thread_Mutex MonitorMapMutex;

	std::list<int>							DeviceList;	//ͨ�����У�
	//
	ACE_Thread_Mutex DelTaskInfoMutex;
	std::list<DelTaskInfo>					DelTaskInfoList;	//ɾ��������Ϣ��
	//
	ACE_Thread_Mutex AddRecMutex;
	std::list<XMLTask*>						RecTaskInfoList;	//ɾ��������Ϣ��
	//
	std::map<int,std::list<XMLTask*> >		TaskMonitorMap;	//���ȶ��У�map<ͨ����,list<����ָ��> >
	std::map<int,TaskExecuteMgr*>			TaskExecuteMgrMap;	//ִ�ж��У�map<ͨ����,list<ִ�ж���ָ��> >
	
};
