#pragma once

#ifndef  _CARDTYPE_H_
#define _CARDTYPE_H_

#include <time.h>
#include <iostream>
//Э��ͷ,�����ֽ�
#define PROTOHEAD0 0xFF
#define PROTOHEAD1 0xFE
//�����룬һ���ֽ�
#define SETDSTNET 0x10
#define GETDSTNET 0x11

#define SETSRCNET 0x12 
#define GETSRCNET 0x13

#define SETDSTPID 0x20
#define GETDSTPID 0x21
 
#define SETSRCPID 0x22
#define GETSRCPID 0x23
 
#define SETCHNCON 0x30
#define GETCHNCON 0x31
 
#define SETGLOBALCON 0x40
#define GETGLOBALCON 0x41

//ʱ������ģʽ
#define OSD_CYEAR       0x00100000
#define OSD_YEAR4       0x00020000
#define OSD_YEAR2       0x00010000

#define OSD_EMONTH      0x00002000
#define OSD_MONTH       0x00001000

#define OSD_EWEEK       0x00000200
#define OSD_CWEEK       0x00000100

#define OSD_CHOUR       0x00000010
#define OSD_HOUR24      0x00000002
#define OSD_HOUR12      0x00000001

//��������
#define V_BLACK     0x1          //��Ƶ�ڳ�
#define C_SLICE     0x2          //����
#define V_STATIC	0x4          //���澲ֹ
#define V_MISS      0x8          //����Ƶ�ź�
#define S_UNLOCK	0xE          //���ز�
#define A_SILENCE	0x10         //��Ƶ����

#define NO_SIGNAL   0x81         //���ź�
#define NO_VIDEO    0x82         //����Ƶ
#define NO_AUDIO    0x84         //����Ƶ
#define UNCA_VIDEO  0x88         //��ƵCAʧ��
#define UNCA_AUDIO  0x90         //��ƵCAʧ��


#define V_BLACK_V_STATIC     0x5          //��Ƶ�ڳ�+���澲ֹ
#define C_SLICE_V_STATIC     0x6          //����+���澲ֹ
#define CARDREBOOT			 0xFFFF		  //�忨����


//��Ϣ���Ͷ���
enum MSG_TYPE
{
	MSG_UNKNOWN					= 0x0,				//δ֪��Ϣ
	
	MSG_SET_REBOOT				= 0x100,			//����
	MSG_RET_REBOOT				= 0x200,

	MSG_SET_INIT				= 0x101,			//��ʼ��
	MSG_RET_INIT				= 0x201,
	
	MSG_SET_START				= 0x102,			//��ʼת��
	MSG_RET_START				= 0x202,
	
	MSG_SET_STOP				= 0x103,			//ֹͣת��
	MSG_RET_STOP				= 0x203,
	
	MSG_SET_VIDEOBITRATE		= 0x104,			//��Ƶ����
	MSG_RET_VIDEOBITRATE		= 0x204,
	
	MSG_SET_AUDIOBITRATE		= 0x105,			//��Ƶ����
	MSG_RET_AUDIOBITRATE		= 0x205,
	
	MSG_SET_OSD0	        	= 0x106,			//0ͨ��OSD
	MSG_RET_OSD0				= 0x206,
	
	MSG_SET_OSD1	        	= 0x107,			//1ͨ��OSD
	MSG_RET_OSD1				= 0x207,
	
	MSG_SET_OSD2	       		= 0x108,			//2ͨ��OSD
	MSG_RET_OSD2				= 0x208,
	
	MSG_SET_OSD3	        	= 0x109,			//3ͨ��OSD
	MSG_RET_OSD3				= 0x209,
	
	MSG_SET_TIME				= 0x110,			//ϵͳʱ��
	MSG_RET_TIME				= 0x210,
	
	MSG_SET_DSTINFO        		= 0x111,			//��λ����Ϣ
	MSG_RET_DSTINFO				= 0x211,

	MSG_SET_VIDEOTUNER			= 0x112,			//��Ƶ�����ӣ���Ƶ
	MSG_RET_VIDEOTUNER			= 0x212,
	
	MSG_SET_FMTUNER				= 0x113,			//��Ƶ��FM����Ƶ
	MSG_RET_FMTUNER				= 0x213,

	MSG_SET_AMTUNER				= 0x120,			//��Ƶ��AM����Ƶ
	MSG_RET_AMTUNER				= 0x220,
	
	MSG_SET_ALERTVIDEO			= 0x114,			//��Ƶ��������
	MSG_RET_ALERTVIDEO			= 0x214,
	
	MSG_SET_ALERTAUDIO     		= 0x115,			//��Ƶ��������
	MSG_RET_ALERTAUDIO			= 0x215,
	
	MSG_GET_RUNSTATUS			= 0x116,			//����״̬
	MSG_RET_RUNSTATUS			= 0x216,
	
	MSG_GET_STDCHANSCAN			= 0x117,			//��׼Ƶ��ɨ��
	MSG_RET_STDCHANSCAN			= 0x217,
	
	MSG_GET_CHANSCAN			= 0x118,			//ȫƵ��ɨ��
	MSG_RET_CHANSCAN			= 0x218,

	MSG_GET_CHANFIX				= 0x119,			//��ȡ�Ƿ�����
	MSG_RET_CHANFIX				= 0x219,
	
	MSG_RET_ALERT				= 0x301,			//��̬�ϱ�

	MSG_I2C_OPEN				= 0x401,			//��I2C�豸
	MSG_RET_I2C_OPEN			= 0x501,
	
	MSG_I2C_CLOSE				= 0x402,			//�ر�I2C�豸
	MSG_RET_I2C_CLOSE			= 0x502,
	
	MSG_I2C_SWITCH				= 0x403,			//ѡ��...
	MSG_RET_I2C_SWITCH			= 0x503,
	
	MSG_I2C_WRITE				= 0x404,			//дI2C�豸
	MSG_RET_I2C_WRITE			= 0x504,
	
	MSG_I2C_READ				= 0x405,			//��I2C�豸
	MSG_RET_I2C_READ			= 0x505,

	//DVB
	MSG_SET_DIGITALINIT			= 0x150,			//���ֿ���ʼ��
	MSG_RET_DIGITALINIT			= 0x250,

	MSG_SET_TUNERPID			= 0x151,			//��Ƶ��PID
	MSG_RET_TUNERPID			= 0x251,

	MSG_SET_CODECPID			= 0x152,			//�����TS���е�PID
	MSG_RET_CODECPID			= 0x252,

	MSG_SET_DIGITALTUNER		= 0x153,			//���ֿ���Ƶ
	MSG_RET_DIGITALTUNER		= 0x253,

	MSG_SET_HDSD				= 0x154,			//�����Ŀ��ʶ
	MSG_RET_HDSD				= 0x254,

	//ָ�����
	MSG_GET_QUA					=0x415,
	MSG_RET_QUA					=0x515,

	MSG_GET_RADIO				=0x416,
	MSG_RET_RADIO				=0x516,

	MSG_GET_SPECIAL_AUDIO_CHANSCAN		=0x417,
	MSG_RET_SPECIAL_AUDIO_CHANSCAN		=0x517,
};

//��������ʶ����
enum MSG_RETURN
{      
	RET_OK		=	0x1,       //��ʾ������Ϣ���óɹ�
	RET_FAIL 	=	0x0,       //��ʾ������Ϣ����ʧ��
};

//ǰ���豸��Ϣ�ṹ����
typedef struct _tagDstInfo
{
	char dstIP[30];
	int  dstport;
	int  alarmport;
}DstInfo_Obj, *DstInfo_Handle;

//�忨��ʼ����Ϣ�ṹ����
typedef struct _tagInitBoard
{
	DstInfo_Obj	dstinfo;		//Ŀ���ַ��Ϣ
	int video_bitrate;			//��Ƶת��Ĭ������
	int audio_idx;				//��Ƶת��Ĭ��������ţ�
	int videosize;				//��ʼ�����С  videosize=1 ΪCIF��videosize=2ΪQCIF
	time_t systime;				//ϵͳʱ��

	_tagInitBoard()
	{
		video_bitrate	= 700;
		audio_idx		= 5;
		videosize		= 1;
		systime			= time(0);
	};
}InitBoard_Obj, *InitBoard_Handle;

//��Ƶ��Ϣ�ṹ����
typedef struct _tagTunerConfig
{
	int chan;  //ͨ����
	int freq;   //Ƶ�� 
}TunerConfig_Obj, *TunerConfig_Handle;

//������Ϣ�ṹ����
typedef struct _tagMediaConfig
{
	int chan;					//ͨ��
	int video_bitrate;			//��Ƶ���ʣ��Թ㲥ת����Ч��
	int audio_idx;				//��Ƶ��������

	_tagMediaConfig()
	{
		chan = 0;
		video_bitrate = 400;
		audio_idx = 5;
	};
}MediaConfig_Obj, *MediaConfig_Handle;

//OSD��Ϣ�ṹ����
typedef struct _tagSubtitleConfig
{
	int size;				// �����С��Ĭ��ʹ��16�ţ��Ƽ�ʹ��16����

	int subtitle0_Enable;  //�Ƿ���ʾ
	char subtitle0[40];    //��ʾ����Ļ����
	int subtitle0_dislen;   //ÿ����ʾ�ĳ��ȣ��ֽ�Ϊ��λ��ÿ������2���ֽ�
	int subtitle0_x;       //��ʾλ��
	int subtitle0_y;

	int subtitle1_Enable;
	char subtitle1[40];
	int subtitle1_dislen;
	int subtitle1_x;
	int subtitle1_y;

	int time_Enable;     //�Ƿ���ʾʱ��
	int mode;           //ʱ����ʾģʽ
	int time_x;          //ʱ����ʾλ��
	int time_y;

	_tagSubtitleConfig()
	{
		size				= 16;
		subtitle0_Enable	= 1;		
		subtitle0_dislen	= 20;
		subtitle0_x			= 10;
		subtitle0_y			= 10;

		subtitle1_Enable	= 1;		
		subtitle1_dislen	= 20;
		subtitle1_x			= 250;
		subtitle1_y			= 260;

		time_Enable			= 1;
		mode				= OSD_CYEAR | OSD_YEAR4 |  OSD_MONTH | OSD_HOUR24;
		time_x				= 160;
		time_y				= 10;
		memset(subtitle0,0,40);
		memset(subtitle1,0,40);
	};
}SubtitleConfig_Obj, *SubtitleConfig_Handle;

//Ƶ��ɨ��ṹ����
typedef struct _tagChannelScanConfig
{
	int chan;  			//ͨ����
	int Startfreq;   	//��ʼƵ��
	int Endfreq;		//����Ƶ��
	int step;			//Ƶ���ۼӲ��� ����׼Ƶ��ɨ��ʱ�����壩
}ChannelScan_Obj, *ChannelScan_Handle;

//����������Ϣ�ṹ����
typedef struct _tagAlertConfig
{
	int video_black;		//�ڳ�
	int color_slice;		//����
	int video_static;		//��֡
	int video_miss;			//��Ƶ��ʧ
	int audio_silence;		//����
}AlertConfig_Obj, *AlertConfig_Handle;

//����ͷ�ṹ����
typedef struct _tagPkgHeader
{
	char header;
	int  msg_type;
}PkgHeader_Obj, *PkgHeader_Handle;

//�������ṹ����
typedef struct _tagRetMessage
{
	PkgHeader_Obj ph;
	int	status;
}
RetMessage_Obj, *RetMessage_Handle;

// �����ǵ��ӹ㲥ָ������ƿ�ͨѶ�Ľṹ����

//�忨�����ض�Ƶ��ĵ�ƽֵ�ṹ
typedef struct _tagRadioInfo
{
	char flag;      // ����ָ�����Ƶ�� 01 Ƶ�� 02 ָ�� 
	int freq;		//Ƶ��
	int gain;       //����
}Radiopayload_Obj, * Radiopayload_Handle;

typedef struct _tagSpecialRadioInfo  //���������Ŀʵ�ֵ���ʱ�������
{
	int startfreq;  //��ʼƵ��
	int endfreq;    //����Ƶ��
	int step;       //����
	int chan;    //ͨ����
}SpecialRadiopayload_Obj,* SpecialRadiopayload_Handle;

typedef struct _tagSpecialRadioRetMessage  
{
	PkgHeader_Obj ph;
	int	status;
	int len;              //value�ĳ��ȣ�
	int value[240];       //�����Ƶ��ɨ�践�ص�����Ϊ������Ƶ�㣬�����Ƶ��ɨ�践�ص�Ϊ��ӦƵ��ĵ�ƽֵ
}SpecialRadioRetMessage_Obj,* SpecialRadioRetMessage_Handle;

typedef struct _tagTVQuaRetMessage
{
	PkgHeader_Obj ph;
	int	status;
	int freq;
	int level;
}TVQuaRetMessage_Obj, * TVQuaRetMessage_Handle;

typedef struct _tagRadioQuaRetMessage
{
	PkgHeader_Obj ph;
	int status;			//����ֵ״̬ 0ʧ�� 1��ȷ
	int freq;			//Ƶ��
	int level_int;		//�źŵ�ƽ
	int dev_int;		//���ƶ�
	int dF;				//Ƶƫ
	time_t getTime; 
}RadioQuaRetMessage_Obj, *RadioQuaRetMessage_Handle;

typedef struct _tagRadioSpecRetMessage
{
	PkgHeader_Obj ph;
	int status;
	int freq;
	int level_int;
	int gain;
	char spec[1024];	//16��Ϊһ��Ƶ��ģ���64��Ƶ��	
}RadioSpecRetMessage_Obj, *RadioSpecRetMessage_Handle;

typedef struct _tagChannelScanRetMessage
{
	PkgHeader_Obj ph;
	int status;
	char freq[110];
}ChannelScanRetMessage_Obj,*ChannelScanRetMessage_Handle;
typedef struct _tagChannelScanAudio
{
	int startfreq;
	int endfreq;
}ChanelScanAudio_Obj,*ChannelScanAudio_Handle;

typedef struct _tagChannelScanAudioRetMessage
{
	PkgHeader_Obj ph;
	int status;
	int freqnum;
	int freqs[100];
}ChannelScanAudioRetMessage_Obj,*ChannelsScanAudioRetMessage_Handle;

typedef struct 
{
	int Freq;
	float ImageLevel;  //ͼ���ƽ,��λ:dbuv
	float AudioLevel;  //������ƽ,��λ:dbuv
	float I2AOffLevel; //ͼ���ز�������ز��ĵ�ƽ��,��λ:dbuv
	float CN;          //�����,��λ:dbuv
	float FreqOffSet;  //��ƵƵƫ,��λ:kHz
	float Slope;       //б��
	time_t getTime; 
}TVQuality;

typedef struct  
{
	std::string Type;
	std::string Desc;
	std::string Value;
}QualityDesc;


typedef struct TypeDesc
{
	std::string Type;
	std::string Desc;
}sTypeDesc;

//�Ļ���ʹ��
typedef struct _tagChanNetCfg
{
	unsigned int u32ChanID; //(in)��ʶͨ����
    char u8ChanIp[16]; //(in/out)ts���鲥IP
    unsigned int u32ChanPort; //(in/out)ts���鲥�˿�
} ChanNetCfgObj, *ChanNetCfgHandle;

//PID��Ϣ�ṹ��

typedef struct _tagChanPIDCfg
{
	unsigned int u32PreChanID;//(in)��ʾ��ԭ���忨�ϵ�ͨ���ţ����ڵ�һ�׶Σ�
						//Ϊ�˺͵ڶ��׶μ��ݽ����� 0x10��ʶͨ��0��0x11��ʶͨ��1��0x12��ʶͨ��2��0x13��ʶͨ��3.
    unsigned int u32ChanID;//(in)��ʶͨ����
    unsigned int u32PmtPid;//(in/out)��ʶPMT PID
    unsigned int u32PcrPid;//(in/out)��ʶ PCR PID
    unsigned int u32VideoPid; //(in/out)��ʶVIDEO PID
    unsigned int u32AudioPid; //(in/out)��ʶAUDIO PID
} ChanPIDObj, *ChanPIDHandle;
#endif