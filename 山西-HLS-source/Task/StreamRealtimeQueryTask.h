///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����StreamRealtimeQueryTask.cpp
// �����ߣ�gaoxd
// ����ʱ�䣺2009-06-08
// ����������ʵʱ��Ƶ������
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DeviceRelatedTask.h"
#include <string>
#include "ace/Task.h"
#include "ace/OS.h"
#include "../Foundation/FileOperater.h"


class StreamRealtimeQueryTask : public DeviceRelatedTask
{
public:
	StreamRealtimeQueryTask();
	StreamRealtimeQueryTask(std::string strXML);
	virtual ~StreamRealtimeQueryTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName(){return std::string("StreamRealtimeQueryTask");};
	virtual std::string GetTaskName(){return std::string("ʵʱ��Ƶ����");};
	virtual bool IsRealTimeTask(){return true;};	
	virtual bool IsVedioTask(){return true;};
	bool StartManualRecord(std::string ManualRecordXml);//֪ͨ��ʼ�ֶ�¼��ӿ�
private:
	bool bManualRecord;		//�Ƿ����ֶ�¼������
	std::string mRecordTime,mManualRecordXML,mURL;
	std::string mStartTime;
	std::string mBps;
	FileOperater mRecordFile;

	//�ֶ�¼���ļ���Ϊ��tempfile���д洢��sRecordInfo����Ҫ�� jidushan 110406
	//sRecordInfo mRecordInfo;
	std::string mRecordFileName;
	std::string m_cdevid,m_tdevid;

#ifdef ZONG_JU_ZHAO_BIAO
	std::string m_strBeginTime;
	int m_nRunTime;

	std::string m_strExpiredTime;
#endif
};
