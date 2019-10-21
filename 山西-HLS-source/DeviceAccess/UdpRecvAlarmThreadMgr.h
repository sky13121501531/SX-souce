///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����UdpRecvAlarmThreadMgr.h
//////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ace/Task.h"
#include "ace/Synch.h"
#include "ace/Singleton.h"
#include <map>
class UdpRecvAlarmThread;
class UdpRecvAlarmThreadMgr : public ACE_Task<ACE_MT_SYNCH>
{
public:
	UdpRecvAlarmThreadMgr();
	virtual ~UdpRecvAlarmThreadMgr();
public:
	int Start();

	int open(void*);

	virtual int svc();

	int Stop();

	char GetChanLockAlarmStatus(int devid)
	{
		return m_lockalarm[devid];
	}
	char GetChanCaAlarmStatus(int devid)
	{
		return m_caalarm[devid];
	}
	char GetChanTSAlarmStatus(int devid)
	{
		return m_tsalarm[devid];
	}
private:
	bool bFlag;
	std::map<int,UdpRecvAlarmThread*> Alarm290UdpRecvThreadMap;
	std::map<int,UdpRecvAlarmThread*> AlarmProUdpRecvThreadMap;
	char m_lockalarm[300];//m_lockalarm[0] ��ʾ0ͨ������ m_lockalarm[1] ��ʾ1ͨ������......
	char m_caalarm[300];//m_caalarm[0] ��ʾ0ͨ������ m_caalarm[1] ��ʾ1ͨ������......
	char m_tsalarm[300];//m_tsalarm[0] ��ʾ0ͨ������ m_tsalarm[1] ��ʾ1ͨ������......

};
typedef  ACE_Singleton<UdpRecvAlarmThreadMgr,ACE_Mutex>  UDPRECVALARMTHREADMGR;
