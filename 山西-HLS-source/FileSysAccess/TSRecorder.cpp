///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����TSRecorder.cpp
// �����ߣ�lichunfeng
// ����ʱ�䣺2009-06-04
// ��������������¼���ļ���
///////////////////////////////////////////////////////////////////////////////////////////
#include "../BusinessProcess/RunPlanInfoMgr.h"
#include "TSRecorder.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/XmlParser.h"
#include "../Foundation/OSFunction.h"
#include "../DBAccess/DBManager.h"
#include "DiskOperation.h"
#include "../foundation/applog.h"
TSRecorder::TSRecorder(int deviceid,std::string taskxml,std::string taskid)
{
	mbIsUpdateDbEndtime = true;
	mUpdateDbEndtime = time(0);
	//
	mIsCreate=false;
	mCurTime = 0;
	TaskXml = taskxml;
	DeviceID = deviceid;//ͨ����
	TaskID=taskid;

	XmlParser parser( TaskXml.c_str() );
	pXMLNODE rootNode = parser.GetNodeFromPath( "Msg" );
	parser.GetAttrNode( rootNode,"DVBType",downType );
	parser.GetAttrNode( rootNode,"Version",m_Version );
	pXMLNODE recordNode=NULL;
	if(TaskXml.find("AutoRecord")!=string::npos)
	{
		recordNode=parser.GetNodeFromPath("Msg/AutoRecord/Record");
	}
	else if(TaskXml.find("TaskRecord")!=string::npos)
	{
		recordNode=parser.GetNodeFromPath("Msg/TaskRecord/Record");
	}
	else if(TaskXml.find("TSQuery")!=string::npos)
	{
		recordNode=parser.GetNodeFromPath("Msg/TSQuery/TS");
	}
	//�����Ƶ������Ϣ
	if(recordNode)
	{
		parser.GetAttrNode(recordNode,"TaskID",TaskID);
		parser.GetAttrNode(recordNode,"Freq",Freq);
		parser.GetAttrNode(recordNode,"Code",Code );
		parser.GetAttrNode(recordNode,"OrgNetID",OrgNetID);
		parser.GetAttrNode(recordNode,"TsID",TsID);
		parser.GetAttrNode(recordNode,"ServiceID",ServiceID);
		parser.GetAttrNode(recordNode,"VideoPID",VideoPID);
		parser.GetAttrNode(recordNode,"AudioPID",AudioPID);
		//
		if(m_Version == "3.0")
		{
			parser.GetAttrNode(recordNode,"CChassisID",m_strCChassisID);
			parser.GetAttrNode(recordNode,"CPosition",m_strCPosition);
			parser.GetAttrNode(recordNode,"CModuleID",m_strCModuleID);
			parser.GetAttrNode(recordNode,"CTranscode",m_strCTranscode);
		}
		//
#ifdef ZONG_JU_ZHAO_BIAO
		parser.GetAttrNode(recordNode,"Lifecycle",m_strExpireDays);	//2.5�ӿ�
#else
		parser.GetAttrNode(recordNode,"ExpireDays",m_strExpireDays);
#endif
		if (m_strExpireDays!="")
		{
			mRecordInfo.expiredays = m_strExpireDays;
		}

	}

	//���Ը�ֵ��mRecordInfo�ṹ��
	if (TaskID.length()==0)
	{
		TaskID="0";
	}
	mRecordInfo.dvbtype = OSFunction::GetEnumDVBType(downType);	//�������

	CHANNELINFOMGR::instance()->GetChanIdByFreqServId(mRecordInfo.dvbtype, Freq, ServiceID, mRecordInfo.channelID);
	//CHANNELINFOMGR::instance()->GetChannelID(mRecordInfo.dvbtype,OrgNetID,TsID,ServiceID,VideoPID,AudioPID,Code,mRecordInfo.channelID);//���Ƶ��id
	if(mRecordInfo.channelID.length()<1)
		mRecordInfo.channelID=Freq;
	
	mRecordInfo.taskid=TaskID;
}

TSRecorder::~TSRecorder(void)
{
	DBMANAGER::instance()->UpdateFileEndTime(mFileName,TimeUtil::GetCurDateTime());	
}

bool TSRecorder::CreateRecFile()
{
	bool ret=false;
	std::string path = "";

	while(1)
	{
		std::string storetype = PROPMANAGER::instance()->GetStoreType();	//��ȡ¼��洢��ʽ����ɢ ���� ˳��
		if (storetype == "0")
			DISKOPERATION::instance()->CheckDiskIsFullForRecord(path);	//˳��deviceid��Ĭ��ֵ����
		else if (storetype == "1")
			DISKOPERATION::instance()->CheckDiskIsFullForRecord(path, DeviceID);  //��ɢ
		if (path.length()==0 || path.length()<1)
		{
			ACE_DEBUG ((LM_DEBUG,"(%T| %t)��ȡ¼��洢·��ʧ��!\n"));
			
			std::string strMsg = "ͨ����";
			strMsg += StrUtil::Int2Str(DeviceID);
			strMsg += "��ȡ¼��洢·��ʧ��!";
			
			APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,strMsg,LOG_OUTPUT_FILE);
			OSFunction::Sleep(0,50);
			continue;
		}
		break;
	}	

	//����ļ�����ͨ����_����
	std::string datetime=TimeUtil::DateTimeToString(time(0));
	datetime=datetime.substr(4, datetime.size()- 4);
	mFileName=path + StrUtil::Int2Str(DeviceID) + string("_")  + TaskID + string("_") + datetime;
	
	mFileName+=".ts";
	
	//�����ļ�
	mStartTime=TimeUtil::GetCurDateTime();
	ret=mPFile.CreateNewFile(mFileName.c_str());
	if (!ret)
	{
		std::string strMsg = mFileName;
		strMsg += "�����ļ�ʧ��!";
		APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,strMsg,LOG_OUTPUT_FILE);
	}
	else
	{
		std::string strMsg = mFileName;
		strMsg += "�����ļ��ɹ�!";
		APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,strMsg,LOG_OUTPUT_FILE);
	}
	mCurTime = time(0);												//���¿�ʼʱ��
	ret = Save2DB();														//¼���ļ���¼�������ݿ�
	if (!ret)
	{
		std::string strMsg = mFileName;
		strMsg += "���ʧ��!";
		APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,strMsg,LOG_OUTPUT_FILE);
		strMsg = mFileName;
		strMsg += "�ļ�ɾ��!";
		DeleteFile(mFileName.c_str());//ע������ɾ���ļ���Ӳ�̵�Ӱ��
		APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,strMsg,LOG_OUTPUT_FILE);
		Sleep(10000);
	}
	else
	{
		std::string strMsg = mFileName;
		strMsg += "���ɹ�!";
		APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,strMsg,LOG_OUTPUT_FILE);
	}
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ����[%s]!\n",mFileName.c_str()));
	

	mIsCreate=ret;	
	return ret;
}
bool TSRecorder::SaveFile( unsigned char* buf,size_t num )
{
	UpAlarmInfo info;
	PROPMANAGER::instance()->GetUpAlarmInfo(mRecordInfo.dvbtype,info);

	if(info.runplanrecord=="1" && mRecordInfo.taskid=="0" && RUNPLANINFOMGR::instance()->InRunPlan(mRecordInfo.dvbtype,mRecordInfo.channelID)==false)
	{
		if(mIsCreate)
		{
			DBMANAGER::instance()->UpdateFileEndTime(mFileName,TimeUtil::GetCurDateTime());	
			CloseFile();//�ر��ļ�
		}
		return false;
	}
	bool ret=false;
	if (!mIsCreate)//�ļ��Ƿ񴴽�
	{
		CreateRecFile();
	}

	if (time(0)-mCurTime>StrUtil::Str2Int(PROPMANAGER::instance()->GetRecordPeriod())*60/*��λ���룻�������ļ��л��*/)
	{
		DBMANAGER::instance()->UpdateFileEndTime(mFileName,TimeUtil::GetCurDateTime());	
		CloseFile();//�ر��ļ�
		CreateRecFile();//�������ļ�
		mbIsUpdateDbEndtime = false;
	}
	//
	if(mbIsUpdateDbEndtime)
	{
		if(time(0)-mUpdateDbEndtime >= 10)
		{
			DBMANAGER::instance()->UpdateFileEndTime(mFileName,TimeUtil::GetCurDateTime());	
			mUpdateDbEndtime = time(0);
		}
	}
	//
	if (mIsCreate)
	{
		int numWrite=mPFile.WriteBuf((void*)buf,(int)num,true);//����д���ļ�
		if (numWrite>0)
			ret=true;
		//DBMANAGER::instance()->UpdateFileEndTime(mFileName,TimeUtil::GetCurDateTime());//ûдһ���ļ��ͱ���һ�ν���ʱ�� ����Ӱ���������
	}
	return ret;
}

void TSRecorder::CloseFile( void )
{
	mIsCreate=false;
	mPFile.Close();
}

bool TSRecorder::Save2DB()
{
	if(m_Version == "3.0")
	{
		mRecordInfo.DeviceID=m_strCChassisID+"_"+m_strCPosition+"_"+m_strCModuleID+"_"+m_strCTranscode;
	}
	else
	{
		mRecordInfo.DeviceID=StrUtil::Int2Str(DeviceID);
	}
	mRecordInfo.filename=mFileName;
	mRecordInfo.starttime=mStartTime;
	
	string endtime=TimeUtil::CalMinutes(mStartTime,StrUtil::Str2Int(PROPMANAGER::instance()->GetRecordPeriod()));
	mRecordInfo.endtime=endtime;
	
#ifdef ZONG_JU_ZHAO_BIAO
	//if ( !m_strExpireDays.empty() )
	//{
	//	mRecordInfo.expiredays = StrUtil::Long2Str(TimeUtil::DiffDay(m_strExpireDays, mRecordInfo.starttime));
	//}
#endif
	if (mRecordInfo.expiredays.length()==0)//����·���¼������û�й��������Ͷ�ȡ�����ļ��еĹ�������
	{
		mRecordInfo.expiredays=PROPMANAGER::instance()->GetRecordExpiredays(mRecordInfo.dvbtype);//�������ļ��л�ù�������
	}
	if (mFileName.length()==0 || mStartTime.length()==0 || PROPMANAGER::instance()->GetRecordExpiredays(mRecordInfo.dvbtype).length()==0)
	{
		return false;
	}
	else
	{
		if(m_Version == "3.0")
		{
			return DBMANAGER::instance()->AddRecord30V(mRecordInfo);
		}
		else
		{
			return DBMANAGER::instance()->AddRecord(mRecordInfo);
		}
	}
	
}

bool TSRecorder::SaveManualRecordFile( unsigned char* buf,size_t num )
{
	bool ret=false;
	if (!mIsCreate)//�ļ��Ƿ񴴽�
	{
		CreateRecFile();
	}
	if (mIsCreate)
	{
		int numWrite=mPFile.WriteBuf((void*)buf,(int)num,true);//����д���ļ�
		if (numWrite>0)
			ret=true;
	}
	return ret;
}