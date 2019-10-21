#ifndef _PROPMANAGER_H_
#define _PROPMANAGER_H_

#include <string>
#include <list>
#include <map>
#include "ace/Singleton.h"
#include "PropConfig.h"
#include "TypeDef.h"


class PropManager
{
public:
	PropManager(void);
	~PropManager(void);

	//ʵʱ��Ƶ�������
	std::string GetHttpVideoIp(void)		{return mHttpVideoIP;};
	std::string GetHttpVideoPort(void)		{return mHttpVideoPort;};
	std::string GetVideoMaxnum(void)	{return mVideoMaxnum;};

	std::string GetRtspVideoIp(void)		{return mRtspVideoIP;};
	std::string GetRtspVideoPort(void)		{return mRtspVideoPort;};

	//ʵʱָ���������
	std::string GetQualityIp(void)		{return mQualityIp;};
	std::string GetQualityPort(void)	{return mQualityPort;};
	std::string GetQualityMaxnum(void)	{return mQualityMaxnum;};

	//¼��ָ���������
	std::string GetRecordQualityIp(void)		{return mRecordQualityIp;};
	std::string GetRecordQualityPort(void)	{return mRecordQualityPort;};
	std::string GetRecordQualityMaxnum(void)	{return mRecordQualityMaxnum;};

	//�忨�����������
	std::string GetDeviceIp(void)		{return mDeviceIp;};
	std::string GetDevicePort(void)		{return mDevicePort;};
	std::string GetDeviceMaxnum(void)	{return mDeviceMaxnum;};

	//xml���շ����������
	std::string GetXmlServerIP(void)	{return mXmlServerIP;};
	std::string GetXmlServerPort(void)	{return mXmlServerPort;};

	//��־���շ����������
	std::string GetLogIP(void)          {return mLogIp;}
	std::string GetLogPort(void)        {return mLogPort;}
	std::string GetLogMaxnum(void)      {return mLogMaxnum;}

	//���ñ����ϱ���Ϣ 
	bool SetUpAlarmInfo(eDVBType dvbtype,UpAlarmInfo alarminfo);
	//��ȡ�����ϱ���Ϣ
	bool GetUpAlarmInfo (eDVBType dvbtype,UpAlarmInfo& alarminfo);

	//��ȡ Quality_DevInfo �ṹ����Ϣ
	Quality_DevInfo* GetQuaDevInfo(int Index);
	//��ȡ Quality_DevInfo �ṹ�� ����
	int GetQuaDevInfoNum(void) { return m_VecQuaDevInfo.size();};
	//��ȡ HDMIInfo ��Ϣ
	HDMIInfo* GetHDMIInfo(void) { return &m_HDMIInfo;}
	VirtualKeyInfo* GetVirtualKeyInfo(std::string sVirtualKey);
	bool AdjustingFreqLevel(std::string sFreq, std::string& sLevel);//У׼ Ƶ���ƽ
	std::string GetCurWorkVersion(void) {return m_sCurWorkVersion;}
	std::string GetAMNum(void) {return m_sAMNum;}
	void GetAlarmParamConfig(eDVBType dvbtype, AlarmParamConfig& alarmParam) {alarmParam = m_mapAlarmParamConfig[dvbtype];};
	bool GetHDInfo(std::string freq, std::string serviceID, std::string& hdType);//��ȡ��Ŀ�ĸ�����Ϣ

	//��־�������
	std::string GetLogPath(void)		{return mLogPath;};
	std::string GetLogPath_VIDEO(void)	{return mLogPath_VIDEO;};	//FRee_fanrong_20140107
	std::string GetLogPath_SI(void)		{return mLogPath_SI;};		//FRee_fanrong_20140107
	std::string GetLogPath_SMS(void)	{return mLogPath_SMS;};		//FRee_fanrong_20140107
	std::string GetLogPath_CAS(void)	{return mLogPath_CAS;};		//FRee_fanrong_20140107

	std::string GetLogExpire(void)		{return mLogExpire;};
	std::string GetLogAnalyser(void)	{return mLogAnalyser;};
	std::string GetLogVideo(void)		{return mLogVideo;};
	std::string GetLogRecord(void)		{return mLogRecord;};
	std::string GetLogOther(void)		{return mLogOther;};
	std::string GetLogDevice(void)      {return mLogDevice;}
	std::string GetLogDefault(void)		{return mLogDefault;};
	eLogType    GetLogType(void);
	eLogOutPut  GetOutputFile(void);

	std::string GetPath(string path, std::vector<std::string>&velc);      
	void Creatfolder(std::string path);
    //��Ƶ�ַ�������
    std::string GetFreqString(void)     {return tempFreq;};
    std::string GetAlarmTimeReleaseString(void)     {return m_AlarmTimeRelease;};
	//���ݿ��������
	std::string GetDbType(void)			{return mDbType;};
	std::string GetDbIp(void)			{return mDbIp;};
	std::string GetBandDbIp(void)		{return mBandDbIp;};
	std::string GetDbPort(void)			{return mDbPort;};
	std::string GetDbUsername(void)		{return mDbUsername;};
	std::string GetDbPwd(void)			{return mDbPwd;};
	std::string GetDbName(void)			{return mDbName;};

	//¼���������
	std::string GetRecordPeriod(void)		{return mRecordPeriod;};
	std::string GetMaxAvailableSize(void)	{return mMaxAvailableSize;};
	std::string GetDBCleanInterval(void)	{return mDBCleanInterval;};
	std::string GetSystemCleanTime(void)	{return mSystemCleanTime;};
	
	void		GetRecFileLocVec(std::vector<string>& vecLoc)	{vecLoc = mRecFileLocVec;};		//��ȡ����¼���ļ�·��������
	std::string GetSharePathByLoc(std::string loc);										//ͨ��¼���ļ�·����ȡ
	bool		GetSharePathByLoc(std::string loc, std::string& sharepath);					//ͨ��¼���ļ�·����ȡ

	//std::string GetAlarmRecordPeriod(void)	{return  mAlarmRecordPeriod; }	//��ȡ��̬¼��ʱ����(ͳһʹ����ͨ¼���ļ���period,�˽ӿ���ʱע�͵�)

	//��̬¼��	
	std::string GetAlarmHeadOffset(enumDVBType eDvbtype);		//��ȡ��̬¼���ļ�ͷƫ������
	std::string GetAlarmTailOffset(enumDVBType eDvbtype);		//��ȡ��̬¼���ļ�βƫ������
	std::string GetAlarmRecordMode(enumDVBType eDvbtype);		//��ȡ��̬¼��mode
	std::string GetAlarmRecordExpire(enumDVBType eDvbtype);		//��ȡ��̬¼���������
	std::string GetRecordExpiredays(enumDVBType eDvbtype);		//��ȡ��ͨ¼���������
	std::string GetCardType(enumDVBType eDvbtype);		//��ȡ��ͨ¼���������

	//¼���ļ����������
	std::string GetHttpServerIP(void)		{return mHttpServerIP;};
	std::string GetHttpServerPort(void)		{return mHttpServerport;};
	std::string GetFtpServerIP(void)		{return mFtpServerIP;};
	std::string GetFtpServerPort(void)		{return mFtpServerPort;};	

	//���Ĭ�ϵ�DstCode��SrcCode
	std::string GetDefDstCode(eDVBType dvbtype) {return mDefDstCode[dvbtype];};
	std::string GetDefSrcCode(eDVBType dvbtype) {return mDefSrcCode[dvbtype];};
	std::string GetSrcCodeByType(string BZType);
	//
	std::string GetAgentInfoSet(string AgentType);
	//*************************************************
	bool InitiateUnUseRecDev();
	std::string GetUdpVideoServerClientIP(void)		{return m_strUdpVideoServerClientIP;};
	bool GetUdpVideoServerUsed(void)		{return m_bUdpVideoServerUsed;};
	bool GetScanInfo(string& scantype,string& freq,string& weeklyswitch,string& epg, string& scantime, string& checkHD,string& ScanVer);
	int GetUnUsedCoderDevNum();
	bool IsHadUnUsedTunerOrCoder(eDVBType dvbtype, map<string, vector<string>> mapFreqAndServID);
	int GetCoderDevNum() {return mCoderDevVec.size();}
	int GetTunerDevNum() {return mTunerDevVec.size();}
	int GetTunerDevNumByDVBType(eDVBType dvbType);
	int GetMonitorDevNum() {return mMonitorDevInfo.size();}
	bool IsUnUseRecDev(int devtype,eDVBType dvbtype,int deviceid);//devtype = 0��ʾtuner,devtype = 1��ʾcoder
	bool GetDevMonitorIndex(int devtype,eDVBType tunerDVB,int deviceid,string& monitorindex);//devtype = 0��ʾtuner,devtype = 1��ʾcoder
	bool GetDevMonitorInfo(int devtype,eDVBType tunerDVB,int deviceid,string& monitorip,string& monitorport);//devtype = 0��ʾtuner,devtype = 1��ʾcoder
	bool GetDevMonitorInfo(int monitorindex,string& monitorip,string& monitorport);//ͨ������id��ip
	bool GetDevMonitorInfo(int index,sDeviceInfo& DeviceInfo);//���ڱ������л���
	bool IncMonitorHDSD(int monitorindex,bool bIsHD);
	bool DecMonitorHDSD(int monitorindex,bool bIsHD);
	bool GetTunerInfo(int tunerindex,TunerDev& TunerDevInfo);
	bool GetTunerInfo(int deviceid,eDVBType tunerDVB,TunerDev& TunerDevInfo);
	bool GetDeviceInfo(string ChassisID,string Position,string ModuleID,sDeviceInfo& DevInfo);
	bool GetCoderInfo(int deviceid,CoderDev& CoderDevInfo);
	bool GetCoderVecFromFreqSerID(string Freq,string Serviceid, vector<CoderDev>& CoderDevInfo);
	bool GetDevID(int& devid,eDVBType tunerDVB,int priority,string freq,string servid,string taskid,bool bIsHD);
	bool GetUnUseDev(int& coderdevid,int& tundevid,eDVBType tunerDVB,string freq,string servid,int priority,bool bIsHD);
	bool SetCoderLinkTunerinfo(int coderdevid,int tundevid,eDVBType tunerDVB,string freq,string servid,bool bIsHD,string bps,string desc);
	bool SetTunerinfo(int tundevid,eDVBType tunerDVB,string freq,string servid,string desc);
	bool SetExistCmd(int tundevid,eDVBType tunerDVB,string cmdinfo);
	bool FlagCoderInfo(int coderdevid,string freq,string servid,eDVBType tunerDVB,bool bIsHD,string desc,string maincoderdeviceid);
	bool FreeSetCoderDev(int coderdevid);
	bool FreeSetTunerDev(int tundevid,eDVBType tunerDVB,string freq,string servid);
	bool GetDevFromPriority(int& coderdevid,int& tundevid,eDVBType tunerDVB,string freq,string servid,int priority,bool bIsHD);//1.�滻Ȩ����С�ڴ�Ȩ����Դ
	bool GetDevFromPriorityDevID(int devid,int& tundevid,eDVBType tunerDVB,string freq,string servid,int priority,bool bIsHD);//
	bool GetUnUseTunerDev(int& devid,eDVBType tunerDVB,string codermntindex,string freq,string servid);
	bool GetTunerDevFromFreq(int& devid,eDVBType& tunerDVB,string codermntindex,string freq,string servid);
	bool GetTunerDevFromPriority(int& devid,eDVBType tunerDVB,string codermntindex,int priority,string freq,string servid);//tuner���ڹ���Ҫ�ҵ�����Դ�����Ȩ��1.��ȡ����С�ڴ�Ȩ����Դ 2.����ÿ����Դ��ȡÿ����Դ�����Ȩ�� 3.�滻���Ȩ������Сֵ��
	bool GetExistServListFromFreqInTunerDev(string freq,string codermntindex,std::string& ServIdList);
	bool GetTunerDevFromCoderDev(int& tunedevid,eDVBType& tunerDVB,int codedevid);
	bool GetCoderDevFromTunerDev(vector<int>& vCoderDev, int iTunerId);
	int  FreeDev(string freq,string servid);
	bool FreeCoderDev(int devid);
	bool FreeTunerDev(eDVBType tunerDVB,int devid);
	bool FreeRecTunerCoder(eDVBType tunerDVB);
	bool DeleteTunerDevServId(eDVBType tunerDVB, int tunerId, string sServId);
	bool FreeDevLink(string taskid);//���ڸ�Ȩ�����񵽽���ʱ���˳�
	bool FreeDevLink(int type,eDVBType tunerDVB,int devid);//type = 0 free coder, type = 1 free tuner
	bool TunerDevIsInDevLink(eDVBType tunerDVB,int tundevide);
	int  GetTunerDeviceID(eDVBType tunerDVB,int ChassisID,int Position,int Demodulate);//����-1��ʾû���ҵ���Ӧ�豸ͨ��
	int  GetCoderDeviceID(int ChassisID,int Position,int ModuleID,int Transcode);//����-1��ʾû���ҵ���Ӧ�豸ͨ��
	bool ExistTunerCardFromCardindex(int ChassisID,int Cardindex);
	float GetChassisBandWidth(int ChassisID);//Mbps
	bool SetStandardChResourceInfo(standardChannelResource chres);
	int GetAutoModelID(int ChassisID,int Slot);
	bool GetCACardInfoByCardNo(std::string strCardNo, CACardInfo& cardInfo);
	//*************************************************

#ifdef ZONG_JU_ZHAO_BIAO
	//�������ݿ������õ�ǰ��code��ҳ��code
	void SetDefSrcCode(eDVBType eType, std::string strSrcCode){ mDefSrcCode[eType] = strSrcCode; };
	//void SetUpUr(eDVBType eType, std::string strSetUpUr){ mDefDstCode[eType] = strSetUpUr; }; bug url -> mDefDstCode

	//�����ϱ���ַ��code��url
	bool SetUpUrlByType(std::string strType, std::string strUpUrl);
	bool SetUpCodeByType(std::string strType, std::string strCode);

	//���ñ���URL
	void SetUpAlarmUrl(eDVBType dvbtype, std::string strUpUrl);

	//��ȡͬһ��忨�ϵ�ͨ���б�
	bool GetDevListOfSameIp(int deviceid, std::list<int>& deviceidlist);	//ͨ��deviceid��ȡͬһ��ip�ϵ�����ͨ��

	//�������񷵻�xmlĬ���ϱ���ַ
	std::string GetUrlByType(const std::string& srcEquType);

	bool GetAlarmUpInfoByType(const std::string& srcEquType,std::string& srccode,std::string& dstcode,std::string& url);
	//�����������ֲ��ϱ���ַ   //FRee_fanrong_20140103
	void SetUpStreamRoundUrl(eDVBType dvbtype,std::string strUpUrl);
	bool GetUpStreamRoundUrl(eDVBType dvbtype,std::string& strUpUrl);  

	map<std::string,int> IpVec;
	
#endif
	//���ġ��������������
	std::string GetUrl(const std::string& srcCode);

	//��������
	bool IsShareTask(std::string taskname);
	//�豸�����������
	//bool GetAllDeviceList(std::list<int>& devicedlist);										//���ȫ���豸ID
	//bool GetDVBDeviceList(eDVBType dvbtype,std::list<int>& devicedlist);					//���ĳһ�����豸ID �޸�by_hjw 20160310 ��ƽ̨ת�벻������ ���غ�GetAllDeviceList��ͬ
	bool GetTaskDeviceList(string taskType,eDVBType dvbtype,std::list<int>& devicedlist);	//�����ɸ������ͨ����
	bool GetTaskDeviceList(string taskType,string& strdlist);	
	bool GetMER_BERDeviceList(eDVBType dvbtype,std::list<int>& devicedlist);	//��ȡMER-BER�豸ͨ��
	bool GetDeviceAddress(int deviceid,string& ip,int& cmdport,int &tsport,string &cmdprotocol,string &tsprotocol);								//����豸IP��Port

	bool GetDeviceCardIndexbyIP(string ip,int & iCardIndex);				//ͨ��IP��ȡ�忨��� //FRee_fanrong_20131212
	bool GetDeviceCardIndex(int deviceid,int & iCardIndex);					//��ȡ�忨���		 //FRee_fanrong_20131212
	bool GetDeviceIndexbyCardIndex(int iCardIndex,int &firstIndex);			//ͨ���忨�Ż�ȡ�˰忨��һ��ͨ����  //FRee_fanrong_20131225

	int GetAllCardIndexCount()  {return m_nAllCardCount;}					//�忨����		//FRee_fanrong_20131212
	int GetAllDeviceIndexCount()	{ return m_nAllCardCount ; }			//ͨ������		//FRee_fanrong_20131212

	bool GetDeviceTSIP(int deviceid,string& ip);										//�豸IP
	bool GetDeviceCMDIP(int deviceid,string& ip);										//�豸IP
	bool GetDeviceCmdPort(int deviceid,int& cmdport);								//�豸����˿� 
	bool GetDeviceCmdProtocol(int deviceid,std::string& cmdprotocol);				//�豸����Э��
	
	bool GetDeviceIndex(string & deviceid,string logindex,string devicetype);
	bool GetDeviceTunerID(int deviceid,int& tunerid);
	bool GetDeviceLogIndex(int deviceid, int& logindex);
	bool GetDeviceTsPort(int deviceid,unsigned int& tsport);									//�豸����Ƶ�˿� 
	bool GetDeviceTsProtocol(int deviceid,std::string& tsprotocol);					//�豸����ƵЭ��


	bool GetDeviceID(string ip,string tunerid,int & deviceid);                      //ͨ��IP��TunerID���DeviceID
	bool GetDeviceIDByIP(string ip,std::list<int> & devicedlist);                   //ͨ��IP���DeviceID�б�
	bool IsDeviceAvaiable(int deviceid,string tasktype,eDVBType dvbtype);			//�ж��ض�������͵�������ڸ�����ͨ�����Ƿ�Ϸ�
	bool IsDeviceAvaiable(int deviceid);											//�ж�ͨ�����Ƿ�Ϸ�
	bool GetDeviceType(int deviceid,eDVBType& dvbtype);                             //����ͨ���Ż�ü������
	bool GetQualityCardInfo(eDVBType dvbtype,std::string &ip,int &port);            //��ȡָ�꿨IP �˿���Ϣ

	//����XML�����������
	long GetXmlSendTimes(void)	{return xmlSendTimes; };
	long GetXmlOvertime(void)	{return xmlOvertime; };

	//��ȡXMl·��
	bool GetXmlPath(std::map<eDVBType,std::string>& xmlpathmap){xmlpathmap = mXmlPath;return true;};
	bool GetXmlPath(eDVBType dvbtype,std::string& xmlpath){xmlpath = mXmlPath[dvbtype];return true;};

	//��ȡtablepath·��
	bool GetXmlTablePath(std::map<eDVBType,std::string>& xmlpathmap){xmlpathmap = mXmlTablePath;return true;};
	bool GetXmlTablePath(eDVBType dvbtype,std::string& xmlpath){xmlpath = mXmlTablePath[dvbtype];return true;};
	//add by jidushan 11.03.31 
	bool GetTableSharePath(eDVBType dvbtype, std::string& sharepath){sharepath = mTableSharePath[dvbtype];return true;};		//����dvbtype��ȡ����·��			
	bool GetTableSharePath(std::map<eDVBType,std::string>& tablesharemap){tablesharemap = mTableSharePath;return true;};		//��ȡ����·��map
	std::string GetTableSharePath(eDVBType dvbtype, bool ret);		//��ȡstring���͹���·��

	//��ȡƵ��ɨ����Ϣ
	bool GetFreqScanInfo(std::map<eDVBType,sFreqScanInfo> FreqScaninfoMap){FreqScaninfoMap = mFreqScanInfoMap;return true;};
	bool GetFreqScanInfo(eDVBType dvbtype,sFreqScanInfo& FreqScanSet){FreqScanSet = mFreqScanInfoMap[dvbtype];return true;};

	bool GetPSISIInfo(eDVBType type,std::string& valu);
	bool SetPSISIInfo(eDVBType type,std::string text);

	//CAS SMS��Ϣ
	bool GetCASIP(std::string& CASIP);
	bool GetSMSIP(std::string& SMSIP);

	bool GetCASURL(std::string& url);
	bool GetSMSURL(std::string& url);

	bool GetCASPort(int& port);
	bool GetSMSPort(int& port);
	bool GetShareDir(std::string& dir);
	bool GetHttpPath(std::string& path);

	bool GetSmsDir(std::string& dir);
	bool GetCasDir(std::string& dir);
	bool GetSIDir(std::string& dir);

#ifdef ZONG_JU_ZHAO_BIAO
	//����CAS��������IP��port��url
	bool SetCasIp(std::string strCasIp);
	bool SetCasPort(std::string strCasPort);
	bool SetCasUrl(std::string strCasUrl);

	//����sms��������ip��port��url
	bool SetSmsIp(std::string strSmsIp);
	bool SetSmsPort(std::string strSmsPort);
	bool SetSmsUrl(std::string strSmsUrl);
#endif

	bool GetOSDInfo(eDVBType dvbtype,std::string hdtv,OSDInfo& info);
	bool SetOSDInfo(eDVBType dvbtype,OSDInfo info);
	//���AlarmID
	bool GetAlarmID(long& alarmid);
	
	//�ֲ�����ͨ����� wz_0217
	bool GetVirDevList( std::list<int>& devicelist );							//(���ּ������)��ȡ�ֲ�����������ͨ����
	bool GetVirDeviceId(eDVBType type, int& deviceid);							//����type��ȡ����ͨ����
	bool GetVirDevType(int deviceid, eDVBType& type);							//����deviceid��ȡ�������
	bool IsRoundChannel(int deviceid);											//�ж��Ƿ��ֲ�������ͨ��
		
	//��ȡ��Ƶ����Э�� wz_110309
	//bool GetVideoProtocol(eDVBType dvbtype,std::string& Protocol);
	bool GetStreamProtocol(eDVBType dvbtype,std::string& Protocol);			//��ȡ������Э��
	bool GetFileProtocol(eDVBType dvbtype,std::string& Protocol);			//��ȡ�ļ�����Э��
	bool GetFileUrlType(eDVBType dvbtype,std::string& urltype);				//��ȡ�ļ�URL����			
	bool GetFileOffset(eDVBType dvbtype,std::string& offset);				//��ȡ�ļ�ƫ������
	
	//�ļ�������� wz_110309
	bool GetRecDownProtocol(eDVBType dvbtype,std::string& Protocol);			//��ȡ�ļ����ش���Э��
	bool GetRecDownUrlType(eDVBType dvbtype,std::string& urltype);				//��ȡ�ļ�����URL����			
	bool GetRecDownOffset(eDVBType dvbtype,std::string& offset);				//��ȡ�ļ�����ƫ������

	//��ʱ�ļ����	add by jidushan 11.03.31
	std::string GetTempFileLoac()	{return mTempFileLocation;};	//��ȡ��ʱ�ļ��洢·��
	std::string GetTempSharePath()	{return mTempSharePath;};		//��ȡ��ʱ�ļ�����Ŀ¼
	std::string GetTempFileExpire()	{return mTempFileExpire;};		//��ȡ��ʱ�ļ�����ʱ��

	std::string GetDeviceManagetype()     {return DeviceManagetype;};    //��ȡ�忨����ʱ������
	std::string GetDeviceManageweekday()  {return DeviceManageweekday;};
	std::string GetDeviceManagesingleday() {return DeviceManagesingleday;};
	std::string GetDeviceManagetime()      {return DeviceManagetime;};


	std::string GetPsisiManagetype()     {return PsisiManagetype;};    //��ʱ����PSISIɨ��
	std::string GetPsisiManageweekday()  {return PsisiManageweekday;};
	std::string GetPsisiManagesingleday() {return PsisiManagesingleday;};
	std::string GetPsisiManagetime()      {return PsisiManagetime;};

	void GetAlarmRecStorageCfg(enumDVBType eDvbtype, sAlarmRecordStorageCfg& info)	{ info=mAlarmRecStorageCfgMap[eDvbtype]; }		//��ȡ��̬¼��洢��������Ϣ

	bool SetTempFileExpire(std::string expire);						//������ʱ�ļ�����ʱ��
	bool IsScanFile(eDVBType dvbtype){return (mScanFile[dvbtype]=="1");};      //�Ƿ�ͨ�������ļ����Ƶ��ɨ��
	string GetScanType(eDVBType dvbtype){return mScanFile[dvbtype];};          //��ȡƵ��ɨ������
	bool IsRealTimeFromRecord(eDVBType dvbtype){return (mRealTimeFromRecord[dvbtype]=="1");};//�Ƿ��¼��ͨ����ȡʵʱ��Ƶ

	std::string GetStoreType();		//��ȡ¼���ļ��洢��ʽ����ɢ����˳��		add by jidushan 11.05.04
	std::string GetRecPathByDevId(int deviceid);	//ͨ��ͨ���Ż�ȡ¼���ļ����·��  add by jidushan 11.05.04
	bool InitInfoFromDB();          //�����ݿ��л�ȡ������Ϣ
	
#ifdef ZONG_JU_ZHAO_BIAO
	//����¼�������
	void SetHDRecordVideoParam(eDVBType dvbtype, RateParam param);
	void SetSDRecordVideoParam(eDVBType dvbtype, RateParam param);
#endif

	void GetHDRecordVideoParam(eDVBType dvbtype,VideoParam &param){param=mHDRecordVideoPram[dvbtype];};//��ȡ����������ʣ��ֱ�����Ϣ
	void GetHDRealStreamVideoParam(eDVBType dvbtype,VideoParam &param){param=mHDRealStreamVideoPram[dvbtype];};
	void GetSDRecordVideoParam(eDVBType dvbtype,VideoParam &param){param=mSDRecordVideoPram[dvbtype];};
	void GetSDRealStreamVideoParam(eDVBType dvbtype,VideoParam &param){param=mSDRealStreamVideoPram[dvbtype];};
	void GetAudioBps(eDVBType dvbtype,string &audiobps){audiobps=mAudioBps[dvbtype];};
	bool GetDeviceInfo();
	bool SeparateStrVec( string src,vector<int>& lis );
	bool SeparateStr(string src,std::list<int>& lis);		            //��src���ݷֺŷָ�ɲ�ͬ���ַ��������뵽lis��
private:
	void InitiateBaseInfo();		//��ȡ������Ϣ
	bool InitiateDeviceInfo();       //��������Ϣ��ͨ����Ϣ�����������Ϣ��XML·����Ϣ�����ڴ�
	bool InitiateVirtualKeySetInfo();	//��ȡ ����ң����������Ϣ
	bool LoadHDInfo();
private:
	bool DeleteWrongDeviceID(std::list<int>& deviceIDList);		        //ɾ������ͨ�����ͨ�����в������ͨ����
	bool CheckDeviceInList(int deviceid,string deviceList);		//�ж�ͨ����deviceid�Ƿ�����ڸ������ͨ������
	//�ֽ��ַ���srcloc��srcshare�е�·�����洢��map��vector��	add by jidushan 11.03.31
	bool SepLocShareAndInit(string srcloc, string srcshare, map<string ,string>& loctoshare, vector<string>& vecLoc);
	bool SeparateRecordDir( string src );
private:

	std::string DeviceManagetype;//�忨����ʱ������
	std::string DeviceManageweekday;
	std::string DeviceManagesingleday;
	std::string DeviceManagetime;
	
	std::string PsisiManagetype;//�忨����ʱ������
	std::string PsisiManageweekday;
	std::string PsisiManagesingleday;
	std::string PsisiManagetime;

	PropConfig* mConfig;
	//ϵͳ
	std::string mHttpVideoIP;		//��ƵHttp IP
	std::string mHttpVideoPort;		//��ƵHttp�����˿�
	std::string mVideoMaxnum;		//���������

	std::string mRtspVideoIP;		//��ƵRtsp IP
	std::string mRtspVideoPort;		//��ƵRtsp�����˿�

	std::string mQualityIp;			//ָ��IP
	std::string mQualityPort;		//ָ������˿�
	std::string mQualityMaxnum;		//ָ�����������

	std::string mRecordQualityIp;			//ָ��IP
	std::string mRecordQualityPort;		//ָ������˿�
	std::string mRecordQualityMaxnum;		//ָ�����������

	std::string mDeviceIp;			//�忨����IP
	std::string mDevicePort;		//�忨����˿�
	std::string mDeviceMaxnum;		//���������

	std::string mXmlServerIP;		//xml����IP
	std::string mXmlServerPort;		//xml����Port

	std::string mLogIp;             //��־����IP
	std::string mLogPort;           //��־��������˿�
	std::string mLogMaxnum;         //��־�������������

	std::string mLogPath;			//��־·��
	std::string mLogPath_VIDEO;			//FRee_fanrong_20140107	
	std::string mLogPath_SI;			//FRee_fanrong_20140107	
	std::string mLogPath_SMS;			//FRee_fanrong_20140107	
	std::string mLogPath_CAS;			//FRee_fanrong_20140107	

	std::string mLogExpire;			//��־��������
	std::string mLogType;			//��־����
	std::string mLogOutputFile;		//
	std::string mLogAnalyser;		//ָ����־�ļ�����
	std::string mLogVideo;			//��Ƶ��־�ļ�����
	std::string mLogRecord;			//¼��������־�ļ�����
	std::string mLogDevice;         //�豸��־�ļ�����
	std::string mLogOther;			//������־�ļ�����
	std::string mLogDefault;		//Ĭ�ϵ���־�ļ�����

	std::string mDbType;			//���ݿ�����
	std::string mDbIp;				//���ݿ��ַ
	std::string mBandDbIp;			//�������ݿ��ַ
	std::string mDbPort;			//���ݿ�˿�
	std::string mDbUsername;		//���ݿ��û���
	std::string mDbPwd;				//���ݿ�����
	std::string mDbName;			//���ݿ�����

	//¼���ļ���Ϣ
	std::map<std::string, std::string> mRecLocToShare;		//¼���ļ�·���빲��·������map
	std::vector<std::string> mRecFileLocVec;			//¼���ļ��洢·���б�

	std::string mRecordPeriod;		//¼��ʱ����
	std::string mMaxAvailableSize;	//������С�ռ�GB
	std::string mDBCleanInterval;	//���ݿ�����ʱ����
	std::string mSystemCleanTime;	//ϵͳ����ʱ��

	std::map<enumDVBType, sRecordParamInfo> mRecParamInfo;	//¼�������Ϣmap
	//¼���ļ����������
	std::string mHttpServerIP;       //http������IP
	std::string mHttpServerport;     //http�������˿�
	std::string mFtpServerIP;        //ftp������IP
	std::string mFtpServerPort;      //ftp�������˿�

	//��ͬ����Ĭ��dstcode��srccode
	std::map<eDVBType,std::string> mDefDstCode;
	std::map<eDVBType,std::string> mDefSrcCode;
	//***********************************************
	//UDP��Ƶ�ͻ���IP
	string m_strUdpVideoServerClientIP;
	//
	bool m_bUdpVideoServerUsed;
	//Ƶ��ɨ�跽ʽ
	Scaninfo chantypeinfo;
	//IO����豸��Ϣ
	std::vector<TunerDev> mTunerDevVec;
	//ת���豸��Ϣ
	std::vector<CoderDev> mCoderDevVec;
	//
	std::vector<sDeviceInfo> mMonitorDevInfo;
	//
	std::vector<sDevLinkInfo> msDevLinkInfoVec;
	//
	std::list<int> m_unUseRecCoder;//��¼ʵʱ��Ƶ,Ƶ���ֲ���ʹ�õ�ת��ͨ��,ת��ͨ�������ź�����;¼��ͨ������ʱ����ʹ��
	std::map<eDVBType,std::list<int>> m_unUseRecTuner;//��¼ʵʱ��Ƶ,Ƶ���ֲ�,Ƶ��ɨ��,MER-BER������ʹ�õĽ��ͨ��;¼��ͨ������ʱ����ʹ��
	//
	int m_itunerdeviceid;
	int m_icodedeviceid;
	//***********************************************
	//�豸������Ϣ
	//std::map<int,sChanelInfo> mDeviceInfo;
	std::map<std::string,std::string> mTaskInfo;
	std::map<std::string,bool> mTaskShare;
	int m_nAllCardCount;   //FRee_fanrong_20131212
	int m_nAllDeviceIndexCount; //FRee_fanrong_20131212

	//����
	long xmlSendTimes;         //xml�ļ����ʹ���
	long xmlOvertime;          //xml�ļ����ͳ�ʱʱ��[��λ:��]

	std::map<std::string, sEquInfo*> m_mapEquInfo;	//�豸code��url��Ϣ

	std::map<eDVBType,std::string> mXmlPath;			//�������·��
	std::map<eDVBType,std::string>mXmlTablePath;        //����Ϣ����·��
	//add by jidushan 11.03.31
	std::map<eDVBType,std::string>mTableSharePath;		//����·��map

	//wz_110309
	std::map<eDVBType, sVideoProcotol>mVideoProtocolMap;	//��Ƶ����Э��map
	std::map<eDVBType, sRecordDownInfo>mRecordDownMap;		//�ļ�������Ϣmap

	//psisi��һ����ɱ�ʶ
	std::string mDVBCPsiSiValue;
	std::string mCTTBPsiSiValue;

	std::map<eDVBType,QualityCard> mQualityCard;//ָ�꿨��Ϣ

	//CAS SMS��ַ��Ϣ
	std::string ShareDir;//���ع���cassms�ļ���Ŀ¼
	std::string HttpPath;//Http������Ŀ¼����

	std::string CASIPAddr;
	std::string SMSIPAddr;

	int CASPort;
	int SMSPort;

	std::string CASRequestURL;  //FRee_fanrong_20140108
	std::string SMSRequestURL;


	
	std::string SmsDir;//���ش洢SMS�ļ���·��
	std::string CasDir;
	std::string SIDir;

	std::map<eDVBType,std::string> mRealTimeFromRecord; //ʵʱ��Ƶ�Ƿ��¼��ͨ����ȡ 0����1����
	std::list<int> RecordList; //����¼��ͨ��
	//Ƶ��ɨ����Ϣ
	std::map<eDVBType,sFreqScanInfo> mFreqScanInfoMap;
	std::map<eDVBType,OSDInfo> mOSDInfoMap;
	//��ͬ������͵�Э��
	//AlarmID
	static long AlarmID;
	
	std::map< enumDVBType, int > mVirtualDevInfo;	//�ֲ�����ͨ����Ϣ  wz_0217
	
	std::map<enumDVBType,UpStreamRoundInfo> mUpStreamRoundInfo; //�������ֲ������ϱ�  //FRee_fanrong_20140103
	// ���������ϱ�
	std::map<enumDVBType,UpAlarmInfo> mUpAlarmInfo;
	std::map<enumDVBType,std::string> mScanFile;
	//��ʱ�ļ��������	add by jidushan 11.03.31
	std::string mTempFileLocation;		//��ʱ�ļ������ַ
	std::string mTempSharePath;			//��ʱ�ļ�����·��
	std::string mTempFileExpire;		//��ʱ�ļ�����ʱ��

	std::string mStoreType;			//¼���ļ��洢���ͣ���ɢ����˳��		add by jidushan 11.05.04
	std::map<std::string, std::list<int>> mDeviceIdToPath;		//ͨ������¼���ļ����path����map  add by jidushan 11.05.04

	std::map<enumDVBType,VideoParam> mHDRealStreamVideoPram;
	std::map<enumDVBType,VideoParam> mHDRecordVideoPram;
	std::map<enumDVBType,VideoParam> mSDRealStreamVideoPram;
	std::map<enumDVBType,VideoParam> mSDRecordVideoPram;
	
	std::map<enumDVBType,std::string> mAudioBps;
	std::map<enumDVBType, sAlarmRecordStorageCfg> mAlarmRecStorageCfgMap;

	vector<Quality_DevInfo> m_VecQuaDevInfo;//�������� ��ȡ��ǿָ�� �豸��Ϣ
	HDMIInfo m_HDMIInfo;//����HDMI��������Ϣ
	std::map<std::string, VirtualKeyInfo*> m_mapVirtualKeyInfo;
	std::map<std::string , std::string> m_mapFreqLevels;//Ƶ���Ӧ�ĵ�ƽָ��
	std::string m_sCurWorkVersion;						//��ǰЭ��汾
	std::map<eDVBType, std::list<int>> mscanValidFreq;		
	std::string m_sAMNum;						//��ǰЭ��汾
	std::map<eDVBType, AlarmParamConfig> m_mapAlarmParamConfig;//��������������Ϣ
	vector<HDInfo> m_vHDInfos;

    std::string tempFreq;//���ö�Ƶ�ַ���
    std::string m_AlarmTimeRelease;
};
typedef ACE_Singleton <PropManager, ACE_Thread_Mutex>  PROPMANAGER;
#endif