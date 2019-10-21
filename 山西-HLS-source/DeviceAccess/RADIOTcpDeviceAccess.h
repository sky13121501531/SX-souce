


#ifndef RADIO_TCP_ACCESS_H
#define	RADIO_TCP_ACCESS_H


#include "./TCPDeviceAccess.h"
#include <string>
using namespace std;


class RadioTcpDeviceAccess : public TCPDeviceAccess
{
public:
	RadioTcpDeviceAccess(int deviceid,std::string strIP,int nPort);
	virtual ~RadioTcpDeviceAccess();


	/**	�����������ܽӿ�
	*/
private:
	virtual bool GetQualityRetXML(const std::string& strCmdMsg,std::string& strRetMsg);
	virtual bool GetSpectrumRetXML(const std::string& strCmdMsg,std::string& strRetMsg);
	virtual bool GetChannelScanRetXML(const std::string& strCmdMsg,std::string& strRetMsg);
	virtual bool GetTsQueryRetXML(const std::string& strCmdMsg,std::string& strRetMsg);


	/**	��������ָ��ӿ�
	*/
private:
	//virtual int  SendCmdForChannelScan(MSG_TYPE msg_type,void* info,ChannelScanRetMessage_Handle pRetObj, int infolen)	{return -1 ; }	//����Ƶ��ɨ��
	virtual int  SendCmdForSpecScan(MSG_TYPE msg_type,void* info, RadioSpecRetMessage_Handle pRetObj,int infolen);
	virtual int  SendCmdForQuality(MSG_TYPE msg_type,void* info,RadioQuaRetMessage_Handle pRetObj,int infolen);
	//Ƶ��ɨ�����⴦��ӿ�
	int  SendCmdForRadioSpecialChannelScan(MSG_TYPE msg_type,void* info,SpecialRadioRetMessage_Handle pRetObj, int infolen);


	/**	�㲥Ƶ��ɨ���Ƶ��ɨ����ؽӿ�
	*/
private:
	bool SpecScan(int freq,RadioSpecRetMessage_Obj& RadioSpec);
	void CaculateCenterFreq(int startfreq,int endfreq,int *centerfreqs,int &len);		//��������Ƶ��
	int  Findpolars(char spec[1023],int fFreq,int* result);                             //��ȡƵ��ɨ�����㷨
	//Ƶ��ɨ��
	bool GetRadioSpecResult(float fFreq, RadioSpecRetMessage_Obj &rqr);


	/**	�㲥ָ�������ؽӿ�
	*/
public:
	virtual bool GetQuality(float fFreq, RadioQuaRetMessage_Obj &rqr);				//(�ᱻ�ⲿ���ã�����Ϊpublic)
private:
	//bool QueryQuality(int freq,RadioQuaRetMessage_Obj& RadioQua);		//�㲥ʵʱָ��
	bool GetRadioFreqLevel(float fFreq, float &fLevel);							//(��ʱû��)

	
	/**	��Ƶ��ؽӿ�
	*/
private:
	virtual bool TurnFreq(TunerConfig_Obj& tunerinfo);			//��Ƶ
	bool SetAudioRate(const MediaConfig_Obj& rateinfo);			//��Ƶ����
};




































#endif