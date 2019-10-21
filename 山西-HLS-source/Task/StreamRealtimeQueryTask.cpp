
#include "StreamRealtimeQueryTask.h"
#include "Scheduler.h"
#include "TranslateDownXML.h"
#include "TranslateUpXML.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/PropManager.h"
#include "../Communications/TsSenderMgr.h"
#include "../DeviceAccess/DeviceAccessMgr.h"
#include "../DeviceAccess/TsFetcherMgr.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include "../Foundation/XmlParser.h"
#include "../Foundation/AppLog.h"
#include "ace/Synch.h"
#include "./TranslateXMLForDevice.h"
#include "../FileSysAccess/TSRecorder.h"
#include "../DBAccess/DBManager.h"
#include "../BusinessProcess/BusinessLayoutMgr.h"
#include <vector>
#include <iostream>

using namespace std;
ACE_Thread_Mutex g_RealtimeMutex;
int g_realstreamusedev = -1;//ʵʱ��Ƶ������
const std::string MANURECORD_EXPIREDAYS = "7";

class ManuRecord_Task : public ACE_Task<ACE_MT_SYNCH>
{
public:
	ManuRecord_Task();
	~ManuRecord_Task(){};

};

ManuRecord_Task::ManuRecord_Task()
{
	msg_queue()->high_water_mark(188*8192);
	msg_queue()->low_water_mark(188*8192);
}

StreamRealtimeQueryTask::StreamRealtimeQueryTask() : DeviceRelatedTask()
{

}

StreamRealtimeQueryTask::StreamRealtimeQueryTask(std::string strXML) : DeviceRelatedTask(strXML)
{
	bManualRecord=false;
	std::string freq,version,OrgNetID,TsID,ServiceID,VideoPID,AudioPID,Code,Bps,CodingFormat,sWidth,sHeight,sOsd;
	XmlParser parser(strXML.c_str());
	pXMLNODE rootNode = parser.GetNodeFromPath("Msg");
	parser.GetAttrNode(rootNode,"Version",version);            //��ñ�׼xml��RealtimeQueryTask�ڵ��Freq����ֵ
	pXMLNODE queryNode = parser.GetNodeFromPath("Msg/StreamRealtimeQuery/RealtimeQueryTask");
	parser.GetAttrNode(queryNode,"Freq",freq);            //��ñ�׼xml��RealtimeQueryTask�ڵ��Freq����ֵ
	parser.GetAttrNode(queryNode,"OrgNetID",OrgNetID);    //��ñ�׼xml��RealtimeQueryTask�ڵ��OrgNetID����ֵ
	parser.GetAttrNode(queryNode,"TsID",TsID);            //��ñ�׼xml��RealtimeQueryTask�ڵ��TsID����ֵ
	parser.GetAttrNode(queryNode,"ServiceID",ServiceID);  //��ñ�׼xml��RealtimeQueryTask�ڵ��ServiceID����ֵ
	parser.GetAttrNode(queryNode,"VideoPID",VideoPID);  //��ñ�׼xml��RealtimeQueryTask�ڵ��VideoPID����ֵ
	parser.GetAttrNode(queryNode,"AudioPID",AudioPID);  //��ñ�׼xml��RealtimeQueryTask�ڵ��AudioPID����ֵ
	parser.GetAttrNode(queryNode,"Bps",Bps);  //��ñ�׼xml��RealtimeQueryTask�ڵ��AudioPID����ֵ
	parser.GetAttrNode(queryNode,"CodingFormat",CodingFormat);  //��ñ�׼xml��RealtimeQueryTask�ڵ��AudioPID����ֵ
	parser.GetAttrNode(queryNode,"Width",sWidth);  
	parser.GetAttrNode(queryNode,"Height",sHeight);  
	parser.GetAttrNode(queryNode,"CDeviceID",m_cdevid);  
	parser.GetAttrNode(queryNode,"TDeviceID",m_tdevid);  
	parser.GetAttrNode(queryNode,"OSDEnable",sOsd); 
	//
	strServiceID = ServiceID;
	strVideoPID = VideoPID;
	strAudioPID = AudioPID;
	mBps = Bps;
	//

	float nTempTime = 1;
	parser.GetAttrNode(queryNode, "RunTime", nTempTime);	//��λΪ��Сʱ
	if (nTempTime == 0)
	{
		m_nRunTime = 60*60*24;	//һ��
	}
	else
	{
		m_nRunTime = nTempTime*60*60 + 3;	//�Ӵ�3s�ĵ�Ƶʱ��
	}
	Freq = freq;                                          
	strDeviceXML = TranslateXMLForDevice::TranslateStreamRealTimeXML(strXML);//�Զ����׼XMLת��Ϊ��Ӳ��ͨ�ŵ�xml
	if (ATV==DVBType || RADIO==DVBType || DVBType==AM || DVBType==CTV)
	{
		ChannelID=freq;
	}
	else
	{
		if (Freq == strHDMIFreq)
		{
			ChannelID = Freq;
		}
		else
		{
			CHANNELINFOMGR::instance()->GetChannelID(DVBType,OrgNetID,TsID,ServiceID,VideoPID,AudioPID,Code,ChannelID);
			if(ChannelID.size()<=0)
			{
				CHANNELINFOMGR::instance()->GetChannelID(DVBType,freq,ServiceID,ChannelID);
			}
		}

	}
	mIsHD = false;
	if (CHANNELINFOMGR::instance()->IsHDTV(DVBType,OrgNetID,TsID,ServiceID))
	{
		mIsHD = true;
	}
	if(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")
	{
		if (StrUtil::Str2Int(sWidth)>720||StrUtil::Str2Int(sHeight)>576)
		{
			mIsHD = true;
		}
	}
	else
	{
		if (StrUtil::Str2Int(sWidth)>640||StrUtil::Str2Int(sHeight)>480)
		{
			mIsHD = true;
		}
	}
	//
	if(version == "3.0")
	{
		vector<int> codlis;
		PROPMANAGER::instance()->SeparateStrVec(m_cdevid,codlis);
		for(int i=0;i<codlis.size();i++)
		{
			if(i == 0)
			{
				if(PROPMANAGER::instance()->SetCoderLinkTunerinfo(codlis[i],StrUtil::Str2Int(m_tdevid),DVBType,Freq,ServiceID,mIsHD,Bps,"ʵʱ��Ƶ������..."))
				{
					DeviceID = codlis[i];
				}
				else
				{
					break;
				}
			}
			else
			{
				PROPMANAGER::instance()->FlagCoderInfo(codlis[i],Freq,ServiceID,DVBType,mIsHD,"ʵʱ��Ƶ������...",StrUtil::Int2Str(DeviceID));
			}
		}
	}
	//
	if(sOsd == "0")
	{
		mIsOSD = true;
	}
	else
	{
		mIsOSD = false;
	}
	mRecordFileName = "";	//�ֶ�¼���Ϊ�浽tempfile����  jidushan 110406

    TaskScheduler = new Scheduler();
	if (TaskScheduler != NULL)
	{
		TaskScheduler->SetRunTime(TimeUtil::GetCurDateTime());
	}
}

StreamRealtimeQueryTask::~StreamRealtimeQueryTask()
{

}
void StreamRealtimeQueryTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]ʵʱ��Ƶ����ִ�� !\n",DeviceID));
	//
	if(GetVersion() == "3.0")
	{
		vector<int> codlis;
		PROPMANAGER::instance()->SeparateStrVec(m_cdevid,codlis);
		for(int i=0;i<codlis.size();i++)
		{
			if(i == 0)
			{
				PROPMANAGER::instance()->SetCoderLinkTunerinfo(codlis[i],StrUtil::Str2Int(m_tdevid),DVBType,Freq,strServiceID,mIsHD,mBps,"ʵʱ��Ƶ������...");
			}
			else
			{
				PROPMANAGER::instance()->FlagCoderInfo(codlis[i],Freq,strServiceID,DVBType,mIsHD,"ʵʱ��Ƶ������...",StrUtil::Int2Str(DeviceID));
			}
		}
	}
	//
	g_realstreamusedev = DeviceID;
	bRun = true;
	SetRunning();//����״̬����Ϊ��������

	if(RetValue != SET_SUCCESS)	//ʵʱ��Ƶ��������ʧ��
	{			
		SendXML(TranslateUpXML::TranslateStreamRealtimeQuery(this));
		SetFinised();//����ֹͣ
		SetExpired();
		ACE_DEBUG ((LM_DEBUG,"(%T| %t)ʵʱ��Ƶ��������ʧ��!\n"));
		return;
	}

	TSFETCHERMGR::instance()->StopHistoryTask(DeviceID);			//�����ʷ��Ƶ

	bool FindRecordDevice = false;
	int tempID = -1;
	if(ChannelID == strHDMIFreq)//��������¼��ͷ���¼��ͨ��,����һ��HDMI����ͬʱ��ת��ͨ����¼����ʵʱ
	{
		if(BUSINESSLAYOUTMGR::instance()->GetAutoRecordDeviceIdByChannelID(DVBType,ChannelID,tempID))
		{
			FindRecordDevice = true;
			DeviceID = tempID;			
		}
	}
	else
	{
		if(PROPMANAGER::instance()->IsRealTimeFromRecord(DVBType)&&BUSINESSLAYOUTMGR::instance()->GetAutoRecordDeviceIdByChannelID(DVBType,ChannelID,tempID))
		{
			FindRecordDevice = true;
			DeviceID = tempID;			
		}
	}
	
	RetValue=RUN_SUCCESS;

	if (FindRecordDevice)	//����ظ������Զ�¼���ͨ��������ֱ�ӷ����˳�
	{
		SendXML(TranslateUpXML::TranslateStreamRealtimeQuery(this));	//����ת�����xml

		if (TaskScheduler != NULL)
			TaskScheduler->ModifyExpiredTime(TimeUtil::DateTimeToStr(time(0)-1));

		SetFinised();																			//����ֹͣ
		bRun = false;
		ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]ʵʱ��Ƶ����¼��ͨ����ֹͣ !\n",DeviceID));
		return;
	}
	else//���û���ҵ�����Ƶ�������¼��ͨ��
	{
		int idevID = -1;
		SendXML(TranslateUpXML::TranslateStreamRealtimeQuery(this));	//����ת�����xml
		TSFETCHERMGR::instance()->SetTsDeviceXml(DeviceID,strDeviceXML);
		TSFETCHERMGR::instance()->IncreaseTaskNum(DeviceID);
	}

	//�ֶ�¼��
	ACE_Task<ACE_MT_SYNCH>* Task = new ManuRecord_Task;
	TSFETCHERMGR::instance()->SetRecordTask(DeviceID,Task);

	const int BufLen = 188*4096;
	const int BufSize = 188*1024;

	unsigned char* TsBuf = new unsigned char[BufLen];//�������ݻ�����
	memset(TsBuf,0,BufLen);

	int PacketLen = 0;
	int FirstCheck = 10;

	std::string strBeginTime = TimeUtil::GetCurDateTime();

	time_t curtime = time(0);
	int waittime = 10;
	bool bfirst = true;
	while(bRun && IsRunning())
	{
		std::string strCurTime = TimeUtil::GetCurDateTime();
		if ( TimeUtil::DiffSecond(strCurTime,strBeginTime) >= m_nRunTime )
		{
			std::cout<<"ʵʱ��Ƶ����ʱ���ѵ����Զ�ֹͣ!"<<std::endl;
			break;
		}


		if (TSSENDERMGR::instance()->HasClient(DeviceID) == false)//û�������û��������˳�
		{
			bfirst = false;
			if(time(0)-curtime > waittime)
			{
				break;
			}
			else
			{
				OSFunction::Sleep(0,50);
				continue;
			}
		}
		if(!bfirst)
		{
			waittime = 0;
			curtime = time(0);
		}
		//���ֶ�¼������,���ֶ�¼��������ִ��ʱ������
		if(bManualRecord&&time(0)-TimeUtil::StrToDateTime(mStartTime)<=StrUtil::Str2Int(mRecordTime)*60/*TimeUtil::StrToSecondTime(mRecordTime)*/)
		{
			ACE_Message_Block *mb = NULL;

			ACE_Time_Value OutTime(ACE_OS::gettimeofday()+ACE_Time_Value(2));
			if (Task->getq(mb,&OutTime) != -1 && mb != NULL)//�����ݽ����߳�ȡ����
			{
				if (mb->length() > 0)
				{
					if (PacketLen>=BufLen-mb->length())
					{
						mRecordFile.WriteBuf(TsBuf,PacketLen);//����д���ļ�
						memset(TsBuf,0,BufLen);
						PacketLen = 0;
					}
					memcpy(TsBuf+PacketLen,(unsigned char*)mb->rd_ptr(),mb->length());//�������ڴ�
					PacketLen +=(int) mb->length();
				}

				
				mb->release();
			}
		}
		//���ֶ�¼������,���ֶ�¼�������Ѿ���ʱ
		else if(bManualRecord&&time(0)-TimeUtil::StrToDateTime(mStartTime)>StrUtil::Str2Int(mRecordTime)*60)
		{
			ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]�ֶ�¼������ֹͣ !\n",DeviceID));
			bManualRecord=false;
			mRecordFile.Close();
			//�޸��ֶ�¼���ļ����  jidushan 110406
			string temptime = TimeUtil::GetCurDateTime();
			DBMANAGER::instance()->AddTempFile(mRecordFileName, temptime, m_strExpiredTime);
			SendXML(TranslateUpXML::TranslateManualRecordReport(this,mManualRecordXML,mURL));
		}
		else 
		{
			Task->msg_queue()->flush();
			OSFunction::Sleep(0,10);
		}
		
	}
	//�ֶ�¼��ʱ֮ǰ,ʵʱ��Ƶֹͣ��,�ͽ��ֶ�¼��Ҳֹͣ
	if(bManualRecord&&time(0)-TimeUtil::StrToDateTime(mStartTime)<=StrUtil::Str2Int(mRecordTime)*60)
	{
		ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]�ֶ�¼������ֹͣ !\n",DeviceID));
		string temptime = TimeUtil::GetCurDateTime();
		DBMANAGER::instance()->AddTempFile(mRecordFileName, temptime, m_strExpiredTime);
		SendXML(TranslateUpXML::TranslateManualRecordReport(this,mManualRecordXML,mURL));
	}

	delete[] TsBuf;//�ͷ��ڴ�
	TSFETCHERMGR::instance()->DelRecordTask(DeviceID,Task);
	delete Task;

	bRun = false;

	if (TaskScheduler != NULL)
		TaskScheduler->ModifyExpiredTime(TimeUtil::DateTimeToStr(time(0)-1));

	SetFinised();		//����ֹͣ
	TSFETCHERMGR::instance()->DecreaseTaskNum(DeviceID);
	if(GetVersion() == "3.0")
	{
		PROPMANAGER::instance()->FreeSetTunerDev(StrUtil::Str2Int(m_tdevid),DVBC,Freq,strServiceID);
		vector<int> codlis;
		PROPMANAGER::instance()->SeparateStrVec(m_cdevid,codlis);
		for(int i=0;i<codlis.size();i++)
		{
			PROPMANAGER::instance()->FreeSetCoderDev(codlis[i]);
		}
	}
	//g_realstreamusedev = -1;
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]ʵʱ��Ƶ����ֹͣ !\n",DeviceID));
}

bool StreamRealtimeQueryTask::StartManualRecord(std::string ManualRecordXml)
{
	bool ret=false;
	
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]�ֶ�¼������ʼ !\n",DeviceID));

	mManualRecordXML=ManualRecordXml;

	XmlParser parser;
	parser.Set_xml(ManualRecordXml);
	pXMLNODE manualRecordNode=parser.GetNodeFromPath("Msg/ManualRecordSet/ManualRecord");
	parser.GetAttrNode(manualRecordNode,"Time",mRecordTime);
#ifdef ZONG_JU_ZHAO_BIAO
	parser.GetAttrNode(manualRecordNode,"Lifecycle",m_strExpiredTime);

	if(m_strExpiredTime.length()<1)
	{
		m_strExpiredTime=MANURECORD_EXPIREDAYS;
	}
#endif
	
	//�������ļ��л��¼���ļ���ʵ�ʱ���λ�ã�������ʱ�ļ���
	std::string realpath=PROPMANAGER::instance()->GetTempFileLoac();

	//�������
	std::string datetime=TimeUtil::DateTimeToString(time(0));
	datetime=datetime.substr(4, datetime.size()- 4);

	//ƴ���ļ�����·��+ͨ����+����id+����
	std::string fileName = StrUtil::Int2Str(DeviceID) + string("_")  + TaskID + string("_") + datetime;
	fileName+=".ts";
	
	std::string fullname = realpath + fileName;		//����ʵ��·�����ļ���
	mRecordFileName = fullname;
	//�����ļ�
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ����[%s]!\n",fullname.c_str()));
	ret=mRecordFile.CreateNewFile(fullname.c_str());

	//��ȡ����·��
	std::string sharepath = PROPMANAGER::instance()->GetTempSharePath();
	
	//��ȡ�ļ�����Э������	wz_110310
	string fileprotocol = "";
	ret = PROPMANAGER::instance()->GetFileProtocol(GetDVBType(), fileprotocol);
	if (!ret)
	{
		ACE_DEBUG ((LM_DEBUG,"(%T| %t) ��ȡ�ļ�����Э��ʧ��!\n"));
		return false;
	}
	mURL=fileprotocol+"://" +PROPMANAGER::instance()->GetHttpServerIP() +":"+PROPMANAGER::instance()->GetHttpServerPort() + sharepath + fileName;//¼���ļ���ŵ�url
	mStartTime=TimeUtil::GetCurDateTime();
	bManualRecord=true;
	return ret;
}