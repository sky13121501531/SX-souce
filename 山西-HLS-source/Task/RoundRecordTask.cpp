///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����RoundRecordTask.h
// �����ߣ�gaoxd
// ����ʱ�䣺2011-04-16
// ��������������¼��������
///////////////////////////////////////////////////////////////////////////////////////////
#include "RoundRecordTask.h"
#include "TranslateDownXML.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/PropManager.h"
#include <vector>
#include <iostream>
#include "ace/Task.h"
#include "ace/OS.h"
#include "../Foundation/TypeDef.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../FileSysAccess/TSRecorder.h"
#include "Scheduler.h"
#include "../Foundation/XmlParser.h"
#include "../DeviceAccess/TsFetcherMgr.h"
#include "../DeviceAccess/DeviceAccessMgr.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include "../Foundation/AppLog.h"
#include "../Task/TranslateXMLForDevice.h"
#include "../DBAccess/DBManager.h"
using namespace std;

class Round_Record_Task : public ACE_Task<ACE_MT_SYNCH>
{
public:
	Round_Record_Task();
	~Round_Record_Task(){};

};

Round_Record_Task::Round_Record_Task()
{
	msg_queue()->high_water_mark(188*8192);
	msg_queue()->low_water_mark(188*8192);
}

RoundRecordTask::RoundRecordTask() : DeviceRelatedTask()
{

}

RoundRecordTask::RoundRecordTask(std::string strXML) : DeviceRelatedTask(strXML)
{
	XmlParser parser;
	parser.Set_xml(strStandardXML);

	DecivcXMLVec=TranslateXMLForDevice::TranslateRoundRecordXML(strXML);//���͸�Ӳ�����������
	ChannelNum=DecivcXMLVec.size();	//�ֲ���Ƶ����
	ChannelIndex=0;				//������
	pXMLNODE recordNode=parser.GetNodeFromPath("Msg/RoundRecord");
	string OrgNetID, TsID, ServiceID,VideoPID,AudioPID,Code;
	string dayofweek,starttime,endtime,startdatetime,enddatetime,roundtime;
	if(recordNode)
	{
		parser.GetAttrNode(recordNode,"TaskID",TaskID);
		parser.GetAttrNode(recordNode,"DeviceID",DeviceID);
		parser.GetAttrNode(recordNode,"StartDateTime",startdatetime);
		parser.GetAttrNode(recordNode,"EndDateTime",enddatetime);
		parser.GetAttrNode(recordNode,"RoundTime",roundtime);

		RoundTime=TimeUtil::StrToSecondTime(roundtime);//�ֲ�ʱ��
	}
	pXMLNODELIST nodelist=parser.GetNodeList(recordNode);

	for (int i=0;i<nodelist->Size();i++)
	{
		
		pXMLNODE node=parser.GetNextNode(nodelist);
		string Freq,OrgNetID,TsID,ServiceID,VideoPID,AudioPID;
		parser.GetAttrNode(node,"Freq",Freq);
		parser.GetAttrNode(node,"OrgNetID",OrgNetID);
		parser.GetAttrNode(node,"TsID",TsID);
		parser.GetAttrNode(node,"ServiceID",ServiceID);
		parser.GetAttrNode(node,"VideoPID",VideoPID);
		parser.GetAttrNode(node,"AudioPID",AudioPID);
		if (DVBType==ATV||RADIO==DVBType||AM==DVBType||CTV==DVBType)
		{
			ChannelID=Freq;
		}
		else
		{
			CHANNELINFOMGR::instance()->GetChannelID(DVBType,OrgNetID,TsID,ServiceID,VideoPID,AudioPID,"",ChannelID);
		}
		if (ChannelID .length()<1)
		{
			bRun = false;
			break;
		}
	}
	TaskScheduler=new Scheduler();//�����ִ��ʱ�����
	TaskScheduler->SetRunTime(startdatetime,enddatetime,enddatetime);
}
RoundRecordTask::~RoundRecordTask()
{
}
void RoundRecordTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]����¼������ִ�� !\n",DeviceID));
	bRun = true;
	SetRunning();
	ACE_Task<ACE_MT_SYNCH>* Task = new Round_Record_Task;


	int BufLen = 188*4096;
	int BufSize = 188*1024;

	if (DVBType == RADIO  || DVBType==AM)
	{
		BufSize	 /= 16;
		BufLen   /= 16;
	}

	unsigned char* TsBuf = new unsigned char[BufLen];//������
	memset(TsBuf,0,BufLen);//��ʼ��������

	size_t PacketLen = 0;
	TSFETCHERMGR::instance()->SetTsDeviceXml(DeviceID,DecivcXMLVec[0]);
	TSFETCHERMGR::instance()->SetRecordTask(DeviceID,Task);
	TSFETCHERMGR::instance()->IncreaseTaskNum(DeviceID);
	TSRecorder * precord = new TSRecorder(DeviceID,DecivcXMLVec[0],TaskID);
	time_t lastTime=time(0);
	while(bRun == true && IsRunning())
	{
		try
		{
			if(time(0)-lastTime>RoundTime)
			{
				lastTime=time(0);
				ChannelIndex++;
				if(ChannelIndex>=ChannelNum)
					ChannelIndex=0;
				string currentxml=DecivcXMLVec[ChannelIndex];
				
				TSFETCHERMGR::instance()->SetTsDeviceXml(DeviceID,currentxml);
				if(precord!=NULL)
				{
					delete precord;
					precord = NULL;
				}
				precord = new TSRecorder(DeviceID,currentxml,TaskID);
			}
			ACE_Message_Block *mb = NULL;

			ACE_Time_Value OutTime(ACE_OS::gettimeofday()+ACE_Time_Value(2));
			if (Task->getq(mb,&OutTime) != -1 && mb != NULL)//�����ݽ����߳�ȡ����
			{
				if (mb->length() > 0)
				{
					memcpy(TsBuf+PacketLen,(unsigned char*)mb->rd_ptr(),mb->length());//�������ڴ�
					PacketLen += mb->length();
				}

				if (PacketLen>=BufLen-BufSize && precord != NULL)
				{
					precord->SaveFile(TsBuf,PacketLen);//����д���ļ�
					memset(TsBuf,0,BufLen);
					PacketLen = 0;
				}
				mb->release();
			}
		}
		catch(...)
		{
			string error = "ѭ��¼�������쳣";
			APPLOG::instance()->WriteLog( OTHER,LOG_EVENT_ERROR,error,LOG_OUTPUT_BOTH );			
			OSFunction::Sleep(0,100);
		}
	}
	if(precord!=NULL)
	{
		delete precord;
		precord = NULL;
	}

	delete[] TsBuf;//�ͷ��ڴ�
	TSFETCHERMGR::instance()->DelRecordTask(DeviceID,Task);
	delete Task;
	if (TaskScheduler != NULL)
		TaskScheduler->SetNextRunTime();//����������´�ִ��ʱ��
	SetFinised();
	TSFETCHERMGR::instance()->DecreaseTaskNum(DeviceID);
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]����¼������ֹͣ !\n",DeviceID));
}

std::string RoundRecordTask::GetObjectName()
{
	return std::string("RoundRecordTask");
}
string RoundRecordTask::GetTaskName()
{
	return std::string("����¼��");
}

void RoundRecordTask::SetDeciveID( int deviceid )
{
	DeviceID=deviceid;
}

bool RoundRecordTask::AddTaskXml( void )
{
	XmlParser psr;
	psr.Set_xml(strStandardXML);
	pXMLNODE node=psr.GetNodeFromPath("Msg/RoundRecord");
	if (node)
	{
		psr.SetAttrNode("DeviceID",DeviceID,node); //���������deviceid
		psr.Get_xml(strStandardXML);
	}
	//return DBMANAGER::instance()->AddXmlTask(DVBType,strStandardXML);
	
	//������������ִ��5�Σ����ȫ��ʧ�ܣ��ͷ���false
	bool ret = false;
	int nAddCount = 5;
	while (nAddCount-- > 0)
	{
		ret = DBMANAGER::instance()->AddXmlTask(DVBType,strStandardXML);
		if (ret)
		{
			break;
		}
	}

	return ret;
}

bool RoundRecordTask::DelTaskXml( void )
{
	return DBMANAGER::instance()->DeleteTask(DVBType,strStandardXML);
}