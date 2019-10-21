///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����DownXMLProcess.cpp
// �����ߣ�jiangcheng
// ����ʱ�䣺2009-05-21
// ������������ȡͨ�Ų������ָ�XML��ʽ��������ָ�������Ӧ������ʵ���������������ģ�鴦��
///////////////////////////////////////////////////////////////////////////////////////////
#include "DownXMLProcess.h"
#include "../Communications/CommunicationMgr.h"
#include "../Foundation/OSFunction.h"
#include "../Task/TranslateDownXML.h"
#include "../Task/TaskFactory.h"
#include "../Task/XMLTask.h"
#include "../Foundation/AppLog.h"
#include "ace/OS.h"
#include "BusinessLayoutMgr.h"
#include "TaskMonitor.h"
#include "TaskRealTimeExecute.h"
#include "../DBAccess/DBManager.h"

DownXMLProcess::DownXMLProcess()
{
	bFlag = false;
}

DownXMLProcess::~DownXMLProcess()
{

}

int DownXMLProcess::Init()
{
	bFlag = true;
	return 0;
}

int DownXMLProcess::open(void*)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ����XML�����߳̿�ʼִ�� !\n"));

	this->activate();
	return 0;
}

int DownXMLProcess::svc()
{
	ACE_Message_Block *mb = 0;

	while (bFlag)
	{
		OSFunction::Sleep(0,50);
		try 
		{
			ACE_Time_Value OutTime(ACE_OS::gettimeofday()+ACE_Time_Value(2));
			if ((COMMUNICATIONMGR::instance()->AccessOrderReceive()->getq(mb,&OutTime)) != -1 && mb != NULL)//�Ӷ�����ȡ��xml
			{
				if (mb->msg_type() == ACE_Message_Block::MB_BREAK)//�Ƿ�Ҫ�ж���Ϣ
				{
					mb->release();//�ͷ��ڴ�
					break;
				}
				int nRet = ProcessMessage(mb);		//����xml
				if (-1 == nRet)
				{
					mb->release();
					break;
				}
				if (2 == nRet)	//�����ݿ����������ʧ��,������putq�������
				{
					ACE_Time_Value tmValue(ACE_OS::time(0)+1);
					COMMUNICATIONMGR::instance()->AccessOrderReceive()->putq(mb,&tmValue);
					continue;
				}
				mb->release();
			}
		}
		catch (...)
		{
		}

	}

	bFlag = false;
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ����XML�����߳�ִֹͣ�� !\n"));

	return 0;
}

int DownXMLProcess::ProcessMessage(ACE_Message_Block* mb)
{
	std::string strStdXml,strSrcXml;
	bool invalidxml=true;
	if(mb->length()>0)
	{
		strSrcXml=std::string(mb->rd_ptr(),mb->length());
		invalidxml=false;
	}

	//��ÿ������ŵ���־��
	//APPLOG::instance()->WriteLog(TASK,LOG_EVENT_DEBUG,strSrcXml,LOG_OUTPUT_BOTH);//FRee_fanrong
	if (strSrcXml.length() > 20*1024)
	{
		string strPrint = strSrcXml.substr(0, 2048);
		strPrint += ".....";
		APPLOG::instance()->WriteLog(TASK,LOG_EVENT_DEBUG,strPrint,LOG_OUTPUT_SCREEN);//XML̫��,�������Ļ 2K ����
		APPLOG::instance()->WriteLog(TASK,LOG_EVENT_DEBUG,strSrcXml,LOG_OUTPUT_FILE);//XML̫��,ֻ�����ļ�
	}
	else
		APPLOG::instance()->WriteLog(TASK,LOG_EVENT_DEBUG,strSrcXml,LOG_OUTPUT_BOTH);//

	if (invalidxml || TranslateDownXML::TranslateTaskXML(strSrcXml,strStdXml) == false)
	{
		DBMANAGER::instance()->DeleteTask(UNKNOWN,strSrcXml);

		string msg = "����XMLת������:" + strSrcXml;
		//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);
		return 0;
	}

	TaskFactory *pTaskFactory = NULL;
	XMLTask *pXMLTask = NULL;

	pTaskFactory = new TaskFactory;

	if (NULL == pTaskFactory || strStdXml.empty())
	{
		std::string msg = "�����޷��������񹤳�����";
		//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);
	}
	else
		pXMLTask = pTaskFactory->CreateTask(strStdXml);//�����յ�xml������Ӧ������

	if (NULL == pXMLTask)
	{
		std::string msg = "�����޷������������";
		//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);
	}
	else
	{
		bool bAddRet = BUSINESSLAYOUTMGR::instance()->AddTask(pXMLTask);
		if (!bAddRet && pXMLTask->GetRetValue()==DATABASEERROR)		//��������ݿ����������ʧ��
		{
			return 2;
		}
	}

	if (NULL != pTaskFactory)//�ͷ��ڴ�
	{
		delete pTaskFactory;
		pTaskFactory = NULL;
	}

	return 0;
}
int DownXMLProcess::Stop()
{
	bFlag = false;
	ACE_Message_Block* pMsgBreak;
	ACE_NEW_NORETURN(pMsgBreak, ACE_Message_Block (0, ACE_Message_Block::MB_BREAK) );
	msg_queue()->enqueue_head(pMsgBreak);
	this->wait();

	return 0;
}