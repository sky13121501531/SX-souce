


#ifndef IC_INFO_QUERY_TASK_H
#define IC_INFO_QUERY_TASK_H


#include "DeviceIndependentTask.h"


#ifdef ZONG_JU_ZHAO_BIAO

//С�����Ų�ѯ����
class ICInfoQueryTask : public DeviceIndependentTask
{
public:
	ICInfoQueryTask();
	ICInfoQueryTask(std::string strXML);
	virtual ~ICInfoQueryTask();
public:
	virtual void Run(void);
	virtual std::string GetObjectName();
	virtual std::string GetTaskName();
};

#endif






















#endif