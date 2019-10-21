///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����HTTPDeviceAccess.h
// �����ߣ�gaoxd
// ����ʱ�䣺2010-07-06
// ����������Ӳ���豸�����࣬����TCPЭ��
///////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "ace/Synch.h"
#include "ace/SOCK_Stream.h"
#include "ace/SOCK_Connector.h"
#include "ace/INET_Addr.h"
#include <string>
#include <vector>
#include "CardType.h"
#include "DeviceAccess.h"

class TCPDeviceAccess : public DeviceAccess
{
public:
	TCPDeviceAccess(int deviceid,std::string strIP,int nPort);
	virtual ~TCPDeviceAccess(void);
private:
	TCPDeviceAccess(void);

public:
	bool SendTaskMsg(const std::string& strCmdMsg,std::string& strRetMsg);
	bool SendMultiTask(std::string strtsIp,unsigned int tsport,int cardindex,int MultiIndex);
	bool GetTsReceiveHandle(const std::string& strAddress,ACE_SOCK_Stream& streamHandle){return false;};
	std::string GetDeviceIP();
	bool CheckFreqLock();					//�жϸ�ͨ���Ƿ�����
	//����ts����������������
	unsigned int SetMultiScrNetParam(unsigned char *dest, const ChanNetCfgObj& netcfg, unsigned char cmdCode);
	//����ts���������PID
	unsigned int SetMultiScrPIDParam(unsigned char *dest, const ChanPIDObj& channelpid, unsigned char cmdCode);
	unsigned short CRC16(unsigned char *ptr,unsigned int len);
protected:
	bool ConnectToServer();													//���ӷ�����
	bool SendCmdToServer(MSG_TYPE msg_type,void* info,int infolen);			//�������ݲ��ж��Ƿ�ɹ�
	bool SendCmdToServer(void* indata,int indatalen,void* outdata,int outdatalen);												
	

	/**	�����������ܽӿ�
	*/
private:
	virtual bool GetQualityRetXML(const std::string& strCmdMsg,std::string& strRetMsg) { return true ; }
	virtual bool GetSpectrumRetXML(const std::string& strCmdMsg,std::string& strRetMsg){ return true ; }
	virtual bool GetChannelScanRetXML(const std::string& strCmdMsg,std::string& strRetMsg){ return true ; }
	virtual bool GetTsQueryRetXML(const std::string& strCmdMsg,std::string& strRetMsg){ return true ; }


	/**	��������ָ��ӿ�
	*/
protected:
	virtual int  SendCmdForChannelScan(MSG_TYPE msg_type,void* info,ChannelScanRetMessage_Handle pRetObj, int infolen)	{return -1 ; }	//����Ƶ��ɨ��
	virtual int  SendCmdForSpecScan(MSG_TYPE msg_type,void* info, RadioSpecRetMessage_Handle pRetObj,int infolen)	{return -1 ; }
	virtual int  SendCmdComForQuality(MSG_TYPE msg_type,void* info,RadioQuaRetMessage_Handle pRetObj,int infolen)	{return -1 ; }
public:
	int  SendCmdToTVCom(MSG_TYPE msg_type,void* info,int infolen);


	/**	��ȡָ��ӿ�(ֻ�ṩ�㲥��AM)
	*/
public:
	virtual bool GetQuality(float fFreq, RadioQuaRetMessage_Obj &rqr)	{ return true ; }
	//virtual bool GetRadioQuality(float fFreq, RadioQuaRetMessage_Obj &rqr)	{ return true ; }
	//virtual bool GetAMQuality(float fFreq, RadioQuaRetMessage_Obj &rqr)	{ return true ; }


	/**	��Ƶ
	*/
private:
	virtual bool TurnFreq(TunerConfig_Obj& tunerinfo) { return true ; }
protected:
	bool SetOSD(int chan,const SubtitleConfig_Obj& osdinfo);	//����OSD
	bool SetSysTime();

private:
	bool InitCard();						//��ʼ���忨

	bool StartTranscode();					//��ʼת��
	bool StopTranscode();					//ֹͣת��<�����������>

	bool SetVideoAlertInfo( AlertConfig_Obj& alertinfo );		//������Ƶ��̬����
	bool SetAudioAlertInfo( AlertConfig_Obj& alertinfo );		//������Ƶ��̬����
	

protected:
	int GetAudioIndex(int audiorate);		//��ȡ��Ƶ���ʱ��
protected:
	ACE_SOCK_Connector connector;
	ACE_SOCK_Stream stream;
	ACE_INET_Addr server;

	int mChannelID;
	//std::string mstrDeviceIP;
};
