///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����XMLTask.h
// �����ߣ�jiangcheng
// ����ʱ�䣺2009-05-25
// �����������������
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "../Foundation/TypeDef.h"
#include "../Foundation/PropManager.h"
#include <string>
#include <list>

/****************************״̬�޸�ʱ��************************/
/* WAITING,				//��������ʱ��״̬ΪWAITING 
/* READY,				//�������ڼ�����״̬ΪREADY������������Ӻ󣻶�ʱ�����������ڼ䣩	--UpdataStatus()
/* RUNNING,				//��������״̬ΪRUNNING												--Run()
/* FINISHED,			//�������н�����ΪFINISHED											--Run()
/* EXPIRED				//������ڣ��������������Ϊ���ڣ���ʱ�������һ��ִ����Ϻ�Ϊ���ڣ�--UpdataStatus()
/****************************************************************/

class Scheduler;
class XMLTask
{
public:
	XMLTask();
	XMLTask(std::string strXML);
	virtual ~XMLTask();
public:
	virtual void Run(void);				//����ִ�к���
	virtual bool Stop(void) = 0;		//����ֹͣ����
	virtual bool UpdataStatus();		//����״̬���º���,�����������״̬. ���������������״̬��
										//�����߳�Ҫ��֤ѭ������ÿ������ĸýӿ��Ա�֤����״̬�ļ�ʱ���º���ȷ�ԡ�
public:		
	//������غ���
	bool IsReady(){ return (TaskStatus == READY); };
	bool IsRunning(){ return (TaskStatus == RUNNING); };
	bool IsFinised(){ return (TaskStatus == FINISHED); };
	bool IsExpired(){ return (TaskStatus == EXPIRED); };
	bool IsSetTaskOK(){ return bSetOK; };

	bool SetExpired();
	bool SetTaskOK();

protected:
	bool SetWaiting();
	bool SetReady();
	bool SetRunning();
	bool SetFinised();
	
public:
	//���XML��ͷ��Ϣ
	std::string GetVersion() const {return strVersion;};
	std::string GetMsgID() const {return strMsgID;};
	std::string GetType() const {return strType;};
	std::string GetProtocol() const {return strProtocol;};
	std::string GetSetDateTime() const {return strSetDateTime;};//���������������ʱ��
	std::string GetSrcCode() const {return strSrcCode;};
	std::string GetDstCode() const {return strDstCode;};
	std::string GetPriority() const {return strPriority;};
	std::string GetSrcURL() const {return strSrcURL;};
	
	//����ͷ��Ϣ
	void SetVersion(std::string strValue) {strVersion = strValue;};
	void SetMsgID(std::string strValue) {strMsgID = strValue;};
	void SetType(std::string strValue) {strType = strValue;};
	void SetProtocol(std::string strValue) {strProtocol = strValue;};
	void SetSetDateTime(std::string strValue) {strSetDateTime = strValue;};
	void SetSrcCode(std::string strValue) {strSrcCode = strValue;};
	void SetDstCode(std::string strValue) {strDstCode = strValue;};
	void SetPriority(std::string strValue) {strPriority = strValue;};

	bool SendXML(const std::string& strXML);

	virtual std::string GetBaseObjectName() = 0;
	virtual std::string GetObjectName() = 0;
	virtual std::string GetTaskName() = 0;
public:
	eTaskRetStatus GetRetValue()const {return RetValue;};
	void SetRetValue(eTaskRetStatus retvalue){RetValue = retvalue;};

	eDVBType GetDVBType() const {return DVBType;};
	void SetDVBType(eDVBType type){DVBType = type;};

	std::string GetTaskID() const  {return TaskID;};
	void SetTaskID(std::string taskid){TaskID = taskid;};

	int GetDeviceID() const  {return DeviceID;};
	void SetDeciveID(int deviceid){DeviceID = deviceid;};
	void SetMultiWindowNumber(int WindowNumber){m_WindowNumber = WindowNumber;};

	std::string GetRunPriority() const  {return strRunPriority;};
	void SetRunPriority(std::string runPriority){strRunPriority = runPriority;};

	virtual bool AddTaskXml(){return true;};
	virtual bool DelTaskXml(){return true;};

	virtual bool IsRealTimeTask(){return false;};	//ʵʱ����
	virtual bool IsVedioTask(){return false;};		//����Ƶ����
	virtual std::string CreateSchedulerAlarm() {return "";};
	std::string GetTaskFreq()const {return Freq;};
	std::string GetTaskServiceID()const {return strServiceID;};
	std::string GetTaskVideoPID()const {return strVideoPID;};
	std::string GetTaskAudioPID()const {return strAudioPID;};
	std::string GetChannelID()const {return ChannelID;};
	int GetMultiWindowNumber()const {return m_WindowNumber;};
	bool IsMulti(){return m_bmulti;};
	std::string GetDeviceXml()const {return strDeviceXML;};
	std::string GetStandardXml()const {return strStandardXML;};

	time_t GetStartDateTime();
	time_t GetEndDateTime();
	long GetCycleInterval();

	bool IsShareTask(){return PROPMANAGER::instance()->IsShareTask(GetObjectName());};

#ifdef ZONG_JU_ZHAO_BIAO
	virtual bool IsHDProgram(){return mIsHD;};
	virtual bool IsOSDProgram(){return mIsOSD;};
#endif
protected:
	Scheduler* TaskScheduler;	//��ʱ���Զ���
	bool bRun;					//ִ���߳���������ʶ

	std::string TaskID;
	int DeviceID;
	eDVBType DVBType;

	std::string Freq;
	std::string ChannelID;
	
	bool mIsHD;
	bool mIsOSD;

protected:
	std::string strStandardXML;		//�����׼XML
	std::string strDeviceXML;		//����Ӳ����XML

	//XML��ͷ��Ϣ
	std::string strVersion;		//�汾���
	std::string strMsgID;		//��Ϣ���
	std::string strType;		//��Ϣ����
	std::string strProtocol;	//����Э������
	std::string strSetDateTime;	//������������ʱ��
	std::string strSrcCode;		//��ϢԴ����
	std::string strDstCode;		//��ϢĿ�����
	std::string strPriority;	//��������ȼ�
	std::string strSrcURL;      //��Ϣ�ظ�URL
	std::string strServiceID;
	std::string strVideoPID;
	std::string strAudioPID;
	BOOL m_bmulti;
	int m_WindowNumber;//�໭�洰������

protected:
	std::string strRunPriority;	//����������ȼ�

protected:
	eTaskRetStatus RetValue;	//����ִ�н��      
private:
	enumTaskStatus TaskStatus;	//����״̬����
	bool bSetOK;

};