#pragma once

///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����AlarmMgr.h
// �����ߣ�gaoxd
// ����ʱ�䣺2009-09-08
// ��������������ģ�������
///////////////////////////////////////////////////////////////////////////////////////////
#include "ace/Synch.h"
#include "ace/Singleton.h"
#include <string>
#include <vector>
#include "../Foundation/TypeDef.h"
#include <ace/Task.h>
class AlarmSender;
class ReSendAlarm;

class AlarmMgr:public ACE_Task<ACE_MT_SYNCH>
{
public:
	AlarmMgr(void);
	~AlarmMgr(void);

public:
	bool Start();
	virtual int open(void*);
	virtual int Stop();
	virtual int svc();

public:
	bool CheckAlarm(sCheckParam& checkparam,bool IsConvert);		//�����ṩ��ͬ�ӿڣ�ʹ�ýṹ������������ڽ�����Ӧ�������͵ı仯
	bool ClearAlarm(string Freq);									//�������״̬
	void SendAlarm(std::string alarmxml);
protected:	//��Ӧ��ͬ�ı����߼�
	bool CheckEnvironmenAlarmParam(const sCheckParam& checkparam,sAlarmParam& alarmparam,std::string& alarmxml);
	bool CheckequipmenAlarmParam(const sCheckParam& checkparam,std::string& alarmxml);
	bool CheckFreqAlarmParam(const sCheckParam& checkparam,sAlarmParam& alarmparam,std::string& alarmxml);
	
	int CheckETRAlarm(time_t curTime,vector<sCheckParam>& vecParam);					//���ݵ�ǰʱ���ж�ETR290�Ƿ���Ҫ����
	int CheckProgramAlarm(time_t curTime,vector<sCheckParam>& vecParam,time_t& alarmtime);
	int CheckProgramAlarmV3( time_t curTime,vector<sCheckParam>& vecParam,sCheckParam& ResultAlarm,time_t& alarmtime );
protected:
	//���챨����������xml
	bool CreateAlarmXML( const sCheckParam& checkparam,std::string mode,std::string reason,std::string& alarmxml);

	std::string CreateFreqAlarmXML(const sCheckParam& checkparam,std::string alarmvalue,std::string alarmid);
	std::string CreateProgramAlarmXML(const sCheckParam& checkparam,std::string alarmvalue,std::string alarmid);
	std::string CreateEnvironmentAlarmXML(const sCheckParam& checkparam,std::string alarmvalue);
	std::string CreateEquipmentAlarmXML(const sCheckParam& checkparam,std::string alarmvalue);

protected:
	bool IsAlarmed(std::string typeId);				//�ж�ĳ��typeid�Ƿ��Ѿ�������
	bool ReleaseAlarm(std::string typeId);			//���typeid�ı���
	bool AddAlarm(std::string typeId);				//���typeid�ı���
	bool AddAlarmID(std::string type,int alarmid);
	bool GetAlarmID(std::string type,int& alarmid); 

	string GetAlarmTypeID(string headstr);
	string GetAlarmPriority(eDVBType dvbtype,string type);

	bool GetAlarmParm(const sCheckParam& checkparam,sAlarmParam& alarmparam);	//��ȡ��������


private:
	std::map<std::string,bool> AlarmSended;//��¼�����Ƿ���

	bool bFlag;
	std::string AlarmID;
	AlarmSender* pAlarmSender;
	ReSendAlarm * pAlarmReSender;
	std::vector<std::string> vecTypeID;//���汨��id
	std::vector<SeperateTypeID> vecSeperateTypeID;
	std::map<std::string,std::vector<sCheckParam> > ETRAlarm;
	std::map<std::string,std::vector<sCheckParam> > ProgramAlarm;
	std::map<std::string, int > mapAlarmID;//�����ͽ������ʹ��ͬһ��alarmid

	std::vector<sAlarmPriority> vecAlarmPriority;
	ACE_Thread_Mutex ETRMutex;
	ACE_Thread_Mutex ProgramMutex;
	ACE_Thread_Mutex MutexTypeID;
	ACE_Thread_Mutex MutexAlarmID;

};
typedef ACE_Singleton<AlarmMgr,ACE_Mutex>  ALARMMGR;
