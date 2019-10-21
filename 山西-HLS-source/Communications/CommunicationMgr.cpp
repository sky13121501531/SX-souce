///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����CommunicationMgr.cpp
// �����ߣ�jiangcheng
// ����ʱ�䣺2009-05-20
// ����������ͨ�Ų�ӿڹ�����
///////////////////////////////////////////////////////////////////////////////////////////
#include "CommunicationMgr.h"

CommunicationMgr::CommunicationMgr()
{
	CreateInstance();
}

CommunicationMgr::~CommunicationMgr()
{
	DestoryInstance();
}

void CommunicationMgr::CreateInstance()
{
	pDownOrderReceive = new XMLReceive;
	pMyReactor = new MyReactor;
	pXMLSend = new TaskXMLSend;
	pXMLReSend = new TaskXMLReSend;
	pDeviceLog = new DeviceLog;
}

void CommunicationMgr::DestoryInstance()
{
	//�Ƴ������������ģ��
	if (NULL != pDownOrderReceive)
	{
		delete pDownOrderReceive;
		pDownOrderReceive = NULL;
	}

	//�Ƴ���Ӧ��
	if (NULL != pMyReactor)
	{
		delete pMyReactor;
		pMyReactor = NULL;
	}

	//�Ƴ�����XML�������ģ��
	if (NULL != pXMLSend)
	{
		delete pXMLSend;
		pXMLSend = NULL;
	}
	if (NULL != pXMLReSend)
	{
		delete pXMLReSend;
		pXMLReSend = NULL;
	}
	//�Ƴ�Ӳ������־����
	if (NULL != pDeviceLog)
	{
		delete pDeviceLog;
		pDeviceLog = NULL;
	}
}

int CommunicationMgr::Start()
{
	//��ʼ�������������ģ��
	if (pDownOrderReceive->Init() == -1)
	{
		ACE_DEBUG((LM_ERROR,"(%T| %t)�����������ģ���ʼ������!\n"));
		return -1;
	}
	else
		pDownOrderReceive->open(0);

	//��ʼ����Ӧ��
	if (pMyReactor->Init() == -1)
	{
		ACE_DEBUG((LM_ERROR,"(%T| %t)��Ӧ����ʼ������!\n"));
		return -1;
	}
	else
		pMyReactor->open(0);

	//��ʼ������XML�������ģ��
	if (pXMLSend->Init() == -1)
	{
		ACE_DEBUG((LM_ERROR,"(%T| %t)�����̳߳�ʼ������!\n"));
		return -1;
	}
	else
		pXMLSend->open(0);

	if (pXMLReSend->Init() == -1)
	{
		ACE_DEBUG((LM_ERROR,"(%T| %t)�����̳߳�ʼ������!\n"));
		return -1;
	}
	else
		pXMLReSend->open(0);

	//Ӳ������־����
	if (pDeviceLog != NULL)
	{
		pDeviceLog->open(0);
	}
	return 0;
}

int CommunicationMgr::Stop()
{
	//ֹͣ�����������ģ��
	if (NULL != pDownOrderReceive)
		pDownOrderReceive->Stop();

	//ֹͣ��Ӧ��
	if (NULL != pMyReactor)
		pMyReactor->Stop();

	//ֹͣ����XML�������ģ��
	if (NULL != pXMLSend)
		pXMLSend->Stop();

	if (NULL != pXMLReSend)
		pXMLReSend->Stop();
	//Ӳ������־����
	if (pDeviceLog != NULL)
		pDeviceLog->Stop();
	return 0;
}