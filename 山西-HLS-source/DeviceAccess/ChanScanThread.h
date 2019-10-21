///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����TsFetcherMgr.h
// �����ߣ�gaoxd
// ����ʱ�䣺2009-06-18
// �������������ݻ�ȡ�̹߳�����
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ace/Task.h"
#include "ace/Synch.h"
#include "ace/Singleton.h"
#include "../Foundation/TypeDef.h"
#include <string>
#include <vector>
#include <map>
using namespace std;

class ChanScanThread : public ACE_Task<ACE_MT_SYNCH>
{
public:
	ChanScanThread();
	virtual ~ChanScanThread();
public:
	int Start();

	int open(void*);

	virtual int svc();
	string CreatePreChanScanRet(eDVBType dvbType);
	string CreatePreChanScanRetTwo(eDVBType dvbType);
	bool ChanScanForDVBC(std::string sDevID, int bepg, std::string sScan, bool bweeklyswitch, std::string sFreq, std::string sCheckHD);//DVBC Ƶ��ɨ��
	bool ChanScanForCTTB(std::string sDevID, int bepg, std::string sScan, bool bweeklyswitch, std::string sFreq);//CTTB Ƶ��ɨ��
	bool ChanScanForFM(std::string sDevID);//FM Ƶ��ɨ��
	bool ChanScanForAM(std::string sDevID);//AM Ƶ��ɨ��
	bool ChanScanForDVBS(std::string sDevID);//DVBS Ƶ��ɨ��
	bool PSISIForDVBC(std::string sDevID,std::string sFreq);
	//���� HTTP ͷ
	string SkipHTTPHead(string http);
	bool GetDVBTypeAndCardIndex(string sDevList, map<string, string>& dvbCardIndex);
	bool ValidProXml(string& strXml);//��Ŀ�������пո��XML, ȥ���ո�
	bool ValidEpgDur(string strEpgDur);//�ж�epg����ʱ���Ƿ��0~9��:�ַ� �Ƿ���true ��false
	void SetChanScanNow(string NoticeXml,string NowScanFreq = "",string NowScanDevid = "");//���� ��ʱ�ɼ� Ƶ���� EPG 
	bool CheckHDProgramForDVBC(string sRetXMl, string& sHDTV);//�������Ŀ
	bool GetHDInfo(string freq,string servid,std::vector<HDInfoForScan>& HDInfoForScanList,string& hdi);//���½�Ŀ��Ϣ�ļ�
	bool GetDvbsFreqInfo(string freq,sdvbsfreqinfo& dvbsfreq);
	int Stop();
    void DoStringFreq(string &strstr,vector<string>& vStr);
private:
	bool bFlag;
	bool m_bChanScanWorkType;
	string m_sNoticeXml;
	string m_sNoticeXmlTwo;//����epg��Ƶ��ͬʱͬͨ��
	string m_sOpVersion;
	string m_sNowScanFreq;
	string m_sNowScanDevid;
	std::vector<sdvbsfreqinfo> sdvbsfreqinfoList;
};
typedef  ACE_Singleton<ChanScanThread,ACE_Mutex>  CHANSCANTHREAD;