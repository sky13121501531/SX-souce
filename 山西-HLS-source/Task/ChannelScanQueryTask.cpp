
#include "ChannelScanQueryTask.h"
#include "TranslateDownXML.h"
#include "TranslateUpXML.h"
#include "Scheduler.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/PropManager.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include "../DeviceAccess/DeviceAccessMgr.h"
#include "../DBAccess/DBManager.h"
#include <vector>
#include <iostream>
#include "ace/Task.h"
#include "../Foundation/XmlParser.h"
#include "./TranslateXMLForDevice.h"
#include "../Foundation/OSFunction.h"
#include "../DeviceAccess/ChanScanThread.h"
#include "../Foundation/StrUtil.h"
using namespace std;

ChannelScanQueryTask::ChannelScanQueryTask() : DeviceIndependentTask()
{

}
//ScanTimeΪ�ձ�ʾ��ʱ�ɼ�,��ظ�Ƶ��ɨ��Ԥ��,�����ɼ�(����)
//ScanTime��Ϊ�ձ�ʾ����
ChannelScanQueryTask::ChannelScanQueryTask(std::string strXML) : DeviceIndependentTask(strXML)
{
	XmlParser parser(strXML.c_str());
	pXMLNODE queryNode = parser.GetNodeFromPath("Msg/ChannelScanQuery/ChannelScan");
	parser.GetAttrNode(queryNode,"ScanTime",m_strScanTime);   
	//***���ź�Դ��֧�ֻ�ȡ����Ƶ����Ϣ***
	m_strScanTime = "2013-08-17 15:30:00";
	//************************************
	if(this->GetVersion() == "3.0")
	{
		string strScanType;
		parser.GetAttrNode(queryNode,"ScanType",strScanType);           
		//
		parser.GetAttrNode(queryNode,"DeviceID",m_strDevID);           
		parser.GetAttrNode(queryNode,"Freq",m_strFreq);  
		DeviceID = StrUtil::Str2Int(m_strDevID);
		m_strFreq = StrUtil::Long2Str(StrUtil::Str2Long(m_strFreq)/1000);//Ϊ�˺������ļ����õ�λMһ��,��ChanScanThread��*1000

		if(strScanType != "3")
		{
			m_strFreq = "All";
		}
	}
}

ChannelScanQueryTask::~ChannelScanQueryTask()
{

}

void ChannelScanQueryTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]Ƶ��ɨ������ִ�� !\n",DeviceID));
	bRun = true;
	SetRunning();
	string strRtnXml; //XML�ļ�����
	//����Ƶ��ɨ��
	//******************************
	if(m_strScanTime.size() > 0)
	{
		string strFileName;  //Ƶ��ɨ�����ļ�·��
		OSFunction::GetFileName(DVBType,GetObjectName(),strFileName);

		XmlParser parser;
		parser.LoadFromFile(strFileName.c_str());
		parser.Get_xml(strRtnXml); //��ȡƵ��ɨ����
		if (strRtnXml.empty())
		{
			SetRetValue(RUN_FAILED);
			cout<<"�����ļ��쳣:"<<strFileName.c_str()<<endl;
		}
		else
		{
			SetRetValue(RUN_SUCCESS);
		}

		
		SendXML(TranslateUpXML::TranslateChannelScan(this,strRtnXml));//����xml,�ϱ���Ƶ������Ȩ��У��,���ݿ��ĺ�Ŀ¼��Ķ��Ǳ�ʶ������������
		//����Ƶ��ɨ����
		CHANNELINFOMGR::instance()->UpdateChannelInfo(GetDVBType(), strRtnXml);
	}
	else
	{
		//Sleep(300000);//5*60*1000 = 5min
		if(this->GetVersion() == "3.0")
		{
			CHANSCANTHREAD::instance()->SetChanScanNow(CreatePreChanScanRet(),m_strFreq,m_strDevID);
		}
		else
		{
			CHANSCANTHREAD::instance()->SetChanScanNow(CreatePreChanScanRet());
		}
		SetRetValue(RUN_SUCCESS);
		//SendXML(CreatePreChanScanRet());
	}
	bRun = false;
	if (TaskScheduler != NULL)
		TaskScheduler->ModifyExpiredTime(TimeUtil::DateTimeToStr(time(0)-1));

    SetFinised();
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]Ƶ��ɨ������ֹͣ !\n",DeviceID));
}
string ChannelScanQueryTask::GetTaskName()
{
	return std::string("Ƶ��ɨ������");
}
std::string ChannelScanQueryTask::GetObjectName()
{
	return std::string("ChannelScanQueryTask");
}
string ChannelScanQueryTask::CreatePreChanScanRet()
{
	string retXml;
	if(this->GetVersion() == "3.0")
	{
		string roundurl;
		PROPMANAGER::instance()->GetUpStreamRoundUrl(DVBC,roundurl);
		char * cheader= "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
		XmlParser headParser(cheader);
		pXMLNODE headRootNode = headParser.GetNodeFromPath( "Msg" );//���ڵ�msg
		headParser.SetAttrNode( "Version",this->GetVersion(),headRootNode );//�汾
		headParser.SetAttrNode( "MsgID",this->GetMsgID(),headRootNode );//��Ϣid����
		headParser.SetAttrNode( "Type",string("MonUp"),headRootNode );//��Ϣ����
		headParser.SetAttrNode( "DateTime",TimeUtil::GetCurDateTime(),headRootNode );//��ǰʱ��
		headParser.SetAttrNode( "SrcCode",this->GetDstCode(),headRootNode );//������ʶ����ͨ���ӿڻ��
		headParser.SetAttrNode( "DstCode",this->GetSrcCode(),headRootNode );//Ŀ�������ʶ
		headParser.SetAttrNode( "DstURL",roundurl,headRootNode);//Ŀ��URL
		headParser.SetAttrNode( "ReplyID",string("-1"),headRootNode );//�ظ�����Ϣid
		headParser.SaveToString( retXml );	
		//
		XmlParser parser;
		parser.Set_xml(retXml);
		pXMLNODE channelScanQueryNode=parser.CreateNodePtr(parser.GetRootNode(),"ScanNoticeReport");//Ϊ���ص�xml����ChannelScanReport�ڵ�
		std::string scantime = TimeUtil::GetCurDateTime();
		parser.SetAttrNode("Type",string("3"),channelScanQueryNode);
		parser.SetAttrNode("NoticeTime",scantime,channelScanQueryNode);
		parser.SaveToString(retXml);
		cout<<"ChannelScanQueryTask::CreatePreChanScanRet():"<<retXml<<endl;
	}
	else
	{
		char * cheader= "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><Return/></Msg>";
		XmlParser headParser(cheader);
		pXMLNODE headRootNode = headParser.GetNodeFromPath( "Msg" );//���ڵ�msg
		headParser.SetAttrNode( "Version",this->GetVersion(),headRootNode );//�汾
		headParser.SetAttrNode( "MsgID",this->GetMsgID(),headRootNode );//��Ϣid����
		headParser.SetAttrNode( "Type",string("MonUp"),headRootNode );//��Ϣ����
		headParser.SetAttrNode( "DateTime",TimeUtil::GetCurDateTime(),headRootNode );//��ǰʱ��
		headParser.SetAttrNode( "SrcCode",this->GetDstCode(),headRootNode );//������ʶ����ͨ���ӿڻ��
		headParser.SetAttrNode( "DstCode",this->GetSrcCode(),headRootNode );//Ŀ�������ʶ
		headParser.SetAttrNode( "DstURL",this->GetSrcURL(),headRootNode);//Ŀ��URL
		headParser.SetAttrNode( "ReplyID",this->GetMsgID(),headRootNode );//�ظ�����Ϣid
		pXMLNODE retNode = headParser.GetNodeFromPath("Msg/Return");
		headParser.SetAttrNode( "Type",string("ChannelScanNotice"),retNode );
		headParser.SetAttrNode( "Value",string("0"),retNode );//return�ڵ��value����
		headParser.SetAttrNode( "Desc",string("�ɹ�"),retNode );//return�ڵ��Desc����
		headParser.SaveToString( retXml );	
		//
		XmlParser parser;
		parser.Set_xml(retXml);
		pXMLNODE returnInfoNode = parser.CreateNodePtr(parser.GetRootNode(),"ReturnInfo");
		pXMLNODE channelScanQueryNode=parser.CreateNodePtr(returnInfoNode,"ChannelScanNotice");//Ϊ���ص�xml����ChannelScanReport�ڵ�
		std::string scantime = TimeUtil::GetCurDateTime();
		parser.SetAttrNode("NoticeTime",scantime,channelScanQueryNode);
		parser.SaveToString(retXml);
		cout<<"ChannelScanQueryTask::CreatePreChanScanRet():"<<retXml<<endl;
	}
	return retXml;
}