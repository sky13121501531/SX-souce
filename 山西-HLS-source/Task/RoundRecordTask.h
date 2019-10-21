///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����RoundRecordTask.h
// �����ߣ�gaoxd
// ����ʱ�䣺2011-04-16
// ��������������¼��������
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceRelatedTask.h"

class RoundRecordTask : public DeviceRelatedTask
{
public:
	RoundRecordTask();
	RoundRecordTask(std::string strXML);
	virtual ~RoundRecordTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
	virtual void SetDeciveID(int deviceid);
	virtual bool AddTaskXml(void);
	virtual bool DelTaskXml(void);
	virtual bool IsVedioTask(){return true;};		//����Ƶ����
private:
	long RoundTime;
	std::vector<std::string> DecivcXMLVec;	//Ӳ��������
	size_t ChannelNum;						//�ֲ���Ƶ����
	size_t ChannelIndex;					//������
};