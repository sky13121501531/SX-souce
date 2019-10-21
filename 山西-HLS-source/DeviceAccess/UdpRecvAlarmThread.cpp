///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：UdpRecvAlarmThread.cpp
///////////////////////////////////////////////////////////////////////////////////////////
#include "UdpRecvAlarmThread.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/PropManager.h"
#include "../Alarm/AlarmMgr.h"
#include "../Foundation/AppLog.h"
#include "../DeviceAccess/HTTPDeviceAccess.h"
//
extern int g_realstreamusedev;
standardRFResult g_standardRFResult[100];
time_t AlarmTime;
//
enum Tr290EventType
{
	TR_UNDEF = 0,
	TR_TSBW,
	TR_PIDBW,
	TR_TSSYNC_LST,
	TR_TSSYNC_ERR,
	TR_TSPID_ERR,
	TR_TSCNT_ERR,
	TR_TSTX_ERR,
	TR_TSCA_ERR,
	TR_TSPID_LST,
	TR_PCR0,
	TR_PCR1,
	TR_PTS,
	TR_TAB,
	TR_RFSET,
	TR_RFSAT,
	TR_RFIQ
};
struct TR290Event
{
	unsigned char evt_type;
	unsigned char evt_unused;
	union EVT_PID_
	{
		unsigned short evt_tepid;
		unsigned short rf_qam;
		struct RF_IQ_
		{
			unsigned char iqy;
			unsigned char iqx;
		}rf_iq;
	}evt_pid;
	//
	union EVT_TIM_
	{
		unsigned int evt_time;
		unsigned int rf_ber;
	}evt_tim;
	//
	union EXT_VAL_
	{
		unsigned int tspkt_cnt;
		unsigned int synclst_cnt;
		unsigned int syncerr_cnt;
		unsigned int piderr_cnt;
		unsigned int cnterr_cnt;
		unsigned int txerr_cnt;
		unsigned int caerr_cnt;
		unsigned int pcr_cnt;
		unsigned int pts_cnt;
		//
		struct rf_set_
		{
			unsigned short rf_synbol;
			unsigned short rf_frequency;

		}rf_set;
		//
		struct rf_sat0_
		{
			unsigned char rf_mse;
			unsigned char rf_snr;
			unsigned char rf_strength;
			unsigned char rf_locked;
		}rf_sat0;
		//
		struct rf_sat1_
		{
			unsigned char rf_acc;
			unsigned char rf_afc;
			unsigned char rf_agcif;
			unsigned char rf_agccrf;
		}rf_sat1;
		//
		struct tab_info_
		{
			unsigned char tabid;
			unsigned char unused0;
			unsigned char unused1;
			unsigned char crcerr;
		}tab_info;
		//
	}ext_val;
};
//
UdpRecvAlarmThread::~UdpRecvAlarmThread()
{
}
UdpRecvAlarmThread::UdpRecvAlarmThread(string ip,string port,int wtype)
{
	iWorktype = wtype;
	string mulitcase = ip + string(":")+ port;
	ACE_INET_Addr multicast_addr_(mulitcase.c_str()); 
	DeviceSock.join(multicast_addr_);
	DeviceSock.set_option(IP_MULTICAST_TTL, 5);
	int nSocketBuffSize = 1024*500;
	ACE_SOCK* sk= &DeviceSock;
	sk->set_option(SOL_SOCKET, SO_RCVBUF, &nSocketBuffSize, sizeof(int));
	//
	memset(m_picalarm,-1,sizeof(char)*6);
	memset(m_picalarmtimes,25,sizeof(char)*3);
	memset(m_picalarmtime_t,0,sizeof(time_t)*3);
	//
	m_errCnttime = time(0);
	m_errPattime = time(0);
	m_errPmttime = time(0);
}

int UdpRecvAlarmThread::Start()
{
	//发送线程开始
	open(0);
	return 0;
}

int UdpRecvAlarmThread::open(void*)
{
	bFlag = true;
	activate();
	return 0;
}
int UdpRecvAlarmThread::svc()
{
	int ReadSize = 480+2;
	if(iWorktype == 1)
	{
		ReadSize = 38+2;
	}
	//
	if(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")
	{
		ReadSize = 1400;
	}
	//
	m_bSetCoderAlarm = false;
	unsigned char* RcvBuf	= new unsigned char[ReadSize];		
	memset(RcvBuf,0,ReadSize);
	ACE_Time_Value RecvTimeOut(5);
	//
	sCheckParam sCheck;
	sCheck.AlarmType	= ALARM_PROGRAM;
	sCheck.DVBType		= m_dvbtype;
	sCheck.ChannelID	= m_strchannelid;
	sCheck.Freq			= m_strfreq;
	sCheck.ServiceID	= m_strServiceID;
	sCheck.STD			= "";
	sCheck.SymbolRate	= "";
	sCheck.TypedValue	= "";
	sCheck.DeviceID		= m_strdeviceid;
	//
	int DeviceID =  StrUtil::Str2Int(m_strdeviceid);
	//
	unsigned int m_PatTime;//PAT错误时间记录,两PAT间时间超过500ms既PAT错误 初始化0
	unsigned int m_290errcount[2];//g_290errcount[0]表示同步丢失错误计数 g_290errcount[1] 表示同步字节错误计数 初始化0
	vector<TR290Event> m_PMTErrList;//记录PMTPID和时间
	vector<TR290Event> m_CNTErrList;//记录CNT错误PID和个数
	m_PatTime = 0;
	m_290errcount[0] = 0;
	m_290errcount[1] = 0;
	time_t startbacktime = 0;
	//
	while (bFlag)
	{
		if((iWorktype == 1)&&((!m_bSetCoderAlarm)||(g_realstreamusedev == DeviceID)))
		{
			Sleep(3000);
			continue;
		}
		//
		memset(RcvBuf,0,ReadSize);
		int RecvLen = DeviceSock.recv(RcvBuf,ReadSize,remote_addr,0,&RecvTimeOut);	//接收数据
		if(RecvLen <= 0)
		{
			continue;
		}
		else
		{

			if(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")
			{
				for(int i=0;i<RecvLen;i++)
				{
					if(RcvBuf[i] == 0x5a)
					{
						for(int j=i+4;j<RecvLen;)
						{
							unsigned char DataType = RcvBuf[j];
							unsigned char DataLength = RcvBuf[j+1];
							if(DataType == 0x01)
							{
							}
							else if(DataType == 0x02)
							{
							}
							else if(DataType == 0x03)
							{
								if(DataLength == 48)
								{
									unsigned char RFInfo[48];
									unsigned char* temBuf = &(RcvBuf[j+4]);
									memcpy(&RFInfo,temBuf,48);
									j=j+52;
									//
									g_standardRFResult[DeviceID].ucharLevel = RFInfo[0];
									g_standardRFResult[DeviceID].ucharlockStatus = RFInfo[1];
									if (RFInfo[3]*256+RFInfo[2] > 0)
									{
										g_standardRFResult[DeviceID].ushortMer = RFInfo[3]*256+RFInfo[2];
									}
									g_standardRFResult[DeviceID].ushortEVM = RFInfo[5]*256+RFInfo[4];
									if (RFInfo[9]*16777216+RFInfo[8]*65536+RFInfo[7]*256+RFInfo[6] > 0)
									{
										g_standardRFResult[DeviceID].uintBer = RFInfo[9]*16777216+RFInfo[8]*65536+RFInfo[7]*256+RFInfo[6];
									}
									g_standardRFResult[DeviceID].ushortCNR = RFInfo[11]*256+RFInfo[10];
								}
								else
								{
									j++;
								}
							}
							else if(DataType == 0x04)
							{
								sCheck.AlarmType	= ALARM_TR101_290;
								if(DataLength == 12)
								{
									standardTR290Result tr290r;
									unsigned char* temBuf = &(RcvBuf[j+4]);
									memcpy(&tr290r,temBuf,12);
									j=j+16;
									//
									if(tr290r.ucharLevel == 1)
									{
										if(tr290r.ucharSubType == 1)
										{
											sCheck.mode = "0";
											sCheck.TypeDesc		= "同步丢失错误";
											sCheck.TypeID		= "2";
											sCheck.CheckTime	= time(0);
											ALARMMGR::instance()->CheckAlarm(sCheck,true);
										}
										else if(tr290r.ucharSubType == 2)
										{
											sCheck.mode = "0";
											sCheck.TypeDesc		= "同步字节错误";
											sCheck.TypeID		= "3";
											sCheck.CheckTime	= time(0);
											ALARMMGR::instance()->CheckAlarm(sCheck,true);
										}
										else if(tr290r.ucharSubType == 3)
										{
											sCheck.mode = "0";
											sCheck.TypeDesc		= "PAT 错误";
											sCheck.TypeID		= "4";
											sCheck.CheckTime	= time(0);
											ALARMMGR::instance()->CheckAlarm(sCheck,true);
										}
										else if(tr290r.ucharSubType == 4)
										{
											sCheck.mode = "0";
											sCheck.TypeDesc		= "连续计数错误";
											sCheck.TypeID		= "5";
											sCheck.CheckTime	= time(0);
											ALARMMGR::instance()->CheckAlarm(sCheck,true);
										}
										else if(tr290r.ucharSubType == 5)
										{
											sCheck.mode = "0";
											sCheck.TypeDesc		= "PMT错误";
											sCheck.TypeID		= "6";
											sCheck.CheckTime	= time(0);
											ALARMMGR::instance()->CheckAlarm(sCheck,true);
										}
										else if(tr290r.ucharSubType == 6)
										{
											sCheck.mode = "0";
											sCheck.TypeDesc		= "PID错误";
											sCheck.TypeID		= "7";
											sCheck.CheckTime	= time(0);
											ALARMMGR::instance()->CheckAlarm(sCheck,true);
										}
									}
									else if(tr290r.ucharLevel == 2)
									{
										if(tr290r.ucharSubType == 1)
										{
											sCheck.mode = "0";
											sCheck.TypeDesc		= "传输错误";
											sCheck.TypeID		= "8";
											sCheck.CheckTime	= time(0);
											ALARMMGR::instance()->CheckAlarm(sCheck,true);
										}
										else if(tr290r.ucharSubType == 2)
										{
											sCheck.mode = "0";
											sCheck.TypeDesc		= "CRC错误";
											sCheck.TypeID		= "9";
											sCheck.CheckTime	= time(0);
											ALARMMGR::instance()->CheckAlarm(sCheck,true);
										}
										else if(tr290r.ucharSubType == 3)
										{
											sCheck.mode = "0";
											sCheck.TypeDesc		= "PCR间隔错误";
											sCheck.TypeID		= "10";
											sCheck.CheckTime	= time(0);
											ALARMMGR::instance()->CheckAlarm(sCheck,true);
										}
										else if(tr290r.ucharSubType == 5)
										{
											sCheck.mode = "0";
											sCheck.TypeDesc		= "PCR精度错误";
											sCheck.TypeID		= "11";
											sCheck.CheckTime	= time(0);
											ALARMMGR::instance()->CheckAlarm(sCheck,true);
										}
										else if(tr290r.ucharSubType == 6)
										{
											sCheck.mode = "0";
											sCheck.TypeDesc		= "PTS错误";
											sCheck.TypeID		= "12";
											sCheck.CheckTime	= time(0);
											ALARMMGR::instance()->CheckAlarm(sCheck,true);
										}
										else if(tr290r.ucharSubType == 7)
										{
											sCheck.mode = "0";
											sCheck.TypeDesc		= "CAT错误";
											sCheck.TypeID		= "13";
											sCheck.CheckTime	= time(0);
											ALARMMGR::instance()->CheckAlarm(sCheck,true);
										}
									}
									else if(tr290r.ucharLevel == 3)
									{
									}
									//
								}
								else
								{
									j++;
								}
							}
							else if(DataType == 0x05)
							{
								sCheck.AlarmType	= ALARM_PROGRAM;
								if(DataLength == 8)
								{
									standardAVErrorStatus programerr;
									unsigned char* temBuf = &(RcvBuf[j+4]);
									memcpy(&programerr,temBuf,8);
									j=j+12;
									//
									unsigned short VholdVal = programerr.ushortVStatus;
									if((m_strVideoPID.size()>1)&&(m_strVideoPID != "5000"))
									{
										if(VholdVal&0x1)//黑场 
										{
											if(m_picalarmtimes[1] >= 5)
											{
												if(m_picalarm[1] != '1')
												{
													sCheck.mode = "0";
													sCheck.TypeDesc		= "黑场";
													sCheck.TypeID		= "0x1";
													sCheck.CheckTime	= m_picalarmtime_t[1];
													ALARMMGR::instance()->CheckAlarm(sCheck,true);
													//APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_DEBUG,string("报警-黑屏-频点:"+m_strfreq+"-channelid="+m_strchannelid+"-device="+m_strdeviceid),LOG_OUTPUT_BOTH);
													//
													m_picalarm[1] = '1';
												}
											}
											else
											{
												if(m_picalarmtimes[1] >= 20)
												{
													m_picalarmtimes[1] = 1;
													m_picalarmtime_t[1] = time(0);
												}
												else
												{
													m_picalarmtimes[1] = m_picalarmtimes[1] + 1;
												}
											}
										}
										else
										{
											if(m_picalarmtimes[1] >= 35)
											{
												if(m_picalarm[1] == '1')
												{
													sCheck.mode = "1";
													sCheck.TypeDesc		= "黑场";
													sCheck.TypeID		= "0x1";
													sCheck.CheckTime	= m_picalarmtime_t[1];
													ALARMMGR::instance()->CheckAlarm(sCheck,true);
													//APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_DEBUG,string("解除报警-黑屏-频点:"+m_strfreq+"-channelid="+m_strchannelid+"-device="+m_strdeviceid),LOG_OUTPUT_BOTH);
													//
													m_picalarm[1] = '0';
												}
											}
											else
											{
												if(m_picalarmtimes[1] < 30)
												{
													m_picalarmtimes[1] = 31;
													m_picalarmtime_t[1] = time(0);
												}
												else
												{
													m_picalarmtimes[1] = m_picalarmtimes[1] + 1;
												}
											}
										}
										if(VholdVal&0x2)//静帧
										{
											if(m_picalarmtimes[0] >= 5)
											{
												if(m_picalarm[0] != '2')
												{
													sCheck.mode = "0";
													sCheck.TypeDesc		= "静帧";
													sCheck.TypeID		= "0x4";
													sCheck.CheckTime	= m_picalarmtime_t[0];
													ALARMMGR::instance()->CheckAlarm(sCheck,true);
													//APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_DEBUG,string("报警-图像静止-频点:"+m_strfreq+"-channelid="+m_strchannelid+"-device="+m_strdeviceid),LOG_OUTPUT_BOTH);
													//
													m_picalarm[0] = '2';
												}
											}
											else
											{
												if(m_picalarmtimes[0] >= 20)
												{
													m_picalarmtimes[0] = 1;
													m_picalarmtime_t[0] = time(0);
												}
												else
												{
													m_picalarmtimes[0] = m_picalarmtimes[0] + 1;
												}
											}
										}
										else
										{
											if(m_picalarmtimes[0] >= 35)
											{
												if(m_picalarm[0] == '2')
												{
													sCheck.mode = "1";
													sCheck.TypeDesc		= "静帧";
													sCheck.TypeID		= "0x4";
													sCheck.CheckTime	= m_picalarmtime_t[0];
													ALARMMGR::instance()->CheckAlarm(sCheck,true);
													//APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_DEBUG,string("解除报警-图像静止-频点:"+m_strfreq+"-channelid="+m_strchannelid+"-device="+m_strdeviceid),LOG_OUTPUT_BOTH);
													//
													m_picalarm[0] = '0';
												}
											}
											else
											{
												if(m_picalarmtimes[0] < 30)
												{
													m_picalarmtimes[0] = 31;
													m_picalarmtime_t[0] = time(0);
												}
												else
												{
													m_picalarmtimes[0] = m_picalarmtimes[0] + 1;
												}
											}
										}
									}
									unsigned short AholdVal = programerr.ushortAStatus;
									if(AholdVal&0x1)//无伴音 
									{
										if(m_picalarmtimes[2] >= 5)
										{
											if(m_picalarm[2] != '1')
											{
												sCheck.mode = "0";
												sCheck.TypeDesc		= "无伴音";
												sCheck.TypeID		= "0x10";
												sCheck.CheckTime	= m_picalarmtime_t[2];
												ALARMMGR::instance()->CheckAlarm(sCheck,true);
												//APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_DEBUG,string("报警-无伴音-频点:"+m_strfreq+"-channelid="+m_strchannelid+"-device="+m_strdeviceid),LOG_OUTPUT_BOTH);
												//
												m_picalarm[2] = '1';
											}
										}
										else
										{
											if(m_picalarmtimes[2] >= 20)
											{
												m_picalarmtimes[2] = 1;
												m_picalarmtime_t[2] = time(0);
											}
											else
											{
												m_picalarmtimes[2] = m_picalarmtimes[2] + 1;
											}
										}
									}
									else
									{
										if(m_picalarmtimes[2] >= 35)
										{
											if(m_picalarm[2] == '1')
											{
												sCheck.mode = "1";
												sCheck.TypeDesc		= "无伴音";
												sCheck.TypeID		= "0x10";
												sCheck.CheckTime	= m_picalarmtime_t[2];
												ALARMMGR::instance()->CheckAlarm(sCheck,true);
												//APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_DEBUG,string("解除报警-无伴音-频点:"+m_strfreq+"-channelid="+m_strchannelid+"-device="+m_strdeviceid),LOG_OUTPUT_BOTH);
												//
												m_picalarm[2] = '0';
											}
										}
										else
										{
											if(m_picalarmtimes[2] < 30)
											{
												m_picalarmtimes[2] = 31;
												m_picalarmtime_t[2] = time(0);
											}
											else
											{
												m_picalarmtimes[2] = m_picalarmtimes[2] + 1;
											}
										}
									}
									//
								}
								else
								{
									j++;
								}
							}
							else if(DataType == 0x06)
							{
							}
							else
							{
								break;
							}
						}
						break;
					}
				}
			}
			else
			{
				if(RecvLen == 480)
				{
					sCheck.AlarmType	= ALARM_TR101_290;
					TR290Event ainfo;
					for(int i=0;i<480;)
					{
						memcpy((void*)&ainfo,&RcvBuf[i],12);
						i=i + 12;
						if(ainfo.evt_type == TR_TSSYNC_LST)//同步丢失错误
						{
							if(m_290errcount[0] != ainfo.ext_val.synclst_cnt)
							{
								m_290errcount[0] = ainfo.ext_val.synclst_cnt;
								//
								sCheck.mode = "0";
								sCheck.TypeDesc		= "同步丢失错误";
								sCheck.TypeID		= "2";
								sCheck.CheckTime	= time(0);
								ALARMMGR::instance()->CheckAlarm(sCheck,true);
								//cout<<"同步丢失错误"<<endl;
							}
						}
						else if(ainfo.evt_type == TR_TSSYNC_ERR)//同步字节错误
						{
							if(m_290errcount[1] != ainfo.ext_val.syncerr_cnt)
							{
								m_290errcount[1] = ainfo.ext_val.syncerr_cnt;
								sCheck.mode = "0";
								sCheck.TypeDesc		= "同步字节错误";
								sCheck.TypeID		= "3";
								sCheck.CheckTime	= time(0);
								ALARMMGR::instance()->CheckAlarm(sCheck,true);
								//cout<<"同步字节错误"<<endl;
							}
						}
						else if(ainfo.evt_type == TR_TSPID_ERR)//未指定PID错误 3级错误
						{
							//sCheck.mode = "0";
							//sCheck.TypeDesc		= "PID错误";
							//sCheck.TypeID		= "7";
							//sCheck.CheckTime	= time(0);
							//ALARMMGR::instance()->CheckAlarm(sCheck,true);
							//cout<<"PID错误"<<endl;
						}
						else if(ainfo.evt_type == TR_TSCNT_ERR)//连续计数错误
						{
							bool find = false;
							for(int ic = 0;ic<m_CNTErrList.size();ic++)
							{
								if(m_CNTErrList[ic].evt_pid.evt_tepid == ainfo.evt_pid.evt_tepid)
								{
									find = true;
									if(m_CNTErrList[ic].ext_val.txerr_cnt != ainfo.ext_val.txerr_cnt)
									{
										m_CNTErrList[ic].ext_val.txerr_cnt = ainfo.ext_val.txerr_cnt;
										//
										if(time(0)-m_errCnttime>=5)
										{
											m_errCnttime = time(0);
											//
											sCheck.mode = "0";
											sCheck.TypeDesc		= "连续计数错误";
											sCheck.TypeID		= "5";
											sCheck.CheckTime	= time(0);
											ALARMMGR::instance()->CheckAlarm(sCheck,true);
										}
										//cout<<"连续计数错误"<<endl;
									}
								}
							}
							if(!find)
							{
								if(m_CNTErrList.size()<=100)
								{
									m_CNTErrList.push_back(ainfo);
								}
								else
								{
									cout<<"m_CNTErrList > 100"<<endl;
								}
							}
						}
						else if(ainfo.evt_type == TR_TSTX_ERR)//TS传输错误   2级错误
						{
							//sCheck.mode = "0";
							//sCheck.TypeDesc		= "TS传输错误";
							//sCheck.TypeID		= "8";
							//sCheck.CheckTime	= time(0);
							//ALARMMGR::instance()->CheckAlarm(sCheck,true);
							//cout<<"TS传输错误"<<endl;
						}
						else if(ainfo.evt_type == TR_TAB)//PAT PMT错误
						{
							if(ainfo.evt_pid.evt_tepid == 0 &&ainfo.ext_val.tab_info.tabid == 0)//PAT错误
							{
								unsigned int timeDiff = 0;
								if(ainfo.evt_tim.evt_time>m_PatTime)	
								{
									timeDiff = ainfo.evt_tim.evt_time-m_PatTime;
								}
								else
								{
									timeDiff = ainfo.evt_tim.evt_time+(0xFFFFFFFF-m_PatTime);
								}
								m_PatTime = ainfo.evt_tim.evt_time;

								if(timeDiff>500*27000)
								{
									if(time(0)-m_errPattime>=5)
									{
										m_errPattime = time(0);
										//
										sCheck.mode = "0";
										sCheck.TypeDesc		= "PAT错误";
										sCheck.TypeID		= "4";
										sCheck.CheckTime	= time(0);
										ALARMMGR::instance()->CheckAlarm(sCheck,true);
										//cout<<"PAT错误"<<endl;
									}
								}
							}
							else
							{ 
								bool find = false;
								for(int ip = 0;ip<m_PMTErrList.size();ip++)
								{
									if(m_PMTErrList[ip].evt_pid.evt_tepid == ainfo.evt_pid.evt_tepid)
									{
										find = true;
										unsigned int timeDiff = 0;
										if(ainfo.evt_tim.evt_time>m_PMTErrList[ip].evt_tim.evt_time)	
										{
											timeDiff = ainfo.evt_tim.evt_time-m_PMTErrList[ip].evt_tim.evt_time;
										}
										else
										{
											timeDiff = ainfo.evt_tim.evt_time+(0xFFFFFFFF-m_PMTErrList[ip].evt_tim.evt_time);
										}
										m_PMTErrList[ip].evt_tim.evt_time = ainfo.evt_tim.evt_time;

										if(timeDiff>500*27000)
										{
											if(time(0)-m_errPmttime>=5)
											{
												m_errPmttime = time(0);
												//
												sCheck.mode = "0";
												sCheck.TypeDesc		= "PMT错误";
												sCheck.TypeID		= "6";
												sCheck.CheckTime	= time(0);
												ALARMMGR::instance()->CheckAlarm(sCheck,true);
												//cout<<"PMT错误"<<endl;
											}
										}
									}
								}
								if(!find)
								{
									if(m_PMTErrList.size()<=100)
									{
										m_PMTErrList.push_back(ainfo);
									}
									else
									{
										cout<<"m_PMTErrList > 100"<<endl;
									}
								}
							}
						}
						else if(ainfo.evt_type == TR_TSPID_LST)//丢失错误
						{
							sCheck.mode = "0";
							sCheck.TypeDesc		= "PID错误";
							sCheck.TypeID		= "7";
							sCheck.CheckTime	= time(0);
							ALARMMGR::instance()->CheckAlarm(sCheck,true);
							//cout<<"丢失错误"<<endl;
						}
					}
				}
				else if(RecvLen == 26)
				{
					sCheck.AlarmType	= ALARM_PROGRAM;
					//RcvBuf "vid=0&aud0=0&aud1=0&aud2=0"	
					string tem = (char*)RcvBuf;
					if(tem.size() > 0)
					{
						char va = RcvBuf[4];//0正常 1 黑场 2静帧
						if(va=='0')
						{
							if(m_picalarm[0] != '0')
							{
								sCheck.mode = "1";
								sCheck.TypeDesc		= "静帧";
								sCheck.TypeID		= "0x4";
								sCheck.CheckTime	= time(0);
								ALARMMGR::instance()->CheckAlarm(sCheck,true);
								//APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_DEBUG,string("解除报警-图像静止-频点:"+m_strfreq+"-channelid="+m_strchannelid+"-device="+m_strdeviceid),LOG_OUTPUT_BOTH);
								//
								m_picalarm[0] = va;
							}
							if(m_picalarm[1] != '0')
							{
								sCheck.mode = "1";
								sCheck.TypeDesc		= "黑场";
								sCheck.TypeID		= "0x1";
								sCheck.CheckTime	= time(0);
								ALARMMGR::instance()->CheckAlarm(sCheck,true);
								//APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_DEBUG,string("解除报警-黑屏-频点:"+m_strfreq+"-channelid="+m_strchannelid+"-device="+m_strdeviceid),LOG_OUTPUT_BOTH);
								//
								m_picalarm[1] = va;
							}
						}
						else if((va=='1')&&(m_strVideoPID.size()>1)&&(m_strVideoPID != "5000"))
						{
							if(m_picalarm[1] != '1')
							{
								sCheck.mode = "0";
								sCheck.TypeDesc		= "黑场";
								sCheck.TypeID		= "0x1";
								sCheck.CheckTime	= time(0);
								ALARMMGR::instance()->CheckAlarm(sCheck,true);
								//APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_DEBUG,string("报警-黑屏-频点:"+m_strfreq+"-channelid="+m_strchannelid+"-device="+m_strdeviceid),LOG_OUTPUT_BOTH);
								//
								m_picalarm[1] = va;
								//
								startbacktime = time(0);
							}
							if(time(0)-startbacktime>=2)
							{
								if(m_picalarm[0] != '0')
								{
									sCheck.mode = "1";
									sCheck.TypeDesc		= "静帧";
									sCheck.TypeID		= "0x4";
									sCheck.CheckTime	= time(0)-1;
									ALARMMGR::instance()->CheckAlarm(sCheck,true);
									//APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_DEBUG,string("****解除报警-图像静止-频点:"+m_strfreq+"-channelid="+m_strchannelid+"-device="+m_strdeviceid),LOG_OUTPUT_BOTH);
									//
									m_picalarm[0] = '0';
								}
							}
						}
						else if((va=='2')&&(m_strVideoPID.size()>1)&&(m_strVideoPID != "5000"))
						{
							if(m_picalarm[0] != '2')
							{
								sCheck.mode = "0";
								sCheck.TypeDesc		= "静帧";
								sCheck.TypeID		= "0x4";
								sCheck.CheckTime	= time(0);
								ALARMMGR::instance()->CheckAlarm(sCheck,true);
								//APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_DEBUG,string("报警-图像静止-频点:"+m_strfreq+"-channelid="+m_strchannelid+"-device="+m_strdeviceid),LOG_OUTPUT_BOTH);
								//
								m_picalarm[0] = va;
							}
						}
						char aa0 = RcvBuf[11];//0正常 1不正常
						if(aa0=='0')
						{
							if(m_picalarm[2] != '0')
							{
								sCheck.mode = "1";
								sCheck.TypeDesc		= "无伴音";
								sCheck.TypeID		= "0x10";
								sCheck.CheckTime	= time(0);
								ALARMMGR::instance()->CheckAlarm(sCheck,true);
								//APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_DEBUG,string("解除报警-无伴音-频点:"+m_strfreq+"-channelid="+m_strchannelid+"-device="+m_strdeviceid),LOG_OUTPUT_BOTH);
								//
								m_picalarm[2] = aa0;
							}
						}
						else if(aa0=='1')
						{
							if(m_picalarm[2] != '1')
							{
								sCheck.mode = "0";
								sCheck.TypeDesc		= "无伴音";
								sCheck.TypeID		= "0x10";
								sCheck.CheckTime	= time(0);
								ALARMMGR::instance()->CheckAlarm(sCheck,true);
								//APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_DEBUG,string("报警-无伴音-频点:"+m_strfreq+"-channelid="+m_strchannelid+"-device="+m_strdeviceid),LOG_OUTPUT_BOTH);
								//
								m_picalarm[2] = aa0;
							}
						}
						//+++++++++++++++++++++start 保留不用++++++++++++++++++++++++++++
						char aa1 = RcvBuf[18];//0正常 1不正常
						if(aa1=='0')
						{
						}
						else if(aa1=='1')
						{
							cout<<"1无伴音"<<endl;
						}
						char aa2 = RcvBuf[25];//0正常 1不正常
						if(aa2=='0')
						{
						}
						else if(aa2=='1')
						{
							cout<<"2无伴音"<<endl;
						}
						//++++++++++++++++++++++end++++++++++++++++++++++++++++
					}
				}
				else if(RecvLen == 38)
				{
					sCheck.AlarmType	= ALARM_PROGRAM;
					//RcvBuf "vid=%d&aud0=%d&aud1=%d&aud2=%d&signalsta=%d"	
					string tem = (char*)RcvBuf;
					if(tem.size() > 0)
					{
						char va = RcvBuf[4];//0正常 1 黑场 2静帧
						if(va=='0')
						{
							if(m_picalarm[0] != '0')
							{
								sCheck.mode = "1";
								sCheck.TypeDesc		= "静帧";
								sCheck.TypeID		= "0x4";
								sCheck.CheckTime	= time(0);
								ALARMMGR::instance()->CheckAlarm(sCheck,true);
								//APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_DEBUG,string("解除报警-图像静止-频点:"+m_strfreq+"-channelid="+m_strchannelid+"-device="+m_strdeviceid),LOG_OUTPUT_BOTH);
								//
								m_picalarm[0] = va;
							}
							if(m_picalarm[1] != '0')
							{
								sCheck.mode = "1";
								sCheck.TypeDesc		= "黑场";
								sCheck.TypeID		= "0x1";
								sCheck.CheckTime	= time(0);
								ALARMMGR::instance()->CheckAlarm(sCheck,true);
								//APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_DEBUG,string("解除报警-黑屏-频点:"+m_strfreq+"-channelid="+m_strchannelid+"-device="+m_strdeviceid),LOG_OUTPUT_BOTH);
								//
								m_picalarm[1] = va;
							}
						}
						else if((va=='1')&&(m_strVideoPID.size()>1)&&(m_strVideoPID != "5000"))
						{
							if(m_picalarm[1] != '1')
							{
								sCheck.mode = "0";
								sCheck.TypeDesc		= "黑场";
								sCheck.TypeID		= "0x1";
								sCheck.CheckTime	= time(0);
								ALARMMGR::instance()->CheckAlarm(sCheck,true);
								//APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_DEBUG,string("报警-黑屏-频点:"+m_strfreq+"-channelid="+m_strchannelid+"-device="+m_strdeviceid),LOG_OUTPUT_BOTH);
								//
								m_picalarm[1] = va;
								//
								startbacktime = time(0);
							}
							if(time(0)-startbacktime>=2)
							{
								if(m_picalarm[0] != '0')
								{
									sCheck.mode = "1";
									sCheck.TypeDesc		= "静帧";
									sCheck.TypeID		= "0x4";
									sCheck.CheckTime	= time(0)-1;
									ALARMMGR::instance()->CheckAlarm(sCheck,true);
									//APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_DEBUG,string("****解除报警-图像静止-频点:"+m_strfreq+"-channelid="+m_strchannelid+"-device="+m_strdeviceid),LOG_OUTPUT_BOTH);
									//
									m_picalarm[0] = '0';
								}
							}
						}
						else if((va=='2')&&(m_strVideoPID.size()>1)&&(m_strVideoPID != "5000"))
						{
							if(m_picalarm[0] != '2')
							{
								sCheck.mode = "0";
								sCheck.TypeDesc		= "静帧";
								sCheck.TypeID		= "0x4";
								sCheck.CheckTime	= time(0);
								ALARMMGR::instance()->CheckAlarm(sCheck,true);
								//APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_DEBUG,string("报警-图像静止-频点:"+m_strfreq+"-channelid="+m_strchannelid+"-device="+m_strdeviceid),LOG_OUTPUT_BOTH);
								//
								m_picalarm[0] = va;
							}
						}
						char aa0 = RcvBuf[11];//0正常 1不正常
						if(aa0=='0')
						{
							if(m_picalarm[2] != '0')
							{
								sCheck.mode = "1";
								sCheck.TypeDesc		= "无伴音";
								sCheck.TypeID		= "0x10";
								sCheck.CheckTime	= time(0);
								ALARMMGR::instance()->CheckAlarm(sCheck,true);
								//APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_DEBUG,string("解除报警-无伴音-频点:"+m_strfreq+"-channelid="+m_strchannelid+"-device="+m_strdeviceid),LOG_OUTPUT_BOTH);
								//
								m_picalarm[2] = aa0;
							}
						}
						else if(aa0=='1')
						{
							if(m_picalarm[2] != '1')
							{
								sCheck.mode = "0";
								sCheck.TypeDesc		= "无伴音";
								sCheck.TypeID		= "0x10";
								sCheck.CheckTime	= time(0);
								ALARMMGR::instance()->CheckAlarm(sCheck,true);
								//APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_DEBUG,string("报警-无伴音-频点:"+m_strfreq+"-channelid="+m_strchannelid+"-device="+m_strdeviceid),LOG_OUTPUT_BOTH);
								//
								m_picalarm[2] = aa0;
							}
						}
						//+++++++++++++++++++++start 保留不用++++++++++++++++++++++++++++
						char aa1 = RcvBuf[18];//0正常 1不正常
						if(aa1=='0')
						{
						}
						else if(aa1=='1')
						{
							cout<<"1无伴音"<<endl;
						}
						char aa2 = RcvBuf[25];//0正常 1不正常
						if(aa2=='0')
						{
						}
						else if(aa2=='1')
						{
							cout<<"2无伴音"<<endl;
						}
						//++++++++++++++++++++++end++++++++++++++++++++++++++++
						//
						char sa = RcvBuf[37];//0 为 无信号 1 为 有信号 2 为 解码异常
						if(sa=='0')
						{
							if(m_picalarm[5] != '0')
							{
								sCheck.mode = "0";
								sCheck.TypeDesc		= "失锁";
								sCheck.TypeID		= "0xE";
								sCheck.CheckTime	= time(0);
                                AlarmTime = sCheck.CheckTime;
                                ALARMMGR::instance()->CheckAlarm(sCheck,true);
                                //string time_print = TimeUtil::DateTimeToString(sCheck.CheckTime);
                                /*if(m_strfreq=="626000" || m_strfreq=="602000" || m_strfreq=="610000")
                                {
                                    APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_DEBUG,string("报警-失锁黑屏-频点:"+m_strfreq+"-channelid="+m_strchannelid+"TIME:"+time_print),LOG_OUTPUT_BOTH);
                                }*/
								//
                                //APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_DEBUG,string("****报警字符串:" + tem),LOG_OUTPUT_BOTH);
								m_picalarm[5] = sa;
							}
						}
						else if(sa=='1')
						{
							if(m_picalarm[5] != '1')
							{
								sCheck.mode = "1";
								if(m_picalarm[5] == '2')
								{
                                    sCheck.TypeDesc		= "解码失败";
                                    sCheck.TypeID		= "0x1";
								}
								else
								{
                                    sCheck.TypeDesc		= "失锁";
                                    sCheck.TypeID		= "0xE";
								}
								sCheck.CheckTime	= time(0);
								ALARMMGR::instance()->CheckAlarm(sCheck,true);
								//APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_DEBUG,string("解除报警-失锁黑屏-频点:"+m_strfreq+"-channelid="+m_strchannelid),LOG_OUTPUT_BOTH);
								//
								m_picalarm[5] = sa;
							}
						}
						else if(sa=='2')
						{
							if((m_picalarm[5] != '0')&&(m_picalarm[5] != '2'))
							{
								sCheck.mode = "0";
								sCheck.TypeDesc		= "解码失败";
								sCheck.TypeID		= "0x1";
								sCheck.CheckTime	= time(0);
								ALARMMGR::instance()->CheckAlarm(sCheck,true);
								//APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_DEBUG,string("报警-解码失败黑屏-频点:"+m_strfreq+"-channelid="+m_strchannelid),LOG_OUTPUT_BOTH);
								//
								m_picalarm[5] = sa;
							}
						}
						//------------------------------------------------------
					}
				}
				else
				{
					cout<<"UdpRecvAlarmThread RecvLen:"<<RecvLen<<endl;
				}
			}
		}
	}
	bFlag = false;
	return 0;
}
int UdpRecvAlarmThread::Stop()
{
	bFlag = false;
	this->wait();
	return 0;
}
