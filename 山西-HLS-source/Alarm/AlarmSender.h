#pragma once

///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����AlarmSender.h
// �����ߣ�gaoxd
// ����ʱ�䣺2009-09-08
// ����������������������
// ��ע��ʹ���̶߳Ա������ݽ��з��ͣ���֤��������������ִ��
///////////////////////////////////////////////////////////////////////////////////////////

#include "ace/Task.h"
#include "../Foundation/TypeDef.h"
#include <string>

class AlarmSender : public ACE_Task<ACE_MT_SYNCH>
{
public:
	AlarmSender();
	virtual ~AlarmSender();
public:
	int open(void*);

	virtual int svc();

	int Stop();
protected:
	int ProcessMessage(ACE_Message_Block* mb);//����xml����
private:
	bool bFlag;

};