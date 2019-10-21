#ifndef _TYPE_DEF_H_
#define _TYPE_DEF_H_
#pragma once
///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����TypeDef.h
// �����ߣ�gaoxd
// ����ʱ�䣺2009-05-20
// �����������Զ������Ͷ��壬����ദʹ�õ��Զ�������ͳһ�ڴ˴�����
///////////////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <list>
#include <vector>
#include <map>
#include "ace/SOCK_Acceptor.h"
#include "TimeUtil.h"
#include "Config.h"
class XMLTask;

const std::string strHDMIFreq = "Set-Top-Box";	//��������������Freq = "Set-Top-Box"
const int ChassisMaxNum = 8;
const int SlotMaxNum = 64;
const int ModulMaxNum = 6;

//��ʱ����ʱ��η���
typedef enum eDVBType 
{
	UNKNOWN	= 0,
	CTTB	= 1,
	CMMB	= 2,
	DVBC	= 3,
	RADIO	= 4,
	ATV		= 5,//������·����
	AM		= 6,
	DVBS    = 7,
	CTV     = 8,//���ߵ���
	THREED  = 9,
	HDMI	= 10

} enumDVBType;
/*
//��ʱ����ʱ��η���
typedef enum eFrequence 
{
	ONCE = 0,
	DAILY,
	WEEKLY,
	MONTHLY
} enumFrequence;
*/
//����״̬���Ͷ���
typedef enum eTaskStatus 
{
	WAITING,			//�ȴ�����������ʱ״̬��
	READY,				//�������ȴ�������
	RUNNING,			//������
	FINISHED,			//�������н���
	EXPIRED				//������ڣ��������������Ϊ���ڣ���ʱ�������һ��ִ����Ϻ�Ϊ���ڣ�
} enumTaskStatus;

//��������/���н��״ֵ̬
typedef enum eTaskRetStatus 
{
	RUN_SUCCESS				= 0,			//���гɹ�
	RUN_FAILED				= 1,			//����ʧ�ܣ�������
	NOFILE_EXIST		    = 2,            //�ļ�������
	CHANNELID_UNAVAIABLE	= 3,			//Ƶ���Ƿ�
	DEVICEID_UNAVAIABLE		= 4,			//ͨ���Ƿ�
	NODEVICEIDMATCH			= 5,			//û��ͨ��ƥ�������
	PREFERENTIALTASK_USE	= 6,			//���ڸ����ȼ�����ռ��
	DATABASEERROR			= 7,			//�������ݿ����
	DEVICEERROR				= 8,			//����Ӳ������
	WAITFORSET				= 100,			//�ȴ�����
	SET_SUCCESS				= 101,			//���óɹ�
	RECORD_DOWN_OUTOFRAND	=102,			//¼��ʱ�䳬����(����)
	ADRESS_ERROR			=103,           //����洢·������ȷ
	USERNAME_ERROR			=104,			//�û��������벻��ȷ
	CHANNELID_DEVICEID_NO	=105,			//��ĿƵ����ͨ����ƥ��
	NO_RECORD				=106,			//ʱ�����¼���ļ�
	CHANLID_CHANGE			=107,			//���ϴ���ǰƵ��¼���ļ�
	TIMEFORMAT_ERROR		=108,			//ʱ���ʽ����(�ϴ���ʼ����ʱ��Ĭ�����ݿ�����ʱ��,�������ʱ�䲻��Ϊ��)
} enumTaskRetStatus ;	


//��־���Ͷ���
typedef enum eLogType              
{
	LOG_EVENT_DEBUG = 0,		//������Ϣ
	LOG_EVENT_WARNNING = 1,		//������Ϣ
	LOG_EVENT_ERROR = 2			//������Ϣ
}enumLogType;

//��־�������
typedef enum eLogOutPut
{
	LOG_OUTPUT_UNDEFINE = 0,	//δ�����������
	LOG_OUTPUT_SCREEN	= 1,	//�������Ļ
	LOG_OUTPUT_FILE		= 2,	//������ļ�
	LOG_OUTPUT_BOTH		= 3		//�������Ļ���ļ�
}enumLogOutPut;

//ģ�����Ͷ���
typedef enum eModuleType              
{
	NONE			= 0,		//�����ͣ����ڴ���
	RECORD			= 1,		//¼��
	VIDEO			= 2,		//��Ƶ������ʵʱ��Ƶ����Ƶ�ֲ���¼����Ƶ�鿴
	DATAANALYSER	= 3,		//ָ�������PSI/SI�����Լ�������������
	ALARM			= 4,		//����		
	OTHER			= 5,		//����������
	DEVICE			= 6,		//Ӳ������־
	TASK			= 7,		//������־
	DATABASE		= 8			//���ݿ������¼��־

}enumModuleType;

//��־�����ļ�����������   //FRee_fanrong_20140109
typedef struct  
{
	eModuleType Module;
	string LogPath;
}sLogPath;
typedef std::list<sLogPath> sLogPathModuleInfo;  


//��������
typedef enum eAlarmType
{
	ALARM_NONE			      = 0,	//δ���屨������
	ALARM_ENVIRONMENT	      = 1,	//������������
	ALARM_FREQ			      = 2,	//��Ƶ��������
	ALARM_PROGRAM		      = 3,	//��Ŀ��������
	ALARM_TR101_290		      = 4,	//TR101_290��������
	ALARM_EQUIPMENT		      = 5,	//�豸���ϱ�������
	ALARM_RECORDSCHEDULER     = 6   //¼���ͻ����
}enumAlarmType;

//ϵͳ��Ϣ���Ͷ���
typedef enum eSysMsgType              
{
	VS_MSG_SYSTINFO = 0,		//ϵͳ��ͨ��Ϣ
	VS_MSG_PROALARM = 1,		//��Ŀ��̬/������Ϣ
	VS_MSG_SYSALARM = 2			//ϵͳ������Ϣ
}enumSysMsgType;

//��Ŀ����
typedef enum ProgramType
{
	H264_SD  = 0,              //H264��������Ŀ
	H264_HD	 = 1,			   //H264��������Ŀ
	MPEG2_SD = 2,			  //MPEG2��������Ŀ
	MPEG2_HD = 3			 //	MPEG2��������Ŀ
};
//***********************************************
//
struct AlarmParamConfig
{
	eDVBType dvbType;
	std::string s_mutehold;
	std::string s_bfhold;
	std::string s_sthold;
	AlarmParamConfig()
	{
		dvbType = UNKNOWN;
		s_mutehold = "";
		s_bfhold = "";
		s_sthold = "";
	}
};
struct VirtualKeyInfo
{
	std::string s_Index;
	std::string s_Name;
	std::string s_Key;
	VirtualKeyInfo()
	{
		s_Index = "";
		s_Name = "";
		s_Key = "";
	}
};
//�����Ŀ��Ϣ�����ļ�
struct HDInfo
{
	std::string sFreq;
	std::string sServiceID;
	std::string sHDType;
};
//�����Ŀ��Ϣ�ֱ��ʻ�ȡ
struct HDInfoForScan
{
	std::string sFreq;
	std::string sServiceID;
	std::string sHD;
};

struct sEPGInfo
{
	std::string sFreq;
	std::string sServiceID;
	std::string sProgram;
	std::string sProgramType;
	std::string sStartTime;
	std::string sProgramLen;
	std::string sState;
};

//HDMI��������Ϣ
struct HDMIInfo
{
	std::string s_DevIndex;//�豸����
	std::string s_HDMIChannelNum;//HDMI ͨ����
	std::string s_COMPort;
	unsigned char c_IDCode;		//����ID��
	int i_IntervalTime;		//����������ʱ��
	HDMIInfo()
	{
		s_DevIndex = "";
		s_HDMIChannelNum = "";
		s_COMPort = "";
		c_IDCode = 0;
		i_IntervalTime = 0;
	}
};

//�������� ��ȡ��ǿָ�� �豸��Ϣ
struct Quality_DevInfo
{
	int i_index;
	int i_port;
	std::string s_qualityIP;
	std::string s_cmdProtocol;
	std::string s_devType;
	std::string s_usedType;//ָ�����1�Ǳ���ʹ������ͨ��,0��ʾ����¼��ɻ�ȡ¼��ͨ����ƽ
	map<string, string> mapDvbTypeChls;
	Quality_DevInfo()
	{
		i_index = 0;
		i_port = 0;
		s_qualityIP = "";
		s_cmdProtocol = "";
		s_devType = "";
		s_usedType = "1";
	}
};

typedef struct   
{
	std::string s_scantype;
	std::string s_freq;
	std::string s_weeklyswitch;
	std::string s_epg;
	std::string s_scanTime;//��ʱɨ���ʱ��
	std::string s_checkHD;//0:ͨ����Ŀ�������ָ��塢1:ͨ���ֱ��ʼ�����
	std::string s_scanversion;//ɨ��֧�ְ汾
}Scaninfo;
//�豸��Ϣ����
typedef struct
{
	std::string s_deviceindex; // �豸��� 1U�豸�������ļ���ȡ ��׼���豸 ����� ��ʵ
	std::string s_deviceip; //�豸ip��ַ
	std::string s_deviceport; //�豸ip�˿�
	std::string s_devicecampronum; //�豸���Ž�Ŀ����,cam����2·��,8·�ĵ�,��������豸���8·
	std::string s_unuseditem; //������
	std::string s_manufacturer;
	int i_HDnum;
	int i_SDnum;
	int i_TcoderNum;
	//
	std::string s_cardindex;//���ۺ�
	std::string s_modindex;//ģ��� 
	unsigned char ucharChanNum;
	unsigned char ucharBoardType;//0-�������1-ת�뿨	
	unsigned char ucharMpeg2Needs; 
	unsigned char ucharH264Needs; 
	unsigned char ucharAVSNeeds;
	unsigned char ucharH265Needs;	
}sDeviceInfo;//1U�豸Ϊ1������ ��׼���豸Ϊ1��ͨѶ��Ԫ
typedef struct  
{
	eDVBType e_tunerdvbtype;
	std::string s_tunerdeviceid;//ȫ���߼�ͨ����
	std::string s_monitorindex;//�����
	std::string s_cardindex;//���ۺ�
	std::string s_modindex;//ģ���
	std::string s_chanindex;//ͨ����
	std::string s_freq; //Ƶ��(K)
	std::string s_sym; //������
	std::string s_qam; //���Ʒ�ʽ
	std::string s_sip; //������TS��IP ת������
	std::string s_stsport; //������TS���˿�  ת������
	std::string s_trport; //����
	std::string s_desc; 
	std::string s_290ip; //290����ip
	std::string s_290port; //290�����˿�
	std::string s_servidList; //���õĽ��Ž�Ŀ�б�
	std::string s_devicecampronum; //�豸���Ž�Ŀ����,cam����2·��,8·�ĵ�,��������豸���8·
	std::string s_manufacturer;
	std::string s_serialnum;
	std::string s_ICcardNO;
	std::string s_ExistCmd;//�Ѿ����õ�����
	bool b_user;
	std::vector<string> vecAllServid;
}TunerDev;
typedef struct  
{
	std::string s_coderdeviceid;//ȫ���߼�ͨ����
	std::string s_monitorindex;//����� ��ʵ
	std::string s_cardindex;//���ۺ�
	std::string s_modindex;//ģ���
	std::string s_modcoderindex;//ģ��ͨ���� ���ƽ̨��ģ�� ��׼���豸ģ��ͨ���ź�ͨ������ͬ
	std::string s_coderindex;//ͨ���� ��ʵ ��׼���豸ģ��ͨ���ź�ͨ������ͬ
	std::string s_ena; //1:ʹ��ʹ�ܺڳ�/����֡/����,  0:ֹͣ 
	std::string s_spro; //ת��Դ��Ŀid
	std::string s_code; //mpeg2\h264\h265
	std::string s_fmt; //0:�Զ�,1:D1,2:CIF
	std::string s_fre; //0:�Զ�,����:֡��
	std::string s_rate; //����
	std::string s_sip; //������TS��IP ת������
	std::string s_stsport; //������TS���˿�  ת������
	std::string s_dip; //ת�����IP
	std::string s_dtsport; //ת������˿�
	std::string s_alarmip; //���汨��ip
	std::string s_alarmport; //���汨���˿�
	std::string s_freq; //Ƶ��
	std::string s_manufacturer;
	std::string s_serialnum;
	std::string s_desc; 
	std::string s_maincoderdeviceid;//���ڸ���ʹ�ö�ͨ��ʱ,��¼������ͨ��
	bool b_user;
	bool b_ishd;
	eDVBType e_tunerdvbtype;//���ź�������������
}CoderDev;

typedef struct
{
	std::string s_coderdeviceid; 
	std::string s_tunerdeviceid; 
	eDVBType e_tunerdvbtype;
	std::string Priority;	//���ȼ�
	std::string Taskid;	
}sDevLinkInfo;

typedef struct
{
	std::string s_taskxml;
	std::string s_taskid; 
	std::string s_active; 
	std::string s_channelid; 
	std::string s_devcodehead; 
	std::string s_uploadsucenddtime; 
	std::string s_uploadsucfilename;
	std::string s_taskinfo;
}sRecordUploadTaskInfo;
//
typedef struct
{
	unsigned char BoardInfo[8];
	bool ExistCard; 
}standardBoardCheckInfo;
typedef struct
{
	unsigned char ucharChassis; 
	unsigned char ucharSlot; 
	unsigned char ucharChanNum;
	unsigned char ucharBoardType;	
	unsigned char ucharIPAddr[4];
	unsigned short ushortPort;
	unsigned short ushortReserved;
	unsigned char ucharMpeg2Needs; 
	unsigned char ucharH264Needs; 
	unsigned char ucharAVSNeeds;
	unsigned char ucharH265Needs;
	time_t startTime;
}standardChannelResource;
typedef struct
{
	unsigned char ucharLevel; 
	unsigned char ucharSubType; 
	unsigned char ucharReason;
	unsigned char ucharReserved;	
	unsigned int  uintPid;
	unsigned int  uintAlarmTime;
}standardTR290Result;
typedef struct
{
	unsigned char ucharLevel; 
	unsigned char ucharlockStatus; 
	unsigned short ushortMer;
	unsigned short ushortEVM;	
	unsigned int  uintBer;
	unsigned short  ushortCNR;
}standardRFResult;
typedef struct
{
	unsigned short ushortVStatus; 
	unsigned short ushortAStatus; 
	unsigned int  uintAlarmTime;
}standardAVErrorStatus;

//**********************************************
typedef struct
{
	int CardIndex; //�忨��� //FRee_fanrong_20131212
	std::string devicetype; // �������
	std::string TSIP; //tsip��ַ
	std::string CMDIP; //cmdip��ַ
	std::string cmdport;//����˿ں�
	std::string tsport;//���ݶ˿ں�
	std::string cmdprotocol;//����Э��
	std::string tsprotocol;//����Э��
	std::string tunerid;//tunerͨ����
	std::string logindex;//�߼�ͨ��
	std::string recordlist;//¼��ͨ��
}sChanelInfo;

typedef std::map<std::string,std::list<int> > TaskInfoMap;
typedef std::vector<sRecordUploadTaskInfo> vTaskUp;
struct sdvbsfreqinfo
{
	std::string slocfreq;
	std::string sdownfreq;
	std::string shv;
	std::string ssymbol;
	std::string sfec;
	std::string s22k;
};

typedef struct
{
	int DevicID;	//�豸ID
	int Statuc; //�豸״̬     ��ȡֵ��0 -����״̬  1 -�ް忨  2 - ����������
	std::string Desc;	//�豸����   
}sDeviceStatus;

//��Ƶ�û���Ϣ
typedef struct VEDIOUSERINFO
{
	ACE_SOCK_Stream client;	//�û�Socket
	eDVBType DvbType;
	int fail_num;			//���ʹ������
	std::string DeviceID;	//�û�ʹ�õ�ͨ����
	std::string Name;		//����
	std::string URL;		//����Ƶ��ַ
	std::string IP;			//��ַ
	std::string Port;		//�˿�
	std::string Priority;	//���ȼ�
	std::string CenterCode;		//���ĺ�
	std::string MsgID;
	std::string Freq;  //Ƶ����Ϣ

	VEDIOUSERINFO()
	{
		DvbType = UNKNOWN;
		fail_num = 0;
		DeviceID = "";
		Name = "";
		IP = "";
		Port = "";
		URL = "";
		Priority = "";
		CenterCode = "";
		MsgID = "";
		Freq="";
	};
	VEDIOUSERINFO&	operator =(const VEDIOUSERINFO& User)
	{
		client= User.client;
		DvbType = User.DvbType;
		fail_num = User.fail_num;
		DeviceID = User.DeviceID;
		Name = User.Name;
		IP = User.IP;
		Port = User.Port;
		URL = User.URL;
		Priority = User.Priority;
		CenterCode=User.CenterCode;
		MsgID = User.MsgID;
		Freq = User.Freq;
		return *this;
	};
	bool	operator ==(const VEDIOUSERINFO& User)
	{
		return client.get_handle()==User.client.get_handle()&&DeviceID==User.DeviceID&&Name==User.Name&& \
				IP==User.IP&&Port==User.Port&&Priority==User.Priority&& CenterCode==User.CenterCode && MsgID==User.MsgID && Freq==User.Freq;
	};

}sVedioUserInfo;

//Ƶ��ɨ����Ϣ����
typedef struct CHANNELINFO{
	//�ؼ�Ƶ����Ϣ
	std::string ChannelID;
	std::string STD;
	std::string Freq; 
	std::string OrgNetID; 
	std::string TsID;
	std::string PmtPID;  //wz_101123
	//Ƶ����Ϣ
	std::string SymbolRate; 
	std::string QAM;
	std::string HeaderMode;
	std::string CarrierNumber;
	std::string EncodeEfficiency;
	std::string InterwovenMode;
	std::string DoublePilot; 
	std::string PnPhase;
	std::string ChannelCode;
	std::string ProgramName;
	std::string ServiceID;
	std::string VideoPID;
	std::string AudioPID;
	std::string HDTV;
	std::string Encryption; //1��ʾ���� 0��ʾ����
	std::string FreeCA; //1��ʾ��Ȩ������ 0��ʾδ��Ȩ���� ��洢�ļ�ICInfoChannelEncryptQuery_LastReport��ChannelScanQuery.xml����Encrypt���Ӧ
	std::string PcrPID;  //wz_101123
	std::string VideoStreamType;
	std::string AudioStreamType;
	std::string ServiceType;   //FRee_fanrong_20140110
public:
	CHANNELINFO(const CHANNELINFO& other) //copy construction
	{
		ChannelID = other.ChannelID;
		STD=other.STD;
		Freq=other.Freq;
		OrgNetID=other.OrgNetID;
		TsID=other.TsID;
		PmtPID = other.PmtPID;  //wz_101123

		SymbolRate=other.SymbolRate;
		QAM=other.QAM;
		HeaderMode=other.HeaderMode;
		CarrierNumber=other.CarrierNumber;
		EncodeEfficiency=other.EncodeEfficiency;
		InterwovenMode=other.InterwovenMode;
		DoublePilot=other.DoublePilot;
		PnPhase=other.PnPhase;
		ChannelCode=other.ChannelCode;
		ProgramName = other.ProgramName;
		ServiceID = other.ServiceID;
		VideoPID = other.VideoPID;
		AudioPID = other.AudioPID;
		HDTV=other.HDTV;
		Encryption = other.Encryption;
		FreeCA = other.FreeCA;
		PcrPID = other.PcrPID;  //wz_101123
		VideoStreamType = other.VideoStreamType;
		AudioStreamType = other.AudioStreamType;
		ServiceType = other.ServiceType; //FRee_fanrong_20140110
	};
   CHANNELINFO()
   {

   };
   bool	operator ==(const CHANNELINFO& other)
   {
	   return STD==other.STD&&Freq==other.Freq&&OrgNetID==other.OrgNetID&&TsID==other.TsID
		   &&ServiceID==other.ServiceID&&VideoPID==other.VideoPID&&AudioPID==other.AudioPID//;
		   &&PmtPID==other.PmtPID&&PcrPID==other.PcrPID;  //wz_101123
   };
   bool	operator >(const CHANNELINFO& other)
   {
	   return true;
   };
   friend  bool	 operator <(const CHANNELINFO& other1,const CHANNELINFO& other2)
   {
	   return other1.Freq<other2.Freq;
   };
   bool	operator <(const CHANNELINFO& other)
   {
	   return Freq<other.Freq;
   };
   bool	operator ()(const CHANNELINFO& other)
   {
	   return true;
   };
   CHANNELINFO&	operator =(const CHANNELINFO& other)
   {
	   ChannelID = other.ChannelID;
	   STD=other.STD;
	   Freq=other.Freq;
	   OrgNetID=other.OrgNetID;
	   TsID=other.TsID;
	   PmtPID = other.PmtPID;  //wz_101123

	   SymbolRate=other.SymbolRate;
	   QAM=other.QAM;
	   HeaderMode=other.HeaderMode;
	   CarrierNumber=other.CarrierNumber;
	   EncodeEfficiency=other.EncodeEfficiency;
	   InterwovenMode=other.InterwovenMode;
	   DoublePilot=other.DoublePilot;
	   PnPhase=other.PnPhase;
#ifndef ZONG_JU_ZHAO_BIAO
	   if (other.ChannelCode!="")
	   {
#endif
		   ChannelCode=other.ChannelCode;
#ifndef ZONG_JU_ZHAO_BIAO
	   }
#endif
	 
#ifndef ZONG_JU_ZHAO_BIAO
	   if (other.ProgramName!="")
	   {
#endif
		   ProgramName = other.ProgramName;
#ifndef ZONG_JU_ZHAO_BIAO
	   }
#endif
	   
	   ServiceID = other.ServiceID;
	   VideoPID = other.VideoPID;
	   AudioPID = other.AudioPID;
	   HDTV=other.HDTV;
	   Encryption = other.Encryption;
	   FreeCA = other.FreeCA;
	   PcrPID = other.PcrPID;  //wz_101123
	   VideoStreamType = other.VideoStreamType;
	   AudioStreamType = other.AudioStreamType;
	   ServiceType = other.ServiceType; //FRee_fanrong_20140110
	   return *this;
   };

}sChannelInfo;

typedef std::map<enumDVBType,std::vector<sChannelInfo> > ChannelInfoMap;
typedef std::map<std::string,std::vector<sChannelInfo>>	STRCHANINFOMAP;

//¼����Ϣ
typedef struct{
	enumDVBType dvbtype;
	std::string channelID;
	std::string DeviceID;
	std::string taskid;
	std::string filename;
	std::string starttime;
	std::string endtime;
	std::string expiredays;
}sRecordInfo;
//�Զ�¼���ļ���Ϣ
typedef struct
{
	std::string url;
	std::string starttime;
	std::string endtime;

}sRecordFileInfo;
//������Ϣ
typedef struct  
{
	std::string centerType;
	std::string centerUrl;
}sCenterInfo;

//��������
typedef struct AlarmParam{
	enumDVBType DVBType;			//�������
	enumAlarmType AlarmType;		//��������
	std::string TypeID;				//����ID
	std::string TypeDesc;			//��������
	std::string STD;				//�����׼
	std::string Freq;				//Ƶ��
	std::string SymbolRate;			//������
	std::string ChannelID;			//Ƶ��ID
	std::string Duration;			//����ʱ��
	std::string Num;				//��������
	std::string TimeInterval;		//ʱ����
	std::string DownThreshold;		//����
	std::string UpThreshold;		//����
	std::string Switch;				//����  1���� 0 �� ��
	std::string DeviceID;           //�豸ͨ����
	
	AlarmParam& operator = (const AlarmParam param)
	{
		DVBType=param.DVBType;
		AlarmType=param.AlarmType;
		TypeID=param.TypeID;
		if (param.TypeDesc!="")
		{
			TypeDesc=param.TypeDesc;
		}
		if(param.STD!="")
		{
			STD=param.STD;
		}
		if (param.TypeDesc!="")
		{
			TypeDesc=param.TypeDesc;
		}
		if(param.Freq!="")
		{
			Freq=param.Freq;
		}
		if (param.SymbolRate!="")
		{
			SymbolRate=param.SymbolRate;
		}
		if(param.ChannelID!="")
		{
			ChannelID=param.ChannelID;
		}
		if (param.Duration!="")
		{
			Duration=param.Duration;
		}
		if(param.Num!="")
		{
			Num=param.Num;
		}
		if (param.TimeInterval!="")
		{
			TimeInterval=param.TimeInterval;
		}
		if(param.DownThreshold!="")
		{
			DownThreshold=param.DownThreshold;
		}
		if (param.UpThreshold!="")
		{
			UpThreshold=param.UpThreshold;
		}
		if(param.Switch!="")
		{
			Switch=param.Switch;
		}
		if(param.DeviceID!="")
		{
			DeviceID=param.DeviceID;
		}

	return *this;
	}
}sAlarmParam;

typedef std::vector<sAlarmParam> AlarmParamVec;
typedef std::map<std::string,std::vector<sAlarmParam>> CodeAlarmParamMap;

//��鱨����Ҫ�Ĳ���
typedef struct CheckParam{
	enumDVBType DVBType;			//�������
	enumAlarmType AlarmType;		//��������
	std::string TypeID;				//����ID
	std::string TypeDesc;			//��������
	std::string STD;				//�����׼
	std::string Freq;				//Ƶ��
	std::string SymbolRate;			//������
	std::string ChannelID;			//Ƶ��ID
	std::string TypedValue;			//����ֵ
	time_t CheckTime;
	std::string AlarmID;
	std::string DeviceID;           //�豸ͨ����
	std::string Cardindex;			//���ۺ�
	std::string Modindex;			//ģ���
	std::string mode;
	std::string ServiceID;
	std::string VideoID;
	std::string AudioID;
	std::string StrCheckTime;
	std::string StartTime;
	std::string EndTime;
}sCheckParam;
//�����ϱ���ʽ
struct  AlarmInfo
{
	enumDVBType DVBType;
	enumAlarmType AlarmType;
	std::string STD;				//�����׼
	std::string Freq;				//Ƶ��
	std::string SymbolRate;			//������
	std::string ChannelID;			//Ƶ��ID
	std::string TypeID;				//����ID
	std::string TypeDesc;			//��������
	std::string AlarmValue;			//����ֵ
	std::string CheckTime;
};
//���������Ϣ
struct AlarmResult
{
	enumDVBType DVBType;			//�������
	enumAlarmType AlarmType;		//��������
	std::string TypeID;				//����ID
	std::string TypeDesc;			//��������
	std::string STD;				//�����׼
	std::string Freq;				//Ƶ��
	std::string SymbolRate;			//������
	std::string ChannelID;			//Ƶ��ID
	std::string TypedValue;			//����ֵ
	std::string AlarmTime;			//��������ʱ��
};
typedef struct  RunPlanParam
{
	enumDVBType dvbtype;			//�������
	std::string ChannelID;			//Ƶ��ID
	int Type;						//ʱ������� 0�����Σ�1�����
	std::string Month;				//�·�
	std::string DayOfMonth;			//�·��е�����
	std::string DayOfWeek;			//����
	std::string StartTime;			//��ʼʱ��
	std::string EndTime;			//����ʱ��
	std::string ValidStartDateTime;	//��Ч��ʼʱ��
	std::string ValidEndDateTime;	//��Ч����ʱ��
	std::string StartDateTime;		//��ʼʱ��
	std::string EndDateTime;		//����ʱ��
}sRunPlanParam;

typedef std::vector<sRunPlanParam> RunPlanParamVec;
typedef std::map< std::string, RunPlanParamVec > RunPlanParamMap;

typedef struct QualityInfo{
	std::string type;
	std::string valu;
	std::string desc;
	std::string STD;
	std::string checktime;
	std::string freq;
}eQualityInfo;

typedef struct QualityDBInfo{
	enumDVBType s_dvbtype;
	std::string s_strTaskId;
	eQualityInfo s_quaInfo;
}sQualityDBInfo, *pQualityDBInfo;

typedef struct SpecInfo
{
	std::string type;
	std::string valu;
	std::string desc;
	std::string STD;
	std::string checktime;
	std::string freq;
	std::string status;
}eSpecInfo;

//CPU��Ϣ
typedef struct
{
	DWORD   dwUnknown1;
	ULONG   uKeMaximumIncrement;
	ULONG   uPageSize;
	ULONG   uMmNumberOfPhysicalPages;
	ULONG   uMmLowestPhysicalPage;
	ULONG   uMmHighestPhysicalPage;
	ULONG   uAllocationGranularity;
	PVOID   pLowestUserAddress;
	PVOID   pMmHighestUserAddress;
	ULONG   uKeActiveProcessors;
	BYTE    bKeNumberProcessors;
	BYTE    bUnknown2;
	WORD    wUnknown3;
} SYSTEM_BASIC_INFORMATION;
typedef struct
{
	LARGE_INTEGER   liIdleTime;
	DWORD           dwSpare[76];
} SYSTEM_PERFORMANCE_INFORMATION;
typedef struct
{
	LARGE_INTEGER liKeBootTime;
	LARGE_INTEGER liKeSystemTime;
	LARGE_INTEGER liExpTimeZoneBias;
	ULONG         uCurrentTimeZoneId;
	DWORD         dwReserved;
} SYSTEM_TIME_INFORMATION;

typedef struct task
{
//	XMLTask* pTask;

	eDVBType dvbtype;
	eTaskStatus status;

	std::string msgid;
	std::string setdatetime;	//�����·�����
	int deviceid;
	time_t startdatetime;		//��ʼʱ������
	time_t enddatetime;			//��ʼʱ������
	long cycleinterval;			//���ڼ��

	std::string taskid;
	std::string taskname;
	std::string freq;
	std::string channelid;
	std::string priority;
	std::string runpriority;
	std::string ServiceID;
	std::string VideoPID;
	std::string AudioPID;
} sTaskInfo;

typedef struct qualityCompenstaion
{
	eDVBType dvbtype;
	std::string deviceid;
	std::string type;
	std::string valu;
	std::string freq;
	std::string level;
} sQualityCompensation;
//������Ϣ
struct EnvInfo
{
	std::string Type;
	std::string Desc;
	EnvInfo()
	{

	};
	bool operator ==(const EnvInfo& other) const
	{
		return Type==other.Type&&Desc==other.Desc;
	};
	bool operator >(const EnvInfo& other) const
	{
		return Type > other.Type;
	};
	friend  bool operator > (const EnvInfo& other1,const EnvInfo& other2)
	{
		return other1.Type>other2.Type;
	};
	friend  bool operator < (const EnvInfo& other1,const EnvInfo& other2)
	{ 
		return other1.Type<other2.Type;
	};
	bool operator ()(const EnvInfo& other) const
	{
		return true;
	};
};
typedef std::map<EnvInfo,std::string>  EnvMapInfo;


struct sFreqScanInfo{
	std::string CenterFreq;
	std::string SymbolRate;
	std::string QAM;
};

struct OSDInfo{
	std::string FontSize;
	std::string Info;
	std::string InfoX;
	std::string InfoY;
	std::string TimeType;
	std::string TimeX;
	std::string TimeY;
	std::string ProgramX;
	std::string ProgramY;
};

struct SpectrumInfo
{
	float freq;
	float level;
};
struct QualityCard
{
	std::string IP;
	std::string Port;
};

//wz_110309
//�����ļ�videoprotocol�ڵ���Ϣ�ṹ
typedef struct _sVideoProtocol
{
	std::string streamprotocol;
	std::string fileprotocol;
	std::string fileurltype;
	std::string	fileoffset;
} sVideoProcotol, *psVideoProtocol;

typedef struct _sEncryptInfo
{
	std::string freq;
	std::string serviceid;
	std::string programid;
	std::string	Encrypt;
} sEncryptInfo, *psEncryptInfo;

//wz_110309
//recorddown�ڵ���Ϣ�ṹ
typedef struct _sRecordDownInfo
{
	std::string protocol;
	std::string urltype;
	std::string offset;
} sRecordDownInfo, *pRecordDownInfo;

//�����ϱ��ṹ
struct UpAlarmInfo
{ 
	std::string type;		//0����ѯ��1�������ϱ�
	std::string interval;	//�ϱ�ʱ���� ��λ����
	std::string onceday;    //1:���챨��;0 �������챨��
	std::string alarmurl;	//�����ϱ���ַ
	std::string runplanrecord; // ����ͼ�ڼ��Ƿ񱣴�¼�� 0������ 1 ������
};
//¼���ļ������ṹ
typedef struct _sRecordParamInfo
{ 
	std::string recordexpire;		//¼���������
	std::string alarmrecordexpire;	//��̬¼���������
	std::string alarmheadoffset;	//��̬¼��ͷƫ�Ƴ���
	std::string alarmtailoffset;	//��̬¼��βƫ�Ƴ���
	std::string alarmrecordmode;	//��̬¼��洢ģʽ
} sRecordParamInfo, psRecordParamInfo;

//��ʱ������Ͷ���  week,day,single;
typedef enum eCheckType              
{
	PERWEEK				= 0,		//ÿ����
	PERDAY				= 1,		//ÿ��
	PERSINGLE			= 2,		//����
	NOTHING             = 3         //��������
}enumCheckType;

typedef struct SignalCheck
{
	enumDVBType  dvbtype;
	std::string DeviceID;
	std::string Freq;
	std::string ChannelID;
	int  level;
	SignalCheck ()
	{
		level=100;
	}
	SignalCheck& operator = (const SignalCheck other)
	{
		dvbtype=other.dvbtype;
		DeviceID=other.DeviceID;
		Freq=other.Freq;
		ChannelID=other.ChannelID;
		level = other.level;
		return *this;
	}
	bool operator ==(const SignalCheck& other)
	{
		return dvbtype==other.dvbtype&&DeviceID==other.DeviceID&&Freq==other.Freq&&ChannelID==other.ChannelID;
	}

} sSignalCheck;

//jsp2017.7.25
struct OSDFormat{
	std::string Freq;
	std::string ServiceID;
	std::string FontSize;
	std::string Position;
	std::string AntiColor;
	std::string Type;
	std::string Align;
};

typedef struct SystemConfigureParameter
{
	string SystemIp;
	string VideoHttpServerIp;
	string VideoHttpPort;
	string VideoHttpMaxnum;
	string VideoRtspServerIp;
	string VideoRtspPort;
	string VideoRtspMaxnum;
	string QualityServerIp;
	string QualityPort;
	string QualityMaxnum;
	string DeviceServerIp;
	string DevicePort;
	string DeviceMaxnum;
	string LogServerIp;
	string LogPort;
	string LogMaxnum;
	string XmlServerIp;
	string XmlPort;
	string HttpServerIp;
	string HttpPort;
	string FtpServerIp;
	string FtpPort;
	string RecordQualityIp;
	string RecordQualityPort;
	string RecordQualityMaxnum;
	string Record_FileLocation;
	string Record_FileSharePath;
	string Record_DeviceId;
	string Record_Period;
	string Record_MaxAvailableSize;
	string Record_DBCleanInterval;
	string Record_SystemCleanTime;
	string Record_StoreType;
	string Log_Path;
	string Log_Expire;
	string Log_Type;
	string Log_outputToFile;
	string LogPathType_Analyser;
	string LogPathType_Record;
	string LogPathType_Video;
	string LogPathType_Device;
	string LogPathType_Other;
	string LogPathType_Default;
	string XmlSendTime;
	string XmlOverTime;
	string TempFileLocation;
	string TempSharePath;
	string TempFileExpireTime;
	string DeviceSchedulerTask_Type;
	string DeviceSchedulerTask_WeekDay;
	string DeviceSchedulerTask_Date;
	string DeviceSchedulerTask_Time;
	string PsisiSchedulerTask_Type;
	string PsisiSchedulerTask_WeekDay;
	string PsisiSchedulerTask_Date;
	string PsisiSchedulerTask_Time;
}sSysConfigParam;

typedef struct DvbConfigureParameter
{
	string type;
	string CenterFreq;
	string Symbolrate;
	string Qam;
	string ChannelScanType;
	string Osd_fontsize;
	string Osd_infoosd;
	string Osd_infoosdx;
	string Osd_infoosdy;
	string Osd_timeosdtype;
	string Osd_timeosdx;
	string Osd_timeosdy;
	string Osd_programx;
	string Osd_programy;
	string TablePath;
	string SharePath;
	string AlarmType;
	string AlarmInterval;
	string OnceDay;
	string RunplanRecord;
	string AlarmServer;
	string StreamRoundUrl; //FRee_fanrong_20140103
	string LogPath_VIDEO;	//FRee_fanrong_20140106
	string LogPath_SI;		//FRee_fanrong_20140106
	string LogPath_SMS;		//FRee_fanrong_20140106
	string LogPath_CAS;		//FRee_fanrong_20140106

	string SmsDir;		//FRee_fanrong_20140108
	string CasDir;		//FRee_fanrong_20140108
	string SIDir;		//FRee_fanrong_20140108

	string VideoStreamProtocol;
	string VideoFileProtocol;
	string VideoFileUrlType;
	string VideoFileOffSet;
	string RecordParam_recordexpire;
	string RecordParam_alarmrecordexpire;
	string RecordParam_alarmheadoffset;
	string RecordParam_alarmtailoffset;
	string RecordParam_alarmrecordmode;
	string RecordDown_protocol;
	string RecordDown_urltype;
	string RecordDown_offset;
	string GeneralDestCode;
	string GeneralSrcCode;
	string RealTimeFromRecord;

	string HDRealStreamWidth;
	string HDRealStreamHeight;
	string HDRealStreamBps;
	string SDRealStreamWidth;
	string SDRealStreamHeight;
	string SDRealStreamBps;

	string HDRecordWidth;
	string HDRecordHeight;
	string HDRecordBps;
	string SDRecordWidth;
	string SDRecordHeight;
	string SDRecordBps;
	string AudioBps;

	string AlarmStorage_switch;		//��̬¼��洢����	wz
	string AlarmStorage_filenum;	//��̬¼��洢�ļ�����	wz
}sDvbConfigParam;
//ģ�����ز�������ƽ����
typedef struct
{
	eDVBType dvbtype;
	string freq;
	string downthreshod;
}ThresHold;

//�ֱ��ʣ�����
typedef struct
{
	string width;
	string height;
	string bps;
}VideoParam;

typedef struct  
{
	std::string _switch;
	std::string _filenum;
} sAlarmRecordStorageCfg;
typedef struct AlarmPriority
{
	eDVBType  dvbtype;
	string    type;
	string    priority;
}sAlarmPriority;
struct SeperateTypeID
{
	string   head;
	string   type;
};
typedef struct AlarmTypePriority
{
	string UnLock;
	string Static;
	string Black;
	string NoAudio;
	string NoVideo;
	string Slice;
}sAlarmTypePriority;


typedef struct AlarmSpecInfo
{
	bool bIsSpecing;				//�Ƿ�����Ƶ��ɨ��
	std::string strLastSpecTime;	//�ϴ�Ƶ��ɨ��Ľ���ʱ��
} sAlarmSpecInfo, *pAlarmSpecInfo;


//�������ֲ��ϱ��ṹ    //FRee_fanrong_20140103
struct UpStreamRoundInfo
{
	std::string type;		//0����ѯ��1�������ϱ�
	std::string streamroundurl;
};

//��������������¼��·����ѯʱʹ��
typedef struct _RecChanInfo
{
	std::string freq;
	std::string serviceId;
	std::string programId;
	bool canRecord;
}RecChanInfo, *pRecChanInfo;

typedef struct _RateParam
{
	std::string width;
	std::string height;
	std::string fps;
	std::string bps;

}RateParam,*pRateParam;

typedef struct _UdpSendTsAddr
{
	std::string destip;
	u_short port;
}UdpSendTsAddr;


typedef struct _AudioParam
{
	std::string videopid;
	std::string audiopid;
	std::string audioencode;
	std::string left;
	std::string center;
	std::string _right; 
	std::string leftsurround;
	std::string rightsurround;
	std::string subwoofer;
}AudioParam,*pAudioParam;

typedef struct _CACardInfo
{
	std::string cardno;
	std::string position;
	std::string desc;
	std::string ip;
}CACardInfo,*pCACardInfo;

typedef struct _MHPInfo
{
	std::string ftpurl;
	std::string username;
	std::string pass;
}MHPInfo,*pMHPInfo;

typedef struct _EquInfo
{
	std::string strSrcCode;
	std::string strDesCode;
	std::string strUrl;
}sEquInfo, *pEquInfo;


typedef struct _RecordSetInfo
{
	std::string strTaskXml;
	int nDeviceId;
	bool bSetAction;
} sRecordSetInfo, *pRecordSetInfo;

typedef struct _RecVideoTimeInfo
{
	std::string strProgramId;
	std::string strFreq;
	std::string strServiceId;
	std::string strStartTime;
	std::string strEndTime;
} sRecVideoTimeInfo, *pRecVideoTimeInfo;
typedef std::vector<sRecVideoTimeInfo> VIDEOTIMEVEC;

typedef struct _AgentSetInfo
{
	std::string s_strType;
	std::string s_strDesc;
	std::string s_strValue;
} sAgentSetInfo, *pAgentSetInfo;

typedef struct _RecordUpLoadInfo
{
	std::string s_FileName;//�ϴ��ļ���
	std::string s_FileSize;//�ϴ��ļ���С
	std::string s_SubStartTime;//��ʼʱ��
	std::string s_SubEndTime;//����ʱ��
	std::string s_LoadTime;//�ϴ�ʱ��

}sRecordUpLoadInfo;

typedef struct
{
	std::string s_taskid;  
	std::string s_uploadsucenddtime; 
	std::string s_uploadsucfilename;
	std::string s_Freq;
	std::string s_ServiceID;
	std::string s_Active;
	std::string s_UserName; 
	std::string s_PassWord; 
	std::string s_Address;
	std::string s_Length;
	std::string s_StartDateTime;
	std::string s_EndDateTime;
	std::string s_strChassisID; 
	std::string s_strPosition; 
	std::string s_strModuleID; 
	std::string s_strTranscode;
	std::string s_strChanId; 
	std::string s_TaskInfo; 
	std::string s_devcodehead;
	std::string s_Folder;
	std::string s_dstURL;
	std::string s_type;
}RecordUpload;
#endif