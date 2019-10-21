///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����TsSenderMgr.h
// �����ߣ�gaoxd
// ����ʱ�䣺2009-06-18
// �������������ݷ����̹߳�����
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ace/Synch.h"
#include "ace/Singleton.h"
#include "../Foundation/TypeDef.h"
#include "./TsSender.h"
#include <map>

class TsSenderMgr
{
public:
	TsSenderMgr();
	virtual ~TsSenderMgr();
public:
	int Start();
	int Stop();
public:
	void SetSendSwitch(int deviceid,bool sendswitch);
	bool AddClient(int deviceid,sVedioUserInfo newclient);
	bool HasClient(int deviceid);
	bool GetAllClient(std::vector<sVedioUserInfo>& uservec);
	bool StopAllClient(int deviceid);

private:
	std::map<int,TsSender*> TsSenderMap;
};

typedef ACE_Singleton<TsSenderMgr,ACE_Mutex>  TSSENDERMGR;