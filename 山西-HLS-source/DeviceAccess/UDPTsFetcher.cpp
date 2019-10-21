
#include "UDPTsFetcher.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/AppLog.h"
#include "../Foundation/TypeDef.h"
#include "ace/OS.h"
#include "ace/Synch.h"
#include "./DeviceAccessMgr.h"
#include "../Communications/TsSender.h"
#include "../Communications/UdpTsSender.h"
#include "DeviceAccess.h"
#include "../Foundation/XmlParser.h"
#include "../Communications/TSServer.h"
#include "../BusinessProcess/BusinessLayoutMgr.h"
#include "../DeviceAccess/HTTPDeviceAccess.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include "../DBAccess/DBManager.h"
#include "../Communications/DeviceServer.h"
#include "ChanScanThread.h"
#include "../Communications/TsSenderMgr.h"
#include <ace/Handle_Set.h>
#include <mbstring.h> 
#define MAX_MULTICAST_IP_TTL  5
ACE_Thread_Mutex UDPTsFetcher::TaskRoundMutex;
ACE_Thread_Mutex g_SetCmdMutex;
extern ACE_Thread_Mutex g_CodecMutex;
extern std::vector<OSDFormat> gOSDFormatVec;
extern int g_RealtimeRoundTaskNum;
extern int g_iCurCodecUserID;
bool g_getrounddata = false;
int g_indexnum = 0;
//
void ConvertGBKToUtf8(string& strGBK) 
{
     int len=MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)strGBK.c_str(), -1, NULL,0);
     unsigned short * wszUtf8 = new unsigned short[len+1];
     memset(wszUtf8, 0, len * 2 + 2);
     MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)strGBK.c_str(), -1, (LPWSTR)wszUtf8, len);
 
    len = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)wszUtf8, -1, NULL, 0, NULL, NULL); 
    char *szUtf8=new char[len + 1];
     memset(szUtf8, 0, len + 1);
     WideCharToMultiByte (CP_UTF8, 0, (LPCWSTR)wszUtf8, -1, szUtf8, len, NULL,NULL);
 
    strGBK = szUtf8;
     delete[] szUtf8;
     delete[] wszUtf8;
 }
 
//
UDPTsFetcher::UDPTsFetcher(int deviceid) : TsFetcher(deviceid)
{
	TsSendTaskPointer = NULL;			//视频任务
	TsSendRoundTaskPointer = NULL;		//轮播任务
	RecordTaskPointerVec.clear();
	bFlag = false;
	bSendSwtich=false;
	m_bsenddatatocodec = false;
	bSendOrder=false;
	CurDeviceXml = "";
	TaskNum=0;
}

UDPTsFetcher::~UDPTsFetcher()
{

}
int UDPTsFetcher::Start()
{
	open(0);
	return 0;
}

int UDPTsFetcher::open(void*)
{
	bFlag = true;
	NewPackageHead = false;
	ReSendSwitch = false;
	
	//设置接收队列缓冲区大小
	msg_queue()->high_water_mark(188*10000);
	msg_queue()->low_water_mark(188*1000);

	this->activate();

	return 0;
}

int UDPTsFetcher::svc()
{
	eDVBType dvbtype = UNKNOWN;
	PROPMANAGER::instance()->GetDeviceType(DeviceId,dvbtype);

	//-----------------
	string TsIp = "238.0.0."/*;"172.16.10."*/+StrUtil::Int2Str(100+DeviceId);
	unsigned int tsport=9000+DeviceId;
	//
	if(DeviceId == PROPMANAGER::instance()->GetCoderDevNum())
	{
		TsIp = "238.123.46.78";
		tsport=8887;
	}
	//***
	Sleep(500);
	if(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")
	{
		while(true)
		{
			CoderDev cdev;
			if(PROPMANAGER::instance()->GetCoderInfo(DeviceId,cdev))
			{
				TsIp = cdev.s_dip;
				tsport = StrUtil::Str2Int(cdev.s_dtsport);
				break;
			}
			else
			{
				Sleep(1000);
			}
		}
	
	}
	//***
	
	string mulitcase = TsIp + string(":")+ StrUtil::Int2Str(tsport);
	ACE_INET_Addr multicast_addr_(mulitcase.c_str()); 
	DeviceSock.join(multicast_addr_,1,(PROPMANAGER::instance()->GetDeviceIp()).c_str());
	DeviceSock.set_option(IP_MULTICAST_TTL, 5);
	int nSocketBuffSize = 1024*1024;
	ACE_SOCK* sk= &DeviceSock;
	sk->set_option(SOL_SOCKET, SO_RCVBUF, &nSocketBuffSize, sizeof(int));
	//用于转发TS流给多画面卡
	m_SendForMulti = false;
	//------------------------

	int ReadSize	= 188*70;	//每次获得数据的大小
	int BufLen		= 188*700;//0*5;	//每次发送或者写文件的块大小 
	unsigned char* RcvBuf	= new unsigned char[ReadSize];		//接收BUF
	unsigned char* TsBuf	= new unsigned char[BufLen];		//缓冲BUF
	memset(RcvBuf,0,ReadSize);
	memset(TsBuf,0,BufLen);
	
	int PacketLen = 0;
	ACE_Time_Value PutOutTime(ACE_OS::gettimeofday()+ACE_Time_Value(5));
	ACE_Time_Value RecvTimeOut(5);

	int NoDataCount = 0;

	int RebootCardCount = 0;
	time_t mRebootTime = time(0);		//初始化重启时间
	int nTimeValue = 60*60*48;			//24个小时重启一次

	time_t mReSendDeviceXMLTime = time(0);	//重发指令时间（同步时间，且保证节目正确）
	int nReSendDeviceXMLInterval = 60*60*24;	//每天重发/每30分钟重发
	
	int TunerId = -1;
	PROPMANAGER::instance()->GetDeviceTunerID(DeviceId,TunerId);
	//
	ACE_Handle_Set shandles;
	ACE_HANDLE handle;
	ACE_Time_Value time(0);	//
	//
	const char* mcast_addr = "235.0.0.57:7698";//用于1路转码
    ACE_INET_Addr mcast(mcast_addr);//用于1路转码
    ACE_SOCK_Dgram_Mcast MultiSend;//用于1路转码
    MultiSend.join(mcast);//用于1路转码
	//
	comamdtype = -1;
	//
	while(bFlag)
	{
		if(DeviceId != PROPMANAGER::instance()->GetCoderDevNum())
		{
			if (DeviceSock.get_handle()==NULL||CurDeviceXml.length()<1||CurDeviceXml.empty()||TaskNum<=0)
			{
				//即使没有任务也接收数据，防止udp缓冲区被占满
				int RecvLen = DeviceSock.recv(RcvBuf,ReadSize,remote_addr,0,&RecvTimeOut);	//接收数据
				memset(RcvBuf,0,ReadSize);

				OSFunction::Sleep(0,5);
				FlushHistoryDatum();	
				continue;
			}
			if (CurDeviceXml.empty() == false && bSendOrder == false)
			{
				if (TsSendTaskPointer != NULL)
				{
					TsSendTaskPointer->SetSendSwitch(false);
				}
				FlushHistoryDatum();
				SendTsDeviceXml();                                                                                                              //重发任务指令

				if (bSendOrder == false)
				{
					OSFunction::Sleep(0, 10 );
					continue;
				}
				for (int i=0;i<3000;++i)
				{
					ACE_Time_Value nowaittime (ACE_Time_Value::zero);
					if(DeviceSock.recv(RcvBuf,ReadSize,remote_addr,0,&nowaittime)<=0)       //消耗历史网络数据
					{
						break;
					}
				}
				if (TsSendTaskPointer != NULL)
				{
					TsSendTaskPointer->SetSendSwitch(true);
				}
			}
		}
		memset(RcvBuf,0,ReadSize);
		int RecvLen = DeviceSock.recv(RcvBuf,ReadSize,remote_addr,0,&RecvTimeOut);	//接收数据
		if(RecvLen <= 0)
		{
			continue;
		}
		//
		if(!strcmp(mcast_addr,"235.0.0.57:7698"))
		{
			g_CodecMutex.acquire();
			if((g_iCurCodecUserID==DeviceId)&&(RecvLen>0))
			{
				//
				MultiSend.send(RcvBuf,RecvLen);
			}
			g_CodecMutex.release();

		}
		//
		if ((TsSendRoundTaskPointer != NULL && bSendSwtich == true))
		{
			ACE_Message_Block *mb = NULL;
			try
			{
				unsigned char* pPackage = RcvBuf;
				int beginPos = -1;
				if((bSendSwtich)&&(comamdtype == 2))
				{
					g_getrounddata = false;
					g_indexnum = 0;
				}
				if((!bSendSwtich)&&g_getrounddata)
				{
					g_indexnum++;
				}
				//
				if(NewPackageHead)
				{
					beginPos = 1;
				}
				if(beginPos != -1 || NewPackageHead == false)
				{
					if(NewPackageHead)
					{
						NewPackageHead = false;
					}
					//
					mb = new ACE_Message_Block(RecvLen);
					if (mb != NULL)
					{
						mb->copy((char*)pPackage,RecvLen);
						TaskRoundMutex.acquire();
						if((g_getrounddata)||(bSendSwtich == true))
						{
							if(-1 == TsSendRoundTaskPointer->putq(mb,&PutOutTime))
							{
								mb->release();
								TsSendRoundTaskPointer->msg_queue()->flush();
							}
						}
						TaskRoundMutex.release();
					}
				}
			}
			catch(ACE_bad_alloc)
			{
				if (mb != NULL)
					mb->release();
				ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d] UDPTsFetcher使用ACE_Message_Block出错\n",DeviceId));
			}
			catch (...)
			{
				if (mb != NULL)
					mb->release();
				ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d] UDPTsFetcher使用ACE_Message_Block出错\n",DeviceId));
			}
		}
		//视频任务指针不为空，则将获得数据放入视频发送线程的数据队列中
		RealStreamMutex.acquire();
		if (TsSendTaskPointer != NULL)
		{
			ACE_Message_Block *mb = NULL;
	
			try
			{
				mb = new ACE_Message_Block(RecvLen);
				if (mb != NULL)
				{
					//printf("packlen : %d .\n", PacketLen);
					mb->copy((char*)RcvBuf,RecvLen);
					if(-1 == TsSendTaskPointer->putq(mb,&PutOutTime))
					{
						mb->release();
						TsSendTaskPointer->msg_queue()->flush();
					//	ACE_DEBUG ((LM_DEBUG,"(%T| %t) \n通道[%d] 放弃数据-TsSend\n",DeviceId));
					}
				}
			}
			catch(ACE_bad_alloc)
			{
				if (mb != NULL)
					mb->release();
				ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d] UDPTsFetcher使用ACE_Message_Block出错\n",DeviceId));
			}
			catch(...)
			{
				if (mb != NULL)
					mb->release();
				ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]UDPTsFetcher使用ACE_Message_Block出错\n",DeviceId));
			}
		}
		RealStreamMutex.release();
		//
		if (BufLen-PacketLen > RecvLen)
		{
			memcpy(TsBuf+PacketLen,RcvBuf,RecvLen);	
			PacketLen += RecvLen;
			continue;
		}
		//第一次接收到数据包，处理包头，保证能够播放和录像文件能够播放
		unsigned char* pPackage = NULL;
		{
			pPackage = TsBuf;
		}
		TaskPointerMutex.acquire();
		std::vector<ACE_Task<ACE_MT_SYNCH>*>::iterator ptr = RecordTaskPointerVec.begin();
		for(;ptr!=RecordTaskPointerVec.end();++ptr)
		{
			if ((*ptr) != NULL)//录像任务指针不为空，则将获得数据放入录像任务的数据队列中
			{
				ACE_Message_Block *mb = NULL;
				try
				{
					mb = new ACE_Message_Block(PacketLen);
					if (mb != NULL)
					{
						mb->copy((char*)pPackage,PacketLen);
						if(-1 == (*ptr)->putq(mb,&PutOutTime))
						{
							mb->release();
							(*ptr)->msg_queue()->flush();
						}
						//PutSysTime(*ptr);//会导致录像播放偶尔花屏
					}
				}
				catch(ACE_bad_alloc)
				{
					if (mb != NULL)
						mb->release();
					ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d] UDPTsFetcher使用ACE_Message_Block出错\n",DeviceId));
				}
				catch(...)
				{
					if (mb != NULL)
						mb->release();
					ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]UDPTsFetcher使用ACE_Message_Block出错\n",DeviceId));
				}
			}
		}
		TaskPointerMutex.release();

		memset(TsBuf,0,BufLen);
		PacketLen = 0;

		//将本次未处理数据拷贝进缓存
		memcpy(TsBuf+PacketLen,RcvBuf,RecvLen);	//拷贝至内存
		PacketLen += RecvLen;
		//
	}
	delete RcvBuf;
	delete TsBuf;
	DeviceSock.close();
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]停止获取TS流\n",DeviceId));
	bFlag = false;
	return 0;
}

bool UDPTsFetcher::SetTsSendTask(TsSender* task)
{
	if (task == NULL)
		return false;

	RealStreamMutex.acquire();
	TsSendTaskPointer = task;		//设置视频任务的指针
	RealStreamMutex.release();
	return true;
}

bool UDPTsFetcher::SetTsRoundTask(TsSender* task)
{
	if (task == NULL)
		return false;

	//TaskRoundMutex.acquire();
	TsSendRoundTaskPointer = task;//设置视频录播任务的指针
	//TaskRoundMutex.release();
	return true;
}

bool UDPTsFetcher::SetRecordTask( ACE_Task<ACE_MT_SYNCH>* task )
{
	if(task==NULL)
		return false;
	TaskPointerMutex.acquire();
	RecordTaskPointerVec.push_back(task);
	TaskPointerMutex.release();
	return true;
}

int UDPTsFetcher::Stop()
{
	bFlag = false;
	this->wait();

	return 0;
}
void UDPTsFetcher::StopHistoryTask()
{
	TaskMutex.acquire();
	CurDeviceXml = "";
	bSendOrder = false;
	FlushHistoryDatum();
	OSFunction::Sleep(0,10);
	TaskMutex.release();
	return;
}
bool UDPTsFetcher::SetTsDeviceXml( std::string devicexml )
{
	bool ret = false;
	
	if(devicexml.empty())
		return false;	

	if(devicexml==CurDeviceXml)
		return true;
	
	TaskMutex.acquire();
	comamdtype = 1;
	CurDeviceXml = devicexml;
	bSendOrder = false;
	TaskMutex.release();

	return true;
}

bool UDPTsFetcher::SendTsDeviceXml()
{
	if (CurDeviceXml.empty())
	{
		std::string info = std::string("通道[") + StrUtil::Int2Str(DeviceId) + std::string("]任务指令为空");
		//SYSMSGSENDER::instance()->SendMsg(info,UNKNOWN,VS_MSG_SYSALARM);
		return false;
	}
	else if(CurDeviceXml=="StreamRealtimeMultiQueryTask"||CurDeviceXml=="StreamRealtimeRoundQueryTask")
	{
		bSendOrder = true;
		return true;
	}

	TaskMutex.acquire();
	XmlParser tmpparser(CurDeviceXml.c_str());
	TaskMutex.release();
	
	eDVBType edtype;
	string strDVBType;
	pXMLNODE TsrootNode = tmpparser.GetNodeFromPath( "Msg" );
	tmpparser.GetAttrNode(TsrootNode,"DVBType",strDVBType);
	edtype = OSFunction::GetEnumDVBType(strDVBType);

	pXMLNODE TsNode = tmpparser.GetNodeFromPath( "Msg/TSQuery/TS" );
	tmpparser.SetAttrNode( "TunerID",StrUtil::Int2Str(TunerID),TsNode );//重设指令中TunerID以及系统时间
	tmpparser.SetAttrNode( "SystemClock",(long)time(0),TsNode );
	string TmpDeviceXml;
	tmpparser.SaveToString(TmpDeviceXml);

	if (TmpDeviceXml.empty())
	{
		std::string info = std::string("通道[") + StrUtil::Int2Str(DeviceId) + std::string("]命令XML处理出错");
		//SYSMSGSENDER::instance()->SendMsg(info,UNKNOWN,VS_MSG_SYSALARM,DEVICE);

		bSendOrder = false;
		return false;
	}
	if (edtype == DVBC)
	{
		SendTsDeviceXmlForDVBC(TmpDeviceXml);
	}
	else if (edtype == CTTB)
	{
		SendTsDeviceXmlForCTTB(TmpDeviceXml);
	}
	else if (edtype == DVBS)
	{
		SendTsDeviceXmlForDVBS(TmpDeviceXml);
	}
	else if ((edtype == RADIO)||(edtype == AM))
	{
		SendTsDeviceXmlForFMAM(TmpDeviceXml);
	}
	bSendOrder = true;
	return true;
}



bool UDPTsFetcher::SendTsDeviceXmlForDVBC(std::string devXml)
{
	ACE_Guard<ACE_Thread_Mutex> guard(g_SetCmdMutex);
	//
	std::string PmtPID,PcrPID,VideoPID,AudioPID ,Freq,ServiceID,strW,strH,strbps,striosdfontsize,osdinfo,strosdinfoX,strosdinfoY,strosdtimeX,strosdtimeY,proname,strosdprogramX,strosdprogramY,isosd;
	string version,TunerDeviceID,CoderDeviceID,IsTranAudio;
	string strComamdType = "";
	XmlParser tmpparser(devXml.c_str());
	pXMLNODE TsNode = tmpparser.GetNodeFromPath( "Msg/TSQuery/TS" );
	tmpparser.GetAttrNode(TsNode,"ServiceID",ServiceID);
	tmpparser.GetAttrNode(TsNode,"Version",version);
	tmpparser.GetAttrNode(TsNode,"TDeviceID",TunerDeviceID);
	tmpparser.GetAttrNode(TsNode,"CDeviceID",CoderDeviceID);
	tmpparser.GetAttrNode(TsNode,"ComamdType",strComamdType);
	if(strComamdType.size()<=0)
	{
		comamdtype = 3;
	}
	//ServiceID = "102";
	tmpparser.GetAttrNode(TsNode,"PmtPID",PmtPID);
	tmpparser.GetAttrNode(TsNode,"PcrPID",PcrPID);
	tmpparser.GetAttrNode(TsNode,"VideoPID",VideoPID);
	tmpparser.GetAttrNode(TsNode,"Width",strW);
	tmpparser.GetAttrNode(TsNode,"Height",strH);
	tmpparser.GetAttrNode(TsNode,"Bps",strbps);
	tmpparser.GetAttrNode(TsNode,"OSDFontSize",striosdfontsize);
	tmpparser.GetAttrNode(TsNode,"OSDInfo",osdinfo);
	tmpparser.GetAttrNode(TsNode,"OSDInfoX",strosdinfoX);
	tmpparser.GetAttrNode(TsNode,"OSDInfoY",strosdinfoY);
	tmpparser.GetAttrNode(TsNode,"OSDTimeX",strosdtimeX);
	tmpparser.GetAttrNode(TsNode,"OSDTimeY",strosdtimeY);
	tmpparser.GetAttrNode(TsNode,"ServiceName",proname);
	//
	proname.erase(0,proname.find_first_not_of(" "));//除空格  
    proname.erase(proname.find_last_not_of(" ") + 1);//除空格  
	//
	tmpparser.GetAttrNode(TsNode,"OSDProgramX",strosdprogramX);
	tmpparser.GetAttrNode(TsNode,"OSDProgramY",strosdprogramY);
	tmpparser.GetAttrNode(TsNode,"OSDEnable",isosd);
	//VideoPID = "9024";
	tmpparser.GetAttrNode(TsNode,"AudioPID",AudioPID);
	tmpparser.GetAttrNode(TsNode,"Audio",IsTranAudio);
	//AudioPID = "834";
	tmpparser.GetAttrNode(TsNode,"Freq",Freq);

	std::string msg;
	if (Freq != strHDMIFreq)
	{
		msg = string("通道[") + StrUtil::Int2Str(DeviceId)+ string("]指令:") + \
			string("PMT[") + PmtPID + string("] PCR[") + PcrPID + string("] VPID[") + \
			VideoPID + string("] APID[") + AudioPID + string("]");

		//SYSMSGSENDER::instance()->SendMsg(msg);
	}

	size_t sz = 0; 
	string strtime = "time";
	string utf8_time;
	string utf8_proname;
	string utf8_osdinfo;
	//
	if(strtime.size()>0)
	{
		utf8_time = strtime;
		ConvertGBKToUtf8(utf8_time);
	}
	//
	if(proname.size()>0)
	{
		utf8_proname = proname;
		ConvertGBKToUtf8(utf8_proname);
	}
	if(osdinfo.size()>0)
	{
		utf8_osdinfo = osdinfo;
		ConvertGBKToUtf8(utf8_osdinfo);
	}

	int ivBps = StrUtil::Str2Int(strbps);
	string sAudioBps = "32";
	if (ivBps < 300)
	{
		sAudioBps = "32";
	}
	else if ((ivBps > 300) && (ivBps < 450))
	{
		sAudioBps = "32";
	}
	else if (ivBps > 450)
	{
		sAudioBps = "32";
	}
	
	int isBps = (ivBps + StrUtil::Str2Int(sAudioBps))*1.1;
	//
	int iosdfontsize = StrUtil::Str2Int(striosdfontsize);
	if(iosdfontsize < 8)
	{
		iosdfontsize = 8;
		striosdfontsize = StrUtil::Int2Str(iosdfontsize);
	}

	//
	string vfmt;
	if(((!(StrUtil::Str2Int(strW)>640||StrUtil::Str2Int(strH)>480))&&(PROPMANAGER::instance()->GetCurWorkVersion() != "5.0"))
		||((!(StrUtil::Str2Int(strW)>640||StrUtil::Str2Int(strH)>480))&&(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")))
	{
		if(ivBps < 250)
		{
			ivBps = 250;
			isBps = (ivBps + StrUtil::Str2Int(sAudioBps))*1.1;
			strbps = StrUtil::Int2Str(isBps);
		}
		vfmt ="2";
		//避免osd位置异常
		if ((PROPMANAGER::instance()->GetCurWorkVersion() == "1.0")||(PROPMANAGER::instance()->GetCurWorkVersion() == "2.0"))
		{
			strW = "352";
			strH = "288";
		}
		//
	}
	else
	{
		if(ivBps < 350)
		{
			ivBps = 350;
			isBps = (ivBps + StrUtil::Str2Int(sAudioBps))*1.1;
			strbps = StrUtil::Int2Str(isBps);
		}
		iosdfontsize = 30;
		striosdfontsize = StrUtil::Int2Str(iosdfontsize);
		vfmt ="1";
		//避免osd位置异常
		if ((PROPMANAGER::instance()->GetCurWorkVersion() == "1.0")||(PROPMANAGER::instance()->GetCurWorkVersion() == "2.0"))
		{
			strW = "720";
			strH = "576";
		}
		//
	}


	string scurTime = TimeUtil::GetCurDateTime();
	int iosdTimeLen = scurTime.length();
	int iosdProLen = proname.length();
	int iosdInfoLen = osdinfo.length();
	int iosdChannelLen = proname.length() + osdinfo.length();
	string antiColor = "1";

	//jsp2017.07.15
	OSDFormat osdf;
	bool bosdsetfind = false;

	if (iosdTimeLen - iosdChannelLen - 1 > 0)
	{
		//OSD位置控制计算 默认右上角右对齐
		strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*iosdfontsize/2);
		strosdinfoY = StrUtil::Int2Str(iosdfontsize/2);
		strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*iosdfontsize/2);
		strosdprogramY = StrUtil::Int2Str(iosdfontsize/2);
		strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*iosdfontsize/2);
		strosdtimeY = StrUtil::Int2Str(iosdfontsize*3/2);

		for(int i=0;i<gOSDFormatVec.size();i++)
		{
			if((gOSDFormatVec[i].Freq == Freq)&&(gOSDFormatVec[i].ServiceID == ServiceID))
			{
				osdf = gOSDFormatVec[i];
				bosdsetfind = true;
				break;
			}
		}
		if(bosdsetfind)
		{	
			int ifontsize = StrUtil::Str2Int(osdf.FontSize);
			if(osdf.Position == "0")
			{
				if(osdf.Align == "0")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize*3/2);
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY =  StrUtil::Int2Str(ifontsize*3/2);
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen+3)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
				}
				else if(osdf.Align == "1")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(((iosdTimeLen-iosdChannelLen-1)/2+1)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str(((iosdTimeLen-iosdChannelLen-1)/2+iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(((iosdTimeLen-iosdChannelLen-1)/2+1)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize*3/2);
						strosdprogramX = StrUtil::Int2Str(((iosdTimeLen-iosdChannelLen-1)/2+iosdInfoLen+2)*ifontsize/2);
						strosdprogramY =  StrUtil::Int2Str(ifontsize*3/2);
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen+3)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
				}
				else if(osdf.Align == "2")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str((iosdTimeLen-iosdChannelLen)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str((iosdTimeLen-iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str((iosdTimeLen-iosdChannelLen)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize*3/2);
						strosdprogramX = StrUtil::Int2Str((iosdTimeLen-iosdProLen+1)*ifontsize/2);
						strosdprogramY =  StrUtil::Int2Str(ifontsize*3/2);
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen+3)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}

				}
			}
			else if(osdf.Position == "1")
			{
				if(osdf.Align == "0")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen-iosdInfoLen)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize*3/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen-iosdInfoLen)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize*3/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdChannelLen+3)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdProLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
				}
				else if(osdf.Align == "1")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen-(iosdTimeLen-iosdChannelLen-1)/2+1)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen-(iosdTimeLen-iosdChannelLen-1)/2-iosdInfoLen)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen-(iosdTimeLen-iosdChannelLen-1)/2+1)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize*3/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen-(iosdTimeLen-iosdChannelLen-1)/2-iosdInfoLen)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize*3/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdChannelLen+3)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdProLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
				}
				else if(osdf.Align == "2")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize*3/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize*3/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdChannelLen+3)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdProLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
				}
			}
			else if(osdf.Position == "2")
			{
				if(osdf.Align == "0")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*5/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen+3)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
				}
				else if(osdf.Align == "1")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(((iosdTimeLen-iosdChannelLen-1)/2+1)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdprogramX = StrUtil::Int2Str(((iosdTimeLen-iosdChannelLen-1)/2+iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(((iosdTimeLen-iosdChannelLen-1)/2+1)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str(((iosdTimeLen-iosdChannelLen-1)/2+iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*5/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen+3)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}	
				}
				else if(osdf.Align == "2")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str((iosdTimeLen-iosdChannelLen)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdprogramX = StrUtil::Int2Str((iosdTimeLen-iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str((iosdTimeLen-iosdChannelLen)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str((iosdTimeLen-iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*5/2);

					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen+3)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}	
				}
			}
			else if(osdf.Position == "3")
			{
				if(osdf.Align == "0")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen-iosdInfoLen)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen-iosdInfoLen)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*5/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdChannelLen+3)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdProLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
				}
				else if(osdf.Align == "1")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen-(iosdTimeLen-iosdChannelLen-1)/2+1)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen-(iosdTimeLen-iosdChannelLen-1)/2-iosdInfoLen)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);		
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen-(iosdTimeLen-iosdChannelLen-1)/2+1)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen-(iosdTimeLen-iosdChannelLen-1)/2-iosdInfoLen)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*5/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdChannelLen+3)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdProLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
				}
				else if(osdf.Align == "2")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);		
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*5/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdChannelLen+3)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdProLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
				}
			}
			striosdfontsize = StrUtil::Int2Str(ifontsize);
		}
	}
	else
	{
		//OSD位置控制计算 默认右上角右对齐
		strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*iosdfontsize/2);
		strosdinfoY = StrUtil::Int2Str(iosdfontsize/2);
		strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*iosdfontsize/2);
		strosdprogramY = StrUtil::Int2Str(iosdfontsize/2);
		strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*iosdfontsize/2);
		strosdtimeY = StrUtil::Int2Str(iosdfontsize*3/2);

		for(int i=0;i<gOSDFormatVec.size();i++)
		{
			if((gOSDFormatVec[i].Freq == Freq)&&(gOSDFormatVec[i].ServiceID == ServiceID))
			{
				osdf = gOSDFormatVec[i];
				bosdsetfind = true;
				break;
			}
		}
		if(bosdsetfind)
		{
			int ifontsize = StrUtil::Str2Int(osdf.FontSize);
			if(osdf.Position == "0")
			{
				if(osdf.Align == "0")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize*3/2);
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY =  StrUtil::Int2Str(ifontsize*3/2);
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen+3)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
				}
				else if(osdf.Align == "1")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(((iosdChannelLen-iosdTimeLen+2)/2+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize*3/2);
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY =  StrUtil::Int2Str(ifontsize*3/2);
						strosdtimeX = StrUtil::Int2Str(((iosdChannelLen-iosdTimeLen+2)/2+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen+3)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
				}
				else if(osdf.Align == "2")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen-iosdTimeLen+2)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize*3/2);
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY =  StrUtil::Int2Str(ifontsize*3/2);
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen-iosdTimeLen+2)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen+3)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
				}
			}
			else if(osdf.Position == "1")
			{
				if(osdf.Align == "0")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize*3/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize*3/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdChannelLen+3)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdProLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
				}
				else if(osdf.Align == "1")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen-(iosdChannelLen-iosdTimeLen+1)/2+2)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize*3/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize*3/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen-(iosdChannelLen-iosdTimeLen+1)/2+2)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdChannelLen+3)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdProLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
				}
				else if(osdf.Align == "2")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize*3/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize*3/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdChannelLen+3)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdProLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
				}
			}
			else if(osdf.Position == "2")
			{
				if(osdf.Align == "0")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*5/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen+3)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
				}
				else if(osdf.Align == "1")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdtimeX = StrUtil::Int2Str(((iosdChannelLen-iosdTimeLen+2)/2+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(((iosdChannelLen-iosdTimeLen+2)/2+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*5/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen+3)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}			
				}
				else if(osdf.Align == "2")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen-iosdTimeLen+2)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen-iosdTimeLen+2)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*5/2);

					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen+3)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}		
				}
			}
			else if(osdf.Position == "3")
			{
				if(osdf.Align == "0")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*5/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdChannelLen+3)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdProLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
				}
				else if(osdf.Align == "1")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen-(iosdChannelLen-iosdTimeLen+1)/2+2)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);		
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen-(iosdChannelLen-iosdTimeLen+1)/2+2)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*5/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdChannelLen+3)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdProLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
				}
				else if(osdf.Align == "2")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);		
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*5/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdChannelLen+3)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdProLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
				}
			}
			striosdfontsize = StrUtil::Int2Str(ifontsize);
		}
	}

	AlarmParamConfig alarmParam;
	PROPMANAGER::instance()->GetAlarmParamConfig(UNKNOWN, alarmParam);
	if (alarmParam.s_bfhold == "" || alarmParam.s_mutehold == "" || alarmParam.s_sthold == "")
	{
		alarmParam.s_bfhold = "20";
		alarmParam.s_mutehold = "-70";
		alarmParam.s_sthold = "1.5";
	}

	TunerDev tdev;
	CoderDev cdev;
	eDVBType edtype;
	if (Freq == strHDMIFreq)
		edtype = HDMI;
	else
		edtype = DVBC;
	string tunermonitorip,tunermonitorport,codemonitorip,codemonitorport;
	int tunid = -1;
	if(version == "3.0")
	{
		tunid = StrUtil::Str2Int(TunerDeviceID);
		PROPMANAGER::instance()->GetTunerInfo(StrUtil::Str2Int(TunerDeviceID),edtype,tdev);
		PROPMANAGER::instance()->GetDevMonitorInfo(0,edtype,StrUtil::Str2Int(TunerDeviceID),tunermonitorip,tunermonitorport);//tuner
	}
	else
	{
		if(PROPMANAGER::instance()->IsUnUseRecDev(1,edtype,DeviceId))//实时视频和频道轮播使用的tuner和coder的编号相同
		{
			tunid = DeviceId;
			PROPMANAGER::instance()->GetTunerInfo(DeviceId,edtype,tdev);
			if (Freq != strHDMIFreq)
			{
				string servlist;
				CHANNELINFOMGR::instance()->GetServIdListByFreq(edtype,Freq,ServiceID,"",tdev.s_devicecampronum,servlist);
				tdev.s_servidList = ServiceID;//servlist;特殊处理，8路卡不够，实时通道实际插2路卡，配置文件配置8路便于录像分配，实时通道不能按8路处理
			}
			PROPMANAGER::instance()->GetDevMonitorInfo(0,edtype,DeviceId,tunermonitorip,tunermonitorport);//tuner
		}
		else
		{
			PROPMANAGER::instance()->GetTunerDevFromCoderDev(tunid,edtype,DeviceId);
			PROPMANAGER::instance()->GetTunerInfo(tunid,edtype,tdev);
			PROPMANAGER::instance()->GetDevMonitorInfo(0,edtype,tunid,tunermonitorip,tunermonitorport);//tuner
			string temp;
			temp = "DeviceId:";
			temp+=StrUtil::Int2Str(DeviceId);
			temp+="****freq:";
			temp+=Freq;
			temp+="****servceid:";
			temp+=ServiceID;
			temp+="****tunerid:";
			temp+=StrUtil::Int2Str(tunid);
			APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,temp,LOG_OUTPUT_FILE);
		}
	}
	PROPMANAGER::instance()->GetCoderInfo(DeviceId,cdev);
	PROPMANAGER::instance()->GetDevMonitorInfo(1,edtype,DeviceId,codemonitorip,codemonitorport);//coder
	if(((tunermonitorip == codemonitorip)&&(tunermonitorport == codemonitorport))||(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0"))
	{
		if(tunermonitorip.size()>0&&tunermonitorport.size()>0)
		{
			HTTPDeviceAccess* pDeviceAccess = new HTTPDeviceAccess(StrUtil::Str2Int(tdev.s_monitorindex),tunermonitorip,StrUtil::Str2Int(tunermonitorport));
			string strRet;
			if (Freq == strHDMIFreq)
			{// HDMI 流
				HDMIInfo* pHDMIInfo = PROPMANAGER::instance()->GetHDMIInfo();
				string cmd = "POST /device/getformat  HTTP/1.1  \r\n\r\n\
							 chl=";
				cmd += pHDMIInfo->s_HDMIChannelNum;
				pDeviceAccess->SendTaskMsg(cmd,strRet);

				int pos = strRet.find("<html>");
				string strtem = strRet;
				if(pos >= 0)
				{
					strtem = strRet.substr(pos,strRet.size()-pos);
				}
				else
				{
					return false;
				}

				string format = "";
				XmlParser xml;
				xml.Set_xml(strtem);
				pXMLNODE pnode = xml.GetNodeFromPath("html/body");
				pXMLNODELIST pnodelist  = xml.GetNodeList(pnode);
				if (pnodelist != NULL)
				{
					pXMLNODE childNode = xml.GetNextNode(pnodelist);
					string strName = xml.GetNodeName(childNode);
					if (strName == "Desc")
					{
						xml.GetAttrNode(childNode, "format", format);
					}
				}
				if (format == "" || format == "UNDEF")
					return false;
				strRet = "";
				//***
				cmd = "POST  /encoder/config  HTTP/1.1  \r\n\r\n";
				cmd+="chl=";
				cmd+=cdev.s_coderindex;
				cmd+="&ena=";
				cmd+="1";
				cmd+="&chk=";
				cmd+="1";
				cmd+="&inchl=";
				cmd+=pHDMIInfo->s_HDMIChannelNum;
				cmd+="&format=";
				cmd+=format;
				cmd+="&vcode=";
				cmd+="h264";
				cmd+="&acode=";
				if((version == "3.0")&&(IsTranAudio == "1"))
				{
					cmd+="copy";
				}
				else
				{
					cmd+="mpeg";
				}
				cmd+="&vfmt=";
				cmd+=vfmt;
				cmd+="&vfre=";
				cmd+="0";
				cmd+="&v1v1=";
				cmd+="2";
				cmd+="&mutehold=";
				//cmd+="-70";
				cmd+= alarmParam.s_mutehold;
				cmd+="&bfhold=";
				//cmd+="20";
				cmd+= alarmParam.s_bfhold;
				cmd+="&sthold=";
				//cmd+="1.5";
				cmd+= alarmParam.s_sthold;
				cmd+="&vrate=";
				cmd+=StrUtil::Int2Str(ivBps);
				cmd+="&arate=";
				cmd+=sAudioBps;
				cmd+="&srate=";
				cmd+=strbps;
				cmd+="&osd0=";
				if (version == "3.0" && isosd == "1")
				{
					cmd+= antiColor;
					cmd+="_";
				}
				cmd+=strosdtimeX;
				cmd+="_";
				cmd+=strosdtimeY;
				cmd+="_";
				cmd+=striosdfontsize;
				cmd+="_";
				if (version == "3.0")
				{
					if(isosd == "1")
					{
						cmd+=utf8_time;
					}
				}
				else
				{
					if(isosd != "1")
					{
						cmd+=utf8_time;
					}
				}
				cmd+="&osd1=";
				if (version == "3.0" && isosd == "1")
				{
					cmd+= antiColor;
					cmd+="_";
				}
				cmd+=strosdprogramX;
				cmd+="_";
				cmd+=strosdprogramY;
				cmd+="_";
				cmd+=striosdfontsize;
				cmd+="_";
				if (version == "3.0")
				{
					if(isosd == "1")
					{
						cmd+=utf8_proname;//proname;
					}
				}
				else
				{
					if(isosd != "1")
					{
						cmd+=utf8_proname;//proname;
					}
				}
				cmd+="&osd2=";
				if (version == "3.0" && isosd == "1")
				{
					cmd+= antiColor;
					cmd+="_";
				}
				cmd+=strosdinfoX;
				cmd+="_";
				cmd+=strosdinfoY;
				cmd+="_";
				cmd+=striosdfontsize;
				cmd+="_";
				if (version == "3.0")
				{
					if(isosd == "1")
					{
						cmd+=utf8_osdinfo;//osdinfo;
					}
				}
				else
				{
					if(isosd != "1")
					{
						cmd+=utf8_osdinfo;//osdinfo;
					}
				}
				cmd+="&dip=";
				cmd+=cdev.s_dip;
				cmd+="&dpt=";
				cmd+=cdev.s_dtsport;
				cmd+="&evip=";
				cmd+=cdev.s_alarmip;
				cmd+="&evpt=";
				cmd+=cdev.s_alarmport;
				//***
				pDeviceAccess->SendTaskMsg(cmd,strRet);
				//
				if(!PROPMANAGER::instance()->IsUnUseRecDev(1,UNKNOWN,DeviceId))
				{
					std::string msg = "通道:";
					msg += StrUtil::Int2Str(DeviceId);
					msg += "发送命令:";
					msg += cmd;
					APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,msg,LOG_OUTPUT_FILE);
				}
				//
			}
			else
			{ 
				//普通 流
				sFreqScanInfo freqscaninfo;
				PROPMANAGER::instance()->GetFreqScanInfo(DVBC,freqscaninfo);
				//
				if(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")
				{
					HTTPDeviceAccess* pTcodeDeviceAccess = new HTTPDeviceAccess(StrUtil::Str2Int(cdev.s_monitorindex),codemonitorip,StrUtil::Str2Int(codemonitorport));
					//
					string  MsgID ;
					DBMANAGER::instance()->GetMsgID(MsgID);
					int msgid=StrUtil::Str2Int(MsgID);
					DBMANAGER::instance()->UpdateMsgID(StrUtil::Int2Str(++msgid));
					//
					std::string DeviceServerIP = PROPMANAGER::instance()->GetDeviceIp();
					std::string DeviceServerPort = PROPMANAGER::instance()->GetDevicePort();
					string Surl = DeviceServerIP+":"+DeviceServerPort;
					//
					string strRet;
					string cmd = "<?xml version=\"1.0\" encoding=\"gb2312\"?>";
					cmd += "<Msg Version=\"1.0\" MsgID=\"";
					cmd += MsgID;
					cmd += "\" SrcUrl=\"http://";
					cmd += Surl;
					cmd += "\">";
					cmd += "<Type>SetDemodulateInfo</Type>";
					cmd += "<Data>";
					cmd += "<Channel>";
					cmd += 	tdev.s_chanindex;
					cmd += "</Channel>";
					cmd += "<Frequency>";
					cmd += 	Freq;
					cmd += "</Frequency>";
					cmd += "<SymbolRate>";
					cmd += 	freqscaninfo.SymbolRate;
					cmd += "</SymbolRate>";
					cmd += "<Modulation>";
					cmd += 	"QAM64";
					cmd += "</Modulation>";
					cmd += "</Data>";
					cmd += "</Msg>";
					string sendaddress = "发送命令地址:"+tunermonitorip+":"+tunermonitorport;
					if((comamdtype == 1)||(comamdtype == 3))
					{
						cout<<"发送命令地址:"<<tunermonitorip<<":"<<tunermonitorport<<endl;
						cout<<TimeUtil::GetCurDateTime()<<"发送命令:"<<cmd<<endl;
						pDeviceAccess->SendTaskMsg(cmd,strRet);
						cout<<TimeUtil::GetCurDateTime()<<"收到结果:"<<strRet<<endl;
						APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,sendaddress,LOG_OUTPUT_FILE);
						APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,cmd,LOG_OUTPUT_FILE);
						APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,strRet,LOG_OUTPUT_FILE);
						Sleep(20);
						//
						/*if(strRet.find("200") != -1)
						{
							time_t curt = time(0);
							string DevRetval;
							while(((time(0)-curt)<30))
							{
								DevRetval = DEVRETSERVER::instance()->GetDevRet(MsgID);
								if(DevRetval.size()>0)
								{
									cout<<"收到结果**:"<<DevRetval<<endl;
									APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,DevRetval,LOG_OUTPUT_FILE);
									break;
								}
								else
								{
									Sleep(200);
									cout<<"解调延时"<<endl;
								}
							}
						}*/
						//***
						if(CHANNELINFOMGR::instance()->IsEncrypt(edtype,Freq,ServiceID))
						{
							DBMANAGER::instance()->GetMsgID(MsgID);
							msgid=StrUtil::Str2Int(MsgID);
							DBMANAGER::instance()->UpdateMsgID(StrUtil::Int2Str(++msgid));
							//
							cmd = "<?xml version=\"1.0\" encoding=\"gb2312\"?>";
							cmd += "<Msg Version=\"1.0\" MsgID=\"";
							cmd += MsgID;
							cmd += "\" SrcUrl=\"http://";
							cmd += Surl;
							cmd += "\">";
							cmd += "<Type>SetDecryptInfo</Type>";
							cmd += "<Data>";
							cmd += "<Channel>";
							cmd += 	tdev.s_chanindex;
							cmd += "</Channel>";
							cmd += "<ServiceList>";
							if(tdev.vecAllServid.size()>0)
							{
								for(int i=0;i<tdev.vecAllServid.size();i++)
								{
									if(CHANNELINFOMGR::instance()->IsEncrypt(tdev.e_tunerdvbtype,Freq,tdev.vecAllServid[i]))
									{
										cmd += "<ServiceID>";
										cmd += 	tdev.vecAllServid[i];
										cmd += "</ServiceID>";
									}
								}
							}
							else
							{
								cmd += "<ServiceID>";
								cmd += 	ServiceID;
								cmd += "</ServiceID>";
							}
							cmd += "</ServiceList>";
							cmd += "</Data>";
							cmd += "</Msg>";
							cout<<"发送命令地址:"<<tunermonitorip<<":"<<tunermonitorport<<endl;
							cout<<TimeUtil::GetCurDateTime()<<"发送命令:"<<cmd<<endl;
							pDeviceAccess->SendTaskMsg(cmd,strRet);
							cout<<TimeUtil::GetCurDateTime()<<"收到结果:"<<strRet<<endl;
							sendaddress = "发送命令地址:"+tunermonitorip+":"+tunermonitorport;
							APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,sendaddress,LOG_OUTPUT_FILE);
							APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,cmd,LOG_OUTPUT_FILE);
							APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,strRet,LOG_OUTPUT_FILE);
							Sleep(20);
							////
							/*if(strRet.find("200") != -1)
							{
								time_t curt = time(0);
								string DevRetval;
								while(((time(0)-curt)<30))
								{
									DevRetval = DEVRETSERVER::instance()->GetDevRet(MsgID);
									if(DevRetval.size()>0)
									{
										cout<<"收到结果**:"<<DevRetval<<endl;
										APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,DevRetval,LOG_OUTPUT_FILE);
										break;
									}
									else
									{
										Sleep(200);
										cout<<"解扰延时"<<endl;
									}
								}
							}*/
						}
										//
						DBMANAGER::instance()->GetMsgID(MsgID);
						msgid=StrUtil::Str2Int(MsgID);
						DBMANAGER::instance()->UpdateMsgID(StrUtil::Int2Str(++msgid));
						//
						cmd = "<?xml version=\"1.0\" encoding=\"gb2312\"?>";
						cmd += "<Msg Version=\"1.0\" MsgID=\"";
						cmd += MsgID;
						cmd += "\" SrcUrl=\"http://";
						cmd += Surl;
						cmd += "\">";
						cmd += "<Type>SetOutputInfo</Type>";
						cmd += "<Data>";
						cmd += "<Channel>";
						cmd += 	tdev.s_chanindex;
						cmd += "</Channel>";
						cmd += "<TSOut>";
						cmd += "<Switch>1</Switch>";
						cmd += "</TSOut>";
						cmd += "<Monitor>";
						cmd += "<Switch>1</Switch>";
						cmd += "</Monitor>";
						cmd += "</Data>";
						cmd += "</Msg>";
						cout<<"发送命令地址:"<<tunermonitorip<<":"<<tunermonitorport<<endl;
						cout<<TimeUtil::GetCurDateTime()<<"发送命令:"<<cmd<<endl;
						pDeviceAccess->SendTaskMsg(cmd,strRet);
						cout<<TimeUtil::GetCurDateTime()<<"收到结果:"<<strRet<<endl;
						sendaddress = "发送命令地址:"+tunermonitorip+":"+tunermonitorport;
						APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,sendaddress,LOG_OUTPUT_FILE);
						APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,cmd,LOG_OUTPUT_FILE);
						APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,strRet,LOG_OUTPUT_FILE);
						Sleep(20);
					}
					////
					//if(strRet.find("200") != -1)
					//{
					//	time_t curt = time(0);
					//	string DevRetval;
					//	while(((time(0)-curt)<30))
					//	{
					//		DevRetval = DEVRETSERVER::instance()->GetDevRet(MsgID);
					//		if(DevRetval.size()>0)
					//		{
					//			cout<<"收到结果**:"<<DevRetval<<endl;
					//			APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,DevRetval,LOG_OUTPUT_FILE);
					//			break;
					//		}
					//		else
					//		{
					//			Sleep(200);
					//			cout<<"解调开关打开延时"<<endl;
					//		}
					//	}
					//}
					//DBMANAGER::instance()->GetMsgID(MsgID);
					//msgid=StrUtil::Str2Int(MsgID);
					//DBMANAGER::instance()->UpdateMsgID(StrUtil::Int2Str(++msgid));
					////
					//cmd = "<?xml version=\"1.0\" encoding=\"gb2312\"?>";
					//cmd += "<Msg Version=\"1.0\" MsgID=\"";
					//cmd += MsgID;
					//cmd += "\" SrcUrl=\"http://";
					//cmd += Surl;
					//cmd += "\">";
					//cmd += "<Type>SaveAllCfg</Type>";
					//cmd += "</Msg>";
					//cout<<"发送命令地址:"<<tunermonitorip<<":"<<tunermonitorport<<endl;
					//cout<<"发送命令:"<<cmd<<endl;
					//pDeviceAccess->SendTaskMsg(cmd,strRet);
					//cout<<"收到结果:"<<strRet<<endl;
					//sendaddress = "发送命令地址:"+tunermonitorip+":"+tunermonitorport;
					//APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,sendaddress,LOG_OUTPUT_FILE);
					//APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,cmd,LOG_OUTPUT_FILE);
					//APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,strRet,LOG_OUTPUT_FILE);
					//Sleep(20);
					////
					//if(strRet.find("200") != -1)
					//{
					//	time_t curt = time(0);
					//	string DevRetval;
					//	while(((time(0)-curt)<30))
					//	{
					//		DevRetval = DEVRETSERVER::instance()->GetDevRet(MsgID);
					//		if(DevRetval.size()>0)
					//		{
					//			cout<<"收到结果**:"<<DevRetval<<endl;
					//			APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,DevRetval,LOG_OUTPUT_FILE);
					//			break;
					//		}
					//		else
					//		{
					//			Sleep(200);
					//			cout<<"解调保存变量延时"<<endl;
					//		}
					//	}
					//}
					//***
					if((comamdtype == 2)||(comamdtype == 3)||(comamdtype == 1))
					{
						DBMANAGER::instance()->GetMsgID(MsgID);
						msgid=StrUtil::Str2Int(MsgID);
						DBMANAGER::instance()->UpdateMsgID(StrUtil::Int2Str(++msgid));
						//
						cmd = "<?xml version=\"1.0\" encoding=\"gb2312\"?>";
						cmd += "<Msg Version=\"1.0\" MsgID=\"";
						cmd += MsgID;
						cmd += "\" SrcUrl=\"http://";
						cmd += Surl;
						cmd += "\">";
						cmd += "<Type>SetOSDInfo</Type>";
						cmd += "<Data>";
						cmd += "<Channel>";
						cmd += 	cdev.s_coderindex;
						cmd += "</Channel>";
						cmd += "<Text>";
						cmd += 	osdinfo;
						cmd += 	" ";
						cmd += 	proname;
						cmd += "</Text>";
						if(bosdsetfind)
						{
							cmd += "<FontSize>";
							cmd += 	osdf.FontSize;
							cmd += "</FontSize>";
							cmd += "<Position>";
							cmd += 	osdf.Position;
							cmd += "</Position>";
							cmd += "<AntiColor>";
							cmd += 	osdf.AntiColor;
							cmd += "</AntiColor>";
							cmd += "<Align>";
							cmd += 	osdf.Align;
							cmd += "</Align>";
							cmd += "<Type>";
							cmd += 	osdf.Type;
							cmd += "</Type>";
						}
						else
						{
							cmd += "<FontSize>";
							cmd += 	striosdfontsize;
							cmd += "</FontSize>";
							cmd += "<Position>";
							cmd += 	"1";
							cmd += "</Position>";
							cmd += "<AntiColor>";
							cmd += 	"1";
							cmd += "</AntiColor>";
							cmd += "<Align>";
							cmd += 	"2";
							cmd += "</Align>";
							cmd += "<Type>";
							cmd += 	"1";
							cmd += "</Type>";
						}
						cmd += "</Data>";
						cmd += "</Msg>";
						cout<<"发送命令地址:"<<codemonitorip<<":"<<codemonitorport<<endl;
						cout<<TimeUtil::GetCurDateTime()<<"发送命令:"<<cmd<<endl;
						pTcodeDeviceAccess->SendTaskMsg(cmd,strRet);
						cout<<TimeUtil::GetCurDateTime()<<"收到结果:"<<strRet<<endl;
						sendaddress = "发送命令地址:"+codemonitorip+":"+codemonitorport;
						APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,sendaddress,LOG_OUTPUT_FILE);
						APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,cmd,LOG_OUTPUT_FILE);
						APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,strRet,LOG_OUTPUT_FILE);
						Sleep(20);
						////
						//if(strRet.find("200") != -1)
						//{
						//	time_t curt = time(0);
						//	string DevRetval;
						//	while(((time(0)-curt)<30))
						//	{
						//		DevRetval = DEVRETSERVER::instance()->GetDevRet(MsgID);
						//		if(DevRetval.size()>0)
						//		{
						//			cout<<"收到结果**:"<<DevRetval<<endl;
						//			APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,DevRetval,LOG_OUTPUT_FILE);
						//			break;
						//		}
						//		else
						//		{
						//			Sleep(200);
						//			cout<<"设置OSD延时"<<endl;
						//		}
						//	}
						//}
						//***

						vector<int> codlis;
						PROPMANAGER::instance()->SeparateStrVec(CoderDeviceID,codlis);
						if(codlis.size()>1)
						{
							for(int ic=0;ic<codlis.size();ic++)
							{
								CoderDev tmcdev;
								PROPMANAGER::instance()->GetCoderInfo(codlis[ic],tmcdev);
								DBMANAGER::instance()->GetMsgID(MsgID);
								msgid=StrUtil::Str2Int(MsgID);
								DBMANAGER::instance()->UpdateMsgID(StrUtil::Int2Str(++msgid));
								cmd = "<?xml version=\"1.0\" encoding=\"gb2312\"?>";
								cmd += "<Msg Version=\"1.0\" MsgID=\"";
								cmd += MsgID;
								cmd += "\" SrcUrl=\"http://";
								cmd += Surl;
								cmd += "\">";
								cmd += "<Type>SetOutputInfo</Type>";
								cmd += "<Data>";
								cmd += "<Channel>";
								cmd += 	tmcdev.s_coderindex;
								cmd += "</Channel>";
								cmd += "<TSOut>";
								cmd += "<Switch>0</Switch>";
								cmd += "</TSOut>";
								cmd += "<Monitor>";
								cmd += "<Switch>0</Switch>";
								cmd += "</Monitor>";
								cmd += "</Data>";
								cmd += "</Msg>";
								cout<<"发送命令地址:"<<codemonitorip<<":"<<codemonitorport<<endl;
								cout<<"发送命令:"<<cmd<<endl;
								pTcodeDeviceAccess->SendTaskMsg(cmd,strRet);
								cout<<"收到结果:"<<strRet<<endl;
								sendaddress = "发送命令地址:"+codemonitorip+":"+codemonitorport;
								APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,sendaddress,LOG_OUTPUT_FILE);
								APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,cmd,LOG_OUTPUT_FILE);
								APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,strRet,LOG_OUTPUT_FILE);
								Sleep(20);
							}
						}


						DBMANAGER::instance()->GetMsgID(MsgID);
						msgid=StrUtil::Str2Int(MsgID);
						DBMANAGER::instance()->UpdateMsgID(StrUtil::Int2Str(++msgid));
						//
						cmd = "<?xml version=\"1.0\" encoding=\"gb2312\"?>";
						cmd += "<Msg Version=\"1.0\" MsgID=\"";
						cmd += MsgID;
						cmd += "\" SrcUrl=\"http://";
						cmd += Surl;
						cmd += "\">";
						cmd += "<Type>SetTcorderInfo</Type>";
						cmd += "<Data>";
						cmd += "<Channel>";
						//vector<int> codlis;
						//PROPMANAGER::instance()->SeparateStrVec(CoderDeviceID,codlis);
						if(codlis.size()>1)
						{
							for(int ic=0;ic<codlis.size();ic++)
							{
								CoderDev tmcdev;
								PROPMANAGER::instance()->GetCoderInfo(codlis[ic],tmcdev);
								if(ic == 0)
								{
									cmd += 	tmcdev.s_coderindex;
								}
								else
								{
									cmd += 	string(",");
									cmd += 	tmcdev.s_coderindex;
								}
							}
						}
						else
						{
							cmd += 	cdev.s_coderindex;
						}
						cmd += "</Channel>";
						cmd += "<In>";
						cmd += "<SrcIP>";
						cmd += 	tdev.s_sip;
						cmd += 	":";
						cmd += 	tdev.s_stsport;
						cmd += "</SrcIP>";
						cmd += "<VCode>";
						cmd += 	"1";
						cmd += "</VCode>";
						cmd += "<ACode>";
						cmd += 	"3";
						cmd += "</ACode>";
						cmd += "<ServiceID>";
						cmd += 	ServiceID;
						cmd += "</ServiceID>";
						cmd += "<VPID>";
						cmd += 	VideoPID;
						cmd += "</VPID>";
						cmd += "<APIDList>";
						cmd += "<APID>";
						cmd += 	AudioPID;
						cmd += "</APID>";
						cmd += "</APIDList>";
						cmd += "<ServiceType>";
						if(VideoPID.size()<=0||VideoPID == "0")
						{
							cmd += "0";
						}
						else
						{
							cmd += "1";
						}
						cmd += "</ServiceType>";
						cmd += "</In>";
						cmd += "<Out>";
						cmd += "<VCode>";
						cmd += 	"2";
						cmd += "</VCode>";
						cmd += "<ACode>";
						cmd += 	"3";
						cmd += "</ACode>";
						cmd += "<Width>";
						cmd += 	strW;
						cmd += "</Width>";
						cmd += "<Height>";
						cmd += 	strH;
						cmd += "</Height>";
						cmd += "<FrameRate>";
						cmd += 	"25";
						cmd += "</FrameRate>";
						cmd += "<VRate>";
						cmd += 	StrUtil::Int2Str(ivBps);
						cmd += "</VRate>";
						cmd += "<ARate>";
						cmd += 	sAudioBps;
						cmd += "</ARate>";
						cmd += "</Out>";
						cmd += "</Data>";
						cmd += "</Msg>";
						cout<<"发送命令地址:"<<codemonitorip<<":"<<codemonitorport<<endl;
						cout<<TimeUtil::GetCurDateTime()<<"发送命令:"<<cmd<<endl;
						pTcodeDeviceAccess->SendTaskMsg(cmd,strRet);
						cout<<TimeUtil::GetCurDateTime()<<"收到结果:"<<strRet<<endl;
						sendaddress = "发送命令地址:"+codemonitorip+":"+codemonitorport;
						APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,sendaddress,LOG_OUTPUT_FILE);
						APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,cmd,LOG_OUTPUT_FILE);
						APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,strRet,LOG_OUTPUT_FILE);
						Sleep(20);
						////
						//if(strRet.find("200") != -1)
						//{
						//	time_t curt = time(0);
						//	string DevRetval;
						//	while(((time(0)-curt)<30))
						//	{
						//		DevRetval = DEVRETSERVER::instance()->GetDevRet(MsgID);
						//		if(DevRetval.size()>0)
						//		{
						//			cout<<"收到结果**:"<<DevRetval<<endl;
						//			APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,DevRetval,LOG_OUTPUT_FILE);
						//			break;
						//		}
						//		else
						//		{
						//			Sleep(200);
						//			cout<<"转码延时"<<endl;
						//		}
						//	}
						//}
						//***
						
						//
						DBMANAGER::instance()->GetMsgID(MsgID);
						msgid=StrUtil::Str2Int(MsgID);
						DBMANAGER::instance()->UpdateMsgID(StrUtil::Int2Str(++msgid));
						//
						cmd = "<?xml version=\"1.0\" encoding=\"gb2312\"?>";
						cmd += "<Msg Version=\"1.0\" MsgID=\"";
						cmd += MsgID;
						cmd += "\" SrcUrl=\"http://";
						cmd += Surl;
						cmd += "\">";
						cmd += "<Type>SetOutputInfo</Type>";
						cmd += "<Data>";
						cmd += "<Channel>";
						cmd += 	cdev.s_coderindex;
						cmd += "</Channel>";
						cmd += "<TSOut>";
						cmd += "<Switch>1</Switch>";
						cmd += "</TSOut>";
						cmd += "<Monitor>";
						cmd += "<Switch>1</Switch>";
						cmd += "</Monitor>";
						cmd += "</Data>";
						cmd += "</Msg>";
						cout<<"发送命令地址:"<<codemonitorip<<":"<<codemonitorport<<endl;
						cout<<"发送命令:"<<cmd<<endl;
						pTcodeDeviceAccess->SendTaskMsg(cmd,strRet);
						cout<<"收到结果:"<<strRet<<endl;
						sendaddress = "发送命令地址:"+codemonitorip+":"+codemonitorport;
						APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,sendaddress,LOG_OUTPUT_FILE);
						APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,cmd,LOG_OUTPUT_FILE);
						APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,strRet,LOG_OUTPUT_FILE);
						Sleep(20);
					}
				}
				else
				{
					string cmd = "POST /io/config  HTTP/1.1  \r\n\r\n";
					cmd+="chl=";
					cmd+=tdev.s_cardindex;
					cmd+="_";
					cmd+=tdev.s_chanindex;
					cmd+="&fre=";
					cmd+=Freq;
					cmd+="&sym=";
					cmd+=freqscaninfo.SymbolRate;
					cmd+="&qam=2&ip=";
					cmd+=tdev.s_sip;
					cmd+="&tspt=";
					cmd+=tdev.s_stsport;
					cmd+="&trpt=";
					cmd+=tdev.s_trport;
					cmd+="&desc=";
					if(tdev.s_servidList.size() > 0)
					{
						cmd+=tdev.s_servidList;
					}
					else
					{
						cmd+="0";//ServiceID;
					}
					cmd+="&evip=";
					cmd+=tdev.s_290ip;
					cmd+="&evpt=";
					cmd+=tdev.s_290port;
					if(cmd != tdev.s_ExistCmd)
					{
						PROPMANAGER::instance()->SetExistCmd(tunid,DVBC,cmd);
					}	//
					pDeviceAccess->SendTaskMsg(cmd,strRet);
					//
					std::string msg = "通道:";
					msg += StrUtil::Int2Str(DeviceId);
					msg += "发送命令:";
					msg += cmd;
					APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,msg,LOG_OUTPUT_FILE);
					
					//***
					cmd = "POST /tcoder/config  HTTP/1.1  \r\n\r\n";
					strRet = "";
					cmd+="chl=";
					cmd+=cdev.s_coderindex;
					cmd+="&ena=";
					cmd+="1";
					cmd+="&chk=";
					cmd+="1";
					cmd+="&sip=";
					cmd+=tdev.s_sip;
					cmd+="&spt=";
					cmd+=tdev.s_stsport;
					cmd+="&spro=";
					if(CHANNELINFOMGR::instance()->IsCAFree(DVBC,Freq,ServiceID))
					{
						cmd+=ServiceID;
					}
					else
					{
						int iSerid = StrUtil::Str2Int(ServiceID);
						iSerid += 0x10000;
						ServiceID = StrUtil::Int2Str(iSerid);
						cmd+=ServiceID;
					}
					cmd+="&svid=";
					if(VideoPID.size()<=0)
					{
						cmd+="0";
					}
					else
					{
						cmd+=VideoPID;
					}
					cmd+="&saud0=";
					cmd+=AudioPID;
					cmd+="&saud1=";
					cmd+="0";
					cmd+="&saud2=";
					cmd+="0";
					cmd+="&vcode=";
					cmd+="h264";
					cmd+="&acode=";
					if((version == "3.0")&&(IsTranAudio == "1"))
					{
						cmd+="copy";
					}
					else
					{
						cmd+="mpeg";
					}
					cmd+="&vfmt=";
					if ((PROPMANAGER::instance()->GetCurWorkVersion() == "1.0")||(PROPMANAGER::instance()->GetCurWorkVersion() == "2.0"))
					{
						cmd+=vfmt;
					}
					else
					{
						cmd+=strW;
						cmd+="_";
						cmd+=strH;
					}
					cmd+="&vfre=";
					cmd+="0";
					cmd+="&v1v1=";
					cmd+="2";
					cmd+="&mutehold=";
					//cmd+="-70";
					cmd+= alarmParam.s_mutehold;
					cmd+="&bfhold=";
					//cmd+="20";
					cmd+= alarmParam.s_bfhold;
					cmd+="&sthold=";
					//cmd+="1.5";
					cmd+= alarmParam.s_sthold;
					cmd+="&vrate=";
					cmd+=StrUtil::Int2Str(ivBps);
					cmd+="&arate=";
					cmd+=sAudioBps;
					cmd+="&srate=";
					cmd+=strbps;
					cmd+="&osd0=";
					if (version == "3.0" && isosd == "1")
					{
						cmd+= antiColor;
						cmd+="_";
					}
					cmd+=strosdtimeX;
					cmd+="_";
					cmd+=strosdtimeY;
					cmd+="_";
					cmd+=striosdfontsize;
					cmd+="_";
					if(isosd == "1")
					{
						cmd+=utf8_time;
					}
					cmd+="&osd1=";
					if (version == "3.0" && isosd == "1")
					{
						cmd+= antiColor;
						cmd+="_";
					}
					cmd+=strosdprogramX;
					cmd+="_";
					cmd+=strosdprogramY;
					cmd+="_";
					cmd+=striosdfontsize;
					cmd+="_";
					if(isosd == "1")
					{
						cmd+=utf8_proname;//proname;
					}
					cmd+="&osd2=";
					if (version == "3.0" && isosd == "1")
					{
						cmd+= antiColor;
						cmd+="_";
					}
					cmd+=strosdinfoX;
					cmd+="_";
					cmd+=strosdinfoY;
					cmd+="_";
					cmd+=striosdfontsize;
					cmd+="_";
					if(isosd == "1")
					{
						cmd+=utf8_osdinfo;//osdinfo;
					}
					cmd+="&dip=";
					cmd+=cdev.s_dip;
					cmd+="&dpt=";
					cmd+=cdev.s_dtsport;
					cmd+="&evip=";
					cmd+=cdev.s_alarmip;
					cmd+="&evpt=";
					cmd+=cdev.s_alarmport;
					pDeviceAccess->SendTaskMsg(cmd,strRet);
					//
					msg = "通道:";
					msg += StrUtil::Int2Str(DeviceId);
					msg += "发送命令:";
					msg += cmd;
					APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,msg,LOG_OUTPUT_FILE);
				}
			}
			////
			delete pDeviceAccess;
		}
	}
	return true;
}

bool UDPTsFetcher::SendTsDeviceXmlForCTTB(std::string devXml)
{
	std::string PmtPID,PcrPID,VideoPID,AudioPID ,Freq,ServiceID,strW,strH,strbps,striosdfontsize,osdinfo,strosdinfoX,strosdinfoY,strosdtimeX,strosdtimeY,proname,strosdprogramX,strosdprogramY,isosd;
	string version,TunerDeviceID,CoderDeviceID,IsTranAudio;
	string strComamdType = "";
	XmlParser tmpparser(devXml.c_str());
	pXMLNODE TsNode = tmpparser.GetNodeFromPath( "Msg/TSQuery/TS" );
	tmpparser.GetAttrNode(TsNode,"ServiceID",ServiceID);
	tmpparser.GetAttrNode(TsNode,"Version",version);
	tmpparser.GetAttrNode(TsNode,"TDeviceID",TunerDeviceID);
	tmpparser.GetAttrNode(TsNode,"CDeviceID",CoderDeviceID);
	tmpparser.GetAttrNode(TsNode,"ComamdType",strComamdType);
	if(strComamdType.size()<=0)
	{
		comamdtype = 3;
	}
	tmpparser.GetAttrNode(TsNode,"PmtPID",PmtPID);
	tmpparser.GetAttrNode(TsNode,"PcrPID",PcrPID);
	tmpparser.GetAttrNode(TsNode,"VideoPID",VideoPID);
	tmpparser.GetAttrNode(TsNode,"Width",strW);
	tmpparser.GetAttrNode(TsNode,"Height",strH);
	tmpparser.GetAttrNode(TsNode,"Bps",strbps);
	tmpparser.GetAttrNode(TsNode,"OSDFontSize",striosdfontsize);
	tmpparser.GetAttrNode(TsNode,"OSDInfo",osdinfo);
	tmpparser.GetAttrNode(TsNode,"OSDInfoX",strosdinfoX);
	tmpparser.GetAttrNode(TsNode,"OSDInfoY",strosdinfoY);
	tmpparser.GetAttrNode(TsNode,"OSDTimeX",strosdtimeX);
	tmpparser.GetAttrNode(TsNode,"OSDTimeY",strosdtimeY);
	tmpparser.GetAttrNode(TsNode,"ServiceName",proname);
	tmpparser.GetAttrNode(TsNode,"OSDProgramX",strosdprogramX);
	tmpparser.GetAttrNode(TsNode,"OSDProgramY",strosdprogramY);
	tmpparser.GetAttrNode(TsNode,"OSDEnable",isosd);
	//VideoPID = "9024";
	tmpparser.GetAttrNode(TsNode,"AudioPID",AudioPID);
	//AudioPID = "834";
	tmpparser.GetAttrNode(TsNode,"Freq",Freq);

	std::string msg;
	if (Freq != strHDMIFreq)
	{
		msg = string("通道[") + StrUtil::Int2Str(DeviceId)+ string("]指令:") + \
			string("PMT[") + PmtPID + string("] PCR[") + PcrPID + string("] VPID[") + \
			VideoPID + string("] APID[") + AudioPID + string("]");

		//SYSMSGSENDER::instance()->SendMsg(msg);
	}
	size_t sz = 0; 
	string strtime = "time";
	string utf8_time;
	string utf8_proname;
	string utf8_osdinfo;
	//
	if(strtime.size()>0)
	{
		utf8_time = strtime;
		ConvertGBKToUtf8(utf8_time);
	}
	//
	if(proname.size()>0)
	{
		utf8_proname = proname;
		ConvertGBKToUtf8(utf8_proname);
	}
	if(osdinfo.size()>0)
	{
		utf8_osdinfo = osdinfo;
		ConvertGBKToUtf8(utf8_osdinfo);
	}

	int ivBps = StrUtil::Str2Int(strbps);
	string sAudioBps = "32";
	if (ivBps < 300)
	{
		sAudioBps = "32";
	}
	else if ((ivBps > 300) && (ivBps < 450))
	{
		sAudioBps = "32";
	}
	else if (ivBps > 450)
	{
		sAudioBps = "32";
	}
	
	int isBps = (ivBps + StrUtil::Str2Int(sAudioBps))*1.1;
	//
	int iosdfontsize = StrUtil::Str2Int(striosdfontsize);
	if(iosdfontsize < 8)
	{
		iosdfontsize = 8;
		striosdfontsize = StrUtil::Int2Str(iosdfontsize);
	}

	//
	string vfmt;
	if(((!(StrUtil::Str2Int(strW)>640||StrUtil::Str2Int(strH)>480))&&(PROPMANAGER::instance()->GetCurWorkVersion() != "5.0"))
		||((!(StrUtil::Str2Int(strW)>640||StrUtil::Str2Int(strH)>480))&&(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")))
	{
		if(ivBps < 250)
		{
			ivBps = 250;
			isBps = (ivBps + StrUtil::Str2Int(sAudioBps))*1.1;
			strbps = StrUtil::Int2Str(isBps);
		}
		vfmt ="2";
		//避免osd位置异常
		if ((PROPMANAGER::instance()->GetCurWorkVersion() == "1.0")||(PROPMANAGER::instance()->GetCurWorkVersion() == "2.0"))
		{
			strW = "352";
			strH = "288";
		}
		//
	}
	else
	{
		if(ivBps < 350)
		{
			ivBps = 350;
			isBps = (ivBps + StrUtil::Str2Int(sAudioBps))*1.1;
			strbps = StrUtil::Int2Str(isBps);
		}
		iosdfontsize = 30;
		striosdfontsize = StrUtil::Int2Str(iosdfontsize);
		vfmt ="1";
		//避免osd位置异常
		if ((PROPMANAGER::instance()->GetCurWorkVersion() == "1.0")||(PROPMANAGER::instance()->GetCurWorkVersion() == "2.0"))
		{
			strW = "720";
			strH = "576";
		}
		//
	}
	string scurTime = TimeUtil::GetCurDateTime();
	int iosdTimeLen = scurTime.length();
	int iosdProLen = proname.length();
	int iosdInfoLen = osdinfo.length();
	int iosdChannelLen = proname.length() + osdinfo.length();
	string antiColor = "1";

	//jsp2017.07.15
	OSDFormat osdf;
	bool bosdsetfind = false;

	if (iosdTimeLen - iosdChannelLen - 1 > 0)
	{
		//OSD位置控制计算 默认右上角右对齐
		strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*iosdfontsize/2);
		strosdinfoY = StrUtil::Int2Str(iosdfontsize/2);
		strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*iosdfontsize/2);
		strosdprogramY = StrUtil::Int2Str(iosdfontsize/2);
		strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*iosdfontsize/2);
		strosdtimeY = StrUtil::Int2Str(iosdfontsize*3/2);

		for(int i=0;i<gOSDFormatVec.size();i++)
		{
			if((gOSDFormatVec[i].Freq == Freq)&&(gOSDFormatVec[i].ServiceID == ServiceID))
			{
				osdf = gOSDFormatVec[i];
				bosdsetfind = true;
				break;
			}
		}
		if(bosdsetfind)
		{	
			int ifontsize = StrUtil::Str2Int(osdf.FontSize);
			if(osdf.Position == "0")
			{
				if(osdf.Align == "0")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize*3/2);
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY =  StrUtil::Int2Str(ifontsize*3/2);
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen+3)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
				}
				else if(osdf.Align == "1")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(((iosdTimeLen-iosdChannelLen-1)/2+1)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str(((iosdTimeLen-iosdChannelLen-1)/2+iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(((iosdTimeLen-iosdChannelLen-1)/2+1)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize*3/2);
						strosdprogramX = StrUtil::Int2Str(((iosdTimeLen-iosdChannelLen-1)/2+iosdInfoLen+2)*ifontsize/2);
						strosdprogramY =  StrUtil::Int2Str(ifontsize*3/2);
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen+3)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
				}
				else if(osdf.Align == "2")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str((iosdTimeLen-iosdChannelLen)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str((iosdTimeLen-iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str((iosdTimeLen-iosdChannelLen)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize*3/2);
						strosdprogramX = StrUtil::Int2Str((iosdTimeLen-iosdProLen+1)*ifontsize/2);
						strosdprogramY =  StrUtil::Int2Str(ifontsize*3/2);
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen+3)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}

				}
			}
			else if(osdf.Position == "1")
			{
				if(osdf.Align == "0")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen-iosdInfoLen)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize*3/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen-iosdInfoLen)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize*3/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdChannelLen+3)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdProLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
				}
				else if(osdf.Align == "1")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen-(iosdTimeLen-iosdChannelLen-1)/2+1)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen-(iosdTimeLen-iosdChannelLen-1)/2-iosdInfoLen)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen-(iosdTimeLen-iosdChannelLen-1)/2+1)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize*3/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen-(iosdTimeLen-iosdChannelLen-1)/2-iosdInfoLen)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize*3/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdChannelLen+3)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdProLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
				}
				else if(osdf.Align == "2")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize*3/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize*3/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdChannelLen+3)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdProLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
				}
			}
			else if(osdf.Position == "2")
			{
				if(osdf.Align == "0")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*5/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen+3)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
				}
				else if(osdf.Align == "1")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(((iosdTimeLen-iosdChannelLen-1)/2+1)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdprogramX = StrUtil::Int2Str(((iosdTimeLen-iosdChannelLen-1)/2+iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(((iosdTimeLen-iosdChannelLen-1)/2+1)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str(((iosdTimeLen-iosdChannelLen-1)/2+iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*5/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen+3)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}	
				}
				else if(osdf.Align == "2")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str((iosdTimeLen-iosdChannelLen)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdprogramX = StrUtil::Int2Str((iosdTimeLen-iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str((iosdTimeLen-iosdChannelLen)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str((iosdTimeLen-iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*5/2);

					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen+3)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}	
				}
			}
			else if(osdf.Position == "3")
			{
				if(osdf.Align == "0")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen-iosdInfoLen)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen-iosdInfoLen)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*5/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdChannelLen+3)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdProLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
				}
				else if(osdf.Align == "1")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen-(iosdTimeLen-iosdChannelLen-1)/2+1)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen-(iosdTimeLen-iosdChannelLen-1)/2-iosdInfoLen)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);		
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen-(iosdTimeLen-iosdChannelLen-1)/2+1)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen-(iosdTimeLen-iosdChannelLen-1)/2-iosdInfoLen)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*5/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdChannelLen+3)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdProLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
				}
				else if(osdf.Align == "2")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);		
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*5/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdChannelLen+3)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdProLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
				}
			}
			striosdfontsize = StrUtil::Int2Str(ifontsize);
		}
	}
	else
	{
		//OSD位置控制计算 默认右上角右对齐
		strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*iosdfontsize/2);
		strosdinfoY = StrUtil::Int2Str(iosdfontsize/2);
		strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*iosdfontsize/2);
		strosdprogramY = StrUtil::Int2Str(iosdfontsize/2);
		strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*iosdfontsize/2);
		strosdtimeY = StrUtil::Int2Str(iosdfontsize*3/2);

		for(int i=0;i<gOSDFormatVec.size();i++)
		{
			if((gOSDFormatVec[i].Freq == Freq)&&(gOSDFormatVec[i].ServiceID == ServiceID))
			{
				osdf = gOSDFormatVec[i];
				bosdsetfind = true;
				break;
			}
		}
		if(bosdsetfind)
		{
			int ifontsize = StrUtil::Str2Int(osdf.FontSize);
			if(osdf.Position == "0")
			{
				if(osdf.Align == "0")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize*3/2);
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY =  StrUtil::Int2Str(ifontsize*3/2);
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen+3)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
				}
				else if(osdf.Align == "1")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(((iosdChannelLen-iosdTimeLen+2)/2+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize*3/2);
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY =  StrUtil::Int2Str(ifontsize*3/2);
						strosdtimeX = StrUtil::Int2Str(((iosdChannelLen-iosdTimeLen+2)/2+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen+3)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
				}
				else if(osdf.Align == "2")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen-iosdTimeLen+2)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize*3/2);
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY =  StrUtil::Int2Str(ifontsize*3/2);
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen-iosdTimeLen+2)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen+3)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
				}
			}
			else if(osdf.Position == "1")
			{
				if(osdf.Align == "0")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize*3/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize*3/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdChannelLen+3)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdProLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
				}
				else if(osdf.Align == "1")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen-(iosdChannelLen-iosdTimeLen+1)/2+2)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize*3/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize*3/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen-(iosdChannelLen-iosdTimeLen+1)/2+2)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdChannelLen+3)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdProLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
				}
				else if(osdf.Align == "2")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize*3/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize*3/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdChannelLen+3)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdProLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
				}
			}
			else if(osdf.Position == "2")
			{
				if(osdf.Align == "0")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*5/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen+3)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
				}
				else if(osdf.Align == "1")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdtimeX = StrUtil::Int2Str(((iosdChannelLen-iosdTimeLen+2)/2+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(((iosdChannelLen-iosdTimeLen+2)/2+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*5/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen+3)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}			
				}
				else if(osdf.Align == "2")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen-iosdTimeLen+2)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen-iosdTimeLen+2)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*5/2);

					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen+3)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}		
				}
			}
			else if(osdf.Position == "3")
			{
				if(osdf.Align == "0")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*5/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdChannelLen+3)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdProLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
				}
				else if(osdf.Align == "1")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen-(iosdChannelLen-iosdTimeLen+1)/2+2)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);		
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen-(iosdChannelLen-iosdTimeLen+1)/2+2)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*5/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdChannelLen+3)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdProLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
				}
				else if(osdf.Align == "2")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);		
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*5/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdChannelLen+3)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdProLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
				}
			}
			striosdfontsize = StrUtil::Int2Str(ifontsize);
		}
	}

	AlarmParamConfig alarmParam;
	PROPMANAGER::instance()->GetAlarmParamConfig(UNKNOWN, alarmParam);
	if (alarmParam.s_bfhold == "" || alarmParam.s_mutehold == "" || alarmParam.s_sthold == "")
	{
		alarmParam.s_bfhold = "20";
		alarmParam.s_mutehold = "-70";
		alarmParam.s_sthold = "1.5";
	}

	TunerDev tdev;
	CoderDev cdev;
	eDVBType edtype = CTTB;
	string tunermonitorip,tunermonitorport,codemonitorip,codemonitorport;
	int tunid = -1;
	if(version == "3.0")
	{
		tunid = StrUtil::Str2Int(TunerDeviceID);
		PROPMANAGER::instance()->GetTunerInfo(StrUtil::Str2Int(TunerDeviceID),edtype,tdev);
		PROPMANAGER::instance()->GetDevMonitorInfo(0,edtype,StrUtil::Str2Int(TunerDeviceID),tunermonitorip,tunermonitorport);//tuner
	}
	else
	{
		if(PROPMANAGER::instance()->IsUnUseRecDev(1,edtype,DeviceId))//实时视频和频道轮播使用的tuner和coder的编号相同
		{
			tunid = DeviceId;
			PROPMANAGER::instance()->GetTunerInfo(DeviceId,edtype,tdev);
			if (Freq != strHDMIFreq)
			{
				string servlist;
				CHANNELINFOMGR::instance()->GetServIdListByFreq(edtype,Freq,ServiceID,"",tdev.s_devicecampronum,servlist);
				tdev.s_servidList = ServiceID;//servlist;特殊处理，8路卡不够，实时通道实际插2路卡，配置文件配置8路便于录像分配，实时通道不能按8路处理
			}
			PROPMANAGER::instance()->GetDevMonitorInfo(0,edtype,DeviceId,tunermonitorip,tunermonitorport);//tuner
		}
		else
		{
			PROPMANAGER::instance()->GetTunerDevFromCoderDev(tunid,edtype,DeviceId);
			PROPMANAGER::instance()->GetTunerInfo(tunid,edtype,tdev);
			PROPMANAGER::instance()->GetDevMonitorInfo(0,edtype,tunid,tunermonitorip,tunermonitorport);//tuner
			string temp;
			temp = "DeviceId:";
			temp+=StrUtil::Int2Str(DeviceId);
			temp+="****freq:";
			temp+=Freq;
			temp+="****servceid:";
			temp+=ServiceID;
			temp+="****tunerid:";
			temp+=StrUtil::Int2Str(tunid);
			APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,temp,LOG_OUTPUT_FILE);
		}
	}
	PROPMANAGER::instance()->GetCoderInfo(DeviceId,cdev);
	PROPMANAGER::instance()->GetDevMonitorInfo(1,edtype,DeviceId,codemonitorip,codemonitorport);//coder
	if((tunermonitorip == codemonitorip)&&(tunermonitorport == codemonitorport))
	{
		if(tunermonitorip.size()>0&&tunermonitorport.size()>0)
		{
			HTTPDeviceAccess* pDeviceAccess = new HTTPDeviceAccess(StrUtil::Str2Int(tdev.s_monitorindex),tunermonitorip,StrUtil::Str2Int(tunermonitorport));
			string strRet;
			string cmd = "POST /io/config  HTTP/1.1  \r\n\r\n";
			//
			cmd+="chl=";
			cmd+=tdev.s_cardindex;
			cmd+="_";
			cmd+=tdev.s_chanindex;
			cmd+="&fre=";
			cmd+=Freq;
			cmd+="&bandwith=8&spectrum=0&ip=";
			cmd+=tdev.s_sip;
			cmd+="&tspt=";
			cmd+=tdev.s_stsport;
			cmd+="&trpt=";
			cmd+=tdev.s_trport;
			cmd+="&desc=";
			if(tdev.s_servidList.size() > 0)
			{
				cmd+=tdev.s_servidList;
			}
			else
			{
				cmd+="0";//ServiceID;
			}
			cmd+="&evip=";
			cmd+=tdev.s_290ip;
			cmd+="&evpt=";
			cmd+=tdev.s_290port;
			pDeviceAccess->SendTaskMsg(cmd,strRet);
			std::string msg = "通道:";
			msg += StrUtil::Int2Str(DeviceId);
			msg += "发送命令:";
			msg += cmd;
			APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,msg,LOG_OUTPUT_FILE);
			//***
			cmd = "POST /tcoder/config  HTTP/1.1  \r\n\r\n";
			strRet = "";
			cmd+="chl=";
			cmd+=cdev.s_coderindex;
			cmd+="&ena=";
			cmd+="1";
			cmd+="&chk=";
			cmd+="1";
			cmd+="&sip=";
			cmd+=tdev.s_sip;
			cmd+="&spt=";
			cmd+=tdev.s_stsport;
			cmd+="&spro=";
			cmd+=ServiceID;
			cmd+="&svid=";
			cmd+=VideoPID;
			cmd+="&saud0=";
			cmd+=AudioPID;
			cmd+="&saud1=";
			cmd+="0";
			cmd+="&saud2=";
			cmd+="0";
			cmd+="&vcode=";
			cmd+="h264";
			cmd+="&acode=";
			cmd+="mpeg";
			cmd+="&vfmt=";
			if(((!(StrUtil::Str2Int(strW)>640||StrUtil::Str2Int(strH)>480))&&(PROPMANAGER::instance()->GetCurWorkVersion() != "5.0"))
				||((!(StrUtil::Str2Int(strW)>640||StrUtil::Str2Int(strH)>480))&&(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")))
			{
				if(isBps < 250)
				{
					isBps = 250;
					strbps = StrUtil::Int2Str(isBps);
					ivBps = isBps - 32 - 50;
				}
			}
			else
			{
				if(isBps < 350)
				{
					isBps = 350;
					strbps = StrUtil::Int2Str(isBps);
					ivBps = isBps - 32 - 50;
				}
			}
			string sAudioBps;
			if (isBps < 300)
				sAudioBps = "32";
			else if ((isBps > 300) && (isBps < 450))
				sAudioBps = "32";
			else if (isBps > 450)
				sAudioBps = "32";
			cmd+=strW;
			cmd+="_";
			cmd+=strH;
			cmd+="&vfre=";
			cmd+="0";
			cmd+="&v1v1=";
			cmd+="2";
			cmd+="&mutehold=";
			//cmd+="-70";
			cmd+= alarmParam.s_mutehold;
			cmd+="&bfhold=";
			//cmd+="20";
			cmd+= alarmParam.s_bfhold;
			cmd+="&sthold=";
			//cmd+="1.5";
			cmd+= alarmParam.s_sthold;
			cmd+="&vrate=";
			cmd+=StrUtil::Int2Str(ivBps);
			cmd+="&arate=";
			cmd+=sAudioBps;
			cmd+="&srate=";
			cmd+=strbps;
			cmd+="&osd0=";
			cmd+=strosdtimeX;
			cmd+="_";
			cmd+=strosdtimeY;
			cmd+="_";
			cmd+=striosdfontsize;
			cmd+="_";
			if(isosd == "1")
			{
				cmd+=utf8_time;
			}
			cmd+="&osd1=";
			cmd+=strosdprogramX;
			cmd+="_";
			cmd+=strosdprogramY;
			cmd+="_";
			cmd+=striosdfontsize;
			cmd+="_";
			if(isosd == "1")
			{
				cmd+=utf8_proname;//proname;
			}
			cmd+="&osd2=";
			cmd+=strosdinfoX;
			cmd+="_";
			cmd+=strosdinfoY;
			cmd+="_";
			cmd+=striosdfontsize;
			cmd+="_";
			if(isosd == "1")
			{
				cmd+=utf8_osdinfo;//osdinfo;
			}
			cmd+="&dip=";
			cmd+=cdev.s_dip;
			cmd+="&dpt=";
			cmd+=cdev.s_dtsport;
			cmd+="&evip=";
			cmd+=cdev.s_alarmip;
			cmd+="&evpt=";
			cmd+=cdev.s_alarmport;
			pDeviceAccess->SendTaskMsg(cmd,strRet);
			//
			msg = "通道:";
			msg += StrUtil::Int2Str(DeviceId);
			msg += "发送命令:";
			msg += cmd;
			APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,msg,LOG_OUTPUT_FILE);
			////
			delete pDeviceAccess;
		}
	}


	return true;
}
bool UDPTsFetcher::SendTsDeviceXmlForDVBS(std::string devXml)
{
	std::string PmtPID,PcrPID,VideoPID,AudioPID ,Freq,ServiceID,strW,strH,strbps,striosdfontsize,osdinfo,strosdinfoX,strosdinfoY,strosdtimeX,strosdtimeY,proname,strosdprogramX,strosdprogramY,isosd;
	string version,TunerDeviceID,CoderDeviceID,IsTranAudio;
	string strComamdType = "";
	XmlParser tmpparser(devXml.c_str());
	pXMLNODE TsNode = tmpparser.GetNodeFromPath( "Msg/TSQuery/TS" );
	tmpparser.GetAttrNode(TsNode,"ServiceID",ServiceID);
	tmpparser.GetAttrNode(TsNode,"Version",version);
	tmpparser.GetAttrNode(TsNode,"TDeviceID",TunerDeviceID);
	tmpparser.GetAttrNode(TsNode,"CDeviceID",CoderDeviceID);
	tmpparser.GetAttrNode(TsNode,"ComamdType",strComamdType);
	if(strComamdType.size()<=0)
	{
		comamdtype = 3;
	}
	tmpparser.GetAttrNode(TsNode,"PmtPID",PmtPID);
	tmpparser.GetAttrNode(TsNode,"PcrPID",PcrPID);
	tmpparser.GetAttrNode(TsNode,"VideoPID",VideoPID);
	tmpparser.GetAttrNode(TsNode,"Width",strW);
	tmpparser.GetAttrNode(TsNode,"Height",strH);
	tmpparser.GetAttrNode(TsNode,"Bps",strbps);
	tmpparser.GetAttrNode(TsNode,"OSDFontSize",striosdfontsize);
	tmpparser.GetAttrNode(TsNode,"OSDInfo",osdinfo);
	tmpparser.GetAttrNode(TsNode,"OSDInfoX",strosdinfoX);
	tmpparser.GetAttrNode(TsNode,"OSDInfoY",strosdinfoY);
	tmpparser.GetAttrNode(TsNode,"OSDTimeX",strosdtimeX);
	tmpparser.GetAttrNode(TsNode,"OSDTimeY",strosdtimeY);
	tmpparser.GetAttrNode(TsNode,"ServiceName",proname);
	tmpparser.GetAttrNode(TsNode,"OSDProgramX",strosdprogramX);
	tmpparser.GetAttrNode(TsNode,"OSDProgramY",strosdprogramY);
	tmpparser.GetAttrNode(TsNode,"OSDEnable",isosd);
	//VideoPID = "9024";
	tmpparser.GetAttrNode(TsNode,"AudioPID",AudioPID);
	//AudioPID = "834";
	tmpparser.GetAttrNode(TsNode,"Freq",Freq);
	//
	sdvbsfreqinfo dvbsf;
	CHANSCANTHREAD::instance()->GetDvbsFreqInfo(Freq,dvbsf);
	//
	std::string msg;
	if (Freq != strHDMIFreq)
	{
		msg = string("通道[") + StrUtil::Int2Str(DeviceId)+ string("]指令:") + \
			string("PMT[") + PmtPID + string("] PCR[") + PcrPID + string("] VPID[") + \
			VideoPID + string("] APID[") + AudioPID + string("]");

		//SYSMSGSENDER::instance()->SendMsg(msg);
	}

	size_t sz = 0; 
	string strtime = "time";
	string utf8_time;
	string utf8_proname;
	string utf8_osdinfo;
	//
	if(strtime.size()>0)
	{
		utf8_time = strtime;
		ConvertGBKToUtf8(utf8_time);
	}
	//
	if(proname.size()>0)
	{
		utf8_proname = proname;
		ConvertGBKToUtf8(utf8_proname);
	}
	if(osdinfo.size()>0)
	{
		utf8_osdinfo = osdinfo;
		ConvertGBKToUtf8(utf8_osdinfo);
	}

	int ivBps = StrUtil::Str2Int(strbps);
	string sAudioBps = "32";
	if (ivBps < 300)
	{
		sAudioBps = "32";
	}
	else if ((ivBps > 300) && (ivBps < 450))
	{
		sAudioBps = "32";
	}
	else if (ivBps > 450)
	{
		sAudioBps = "32";
	}
	
	int isBps = (ivBps + StrUtil::Str2Int(sAudioBps))*1.1;
	//
	int iosdfontsize = StrUtil::Str2Int(striosdfontsize);
	if(iosdfontsize < 8)
	{
		iosdfontsize = 8;
		striosdfontsize = StrUtil::Int2Str(iosdfontsize);
	}

	//
	string vfmt;
	if(((!(StrUtil::Str2Int(strW)>640||StrUtil::Str2Int(strH)>480))&&(PROPMANAGER::instance()->GetCurWorkVersion() != "5.0"))
		||((!(StrUtil::Str2Int(strW)>640||StrUtil::Str2Int(strH)>480))&&(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")))
	{
		if(ivBps < 250)
		{
			ivBps = 250;
			isBps = (ivBps + StrUtil::Str2Int(sAudioBps))*1.1;
			strbps = StrUtil::Int2Str(isBps);
		}
		vfmt ="2";
		//避免osd位置异常
		if ((PROPMANAGER::instance()->GetCurWorkVersion() == "1.0")||(PROPMANAGER::instance()->GetCurWorkVersion() == "2.0"))
		{
			strW = "352";
			strH = "288";
		}
		//
	}
	else
	{
		if(ivBps < 350)
		{
			ivBps = 350;
			isBps = (ivBps + StrUtil::Str2Int(sAudioBps))*1.1;
			strbps = StrUtil::Int2Str(isBps);
		}
		iosdfontsize = 30;
		striosdfontsize = StrUtil::Int2Str(iosdfontsize);
		vfmt ="1";
		//避免osd位置异常
		if ((PROPMANAGER::instance()->GetCurWorkVersion() == "1.0")||(PROPMANAGER::instance()->GetCurWorkVersion() == "2.0"))
		{
			strW = "720";
			strH = "576";
		}
		//
	}
	string scurTime = TimeUtil::GetCurDateTime();
	int iosdTimeLen = scurTime.length();
	int iosdProLen = proname.length();
	int iosdInfoLen = osdinfo.length();
	int iosdChannelLen = proname.length() + osdinfo.length();
	string antiColor = "1";

	//jsp2017.07.15
	OSDFormat osdf;
	bool bosdsetfind = false;

	if (iosdTimeLen - iosdChannelLen - 1 > 0)
	{
		//OSD位置控制计算 默认右上角右对齐
		strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*iosdfontsize/2);
		strosdinfoY = StrUtil::Int2Str(iosdfontsize/2);
		strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*iosdfontsize/2);
		strosdprogramY = StrUtil::Int2Str(iosdfontsize/2);
		strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*iosdfontsize/2);
		strosdtimeY = StrUtil::Int2Str(iosdfontsize*3/2);

		for(int i=0;i<gOSDFormatVec.size();i++)
		{
			if((gOSDFormatVec[i].Freq == Freq)&&(gOSDFormatVec[i].ServiceID == ServiceID))
			{
				osdf = gOSDFormatVec[i];
				bosdsetfind = true;
				break;
			}
		}
		if(bosdsetfind)
		{	
			int ifontsize = StrUtil::Str2Int(osdf.FontSize);
			if(osdf.Position == "0")
			{
				if(osdf.Align == "0")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize*3/2);
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY =  StrUtil::Int2Str(ifontsize*3/2);
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen+3)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
				}
				else if(osdf.Align == "1")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(((iosdTimeLen-iosdChannelLen-1)/2+1)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str(((iosdTimeLen-iosdChannelLen-1)/2+iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(((iosdTimeLen-iosdChannelLen-1)/2+1)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize*3/2);
						strosdprogramX = StrUtil::Int2Str(((iosdTimeLen-iosdChannelLen-1)/2+iosdInfoLen+2)*ifontsize/2);
						strosdprogramY =  StrUtil::Int2Str(ifontsize*3/2);
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen+3)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
				}
				else if(osdf.Align == "2")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str((iosdTimeLen-iosdChannelLen)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str((iosdTimeLen-iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str((iosdTimeLen-iosdChannelLen)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize*3/2);
						strosdprogramX = StrUtil::Int2Str((iosdTimeLen-iosdProLen+1)*ifontsize/2);
						strosdprogramY =  StrUtil::Int2Str(ifontsize*3/2);
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen+3)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}

				}
			}
			else if(osdf.Position == "1")
			{
				if(osdf.Align == "0")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen-iosdInfoLen)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize*3/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen-iosdInfoLen)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize*3/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdChannelLen+3)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdProLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
				}
				else if(osdf.Align == "1")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen-(iosdTimeLen-iosdChannelLen-1)/2+1)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen-(iosdTimeLen-iosdChannelLen-1)/2-iosdInfoLen)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen-(iosdTimeLen-iosdChannelLen-1)/2+1)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize*3/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen-(iosdTimeLen-iosdChannelLen-1)/2-iosdInfoLen)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize*3/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdChannelLen+3)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdProLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
				}
				else if(osdf.Align == "2")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize*3/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize*3/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdChannelLen+3)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdProLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
				}
			}
			else if(osdf.Position == "2")
			{
				if(osdf.Align == "0")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*5/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen+3)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
				}
				else if(osdf.Align == "1")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(((iosdTimeLen-iosdChannelLen-1)/2+1)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdprogramX = StrUtil::Int2Str(((iosdTimeLen-iosdChannelLen-1)/2+iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(((iosdTimeLen-iosdChannelLen-1)/2+1)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str(((iosdTimeLen-iosdChannelLen-1)/2+iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*5/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen+3)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}	
				}
				else if(osdf.Align == "2")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str((iosdTimeLen-iosdChannelLen)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdprogramX = StrUtil::Int2Str((iosdTimeLen-iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str((iosdTimeLen-iosdChannelLen)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str((iosdTimeLen-iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*5/2);

					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen+3)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}	
				}
			}
			else if(osdf.Position == "3")
			{
				if(osdf.Align == "0")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen-iosdInfoLen)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen-iosdInfoLen)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*5/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdChannelLen+3)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdProLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
				}
				else if(osdf.Align == "1")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen-(iosdTimeLen-iosdChannelLen-1)/2+1)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen-(iosdTimeLen-iosdChannelLen-1)/2-iosdInfoLen)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);		
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen-(iosdTimeLen-iosdChannelLen-1)/2+1)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen-(iosdTimeLen-iosdChannelLen-1)/2-iosdInfoLen)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*5/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdChannelLen+3)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdProLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
				}
				else if(osdf.Align == "2")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);		
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*5/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdChannelLen+3)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdProLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
				}
			}
			striosdfontsize = StrUtil::Int2Str(ifontsize);
		}
	}
	else
	{
		//OSD位置控制计算 默认右上角右对齐
		strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*iosdfontsize/2);
		strosdinfoY = StrUtil::Int2Str(iosdfontsize/2);
		strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*iosdfontsize/2);
		strosdprogramY = StrUtil::Int2Str(iosdfontsize/2);
		strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*iosdfontsize/2);
		strosdtimeY = StrUtil::Int2Str(iosdfontsize*3/2);

		for(int i=0;i<gOSDFormatVec.size();i++)
		{
			if((gOSDFormatVec[i].Freq == Freq)&&(gOSDFormatVec[i].ServiceID == ServiceID))
			{
				osdf = gOSDFormatVec[i];
				bosdsetfind = true;
				break;
			}
		}
		if(bosdsetfind)
		{
			int ifontsize = StrUtil::Str2Int(osdf.FontSize);
			if(osdf.Position == "0")
			{
				if(osdf.Align == "0")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize*3/2);
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY =  StrUtil::Int2Str(ifontsize*3/2);
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen+3)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
				}
				else if(osdf.Align == "1")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(((iosdChannelLen-iosdTimeLen+2)/2+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize*3/2);
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY =  StrUtil::Int2Str(ifontsize*3/2);
						strosdtimeX = StrUtil::Int2Str(((iosdChannelLen-iosdTimeLen+2)/2+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen+3)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
				}
				else if(osdf.Align == "2")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen-iosdTimeLen+2)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize*3/2);
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY =  StrUtil::Int2Str(ifontsize*3/2);
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen-iosdTimeLen+2)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen+3)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
				}
			}
			else if(osdf.Position == "1")
			{
				if(osdf.Align == "0")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize*3/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize*3/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdChannelLen+3)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdProLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
				}
				else if(osdf.Align == "1")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen-(iosdChannelLen-iosdTimeLen+1)/2+2)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize*3/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize*3/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen-(iosdChannelLen-iosdTimeLen+1)/2+2)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdChannelLen+3)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdProLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
				}
				else if(osdf.Align == "2")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize*3/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize*3/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdChannelLen+3)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(ifontsize/2);
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdProLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(ifontsize/2);
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(ifontsize/2);
					}
				}
			}
			else if(osdf.Position == "2")
			{
				if(osdf.Align == "0")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*5/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen+3)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
				}
				else if(osdf.Align == "1")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdtimeX = StrUtil::Int2Str(((iosdChannelLen-iosdTimeLen+2)/2+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(((iosdChannelLen-iosdTimeLen+2)/2+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*5/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen+3)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}			
				}
				else if(osdf.Align == "2")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen-iosdTimeLen+2)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen-iosdTimeLen+2)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*5/2);

					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str((iosdInfoLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str((iosdChannelLen+3)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}		
				}
			}
			else if(osdf.Position == "3")
			{
				if(osdf.Align == "0")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*5/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdChannelLen+3)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdProLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
				}
				else if(osdf.Align == "1")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen-(iosdChannelLen-iosdTimeLen+1)/2+2)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);		
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen-(iosdChannelLen-iosdTimeLen+1)/2+2)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*5/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdChannelLen+3)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdProLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
				}
				else if(osdf.Align == "2")
				{
					if (osdf.Type == "1")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*5/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);		
					}
					else if (osdf.Type == "2")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*5/2);
					}
					else if (osdf.Type == "3")
					{
						strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdChannelLen+3)*ifontsize/2);
						strosdinfoY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+iosdProLen+2)*ifontsize/2);
						strosdprogramY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-(ifontsize*3/2));
						strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*ifontsize/2);
						strosdtimeY = StrUtil::Int2Str(StrUtil::Str2Int(strH)-ifontsize*3/2);
					}
				}
			}
			striosdfontsize = StrUtil::Int2Str(ifontsize);
		}
	}

	AlarmParamConfig alarmParam;
	PROPMANAGER::instance()->GetAlarmParamConfig(UNKNOWN, alarmParam);
	if (alarmParam.s_bfhold == "" || alarmParam.s_mutehold == "" || alarmParam.s_sthold == "")
	{
		alarmParam.s_bfhold = "20";
		alarmParam.s_mutehold = "-70";
		alarmParam.s_sthold = "1.5";
	}

	TunerDev tdev;
	CoderDev cdev;
	eDVBType edtype = DVBS;
	string tunermonitorip,tunermonitorport,codemonitorip,codemonitorport;
	int tunid = -1;
	if(version == "3.0")
	{
		tunid = StrUtil::Str2Int(TunerDeviceID);
		PROPMANAGER::instance()->GetTunerInfo(StrUtil::Str2Int(TunerDeviceID),edtype,tdev);
		PROPMANAGER::instance()->GetDevMonitorInfo(0,edtype,StrUtil::Str2Int(TunerDeviceID),tunermonitorip,tunermonitorport);//tuner
	}
	else
	{
		if(PROPMANAGER::instance()->IsUnUseRecDev(1,edtype,DeviceId))//实时视频和频道轮播使用的tuner和coder的编号相同
		{
			tunid = DeviceId;
			PROPMANAGER::instance()->GetTunerInfo(DeviceId,edtype,tdev);
			if (Freq != strHDMIFreq)
			{
				string servlist;
				CHANNELINFOMGR::instance()->GetServIdListByFreq(edtype,Freq,ServiceID,"",tdev.s_devicecampronum,servlist);
				tdev.s_servidList = ServiceID;//servlist;特殊处理，8路卡不够，实时通道实际插2路卡，配置文件配置8路便于录像分配，实时通道不能按8路处理
			}
			PROPMANAGER::instance()->GetDevMonitorInfo(0,edtype,DeviceId,tunermonitorip,tunermonitorport);//tuner
		}
		else
		{
			PROPMANAGER::instance()->GetTunerDevFromCoderDev(tunid,edtype,DeviceId);
			PROPMANAGER::instance()->GetTunerInfo(tunid,edtype,tdev);
			PROPMANAGER::instance()->GetDevMonitorInfo(0,edtype,tunid,tunermonitorip,tunermonitorport);//tuner
			string temp;
			temp = "DeviceId:";
			temp+=StrUtil::Int2Str(DeviceId);
			temp+="****freq:";
			temp+=Freq;
			temp+="****servceid:";
			temp+=ServiceID;
			temp+="****tunerid:";
			temp+=StrUtil::Int2Str(tunid);
			APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,temp,LOG_OUTPUT_FILE);
		}
	}
	PROPMANAGER::instance()->GetCoderInfo(DeviceId,cdev);
	PROPMANAGER::instance()->GetDevMonitorInfo(1,edtype,DeviceId,codemonitorip,codemonitorport);//coder
	if((tunermonitorip == codemonitorip)&&(tunermonitorport == codemonitorport))
	{
		if(tunermonitorip.size()>0&&tunermonitorport.size()>0)
		{
			HTTPDeviceAccess* pDeviceAccess = new HTTPDeviceAccess(StrUtil::Str2Int(tdev.s_monitorindex),tunermonitorip,StrUtil::Str2Int(tunermonitorport));
			string strRet;
			string cmd = "POST /io/config  HTTP/1.1  \r\n\r\n";
			//
			cmd+="chl=";
			cmd+=tdev.s_cardindex;
			cmd+="_";
			cmd+=tdev.s_chanindex;
			cmd+="&polar=0&tone=0&localfreq=";
			cmd+=dvbsf.slocfreq;
			cmd+="&downfreq=";
			cmd+=dvbsf.sdownfreq;
			cmd+="&symbol=";
			cmd+=dvbsf.ssymbol;
			cmd+="&ip=";
			cmd+=tdev.s_sip;
			cmd+="&tspt=";
			cmd+=tdev.s_stsport;
			cmd+="&trpt=";
			cmd+=tdev.s_trport;
			cmd+="&desc=";
			if(tdev.s_servidList.size() > 0)
			{
				cmd+=tdev.s_servidList;
			}
			else
			{
				cmd+="0";//ServiceID;
			}
			cmd+="&evip=";
			cmd+=tdev.s_290ip;
			cmd+="&evpt=";
			cmd+=tdev.s_290port;
			pDeviceAccess->SendTaskMsg(cmd,strRet);
			std::string msg = "通道:";
			msg += StrUtil::Int2Str(DeviceId);
			msg += "发送命令:";
			msg += cmd;
			APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,msg,LOG_OUTPUT_FILE);
			//***
			cmd = "POST /tcoder/config  HTTP/1.1  \r\n\r\n";
			strRet = "";
			cmd+="chl=";
			cmd+=cdev.s_coderindex;
			cmd+="&ena=";
			cmd+="1";
			cmd+="&chk=";
			cmd+="1";
			cmd+="&sip=";
			cmd+=tdev.s_sip;
			cmd+="&spt=";
			cmd+=tdev.s_stsport;
			cmd+="&spro=";
			cmd+=ServiceID;
			cmd+="&svid=";
			cmd+=VideoPID;
			cmd+="&saud0=";
			cmd+=AudioPID;
			cmd+="&saud1=";
			cmd+="0";
			cmd+="&saud2=";
			cmd+="0";
			cmd+="&vcode=";
			cmd+="h264";
			cmd+="&acode=";
			cmd+="mpeg";
			cmd+="&vfmt=";
			if(((!(StrUtil::Str2Int(strW)>640||StrUtil::Str2Int(strH)>480))&&(PROPMANAGER::instance()->GetCurWorkVersion() != "5.0"))
				||((!(StrUtil::Str2Int(strW)>640||StrUtil::Str2Int(strH)>480))&&(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")))
			{
				if(isBps < 250)
				{
					isBps = 250;
					strbps = StrUtil::Int2Str(isBps);
					ivBps = isBps - 32 - 50;
				}
			}
			else
			{
				if(isBps < 350)
				{
					isBps = 350;
					strbps = StrUtil::Int2Str(isBps);
					ivBps = isBps - 32 - 50;
				}
			}
			cmd+=strW;
			cmd+="_";
			cmd+=strH;
			string sAudioBps;
			if (isBps < 300)
				sAudioBps = "32";
			else if ((isBps > 300) && (isBps < 450))
				sAudioBps = "32";
			else if (isBps > 450)
				sAudioBps = "32";
			cmd+="&vfre=";
			cmd+="0";
			cmd+="&v1v1=";
			cmd+="2";
			cmd+="&mutehold=";
			//cmd+="-70";
			cmd+= alarmParam.s_mutehold;
			cmd+="&bfhold=";
			//cmd+="20";
			cmd+= alarmParam.s_bfhold;
			cmd+="&sthold=";
			//cmd+="1.5";
			cmd+= alarmParam.s_sthold;
			cmd+="&vrate=";
			cmd+=StrUtil::Int2Str(ivBps);
			cmd+="&arate=";
			cmd+=sAudioBps;
			cmd+="&srate=";
			cmd+=strbps;
			cmd+="&osd0=";
			cmd+=strosdtimeX;
			cmd+="_";
			cmd+=strosdtimeY;
			cmd+="_";
			cmd+=striosdfontsize;
			cmd+="_";
			if(isosd == "1")
			{
				cmd+=utf8_time;
			}
			cmd+="&osd1=";
			cmd+=strosdprogramX;
			cmd+="_";
			cmd+=strosdprogramY;
			cmd+="_";
			cmd+=striosdfontsize;
			cmd+="_";
			if(isosd == "1")
			{
				cmd+=utf8_proname;//proname;
			}
			cmd+="&osd2=";
			cmd+=strosdinfoX;
			cmd+="_";
			cmd+=strosdinfoY;
			cmd+="_";
			cmd+=striosdfontsize;
			cmd+="_";
			if(isosd == "1")
			{
				cmd+=utf8_osdinfo;//osdinfo;
			}
			cmd+="&dip=";
			cmd+=cdev.s_dip;
			cmd+="&dpt=";
			cmd+=cdev.s_dtsport;
			cmd+="&evip=";
			cmd+=cdev.s_alarmip;
			cmd+="&evpt=";
			cmd+=cdev.s_alarmport;
			pDeviceAccess->SendTaskMsg(cmd,strRet);
			//
			msg = "通道:";
			msg += StrUtil::Int2Str(DeviceId);
			msg += "发送命令:";
			msg += cmd;
			APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,msg,LOG_OUTPUT_FILE);
			////
			delete pDeviceAccess;
		}
	}
	return true;
}
bool UDPTsFetcher::SendTsDeviceXmlForFMAM(std::string devXml)
{
	ACE_Guard<ACE_Thread_Mutex> guard(g_SetCmdMutex);
	//
	std::string PmtPID,PcrPID,VideoPID,AudioPID ,Freq,ServiceID,strW,strH,strbps,striosdfontsize,osdinfo,strosdinfoX,strosdinfoY,strosdtimeX,strosdtimeY,proname,strosdprogramX,strosdprogramY,isosd;
	string version,TunerDeviceID,CoderDeviceID,IsTranAudio;
	string strComamdType = "";
	XmlParser tmpparser(devXml.c_str());
	pXMLNODE TsNode = tmpparser.GetNodeFromPath( "Msg/TSQuery/TS" );
	tmpparser.GetAttrNode(TsNode,"ServiceID",ServiceID);
	tmpparser.GetAttrNode(TsNode,"Version",version);
	tmpparser.GetAttrNode(TsNode,"TDeviceID",TunerDeviceID);
	tmpparser.GetAttrNode(TsNode,"CDeviceID",CoderDeviceID);
	tmpparser.GetAttrNode(TsNode,"ComamdType",strComamdType);
	if(strComamdType.size()<=0)
	{
		comamdtype = 3;
	}
	//ServiceID = "102";
	tmpparser.GetAttrNode(TsNode,"PmtPID",PmtPID);
	tmpparser.GetAttrNode(TsNode,"PcrPID",PcrPID);
	tmpparser.GetAttrNode(TsNode,"VideoPID",VideoPID);
	tmpparser.GetAttrNode(TsNode,"Width",strW);
	tmpparser.GetAttrNode(TsNode,"Height",strH);
	tmpparser.GetAttrNode(TsNode,"Bps",strbps);
	tmpparser.GetAttrNode(TsNode,"OSDFontSize",striosdfontsize);
	tmpparser.GetAttrNode(TsNode,"OSDInfo",osdinfo);
	tmpparser.GetAttrNode(TsNode,"OSDInfoX",strosdinfoX);
	tmpparser.GetAttrNode(TsNode,"OSDInfoY",strosdinfoY);
	tmpparser.GetAttrNode(TsNode,"OSDTimeX",strosdtimeX);
	tmpparser.GetAttrNode(TsNode,"OSDTimeY",strosdtimeY);
	tmpparser.GetAttrNode(TsNode,"ServiceName",proname);
	//
	proname.erase(0,proname.find_first_not_of(" "));//除空格  
    proname.erase(proname.find_last_not_of(" ") + 1);//除空格  
	//
	tmpparser.GetAttrNode(TsNode,"OSDProgramX",strosdprogramX);
	tmpparser.GetAttrNode(TsNode,"OSDProgramY",strosdprogramY);
	tmpparser.GetAttrNode(TsNode,"OSDEnable",isosd);
	//VideoPID = "9024";
	tmpparser.GetAttrNode(TsNode,"AudioPID",AudioPID);
	tmpparser.GetAttrNode(TsNode,"Audio",IsTranAudio);
	//AudioPID = "834";
	tmpparser.GetAttrNode(TsNode,"Freq",Freq);

	std::string msg;
	if (Freq != strHDMIFreq)
	{
		msg = string("通道[") + StrUtil::Int2Str(DeviceId)+ string("]指令:") + \
			string("PMT[") + PmtPID + string("] PCR[") + PcrPID + string("] VPID[") + \
			VideoPID + string("] APID[") + AudioPID + string("]");

		//SYSMSGSENDER::instance()->SendMsg(msg);
	}

	size_t sz = 0; 
	string strtime = "time";
	string utf8_time;
	string utf8_proname;
	string utf8_osdinfo;
	//
	if(strtime.size()>0)
	{
		utf8_time = strtime;
		ConvertGBKToUtf8(utf8_time);
	}
	//
	if(proname.size()>0)
	{
		utf8_proname = Freq;
		ConvertGBKToUtf8(utf8_proname);
	}
	if(osdinfo.size()>0)
	{
		utf8_osdinfo = osdinfo;
		ConvertGBKToUtf8(utf8_osdinfo);
	}

	int ivBps = StrUtil::Str2Int(strbps);
	string sAudioBps = "32";
	if (ivBps < 300)
	{
		sAudioBps = "32";
	}
	else if ((ivBps > 300) && (ivBps < 450))
	{
		sAudioBps = "32";
	}
	else if (ivBps > 450)
	{
		sAudioBps = "32";
	}
	
	int isBps = (ivBps + StrUtil::Str2Int(sAudioBps))*1.1;
	//
	int iosdfontsize = StrUtil::Str2Int(striosdfontsize);
	if(iosdfontsize < 8)
	{
		iosdfontsize = 8;
		striosdfontsize = StrUtil::Int2Str(iosdfontsize);
	}

	//
	string vfmt;
	if(((!(StrUtil::Str2Int(strW)>640||StrUtil::Str2Int(strH)>480))&&(PROPMANAGER::instance()->GetCurWorkVersion() != "5.0"))
		||((!(StrUtil::Str2Int(strW)>640||StrUtil::Str2Int(strH)>480))&&(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")))
	{
		if(ivBps < 250)
		{
			ivBps = 250;
			isBps = (ivBps + StrUtil::Str2Int(sAudioBps))*1.1;
			strbps = StrUtil::Int2Str(isBps);
		}
		vfmt ="2";
		//避免osd位置异常
		if ((PROPMANAGER::instance()->GetCurWorkVersion() == "1.0")||(PROPMANAGER::instance()->GetCurWorkVersion() == "2.0"))
		{
			strW = "352";
			strH = "288";
		}
		//
	}
	else
	{
		if(ivBps < 350)
		{
			ivBps = 350;
			isBps = (ivBps + StrUtil::Str2Int(sAudioBps))*1.1;
			strbps = StrUtil::Int2Str(isBps);
		}
		iosdfontsize = 30;
		striosdfontsize = StrUtil::Int2Str(iosdfontsize);
		vfmt ="1";
		//避免osd位置异常
		if ((PROPMANAGER::instance()->GetCurWorkVersion() == "1.0")||(PROPMANAGER::instance()->GetCurWorkVersion() == "2.0"))
		{
			strW = "720";
			strH = "576";
		}
		//
	}


	string scurTime = TimeUtil::GetCurDateTime();
	int iosdTimeLen = scurTime.length();
	int iosdProLen = proname.length();
	int iosdInfoLen = osdinfo.length();
	int iosdChannelLen = proname.length() + osdinfo.length();
	string antiColor = "1";

	//OSD位置控制计算 默认右上角右对齐
	strosdinfoX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdChannelLen+2)*iosdfontsize/2);
	strosdinfoY = StrUtil::Int2Str(iosdfontsize/2);
	strosdprogramX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdProLen+1)*iosdfontsize/2);
	strosdprogramY = StrUtil::Int2Str(iosdfontsize/2);
	strosdtimeX = StrUtil::Int2Str(StrUtil::Str2Int(strW)-(iosdTimeLen+1)*iosdfontsize/2);
	strosdtimeY = StrUtil::Int2Str(iosdfontsize*3/2);

	AlarmParamConfig alarmParam;
	PROPMANAGER::instance()->GetAlarmParamConfig(UNKNOWN, alarmParam);
	if (alarmParam.s_bfhold == "" || alarmParam.s_mutehold == "" || alarmParam.s_sthold == "")
	{
		alarmParam.s_bfhold = "20";
		alarmParam.s_mutehold = "-70";
		alarmParam.s_sthold = "1.5";
	}

	TunerDev tdev;
	CoderDev cdev;
	eDVBType edtype;
	if (StrUtil::Str2Int(Freq) > 20000)
	{
		edtype = RADIO;
	}
	else
	{
		edtype = AM;
	}
	string tunermonitorip,tunermonitorport,codemonitorip,codemonitorport;
	int tunid = -1;
	if(version == "3.0")
	{
		tunid = StrUtil::Str2Int(TunerDeviceID);
		PROPMANAGER::instance()->GetTunerInfo(StrUtil::Str2Int(TunerDeviceID),edtype,tdev);
		PROPMANAGER::instance()->GetDevMonitorInfo(0,edtype,StrUtil::Str2Int(TunerDeviceID),tunermonitorip,tunermonitorport);//tuner
	}
	else
	{
		if(PROPMANAGER::instance()->IsUnUseRecDev(1,edtype,DeviceId))//实时视频和频道轮播使用的tuner和coder的编号相同
		{
			tunid = DeviceId;
			PROPMANAGER::instance()->GetTunerInfo(DeviceId,edtype,tdev);
			if (Freq != strHDMIFreq)
			{
				string servlist;
				CHANNELINFOMGR::instance()->GetServIdListByFreq(edtype,Freq,ServiceID,"",tdev.s_devicecampronum,servlist);
				tdev.s_servidList = ServiceID;//servlist;特殊处理，8路卡不够，实时通道实际插2路卡，配置文件配置8路便于录像分配，实时通道不能按8路处理
			}
			PROPMANAGER::instance()->GetDevMonitorInfo(0,edtype,DeviceId,tunermonitorip,tunermonitorport);//tuner
		}
		else
		{
			PROPMANAGER::instance()->GetTunerDevFromCoderDev(tunid,edtype,DeviceId);
			PROPMANAGER::instance()->GetTunerInfo(tunid,edtype,tdev);
			PROPMANAGER::instance()->GetDevMonitorInfo(0,edtype,tunid,tunermonitorip,tunermonitorport);//tuner
			string temp;
			temp = "DeviceId:";
			temp+=StrUtil::Int2Str(DeviceId);
			temp+="****freq:";
			temp+=Freq;
			temp+="****servceid:";
			temp+=ServiceID;
			temp+="****tunerid:";
			temp+=StrUtil::Int2Str(tunid);
			APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,temp,LOG_OUTPUT_FILE);
		}
	}
	PROPMANAGER::instance()->GetCoderInfo(DeviceId,cdev);
	PROPMANAGER::instance()->GetDevMonitorInfo(1,edtype,DeviceId,codemonitorip,codemonitorport);//coder
	if(((tunermonitorip == codemonitorip)&&(tunermonitorport == codemonitorport))||(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0"))
	{
		if(tunermonitorip.size()>0&&tunermonitorport.size()>0)
		{
			HTTPDeviceAccess* pDeviceAccess = new HTTPDeviceAccess(StrUtil::Str2Int(tdev.s_monitorindex),tunermonitorip,StrUtil::Str2Int(tunermonitorport));
			string strRet;
			//
			string cmd = "POST /io/config  HTTP/1.1  \r\n\r\n";
			cmd+="chl=";
			cmd+=tdev.s_cardindex;
			cmd+="_";
			cmd+=tdev.s_chanindex;
			cmd+="&fre=";
			cmd+=Freq;
			cmd+="&volume=50";
			pDeviceAccess->SendTaskMsg(cmd,strRet);
			std::string msg = "UDPTsFetcher:";
			msg += "发送命令:";
			msg += cmd;
			APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,msg,LOG_OUTPUT_FILE);
			Sleep(100);
			//
			cmd = "POST /device/getformat  HTTP/1.1  \r\n\r\n\
						 chl=";
			cmd+=tdev.s_cardindex;
			cmd+="_";
			cmd+=tdev.s_chanindex;
			pDeviceAccess->SendTaskMsg(cmd,strRet);
			msg = "UDPTsFetcher:";
			msg += "发送命令:";
			msg += cmd;
			msg += "~收到结果:";
			msg += strRet;
			APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,msg,LOG_OUTPUT_FILE);

			int pos = strRet.find("<html>");
			string strtem = strRet;
			if(pos >= 0)
			{
				strtem = strRet.substr(pos,strRet.size()-pos);
			}
			else
			{
				return false;
			}

			string format = "";
			XmlParser xml;
			xml.Set_xml(strtem);
			pXMLNODE pnode = xml.GetNodeFromPath("html/body");
			pXMLNODELIST pnodelist  = xml.GetNodeList(pnode);
			if (pnodelist != NULL)
			{
				pXMLNODE childNode = xml.GetNextNode(pnodelist);
				string strName = xml.GetNodeName(childNode);
				if (strName == "Desc")
				{
					xml.GetAttrNode(childNode, "format", format);
				}
			}
			if (format == "" || format == "UNDEF")
			{
				return false;
			}
			strRet = "";
			//***
			cmd = "POST  /encoder/config  HTTP/1.1  \r\n\r\n";
			cmd+="chl=";
			cmd+=cdev.s_coderindex;
			cmd+="&ena=";
			cmd+="1";
			cmd+="&chk=";
			cmd+="1";
			cmd+="&inchl=";
			cmd+=tdev.s_cardindex;
			cmd+="_";
			cmd+=tdev.s_chanindex;
			cmd+="&format=";
			cmd+=format;
			cmd+="&vcode=";
			cmd+="h264";
			cmd+="&acode=";
			if((version == "3.0")&&(IsTranAudio == "1"))
			{
				cmd+="copy";
			}
			else
			{
				cmd+="mpeg";
			}
			cmd+="&vfmt=";
			cmd+=strW;
			cmd+="_";
			cmd+=strH;
			cmd+="&vfre=";
			cmd+="0";
			cmd+="&v1v1=";
			cmd+="2";
			cmd+="&mutehold=";
			//cmd+="-70";
			cmd+= alarmParam.s_mutehold;
			cmd+="&bfhold=";
			//cmd+="20";
			cmd+= alarmParam.s_bfhold;
			cmd+="&sthold=";
			//cmd+="1.5";
			cmd+= alarmParam.s_sthold;
			cmd+="&vrate=";
			cmd+=StrUtil::Int2Str(ivBps);
			cmd+="&arate=";
			cmd+=sAudioBps;
			cmd+="&srate=";
			cmd+=strbps;
			cmd+="&osd0=";
			if (version == "3.0" && isosd == "1")
			{
				cmd+= antiColor;
				cmd+="_";
			}
			cmd+=strosdtimeX;
			cmd+="_";
			cmd+=strosdtimeY;
			cmd+="_";
			cmd+=striosdfontsize;
			cmd+="_";
			if(isosd == "1")
			{
				cmd+=utf8_time;
			}
			cmd+="&osd1=";
			if (version == "3.0" && isosd == "1")
			{
				cmd+= antiColor;
				cmd+="_";
			}
			cmd+=strosdprogramX;
			cmd+="_";
			cmd+=strosdprogramY;
			cmd+="_";
			cmd+=striosdfontsize;
			cmd+="_";
			if(isosd == "1")
			{
				cmd+=utf8_proname;//proname;
			}
			cmd+="&osd2=";
			if (version == "3.0" && isosd == "1")
			{
				cmd+= antiColor;
				cmd+="_";
			}
			cmd+=strosdinfoX;
			cmd+="_";
			cmd+=strosdinfoY;
			cmd+="_";
			cmd+=striosdfontsize;
			cmd+="_";
			if(isosd == "1")
			{
				cmd+=utf8_osdinfo;//osdinfo;
			}
			cmd+="&dip=";
			cmd+=cdev.s_dip;
			cmd+="&dpt=";
			cmd+=cdev.s_dtsport;
			cmd+="&evip=";
			cmd+=cdev.s_alarmip;
			cmd+="&evpt=";
			cmd+=cdev.s_alarmport;
			//***
			pDeviceAccess->SendTaskMsg(cmd,strRet);
			msg = "通道:";
			msg += StrUtil::Int2Str(DeviceId);
			msg += "发送命令:";
			msg += cmd;
			msg += "~收到结果:";
			msg += strRet;
			APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,msg,LOG_OUTPUT_FILE);
			//
			delete pDeviceAccess;
		}
	}
	return true;
}

bool UDPTsFetcher::RebootCard()
{
	return true;
}

bool UDPTsFetcher::FlushHistoryDatum()
{
	int roundchannel = 0;
	if(false == PROPMANAGER::instance()->GetVirDeviceId(DVBC, roundchannel))
	{
		roundchannel = 1003;
	}
	if((TSSENDERMGR::instance()->HasClient(roundchannel) == false)&&(g_RealtimeRoundTaskNum<=0))
	{
		TaskRoundMutex.acquire();
		//清空轮播队列中数据
		if (TsSendRoundTaskPointer != NULL)
		{
			TsSendRoundTaskPointer->msg_queue()->flush();
		}
		TaskRoundMutex.release();
	}
	RealStreamMutex.acquire();
	//清空发送队列中数据
	if (TsSendTaskPointer != NULL)
	{
		TsSendTaskPointer->msg_queue()->flush();
	}
	RealStreamMutex.release();
	//清空录像队列中数据
	TaskPointerMutex.acquire();
	std::vector<ACE_Task<ACE_MT_SYNCH>*>::iterator ptr = RecordTaskPointerVec.begin();
	for(;ptr!=RecordTaskPointerVec.end();++ptr)
	{
		if ((*ptr) != NULL)//录像任务指针不为空，则将获得数据放入录像任务的数据队列中
		{
			(*ptr)->msg_queue()->flush();
		}
	}
	TaskPointerMutex.release();
	return true;
}

bool UDPTsFetcher::DelRecordTask( ACE_Task<ACE_MT_SYNCH>* task )
{
	TaskPointerMutex.acquire();
	std::vector<ACE_Task<ACE_MT_SYNCH>*>::iterator ptr=RecordTaskPointerVec.begin();
	for(;ptr!=RecordTaskPointerVec.end();ptr++)
	{		
		if((*ptr)==task)
		{
			RecordTaskPointerVec.erase(ptr);
			break;
		}
	}
	TaskPointerMutex.release();

	return true;
}
void UDPTsFetcher::SetReSendSwitch(bool sendswitch)
{
	ReSendSwitch = sendswitch;
}
void UDPTsFetcher::SetSendSwitch( bool sendswitch )
{
	bSendSwtich=sendswitch;
	if(!sendswitch)
	{
		//cout<<"1111111111111111"<<endl;
		//g_getrounddata = true;
	}
	if (sendswitch == true)
	{
		NewPackageHead = true;	//切换轮播任务指令的时候需呀做包头处理
	}
}
void UDPTsFetcher::SetForDataToCodec( bool senddatatocodec )
{
	m_bsenddatatocodec=senddatatocodec;
}

void UDPTsFetcher::IncreaseTaskNum()
{
	TaskNumMutex.acquire();
	TaskNum++;
	TaskNumMutex.release();
}

void UDPTsFetcher::DecreaseTaskNum()
{
	TaskNumMutex.acquire();
	TaskNum--;
	TaskNumMutex.release();
}

void UDPTsFetcher::TimeConvert(time_t curtime,unsigned char *timebuf)
{
	timebuf[0]=(curtime>>56);
	timebuf[1]=(curtime>>48)&(0x00ff);
	timebuf[2]=(curtime>>40)&(0x0000ff);
	timebuf[3]=(curtime>>32)&(0x000000ff);
	timebuf[4]=(curtime>>24)&(0x00000000ff);
	timebuf[5]=(curtime>>16)&(0x0000000000ff);
	timebuf[6]=(curtime>>8)&(0x000000000000ff);
	timebuf[7]=(curtime)&(0x00000000000000ff);
}
void UDPTsFetcher::PutSysTime(ACE_Task<ACE_MT_SYNCH>* task)
{
	if (task == NULL)
		return;

	//给数据加入系统时间
	unsigned char Timebuf[188]={0};
	Timebuf[0]=0x47;
	Timebuf[2]=0x1A;

	TimeConvert(time(0),&Timebuf[4]);

	ACE_Message_Block *TimeMb = NULL;
	try
	{
		TimeMb = new ACE_Message_Block(188);
		if (TimeMb != NULL)
		{
			TimeMb->copy((char*)Timebuf,188);
		}
		ACE_Time_Value PutOutTime(ACE_OS::gettimeofday()+ACE_Time_Value(1));
		task->putq(TimeMb,&PutOutTime);
	}
	catch(ACE_bad_alloc)
	{
		if (TimeMb != NULL)
			TimeMb->release();
		ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d] UDPTsFetcher使用ACE_Message_Block出错\n",DeviceId));
	}
	catch(...)
	{
		if (TimeMb != NULL)
			TimeMb->release();
		ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]UDPTsFetcher使用ACE_Message_Block出错\n",DeviceId));
	}
	return;
}
void UDPTsFetcher::SetMulti(bool bMulti,std::vector<UdpSendTsAddr> UdpSendTsAddrlist)
{
	m_SendForMulti = bMulti;
	if(bMulti)
	{
		m_pUdpTsSender->SetClient(UdpSendTsAddrlist);
	}
	else
	{
		m_pUdpTsSender->ClearClient();
	}
}
