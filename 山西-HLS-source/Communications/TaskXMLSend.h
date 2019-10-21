///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����TaskTaskXMLSend.h
// �����ߣ�jiangcheng
// ����ʱ�䣺2009-05-20
// ��������������ִ�н�������з�����
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ace/Task.h"
#include <string>

class TaskXMLSend : public ACE_Task<ACE_MT_SYNCH>
{
public:
	TaskXMLSend();
	~TaskXMLSend();
public:
	int Init();

	int open(void*);

	virtual int svc();

	int Stop();
private:
	int ProcessMessage(ACE_Message_Block* mb);
	std::string GetURL(std::string& upXML);
private:	
	bool bFlag;
};