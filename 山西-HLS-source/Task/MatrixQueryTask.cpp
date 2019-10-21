
#include "MatrixQueryTask.h"
#include "TranslateDownXML.h"
#include "TranslateUpXML.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/TypeDef.h"
#include "../Communications/TsSenderMgr.h"
#include <vector>
#include <iostream>

using namespace std;

MatrixQueryTask::MatrixQueryTask() : DeviceIndependentTask()
{

}

MatrixQueryTask::MatrixQueryTask(std::string strXML) : DeviceIndependentTask(strXML)
{
}

MatrixQueryTask::~MatrixQueryTask()
{
}
void MatrixQueryTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]�����л�����ִ�� !\n",DeviceID));
	bRun = true;

	SetRunning(); 

	RetValue = RUN_SUCCESS;

	SendXML(TranslateUpXML::TranslateMatrixQuery(this));//����xml

	SetFinised(); 

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]�����л�����ֹͣ !\n",DeviceID));
}

string MatrixQueryTask::GetTaskName()
{
	return std::string("�����л�����");
}
std::string MatrixQueryTask::GetObjectName()
{
	return std::string("MatrixQueryTask");
}

