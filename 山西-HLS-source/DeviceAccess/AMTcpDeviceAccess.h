


#ifndef AM_TCP_ACCESS_H
#define	AM_TCP_ACCESS_H


#include "./TCPDeviceAccess.h"
#include <string>
using namespace std;

class AmTcpDeviceAccess : public TCPDeviceAccess
{
public:
	AmTcpDeviceAccess(int deviceid,std::string strIP,int nPort);
	virtual ~AmTcpDeviceAccess();


	/**	�����������ܽӿ�
	*/
private:
	virtual bool GetQualityRetXML(const std::string& strCmdMsg,std::string& strRetMsg);
	virtual bool GetSpectrumRetXML(const std::string& strCmdMsg,std::string& strRetMsg);
	virtual bool GetChannelScanRetXML(const std::string& strCmdMsg,std::string& strRetMsg);
	virtual bool GetTsQueryRetXML(const std::string& strCmdMsg,std::string& strRetMsg);


	/**	������������ӿ�
	*/
private:
	//virtual int  SendCmdForChannelScan(MSG_TYPE msg_type,void* info,ChannelScanRetMessage_Handle pRetObj, int infolen)	{return -1 ; }	//����Ƶ��ɨ��ӿڽӿ�
	virtual int  SendCmdForSpecScan(MSG_TYPE msg_type,void* info, RadioSpecRetMessage_Handle pRetObj,int infolen);
	virtual int  SendCmdForQuality(MSG_TYPE msg_type,void* info,RadioQuaRetMessage_Handle pRetObj,int infolen);


	/**	�ж̲�Ƶ��ɨ���Ƶ��ɨ����ؽӿ�
	*/
private:
	bool SpecScan(int freq,RadioSpecRetMessage_Obj& RadioSpec);
	void CaculateCenterFreq(int startfreq,int endfreq,int *centerfreqs,int &len);	//����AM������Ƶ��
	void FindChannels(int startfreq, int endfreq, const std::vector<float>& vecSpecValue, std::vector<int>& vecChanFreq);	//���Ƶ��ɨ����
	//Ƶ��ɨ��
	bool GetAMSpec(float fFreq, RadioSpecRetMessage_Obj &rqr);


	/**	�ж̲�ָ�������ؽӿ�
	*/
public:
	virtual bool GetQuality(float fFreq, RadioQuaRetMessage_Obj &rqr);		//(�ᱻ�ⲿ���ã�����Ϊpublic)
//private:
	//bool QueryQuality(int freq,RadioQuaRetMessage_Obj& RadioQua);      //�ж̲�ʵʱָ��
	vector<RadioQuaRetMessage_Obj> m_vecRQRhistory;//�������� N���ڲ��ظ�����Ӳ��

	/**	��Ƶ��ؽӿ�
	*/
private:
	virtual bool TurnFreq(TunerConfig_Obj& tunerinfo);			//��Ƶ
	bool SetAudioRate(const MediaConfig_Obj& rateinfo);			//��Ƶ����
};





























#endif