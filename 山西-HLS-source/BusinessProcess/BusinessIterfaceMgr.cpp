///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����BusinessIterfaceMgr.cpp
// �����ߣ�jiangcheng
// ����ʱ�䣺2009-05-21
// ����������ҵ��ӿڲ�ӿڹ�����
///////////////////////////////////////////////////////////////////////////////////////////
#include "BusinessIterfaceMgr.h"
#include "DownXMLProcess.h"
#include "../Communications/CommunicationMgr.h"
#include "ace/OS.h"

BusinessIterfaceMgr::BusinessIterfaceMgr()
{
	CreateInstance();
}

BusinessIterfaceMgr::~BusinessIterfaceMgr()
{
	DestoryInstance();
}

void BusinessIterfaceMgr::CreateInstance()
{
	pDownXMLProcess = new DownXMLProcess;
}

void BusinessIterfaceMgr::DestoryInstance()
{
	if (NULL != pDownXMLProcess)
	{
		delete pDownXMLProcess;
		pDownXMLProcess = NULL;
	}
}

int BusinessIterfaceMgr::Start()
{
	//��ʼ������XML�������ģ��
		
	if (pDownXMLProcess->Init() == -1)
	{
		ACE_DEBUG((LM_ERROR,"(%T| %t)�����������ģ���ʼ������!\n"));
		return -1;
	}
	else
		pDownXMLProcess->open(0);
		
	return 0;
}

int BusinessIterfaceMgr::Stop()
{
	if (NULL != pDownXMLProcess)
		pDownXMLProcess->Stop();

	return 0;
}

int BusinessIterfaceMgr::UplaodXML(const std::string& strXML)
{
	if (strXML.empty())
		return -1;
	//ʹ��Message_Block�����ڴ棬��xml�������ڴ���
	ACE_Message_Block *mbXML = new ACE_Message_Block(strXML.length());
	memcpy(mbXML->wr_ptr(),strXML.c_str(),strXML.length());
	mbXML->wr_ptr(strXML.length());

	COMMUNICATIONMGR::instance()->AccessTaskXMLSend()->putq(mbXML);//xml���뷢�Ͷ���

	return 0;
}


