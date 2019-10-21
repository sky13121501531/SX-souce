///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����DeviceRelatedTask.cpp
// �����ߣ�jiangcheng
// ����ʱ�䣺2009-05-26
// �������������豸������ص�������
///////////////////////////////////////////////////////////////////////////////////////////
#include "DeviceRelatedTask.h"
#include "Scheduler.h"
#include "../Foundation/OSFunction.h"

DeviceRelatedTask::DeviceRelatedTask()
{

}

DeviceRelatedTask::DeviceRelatedTask(std::string strXML) : XMLTask(strXML)
{
	DeviceID = -1;//���豸��ص�DeviceID��ʼ��Ϊ-1������������Ҫ�ٴ����á�
}

DeviceRelatedTask::~DeviceRelatedTask()
{
	if (TaskScheduler != NULL)
		delete TaskScheduler;
	TaskScheduler = NULL;
}
bool DeviceRelatedTask::Stop()
{
	bRun = false;
	OSFunction::Sleep(0,5);
	if(IsRunning() == true)
	{
		return false;
	}
	return true;
}