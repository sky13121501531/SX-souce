///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����DownXMLProcess.h
// �����ߣ�jiangcheng
// ����ʱ�䣺2009-05-21
// ������������ȡͨ�Ų������ָ�XML��ʽ��������ָ�������Ӧ������ʵ���������������ģ�鴦��
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ace/Task.h"
#include <string>

class DownXMLProcess : public ACE_Task<ACE_MT_SYNCH>
{
public:
	DownXMLProcess();
	~DownXMLProcess();
public:
public:
	int Init();

	int open(void*);

	virtual int svc();

	int Stop();

	int ProcessMessage(ACE_Message_Block* mb);
private:
	bool bFlag;

};