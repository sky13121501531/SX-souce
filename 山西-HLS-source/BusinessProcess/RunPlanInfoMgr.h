///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����RunPlanInfoMgr.h
// �����ߣ�gaoxd
// ����ʱ�䣺2009-09-01
// ��������������ͼ��Ϣ����
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "ace/Synch.h"
#include "ace/Singleton.h"
#include <string>
#include <vector>
#include "../Foundation/TypeDef.h"

class RunPlanInfoMgr
{
public:
	RunPlanInfoMgr();
	virtual ~RunPlanInfoMgr();
public:
	bool InitPlanInfo(); //��ʼ������ͼ��Ϣ
	bool UpdatePlanInfo(enumDVBType dvbtype,const std::string& ChannelID,const RunPlanParamVec& OneChannelRunPlanParamVec); //��������ͼ��Ϣ
	bool InRunPlan(enumDVBType dvbtype,const std::string& ChannelID);	//�Ƿ�������ͼ��
	bool GetRunPlanByDvbtype(enumDVBType eDvbtype, RunPlanParamVec& vecRunPlanInfo);		//����dvbtype��ȡ����ͼ��Ϣ
	bool CheckAlarmTime(enumDVBType dvbtype,const std::string& ChannelID,time_t& AlarmTime);	//�������ͼ�����󣬱���ʱ���Ƿ�������ͼ����
private:
	RunPlanParamVec  mRunPlanParamVec;

	ACE_Thread_Mutex m_RunPlanMutex;
};

typedef ACE_Singleton<RunPlanInfoMgr,ACE_Mutex>  RUNPLANINFOMGR;
