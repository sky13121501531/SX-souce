///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����RecordUploadProcessMgr.h
// �����ߣ�
// ����ʱ�䣺2017-08-29
// ����������
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ace/Task.h"
#include <vector>
#include "../Foundation/TypeDef.h"
#include "RecordUploadProcess.h"
class RecordUploadProcessMgr : public ACE_Task<ACE_MT_SYNCH>
{
public:
	RecordUploadProcessMgr();
	~RecordUploadProcessMgr();
public:
	int open(void*);
	int Stop();

	virtual int svc();
public:
	bool AddRecordUploadTask(sRecordUploadTaskInfo &utaskinfo);
private:
	bool bFlag;
};
