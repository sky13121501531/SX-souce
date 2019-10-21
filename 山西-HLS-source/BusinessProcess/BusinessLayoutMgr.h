///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����BusinessLayoutMgr.h
// �����ߣ�jiangcheng
// ����ʱ�䣺2009-05-27
// ����������ҵ����ȹ滮��ӿڹ�����
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ace/Synch.h"
#include "ace/Singleton.h"
#include "./TaskMonitor.h"
#include "./TaskRealTimeMonitor.h"
#include "./ClientInfoMgr.h"
#include "DiskSpaceMgr.h"
#include "../DeviceMgr/DeviceManager.h"

class BusinessLayoutMgr
{
public:
	BusinessLayoutMgr();
	virtual ~BusinessLayoutMgr();
public:
	int Start();
	int Stop();	

public:

	bool AddTask(XMLTask* task);			//���ȶ���ͳһ���������ӿ�
	bool AddRecTask(XMLTask* task);			//���ȶ���ͳһ���������ӿ�
	bool DelTask(std::string taskid);		//���ȶ���ͳһ��ɾ������ӿ�
	bool DelTask(int deviceid,std::string taskid);
	bool DelTask(eDVBType dvbtype,std::string taskid);
	bool DelRecInfo(eDVBType dvbtype,std::string taskid);
	bool DeviceStatusQuery(int deviceid,int& DeviceStatus,std::string& TaskName);	//ͨ��״̬��ѯ�ӿ�
	bool SetManualRecord(int deviceid,enumDVBType dvbtype,std::string& Taskxml);	//�����ֶ�¼��

	bool QueryRunTaskInfo(int deviceid,sTaskInfo& taskinfo);								//��ȡָ��ͨ��������������Ϣ
	bool QueryRecRunTaskInfo(int deviceid,sTaskInfo& taskinfo);								//
	bool QueryAllTaskInfo(eDVBType dvbtype,std::vector<sTaskInfo>& taskinfovec);			//��ȡָ�����͵�ȫ��������Ϣ
	bool GetAutoRecordDeviceIdByFreq(eDVBType dvbtype,std::string freq,int& deviceid,std::string serviceid="",std::string videopid="",std::string audiopid="");		//ͨ�����ͺ�Ƶ���ȡͨ����Ϣ
	bool GetAutoRecordDeviceIdByFreqEx(eDVBType dvbtype,std::string freq,int& deviceid,bool isHD,std::string serviceid="",std::string videopid="",std::string audiopid="");		//ͨ�����ͺ�Ƶ���ȡͨ����Ϣ
	bool GetAutoRecordDeviceIdByChannelID(eDVBType dvbtype,std::string channelid,int& deviceid);  //ͨ��������ͣ�channelID��ȡͨ����Ϣ
	bool AddDeviceSocket(ACE_SOCK_Stream socket);

#ifdef ZONG_JU_ZHAO_BIAO
	bool QueryTaskInfo(enumDVBType eDvbType, int deviceId, std::vector<sTaskInfo>& taskInfoVec);
#endif
	bool IsExistTaskByTaskid(eDVBType dvbtype,std::string taskid);//¼��¼��ָ��ʹ��,���ָ������ʱ�ж�ָ�������Ƿ��Ѿ�����,����¼�����һ��Ƶ������Ŀ
	bool IsExistRecTaskByFreq(eDVBType dvbtype,std::string freq);//¼��¼��ָ��ʹ��,ɾ��¼������ʱ,�Ƿ�ɾ��ָ������,����Ƶ���¼�������Ƿ����,���ھͲ�ɾ��,�����ھ�ɾ��
	//����Ƶ�û�����
	bool AddUser(ACE_SOCK_Stream client,string& xml);
	bool GetUser(enumDVBType DvbType,std::vector<sVedioUserInfo>& uservec);
	bool StopUser(int deviceid);

private:
	bool TaskChannleIsValid(XMLTask* task);			//�ж�����Ƶ���Ƿ�Ϸ�
	bool ProcessTaskDeviceID(XMLTask* task);		//��������ͨ����Ϣ����ͨ���жϺϷ��ԣ���ͨ���Զ�����ͨ��
	bool ProcessTaskRunPriority(XMLTask* task);		//���������������ȼ�
	bool ProcessRealtimeTask(XMLTask* task);		//����ʵʱ��������
	bool ProcessAutorecordTask(XMLTask* task);		//�����Զ�¼����������

private:
	void CreateInstance();
	void DestoryInstance();

private:
	TaskMonitor* pTaskMonitor;
	TaskRealTimeMonitor* pTaskRealTimeMonitor;
	DiskSpaceMgr *pDiskSpaceMgr;
	ClientInfoMgr* pClientInfoMgr;
	DeviceManager* pDeviceManager;

public:
	std::list<int> devicelistmul1rt; //�໭��1�����տ�ͨ����
// #ifdef ZONG_JU_ZHAO_BIAO
// 	ChanEncryptCheckMgr *m_pEncryCheckThread;
// #endif
};

typedef ACE_Singleton<BusinessLayoutMgr,ACE_Mutex>  BUSINESSLAYOUTMGR;
