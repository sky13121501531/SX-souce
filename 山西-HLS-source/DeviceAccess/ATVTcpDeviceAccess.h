


#ifndef ATV_TCP_ACCESS_H
#define	ATV_TCP_ACCESS_H

#include "./TCPDeviceAccess.h"
#include <string>
using namespace std;

class AtvTcpDeviceAccess : public TCPDeviceAccess
{
public:
	AtvTcpDeviceAccess(int deviceid,std::string strIP,int nPort);
	virtual ~AtvTcpDeviceAccess();

public:
	//Ƶ��ʱ��������xml
	static float GetOpenTVStandardFreq(float freq);


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
	virtual int  SendCmdForChannelScan(MSG_TYPE msg_type,void* info,ChannelScanRetMessage_Handle pRetObj, int infolen);
	//virtual int  SendCmdForSpecScan(MSG_TYPE msg_type,void* info, RadioSpecRetMessage_Handle pRetObj,int infolen)	{return -1 ; }
	//virtual int  SendCmdComForQuality(MSG_TYPE msg_type,void* info,RadioQuaRetMessage_Handle pRetObj,int infolen)	{return -1 ; }


	/**	ATV����ָ��ӿ�
	*/
private:
	bool GetFreqLevel(float fFreq, float &fLevel);
	bool QualityRealtimeQueryTV(int freq,float & f_level);		//����ʵʱָ��

	float GetImageLevel(float Freq);					//ͼ���ز���ƽ
	float GetAudioLevel(float Freq);					//�����ز���ƽ
	float GetI2AOffLevel(float Freq);					//ͼ�������
	float GetCN(float Freq);							//�����
	float GetFreqOffSet(float Freq);					//Ƶƫ
	float GetSlope(float Freq);							//G��?

	//GetCN() �G����
	static float GetAlignedFreq(float freq);
	bool GetBaseCN(float fFreq, float &fCN);



	/**	��Ƶ��ؽӿ�
	*/
private:
	virtual bool TurnFreq(TunerConfig_Obj& tunerinfo);
	bool SetVideoRate(const MediaConfig_Obj& rateinfo);			//��Ƶ����
	//bool SetOSD(int chan,const SubtitleConfig_Obj& osdinfo);	//����OSD
	//bool SetSysTime();											//��̬����ϵͳʱ��

private:
	float fOldAnalyser[5];
	vector<TVQuality> m_vecTVhistory;//�����i�I N���ڲ��ظ�����Ӳ�s

	float mImageLevel;  //ͼ���ƽ,��λ:dbuv
	float mAudioLevel;  //������ƽ,��λ:dbuv
	float mI2AOffLevel; //ͼ���ز�������ز��ĵ�ƽ��,��λ:dbuv
	float mCN;          //�����,��λ:dbuv
	float mFreqOffSet;  //��ƵƵƫ,��λ:kHz
	float mSlope;       //G��?

	float mAnalyserImageOffset; //ָ���ƽƫ����
	float mAnalyserAudioOffset; //ָ�����ƫ����
	float mAnalyserI2AOffset;   //ָ��ͼ�������ƫ����
	float mAnalyserCnOffset;    //ָ�������ƫ����
	float mAnalyserOffOffset;   //ָ��Ƶƫƫ����
	float mAnalyserSlopeOffset; //ָ��G���ƫ���?
};
































#endif