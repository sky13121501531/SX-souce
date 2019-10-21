///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����UdpRecvAlarmThread.h
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ace/Task.h"
#include "ace/Synch.h"
#include "ace/Singleton.h"
#include <ace/SOCK_Dgram_Mcast.h>
#include <ace/INET_Addr.h>
#include <string>
#include "../Foundation/TypeDef.h"
using namespace std;
class UdpRecvAlarmThread : public ACE_Task<ACE_MT_SYNCH>
{
public:
	UdpRecvAlarmThread(string ip,string port,int wtype);
	virtual ~UdpRecvAlarmThread();
public:
	int Start();

	int open(void*);

	virtual int svc();

	int Stop();

	void SetAlarmParam(eDVBType dvbtype,string channelid,string freq,string serviceid,string videopid,string devid,string secretpro = "")
	{
		m_strchannelid = channelid;
		m_strfreq = freq;
		m_strdeviceid = devid;
		m_dvbtype = dvbtype;
		m_strSecretPro = secretpro;
		m_strVideoPID = videopid;
		m_strServiceID = serviceid;
	}

	void SetCoderAlarm(bool codealarm)
	{
		m_bSetCoderAlarm = codealarm;
	}
private:
	ACE_INET_Addr remote_addr;
	ACE_SOCK_Dgram_Mcast DeviceSock;
	bool bFlag;
	bool m_bSetCoderAlarm;
	int iWorktype;//0 tuner 1 coder
	char m_picalarm[6];//m_picalarm[0]��֡��ʶ m_picalarm[1]�ڳ���ʶ m_picalarm[2]�ް���0��ʶ (m_picalarm[3]�ް���1��ʶ)��ʹ�� (m_picalarm[4]�ް���2��ʶ)��ʹ�� m_picalarm[5]���źŽ���ʧ�ܱ�ʶ
	char m_picalarmtimes[3];//m_picalarmtimes[0]��֡��ʶ���� m_picalarmtimes[1]�ڳ���ʶ���� m_picalarmtimes[2]�ް�����ʶ����     С��20�Ǳ�������    ���ڵ���30�ǽ����������   ��ʼ��Ϊ25 ��ǰ״̬Ϊ����ֵ���ڵ���20��ʾ�ϴ��ǽ���������¼���  ��ǰ״̬Ϊ�������ֵС��30��ʾ�ϴ��Ǳ������¼���   
	time_t m_picalarmtime_t[3];//m_picalarmtime_t[0]��֡��ʶʱ�� m_picalarmtime_t[1]�ڳ���ʶʱ�� m_picalarmtime_t[2]�ް�����ʶʱ��
	string m_strchannelid;
	string m_strfreq;
	string m_strdeviceid;
	string m_strSecretPro;//1��ʾ������ 0��ʾ����
	string m_strServiceID;
	string m_strVideoPID;
	eDVBType m_dvbtype;
	time_t m_errCnttime;
	time_t m_errPattime;
	time_t m_errPmttime;
};
typedef  ACE_Singleton<UdpRecvAlarmThread,ACE_Mutex>  UDPRECVALARMTHREAD;
