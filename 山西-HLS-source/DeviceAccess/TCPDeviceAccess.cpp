#include "TCPDeviceAccess.h"
#include "../Foundation/XmlParser.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/TypeDef.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include <vector>
#include <fstream>
#include <map>
#include <algorithm>
#include <math.h>

TCPDeviceAccess::TCPDeviceAccess(void)
{
}

TCPDeviceAccess::~TCPDeviceAccess(void)
{
	stream.close_reader();
	stream.close_writer();
	stream.dump();
	stream.close();
}
TCPDeviceAccess::TCPDeviceAccess(int deviceid,std::string strIP,int nPort):DeviceAccess(deviceid,strIP,nPort),server(nPort,strIP.c_str())
{
	mChannelID=deviceid;
	strIPAddress = strIP;

	m_bIsFreqLock = false;
	m_tChanFixKeepTime = time(0)-6;//��һ�μ��
}
bool TCPDeviceAccess::SendTaskMsg(const std::string& strCmdMsg,std::string& strRetMsg)
{
	//��Ӿ���ʵ��
	ACE_Guard<ACE_Thread_Mutex> guard(sendTaskMsgMutex);
	XmlParser parser;
	parser.Set_xml(strCmdMsg);
	pXMLNODE root=parser.GetRootNode();
	string dvbtype;
	parser.GetAttrNode(root,"DVBType",dvbtype);
	pXMLNODE childnode=parser.GetNodeFirstChild(root);
	string nodename=parser.GetNodeName(childnode);
	bool Ret=true;
	if ("QualityQuery"==nodename)
	{
		Ret=GetQualityRetXML(strCmdMsg,strRetMsg);
	}
	else if("SpectrumQuery"==nodename)
	{
		Ret=GetSpectrumRetXML(strCmdMsg,strRetMsg);
	}
	else if ("ChannelScanQuery"==nodename)
	{
		Ret=GetChannelScanRetXML(strCmdMsg,strRetMsg);
	}
	else if ("TSQuery"==nodename)
	{
		Ret=GetTsQueryRetXML(strCmdMsg,strRetMsg);
	}
	return Ret;
}
bool TCPDeviceAccess::SendMultiTask(std::string strtsIp,unsigned int tsport,int cardindex,int MultiIndex)
{
	unsigned char buf[1024] = {0};
	ChanNetCfgObj netparam;
	netparam.u32ChanID = 0x0+MultiIndex;
	netparam.u32ChanPort = tsport;
	strcpy(netparam.u8ChanIp, strtsIp.c_str());
	//��Ŀ���pid����ô��·���Ƶ�������л�ȡ
	ChanPIDObj chanpid;
	chanpid.u32PreChanID = 0;//0x10+cardindex;//0 ͨ��
	chanpid.u32ChanID =MultiIndex; //0x10+MultiIndex;
	chanpid.u32PmtPid = 100;
	chanpid.u32PcrPid = 101;
	chanpid.u32VideoPid = 102;
	chanpid.u32AudioPid = 103;
	//
	unsigned int datalen = 0;
	char retinfo[1025];
	datalen = SetMultiScrNetParam(buf, netparam, SETSRCNET);
	cout<<"datalen:"<<datalen<<"-netparam.u32ChanID:"<<netparam.u32ChanID<<"-netparam.u32ChanPort:"<<netparam.u32ChanPort<<"-netparam.u8ChanIp:"<<netparam.u8ChanIp<<endl;
	memset(retinfo,0,sizeof(char)*1025);
	bool rtn = SendCmdToServer((char*)buf, datalen,(void*)retinfo,1024);
	memset(buf, 0, sizeof(buf));
	datalen = SetMultiScrPIDParam(buf, chanpid, SETSRCPID);
	memset(retinfo,0,sizeof(char)*1025);
	rtn = SendCmdToServer((char*)buf, datalen,(void*)retinfo,1024);
	//
	return true;
}
bool TCPDeviceAccess::CheckFreqLock()
{
	int chan = -1;
	PROPMANAGER::instance()->GetDeviceTunerID(DeviceId,chan);	//��ø�ͨ����Ӧ��Tuner
	
	if((time(0)-m_tChanFixKeepTime)>3) //Ĭ��3���ڲ��ظ����
	{
		m_bIsFreqLock = SendCmdToServer(MSG_GET_CHANFIX,(void*)&chan,sizeof(time_t));
		m_tChanFixKeepTime = time(0);
	}
	bool rtn = m_bIsFreqLock;
		
	if (rtn == false)
	{
		string msg = string("ͨ��[") + StrUtil::Int2Str(mChannelID) + std::string("]����ʧ��"); 
		//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);
	}
	else
	{
		string msg = string("ͨ��[") + StrUtil::Int2Str(mChannelID) + std::string("]�����ɹ�"); 
		//SYSMSGSENDER::instance()->SendMsg(msg);
	}

	return rtn;
}

bool TCPDeviceAccess::ConnectToServer()
{
	//��������
	connector.dump();
	if (connector.connect (stream, server) == -1)//����Ӳ��������
	{
		string msg = string("ͨ��[") + StrUtil::Int2Str(mChannelID) + string("]����ʧ��");
		//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);

		stream.close_reader();
		stream.close_writer();
		stream.dump();
		stream.close();
		OSFunction::Sleep(0,500);
		return false;
	}
//	string msg = string("ͨ��[") + StrUtil::Int2Str(mChannelID) + string("]���ӳɹ�");
//	SYSMSGSENDER::instance()->SendMsg(msg);

	return true;
}
/***************************************/
//����ָ��ж�ָ����
/***************************************/
bool TCPDeviceAccess::SendCmdToServer(MSG_TYPE msg_type,void* info,int infolen)
{
	int pkgLen = 0;
	unsigned char sendbuf[200] = {0}; 
	unsigned char RetMsg[200] = {0};
	//��Ϣͷ
	PkgHeader_Obj   ph;
	ph.header   = 0x48;
	ph.msg_type = msg_type;

	memcpy(sendbuf, &ph, sizeof(PkgHeader_Obj));
	pkgLen	+= sizeof(PkgHeader_Obj);

	//��Ϣ����
	memcpy(sendbuf+pkgLen, info, infolen);
	pkgLen	+= infolen;

	bool ret = false;
	ACE_Time_Value timeout(10);

	const int SendNum = 3;
	for (int i=0;i<SendNum;++i)
	{
		stream.close_reader();
		stream.close_writer();
		stream.dump();
		stream.close();
		if ( ConnectToServer() == false)
			continue;

		//������Ϣ
		if(stream.send((char*)sendbuf, 200,&timeout) <= 0)
		{
			string msg = string("ͨ��[") + strIPAddress +string(":") + StrUtil::Int2Str(mChannelID) + string("]����ָ��ʧ��");
			//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);
			stream.close_reader();
			stream.close_writer();
			stream.dump();
			stream.close();
			continue;
		}
		//���շ�������
		if(stream.recv(( char *)&RetMsg,sizeof(RetMessage_Obj),&timeout) <= 0)
		{
			string msg = string("ͨ��[") + strIPAddress +string(":") + StrUtil::Int2Str(mChannelID) + string("]���ݽ���ʧ��");
			//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);
			stream.close_reader();
			stream.close_writer();
			stream.dump();
			stream.close();
			continue;
		}

		RetMessage_Handle rm = (RetMessage_Handle)(RetMsg);
		if(rm->ph.header!=0x49 || rm->ph.msg_type!=(msg_type+0x100) || rm->status != 1)
		{
			string msg = string("ͨ��[") + strIPAddress +string(":") + StrUtil::Int2Str(mChannelID) + string("]�������ݴ���");
			//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);
			stream.close_reader();
			stream.close_writer();
			stream.dump();
			stream.close();
			continue;
		}
		ret = true;	
		break;
	}
	stream.close();
	return ret;
}

bool TCPDeviceAccess::SendCmdToServer(void* indata,int indatalen,void* outdata,int outdatalen)
{
	bool ret = false;
	ACE_Time_Value timeout(15);

	const int SendNum = 3;
	int i=0;
	for (i=0;i<SendNum;++i)
	{
		stream.close();
		if ( ConnectToServer() == false)
			continue;

		//������Ϣ
		if(stream.send((char*)indata, indatalen,&timeout) <= 0)
		{
			ACE_DEBUG((LM_DEBUG,"(%T| %t) ���Ͱ忨ָ��ʧ��\n"));
			continue;
		}
		//���շ�������
		int revlen=stream.recv(( char *)outdata,outdatalen,&timeout);
		if(revlen <= 0)
		{
			ACE_DEBUG((LM_DEBUG,"(%T| %t) ���հ忨ָ���ʧ��:%s\n",indata));			
			continue;
		}
		break;
	}
	if(i<3)
	{
		ret=true;
	}
	stream.close();
	return ret;
}

int TCPDeviceAccess::SendCmdToTVCom(MSG_TYPE msg_type,void* info,int infolen)
{
	if ( ConnectToServer() == false)
		return -1;

	int pkgLen = 0;
	unsigned char sendbuf[200] = {0}; 
	unsigned char RetMsg[200] = {0};
	//��Ϣͷ
	PkgHeader_Obj   ph;

	ph.header   = 0x51;
	ph.msg_type = msg_type;

	memcpy(sendbuf, &ph, sizeof(PkgHeader_Obj));
	pkgLen	+= sizeof(PkgHeader_Obj);

	//��Ϣ����
	memcpy(sendbuf+pkgLen, info, infolen);
	pkgLen	+= infolen;

	//������Ϣ
	ACE_Time_Value timeout(10);
	if(stream.send((char*)sendbuf, pkgLen,&timeout) <= 0)		//int lenth = send(m_sSocket,(char*)sendbuf, len, 0)
	{
		string msg = string("ͨ��[") + strIPAddress +string(":") + StrUtil::Int2Str(mChannelID) + string("]����ָ��ʧ��");
		//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);
		stream.close_reader();
		stream.close_writer();
		stream.dump();
		stream.close();
		return -2;
	}
	//���շ�������
	int lenth=0;
	if((lenth=stream.recv(( char *)&RetMsg,sizeof(TVQuaRetMessage_Obj),&timeout)) <= 0)
	{
		string msg = string("ͨ��[") + strIPAddress +string(":") + StrUtil::Int2Str(mChannelID) + string("]���ݽ���ʧ��");
		//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);
		stream.close_reader();
		stream.close_writer();
		stream.dump();
		stream.close();
		return -3;
	}

	TVQuaRetMessage_Handle rm = (TVQuaRetMessage_Handle)(RetMsg);
	
	stream.close_reader();
	stream.close_writer();
	stream.dump();
	stream.close();
	if(rm->ph.header!=0x52 || rm->ph.msg_type!=(msg_type+0x100) || rm->status != 1)
	{
		string msg = string("ͨ��[")  + strIPAddress +string(":") + StrUtil::Int2Str(mChannelID) + string("]�������ݴ���");
		//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);
		return -4;
	}
	int level=rm->level;
	return level;
}



int TCPDeviceAccess::GetAudioIndex(int audiorate)
{
	int index = 3;//Ĭ��Ϊ3
	if (audiorate<=32)
		index = 1;
	else if (audiorate>32 && audiorate<=40)
		index =2;
	else if (audiorate>40 && audiorate<=48)
		index =3;
	else if (audiorate>48 && audiorate<=56)
		index =4;
	else if (audiorate>56 && audiorate<=64)
		index =5;
	else if (audiorate>64 && audiorate<=80)
		index =6;
	else if (audiorate>80 && audiorate<=96)
		index =7;
	else if (audiorate>96 && audiorate<=112)
		index =8;
	else if (audiorate>112 && audiorate<=128)
		index =9;
	else if (audiorate>128 && audiorate<=160)
		index =3;
	else if (audiorate>160 && audiorate<=192)
		index =3;
	else if (audiorate>192 && audiorate<=224)
		index =3;
	else if (audiorate>224 && audiorate<=256)
		index =3;
	else if (audiorate>256 && audiorate<=320)
		index =3;
	else
		index =3;
	return index;
}

bool TCPDeviceAccess::InitCard()
{
	//��ʼ����Ϣ�ṹ
	InitBoard_Obj ib;
	//����IP��UPD���͵ĵ�ַ��Ҫ�������ļ�ȡ
	strcpy(ib.dstinfo.dstIP,"172.16.10.56");
	//�忨�������ϱ����ݵĶ˿ڣ�����Ƶ���ݶ˿�,ÿ����Ӧһ·ʵʱ����Ƶ����
	ib.dstinfo.dstport = 1234;		//PropManager::GetInstance()->GetDevCarddspPort(mChannelID+2);//1234;//1234,1235,1236,1237
	ib.dstinfo.alarmport = 8000;	//PropManager::GetInstance()->GetDevCardalarmPort(mChannelID+2);//8000;//�������յ�UDP��ַ ÿһ·��������ϱ���
	ib.video_bitrate = 700;
	ib.audio_idx =5;				// GetAudioIndex(48);//Ĭ��Ϊ48K
	ib.systime = time(0)+8*3600;		

	bool rtn = SendCmdToServer(MSG_SET_INIT,(void*)&ib,sizeof(InitBoard_Obj));
	
	if (rtn==false)
	{
		string msg = string("ͨ��[")  + strIPAddress +string(":") + StrUtil::Int2Str(mChannelID) + string("]��ʼ��ʧ��");
		//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);
	}
	else
	{
		string msg = string("ͨ��[")  + strIPAddress +string(":") + StrUtil::Int2Str(mChannelID) + string("]��ʼ���ɹ�");
		//SYSMSGSENDER::instance()->SendMsg(msg);
	}
	return rtn;
}


bool TCPDeviceAccess::SetVideoAlertInfo( AlertConfig_Obj& alertinfo )
{
	bool rtn=SendCmdToServer(MSG_SET_ALERTVIDEO,(void*)&alertinfo,sizeof(AlertConfig_Obj));

	if (rtn==false)
	{
		string msg = string("ͨ��[") + strIPAddress +string(":") + StrUtil::Int2Str(mChannelID) + string("]������Ƶ��������ʧ��");
		//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);
	}
	else
	{
		string msg = string("ͨ��[") + strIPAddress +string(":") + StrUtil::Int2Str(mChannelID) + string("]������Ƶ���������ɹ�");
		//SYSMSGSENDER::instance()->SendMsg(msg);
	}

	return rtn;
}

bool TCPDeviceAccess::SetAudioAlertInfo( AlertConfig_Obj& alertinfo )
{
	bool rtn=SendCmdToServer(MSG_SET_ALERTAUDIO,(void*)&alertinfo,sizeof(AlertConfig_Obj));

	if (rtn==false)
	{
		string msg = string("ͨ��[") + strIPAddress +string(":") + StrUtil::Int2Str(mChannelID) + string("]������Ƶ��������ʧ��");
		//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);
	}
	else
	{
		string msg = string("ͨ��[") + strIPAddress +string(":") + StrUtil::Int2Str(mChannelID) + string("]������Ƶ���������ɹ�");
		//SYSMSGSENDER::instance()->SendMsg(msg);
	}
	
	return rtn;
}
bool TCPDeviceAccess::StartTranscode()
{
	bool rtn = SendCmdToServer(MSG_SET_START,NULL,0);

	if (rtn==false)
	{
		string msg = string("ͨ��[") + strIPAddress +string(":") + StrUtil::Int2Str(mChannelID) + string("]����ת������ʧ��");
		//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);
	}
	else
	{
		string msg = string("ͨ��[") + strIPAddress +string(":") + StrUtil::Int2Str(mChannelID) + string("]����ת�������ɹ�");
		//SYSMSGSENDER::instance()->SendMsg(msg);
	}
	
	return rtn;
}
bool TCPDeviceAccess::StopTranscode()
{
	bool rtn = SendCmdToServer(MSG_SET_STOP,NULL,0);

	if (rtn==false)
	{
		string msg = string("ͨ��[") + strIPAddress +string(":") + StrUtil::Int2Str(mChannelID) + string("]����ת��ֹͣʧ��");
		//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);
	}
	else
	{
		string msg = string("ͨ��[") + strIPAddress +string(":") + StrUtil::Int2Str(mChannelID) + string("]����ת��ֹͣ�ɹ�");
		//SYSMSGSENDER::instance()->SendMsg(msg);
	}

	return rtn;
}

bool TCPDeviceAccess::SetOSD(int chan,const SubtitleConfig_Obj& osdinfo)
{
	bool rtn = false;
	switch(chan)
	{
	case 0:
		rtn = SendCmdToServer(MSG_SET_OSD0,(void*)&osdinfo,sizeof(SubtitleConfig_Obj));
		break;
	case 1:
		rtn = SendCmdToServer(MSG_SET_OSD1,(void*)&osdinfo,sizeof(SubtitleConfig_Obj));
		break;
	case 2:
		rtn = SendCmdToServer(MSG_SET_OSD2,(void*)&osdinfo,sizeof(SubtitleConfig_Obj));
		break;
	case 3:
		rtn = SendCmdToServer(MSG_SET_OSD3,(void*)&osdinfo,sizeof(SubtitleConfig_Obj));
		break;
	default:
		rtn = false;
		break;
	}

	if (rtn==false)
	{
		string msg = string("ͨ��[") + strIPAddress +string(":") + StrUtil::Int2Str(mChannelID) + string("]����OSDʧ��");
		//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);
	}
	else
	{
		string msg = string("ͨ��[") + strIPAddress +string(":") + StrUtil::Int2Str(mChannelID) + string("]����OSD�ɹ�");
		//SYSMSGSENDER::instance()->SendMsg(msg);
	}

	return rtn;
}

bool TCPDeviceAccess::SetSysTime()
{
	time_t now = time(0)+8*3600;
	bool rtn = SendCmdToServer(MSG_SET_TIME,(void*)&now,sizeof(time_t));

	if (rtn==false)
	{
		string msg = string("ͨ��[") + strIPAddress +string(":") + StrUtil::Int2Str(mChannelID) + string("]����ϵͳʱ��ʧ��");
		//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);
	}
	else
	{
		string msg = string("ͨ��[") + strIPAddress +string(":") + StrUtil::Int2Str(mChannelID) + string("]����ϵͳʱ��ɹ�");
		//SYSMSGSENDER::instance()->SendMsg(msg);
	}

	return rtn;
}
string TCPDeviceAccess::GetDeviceIP()
{
	return strIPAddress;
}
unsigned int TCPDeviceAccess::SetMultiScrNetParam(unsigned char *dest, const ChanNetCfgObj& netcfg, unsigned char cmdCode)
{
	dest[0] = PROTOHEAD0;	//���ֽ�Э��ͷ
	dest[1] = PROTOHEAD1;
	unsigned int cmdlen = 2;

	unsigned int datalen = sizeof(ChanNetCfgObj);
	if(strlen(netcfg.u8ChanIp) > 8)
	{
		unsigned short crcCode = CRC16((unsigned char*)&netcfg, datalen);
		dest[3] = (crcCode >> 8)&0xFF;	//���ֽ�CRC
		dest[2] = crcCode&0xFF;
		dest[4] = cmdCode;	//һ�ֽڹ�����
		dest[6] = (datalen >> 8)&0xFF;	//���ֽ����ݳ���
		dest[5] = datalen&0xFF;
		cmdlen += 5;

		memcpy(dest+cmdlen, &netcfg, datalen);
		cmdlen += datalen;
	}
	return cmdlen;
}

unsigned int TCPDeviceAccess::SetMultiScrPIDParam(unsigned char *dest, const ChanPIDObj& channelpid, unsigned char cmdCode)
{
	dest[0] = PROTOHEAD0;	//���ֽ�Э��ͷ
	dest[1] = PROTOHEAD1;

	unsigned int datalen = sizeof(ChanPIDObj);
	unsigned short crcCode = CRC16((unsigned char*)&channelpid, datalen);
	dest[3] = (crcCode >> 8)&0xFF;	//���ֽ�CRC
	dest[2] = crcCode&0xFF;
	dest[4] = cmdCode;	//һ�ֽڹ�����
	dest[6] = (datalen >> 8)&0xFF;	//���ֽ����ݳ���
	dest[5] = datalen&0xFF;

	unsigned int cmdlen = 7;

	memcpy(dest+cmdlen, &channelpid, datalen);
	cmdlen += datalen;

	return cmdlen;
}
unsigned short TCPDeviceAccess::CRC16(unsigned char *ptr,unsigned int len)
{
		unsigned char da=0; 
		unsigned short crc = 0;
		unsigned int crc_table[16]={ 
				0x0000, 0x1081, 0x2102, 0x3183, 
				0x4204, 0x5285, 0x6306, 0x7387, 
				0x8408, 0x9489, 0xa50a, 0xb58b, 
				0xc60c, 0xd68d, 0xe70e, 0xf78f 
		};

		while(len--!=0)
		{ 
				da = (unsigned char)(crc&0x000f); 
				crc>>= 4; 
				crc ^= crc_table[da^(*ptr&0x0f)]; 
				da = (unsigned char)(crc&0x000f); 
				crc>>= 4; 
				crc ^= crc_table[da^(*ptr/16)]; 
				ptr++;   
		} 
		return(crc);
}