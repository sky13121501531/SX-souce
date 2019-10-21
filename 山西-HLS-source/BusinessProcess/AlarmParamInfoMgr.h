#pragma once
#include "ace/Synch.h"
#include "ace/Singleton.h"
#include "./TaskMonitor.h"
#include "../Foundation/TypeDef.h"
#include <string>
#include <vector>

class AlarmParamInfoMgr
{
public:
	AlarmParamInfoMgr(void);
	~AlarmParamInfoMgr(void);

	bool InitAlarmParamInfo();		//�����ݿ��ж�ȡ��������������Ϣ����ʼ�����ڴ���
	bool UpdateAlarmParam(std::vector<sAlarmParam>& vecalaramparam);				//����ָ��Ƶ��ı���������Ϣ
	bool GetAlarmParam(const sCheckParam& checkparam,sAlarmParam& alarmparam);		//���ݱ����Ĳ�����Ϣ��ö�Ӧ�ı���������Ϣ
	bool UpdateAllAlarmParam(std::vector<sAlarmParam>& vecalaramparam);				//��������Ƶ��ı���������Ϣ
	bool GetAlarmParamByDvbtype(enumDVBType eDvbtype, AlarmParamVec& mapAlarmParam);		//ͨ��eDvbtype��ȡĳһ������͵����б�������
private:
	std::vector<sAlarmParam> vecAlarmParam;
	ACE_Thread_Mutex AlarmMapMutex;
};
typedef ACE_Singleton<AlarmParamInfoMgr,ACE_Mutex>  ALARMPARAMINFOMGR;
