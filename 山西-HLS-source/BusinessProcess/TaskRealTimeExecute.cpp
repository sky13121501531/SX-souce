
#include <algorithm>
#include "../Foundation/OSFunction.h"
#include "../Task/XMLTask.h"
#include "TaskRealTimeExecute.h"

using namespace std;


TaskRealTimeExecute::TaskRealTimeExecute(void)
{
	curTask = NULL;
}

TaskRealTimeExecute::~TaskRealTimeExecute(void)
{
}

int TaskRealTimeExecute::open(void*)
{
	this->activate();//��������ִ���߳�
	return 0;
}

int TaskRealTimeExecute::svc()
{
	if (curTask != NULL)
	{
		curTask->Run();	//����ִ��
		delete curTask;
		curTask = NULL;
	}
	return 0;
}

bool TaskRealTimeExecute::SetTask(XMLTask* task)
{
	if (curTask == NULL && task != NULL)
	{
		curTask = task;
	}
	return true;
}
int TaskRealTimeExecute::Stop()
{
	this->wait();
	return 0;
}