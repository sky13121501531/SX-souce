///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����TsSender.h
// �����ߣ�gaoxd
// ����ʱ�䣺2009-06-16
// ������������Ƶ���ݷ����߳�
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ace/Task.h"
#include "../Foundation/TypeDef.h"
#include <vector>

class TsSender : public ACE_Task<ACE_MT_SYNCH>
{
public:
	TsSender(int deviceid);
	~TsSender();
private:
	TsSender();
public:
	int Start();

	int open(void*);

	virtual int svc();

	int Stop();

	void SetSendSwitch(bool sendswitch);		//�����Ƿ�������
	bool AddClient(sVedioUserInfo newclient);	//������Ƶ�����û�
	bool HasClient();							//�ж��Ƿ�����Ƶ�û� 
	bool GetAllClient(std::vector<sVedioUserInfo>& uservec);//���������Ƶ�����û�
	bool StopAllClient();									//ǿ��ֹͣ��ͨ����Ƶ�û�

private:
	bool ClearAllClient();
	int ProcessMessage(ACE_Message_Block* mb);//ts���ݷ������ͻ�
private:
	int DeviceID;
	std::vector<sVedioUserInfo> ClientVec;
	ACE_Thread_Mutex QueueMutex;
	bool bFlag;
	bool bSendSwitch;

};
