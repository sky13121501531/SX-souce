


#ifndef NvrStatusQuery_h__
#define NvrStatusQuery_h__


#include "DeviceIndependentTask.h"


#ifdef ZONG_JU_ZHAO_BIAO



//��Ƶ����ֹͣ����
class NvrStatusQueryTask : public DeviceIndependentTask
{
public:
	NvrStatusQueryTask();
	NvrStatusQueryTask(std::string strXML);
	virtual ~NvrStatusQueryTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
};


#endif










#endif // NvrStatusQuery_h__