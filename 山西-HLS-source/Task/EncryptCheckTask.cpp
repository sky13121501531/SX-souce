


#include "ace/SOCK_Connector.h"
#include "ace/INET_Addr.h"
#include "EncryptCheckTask.h"
#include "Scheduler.h"
#include "../DBAccess/DBManager.h"
#include "../Foundation/Osfunction.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/AppLog.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include "../DeviceAccess/DeviceAccessMgr.h"
#include "../BusinessProcess/ChanEncryptCheckMgr.h"
#include "../DeviceAccess/HTTPDeviceAccess.h"


const int RECVBUFLEN = 1024*20;



bool less_Chan(const sChannelInfo& chanInfo1, const sChannelInfo& chanInfo2) 
{
	if (chanInfo1.Freq < chanInfo2.Freq)
	{
		return true;
	}
	else if (chanInfo1.Freq > chanInfo2.Freq)
	{
		return false;
	}
	else if (chanInfo1.Freq == chanInfo2.Freq)
	{
		if (chanInfo1.ServiceID < chanInfo2.ServiceID)
		{
			return true;
		}
		else
			return false;
	}
	else
		return true;
}


EncryptCheckTask::EncryptCheckTask(std::string strTaskXml) : DeviceRelatedTask( strTaskXml )
{
	m_bChangeCardConfig = true;

	m_bAudioCAFailed = false;
	m_bNoAudio = m_bVideoCAFailed = false;
	m_bUnLock = false;
	m_bNoVideo = false;

	XmlParser parser( strTaskXml.c_str() );
	pXMLNODE infoNode = parser.GetNodeFromPath("Msg/EncryptCheckTask/ICInfo");
	parser.GetAttrNode(infoNode, "DeviceId", DeviceID);
	parser.GetAttrNode(infoNode, "CardNo", m_strCardNum);
	parser.GetAttrNode(infoNode, "Ip", m_strCardIp);
	parser.GetAttrNode(infoNode, "Position", m_strPosition);
	parser.GetAttrNode(infoNode, "ListenPort", m_nListenPort);

	TaskScheduler = new Scheduler();
	if (TaskScheduler != NULL)
		TaskScheduler->SetRunTime(TimeUtil::GetCurDateTime());
}


EncryptCheckTask::~EncryptCheckTask(void)
{
}

void EncryptCheckTask::Run( void )
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%s]��Ȩ�������ִ�� !\n",m_strPosition.c_str()));
	bRun = true;

	SetRunning(); 
	//�����û��ִ�н�������,ca����Ϊ0,���������޿��ж�,Ĭ������Ȩ��
	//���ͨ�����Ƿ���忨IP��Ӧ����ֹ�޸������ļ���������쳣
	int pos = m_strPosition.find("_");
	m_strMonitorIndex = m_strPosition.substr(0,pos);
	string tem = m_strPosition.substr(pos+1,m_strPosition.size());
	pos = tem.find("_");
	m_strCardIndex = tem.substr(0,pos);
	m_strChanIndex = tem.substr(pos+1,tem.size());
	//
	if(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")
	{
		sDeviceInfo deviceInfo;
		PROPMANAGER::instance()->GetDeviceInfo(m_strMonitorIndex,m_strCardIndex,"",deviceInfo);
		m_strMonitorip = deviceInfo.s_deviceip;
		m_strMonitorport = deviceInfo.s_deviceport;
	}
	else
	{
		PROPMANAGER::instance()->GetDevMonitorInfo(StrUtil::Str2Int(m_strMonitorIndex),m_strMonitorip,m_strMonitorport);
	}
	if (m_strMonitorip != m_strCardIp)
	{
		if (TaskScheduler != NULL)
			TaskScheduler->ModifyExpiredTime(TimeUtil::DateTimeToStr(time(0)-1));

		SetFinised(); 
		ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%s]��IP[%s]��IP���豸MonitorIndex��Ų�һ��,��Ȩ�������ֹͣ!\n",m_strPosition.c_str(), m_strCardIp.c_str()));
		return;
	}

	//ִ����Ȩ����߼�
	bool bRet = CheckEncryptInfo();
	if (!bRet || !bRun)
	{
		if (TaskScheduler != NULL)
			TaskScheduler->ModifyExpiredTime(TimeUtil::DateTimeToStr(time(0)-1));

		SetFinised(); 
		ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%s]��Ȩ���ʧ�ܣ���Ȩ�������ֹͣ!\n",m_strPosition.c_str()));
		return;
	}

	//������Ȩ��Ϣ
	CHANENCRYPTCHECKMGR::instance()->SetEncryptInfo(m_strCardNum, m_vecChanInfo);

	RetValue = RUN_SUCCESS;

	if (TaskScheduler != NULL)
		TaskScheduler->ModifyExpiredTime(TimeUtil::DateTimeToStr(time(0)-1));

	SetFinised(); 

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%s]��Ȩ�������ֹͣ !\n",m_strPosition.c_str()));
	return;
}

std::string EncryptCheckTask::GetObjectName()
{
	return std::string("EncryptCheckTask");
}

std::string EncryptCheckTask::GetTaskName()
{
	return std::string("��Ȩ�������");
}

bool EncryptCheckTask::CheckEncryptInfo()
{
	GetAllChanInfo();

	int nCAFailedCount = 1;
	std::string strLastFreq;
	std::vector<sChannelInfo>::iterator chanIter = m_vecChanInfo.begin();
	string Encryp = "1";//Ĭ������Ȩ
	if(PROPMANAGER::instance()->GetCurWorkVersion() != "5.0")
	{
		HTTPDeviceAccess* pDeviceAccess = new HTTPDeviceAccess(StrUtil::Str2Int(m_strMonitorIndex),m_strMonitorip,StrUtil::Str2Int(m_strMonitorport));
		//
		string cmd = "POST /io/info  HTTP/1.1  \r\n\r\n";
		string strRet;
		cmd+="chl=";
		cmd+=m_strCardIndex;
		cmd+="_";
		cmd+=m_strChanIndex;
		pDeviceAccess->SendTaskMsg(cmd,strRet);
		if(!pDeviceAccess)
		{
			delete pDeviceAccess;
		}
		//
		if(strRet.size() <= 0)
		{
			cout<<"CheckEncryptInfo����ʧ��:"<<m_strMonitorip<<"--"<<m_strMonitorport<<endl;
			return true;
		}
		int pos = strRet.find("<html>");
		string strtem = strRet;
		if(pos >= 0)
		{
			strtem = strRet.substr(pos,strRet.size()-pos);
		}
		else
		{
			cout<<"CheckEncryptInfoû��<html>���:"<<m_strMonitorip<<"--"<<m_strMonitorport<<endl;
		}
		//
		string msg = m_strMonitorip+string(":")+cmd+strRet;
		APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,msg,LOG_OUTPUT_FILE);
		//
		XmlParser psr;
		psr.Set_xml(strtem);
		pXMLNODE node=psr.GetNodeFromPath("html/body");
		pXMLNODELIST nodeList = psr.GetNodeList(node);
		int count = nodeList->Size();
		pXMLNODE scanNode = NULL;
		for(int k=0;k<count;k++)
		{
			pXMLNODE childNode = psr.GetNextNode(nodeList);
			string name = psr.GetNodeName(childNode);
			if(name == "CAInfo")
			{
				string ca;
				psr.GetAttrNode(childNode,"caid",ca);
				if(StrUtil::Str2Int(ca)!=152)
				{
					Encryp = "0";
					break;
				}
			}
			else if(name == "err")
			{
				string er;
				psr.GetAttrNode(childNode,"code",er);
				if(er != "0")
				{
					cout<<"err code:"<<er<<endl;
					break;
				}
			}
		}
	}
	//
	for (; chanIter!=m_vecChanInfo.end(); chanIter++)
	{
		if (!bRun)	//����Ҫ���˳���
		{
			break;
		}

		//��ȡ��Ҫ��Ƶ����Ϣ
		sChannelInfo chanInfo = *chanIter;
		std::string strCurFreq = chanInfo.Freq;
		if (strCurFreq != strLastFreq)
		{
			ACE_DEBUG ((LM_DEBUG,"(%T| %t) �л�Ƶ��[%s]!\n", strCurFreq.c_str()));
			strLastFreq = strCurFreq;
		}
		//if(Encryp == "0")//û��CAM����CAM������ �ݲ��� 2017-11-13 �������ղ���
		//{
		//	chanIter->FreeCA = Encryp;
		//}
		OSFunction::Sleep(0, 10);
	}


	return true;
}

void EncryptCheckTask::GetAllChanInfo()
{
	//��ȡ����Ƶ��
	CHANNELINFOMGR::instance()->GetChannelInfoByDvbtype(DVBC, m_vecChanInfo);

	//����Ƶ������
	sort(m_vecChanInfo.begin(), m_vecChanInfo.end(), less_Chan);

	//ɾ��vector�н�Ŀ��Ϊ�յ�Ƶ��
	std::vector<sChannelInfo>::iterator chanIter = m_vecChanInfo.begin();
	while ( chanIter != m_vecChanInfo.end() )
	{
		if ((chanIter->ProgramName.length()<1) || (chanIter->ProgramName.find("unknown") != -1))
		{
			chanIter = m_vecChanInfo.erase( chanIter );
		}
		else
			chanIter ++;
	}

	return;
}

bool EncryptCheckTask::AcceptCheckInfo()
{
	bool bRet = true;

	//open
	std::string strListenIp = PROPMANAGER::instance()->GetDeviceIp();
	ACE_INET_Addr checkAddr(m_nListenPort, strListenIp.c_str());
	ACE_SOCK_Acceptor checkAcceptor;
	if (checkAcceptor.open(checkAddr) == -1)
	{
		return false;
	}

	std::string strBeginTime = TimeUtil::GetCurDateTime();
	while (true)
	{
		//�ж��Ƿ�ʱ
		std::string strCurTime = TimeUtil::GetCurDateTime();
		if (TimeUtil::DiffSecond(strCurTime, strBeginTime) >= 5)	//5��û���յ�ca����ʧ����Ϣ������Ϊ���ųɹ�
		{
			bRet = false;
			break;
		}

		//accept 
		ACE_Time_Value TimeOut(1);	//1s��ʱ
		if (checkAcceptor.accept(m_client,&m_clientAddr, &TimeOut) == -1)		
		{
			bRet = false;
		}
		else
		{
			bRet = RecvEncryInfo();	//���ս��ű���
			if (bRet)
			{
				break;	//���յ����ű���
			}
		}

		m_client.close();
	}

	checkAcceptor.close();

	return bRet;
}

bool EncryptCheckTask::RecvEncryInfo()
{
	bool bEncry = true;

	char *buf = new char[50*RECVBUFLEN];
	if(buf==NULL)
		return false;
	memset(buf, 0, 50*RECVBUFLEN);

	ssize_t recvlen = 0;
	RecvInfo(buf, 50*RECVBUFLEN, recvlen);
	if (recvlen <= 0)
	{
		delete buf;
		buf=NULL;
		return false;
	}

	std::string xml = buf;
	if (xml.find("<?xml") != string::npos)		//PSISI or ETR290
	{
		std::string tempxml;
	}
	else
	{
		bEncry = ProcessEncryInfo(buf,recvlen);
	}

	delete buf;
	buf =NULL;


	return bEncry;
}
bool EncryptCheckTask::RecvInfo(char* Info,const int buflen,ssize_t& rcvlen )
{
	int CountTimes = 10;	//���տͻ��˷������ݵĴ���
	rcvlen = 0;
	try
	{
		char tempbuf[RECVBUFLEN] = {0};
		int templen = 0;
		ACE_Time_Value TimeOut(0,50);
		int recvnum=0;
		while(true)
		{
			templen = m_client.recv(tempbuf,1024,&TimeOut);
			if (templen < 0)	
			{
				if (ACE_OS::last_error() == ETIME)	//�����ʱ�ˣ���ѭ������
				{
					memset(tempbuf, 0, RECVBUFLEN);
					OSFunction::Sleep(0, 50);
					continue;
				}
				else	//��������ֱ������ѭ��
					break;
			}
			else if (templen==0 || rcvlen+templen >= buflen)
			{
				break;
			}

			memcpy(Info+rcvlen,tempbuf,templen);
			memset(tempbuf,0,RECVBUFLEN);

			rcvlen += templen;
		}
	}
	catch(...)
	{
		;
	}

	return true;
}


bool EncryptCheckTask::ProcessEncryInfo(const char* buf, const int len)
{
	if (len < sizeof(RetMessage_Obj))
		return false;

	RetMessage_Handle alarm=(RetMessage_Obj*)buf;

	if (alarm->ph.header != 0x49 || alarm->ph.msg_type != MSG_RET_ALERT)
		return false;

	int Alarm[4] = {0x0,0x0,0x0,0x0};
	int status=alarm->status;					// 0x01020304 ���� Tuner 3:01��2:02��1:03��0:04

	for(int Tunerid=0;Tunerid<4;Tunerid++)
	{
		switch(Tunerid)
		{
		case 0:
			Alarm[0] = (status&0x000000FF);
			break;
		case 1:
			Alarm[1] = (status>>8)&0x000000FF;
			break;
		case 2:
			Alarm[2] = (status>>16)&0x000000FF;
			break;
		case 3:
			Alarm[3] = status>>24;
			break;
		default:
			break;
		}
	}

	for(int Tunerid=0;Tunerid<4;Tunerid++)
	{
		if (Alarm[Tunerid] == 0x0)	//���쳣
			continue;

		int nStatus = Alarm[Tunerid];

		CollectAlarmType( nStatus );
		if ( m_bAudioCAFailed )	//audio and video failed
		{
			m_bAudioCAFailed = m_bVideoCAFailed = false;
			return true;
		}
		else if (m_bVideoCAFailed /*&& m_bNoAudio*/)
		{
			m_bVideoCAFailed /*= m_bNoAudio*/ = false;
			return true;
		}
		else if (m_bNoVideo)
		{
			m_bNoVideo = false;
			return true;
		}
		else if (m_bUnLock)
		{
			m_bUnLock = false;
			return true;
		}
	}

	return false;
}


bool EncryptCheckTask::CollectAlarmType(int AlarmType)
{
	sTypeDesc  TypeDesc;
	static int NoSignal=0;
	static int UnCaAudio=0;
	static int UnCaVideo=0;

	std::string strDesc;
	if((AlarmType&0x80) ==0x0)
	{
		if((AlarmType&S_UNLOCK) == S_UNLOCK)
		{
			strDesc += "ʧ�� ";
			m_bUnLock = true;
		}
		else
		{
			if((AlarmType&V_MISS) ==V_MISS)
			{
				//if (DEVICEACCESSMGR::instance()->CheckFreqLock(deviceid) == TRUE)//��ͨ���������� 
				{
					strDesc += "����Ƶ ";
				}
				//else
				{
					strDesc += "���ز� ";
				}
			}
			if((AlarmType&A_SILENCE) == A_SILENCE)
			{
				strDesc += "�ް��� ";
			}
			if((AlarmType&V_STATIC) == V_STATIC)
			{
				strDesc += "ͼ��ֹ ";
			}
			if((AlarmType&C_SLICE_V_STATIC) == C_SLICE_V_STATIC)
			{
				strDesc += "���� ";
			}
			if((AlarmType&V_BLACK_V_STATIC) == V_BLACK_V_STATIC)
			{
				strDesc += "���� ";
			}	
		}
	}
	else if((AlarmType&0x80) ==0x80)
	{
		if((AlarmType&NO_SIGNAL) == NO_SIGNAL)
		{
			strDesc += "���ź� ";
		}
		if((AlarmType&NO_VIDEO) == NO_VIDEO)
		{
			strDesc += "����Ƶ ";
			m_bNoVideo = true;
		}
		if((AlarmType&NO_AUDIO) == NO_AUDIO)
		{
			strDesc += "����Ƶ ";
			m_bNoAudio = true;
		}
		if((AlarmType&UNCA_AUDIO) == UNCA_AUDIO)
		{
			strDesc += "��Ƶ����ʧ�� ";
			m_bAudioCAFailed = true;
		}
		if((AlarmType&UNCA_VIDEO) == UNCA_VIDEO)
		{
			strDesc += "��Ƶ����ʧ�� ";
			m_bVideoCAFailed = true;
		}	
	}
	//if (strDesc != "�ް��� ")
	{
		ACE_DEBUG ((LM_DEBUG,"(%T| %t) %s !\n", strDesc.c_str()));
	}
	return true;
}



std::string EncryptCheckTask::CreateTunerXml(sChannelInfo chanInfo)
{
	eDVBType eDvbtype = DVBC;

	VideoParam videoParam;
	if(chanInfo.HDTV == "1")
		PROPMANAGER::instance()->GetHDRealStreamVideoParam(eDvbtype, videoParam);
	else if(chanInfo.HDTV == "0")
		PROPMANAGER::instance()->GetSDRealStreamVideoParam(eDvbtype, videoParam);

	std::string strAudioBps;
	PROPMANAGER::instance()->GetAudioBps(eDvbtype, strAudioBps);

	std::string strWidth, strHeight, strBps;
	strWidth = videoParam.width;
	strHeight = videoParam.height;
	strBps = StrUtil::Int2Str(StrUtil::Str2Int(videoParam.bps)*1000);

	int LBps = StrUtil::Str2Int(strBps);
	if(eDvbtype==RADIO||eDvbtype==AM)
	{
		strBps = StrUtil::Int2Str(LBps/1000); 
	}
	else
	{
		strBps = StrUtil::Int2Str(LBps/1000-StrUtil::Str2Int(strAudioBps));
		if(StrUtil::Str2Int(strBps)<150 && chanInfo.HDTV=="0")
		{	
			strBps = "150";
		}
		else if(StrUtil::Str2Int(strBps)>2000 && chanInfo.HDTV=="0")
		{
			strBps = "2000";
		}
		else if(StrUtil::Str2Int(strBps)>4000 && chanInfo.HDTV=="1")
		{
			strBps = "4000";
		}
	}

	//ƴ�շ��͸��忨��xml
	char *pXml = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg DVBType=\"DVBC\"><TSQuery><TS/></TSQuery></Msg>";
	XmlParser retParser( pXml );
	pXMLNODE tsNode = retParser.GetNodeFromPath("Msg/TSQuery/TS");

	//������Ϊ���ص�Ӳ��ͨ����xml��������ֵ
	retParser.SetAttrNode("Freq", chanInfo.Freq, tsNode);
	retParser.SetAttrNode("OrgNetID", chanInfo.OrgNetID, tsNode);
	retParser.SetAttrNode("TsID", chanInfo.TsID, tsNode);
	retParser.SetAttrNode("SymbolRate", chanInfo.SymbolRate, tsNode);
	retParser.SetAttrNode("QAM", chanInfo.QAM, tsNode);
	retParser.SetAttrNode("ServiceID", chanInfo.ServiceID, tsNode);
	retParser.SetAttrNode("ServiceName", chanInfo.ProgramName, tsNode);
	retParser.SetAttrNode("PmtPID", chanInfo.PmtPID, tsNode);
	retParser.SetAttrNode("PcrPID", chanInfo.PcrPID, tsNode);
	retParser.SetAttrNode("VideoPID", chanInfo.VideoPID, tsNode);
	retParser.SetAttrNode("AudioPID", chanInfo.AudioPID, tsNode);
	retParser.SetAttrNode("IsChangeFreq", std::string("1"),tsNode);
	retParser.SetAttrNode("HDTV", chanInfo.HDTV, tsNode);
	retParser.SetAttrNode("DataType", std::string(""), tsNode);
	retParser.SetAttrNode("Width", strWidth, tsNode);
	retParser.SetAttrNode("Height", strHeight, tsNode);
	retParser.SetAttrNode("Fps", std::string("25"), tsNode);
	retParser.SetAttrNode("Bps", strBps, tsNode);

	retParser.SetAttrNode("CodingFormat", std::string("cbr"), tsNode);

	OSDInfo osd;
	PROPMANAGER::instance()->GetOSDInfo(eDvbtype, chanInfo.HDTV, osd);
	retParser.SetAttrNode("SystemClock", (long)time(0), tsNode);
	retParser.SetAttrNode("OSDFontSize", osd.FontSize, tsNode);
	retParser.SetAttrNode("OSDInfo", osd.Info, tsNode);
	retParser.SetAttrNode("OSDInfoX", osd.InfoX, tsNode);
	retParser.SetAttrNode("OSDInfoY", osd.InfoY, tsNode);
	retParser.SetAttrNode("OSDTimeType", osd.TimeType, tsNode);
	retParser.SetAttrNode("OSDTimeX", osd.TimeX, tsNode);
	retParser.SetAttrNode("OSDTimeY", osd.TimeY, tsNode);
	retParser.SetAttrNode("OSDProgramX", osd.ProgramX, tsNode);
	retParser.SetAttrNode("OSDProgramY", osd.ProgramY, tsNode);

	std::string retXML;//���ص�xml
	retParser.SaveToString(retXML);
	return retXML;
}

