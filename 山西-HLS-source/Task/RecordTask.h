///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����RecordTask.h
// �����ߣ�jiangcheng
// ����ʱ�䣺2009-06-01
// ����������¼��������
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceRelatedTask.h"


class RecordTask : public DeviceRelatedTask
{
public:
	RecordTask();
	RecordTask(std::string strXML);
	virtual ~RecordTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
	virtual bool IsVedioTask(){return true;};		//����Ƶ����

	virtual std::string CreateSchedulerAlarm();
	virtual bool AddTaskXml(void);
	virtual bool DelTaskXml(void);

	virtual void SetDeciveID(int deviceid);

private:
	std::string m_strChannelName;
#ifdef ZONG_JU_ZHAO_BIAO
	virtual bool IsHDProgram();
private:
	bool m_bIsHDProgram;
#endif

};