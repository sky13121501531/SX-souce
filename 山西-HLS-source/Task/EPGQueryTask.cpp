
#include "EPGQueryTask.h"
#include "TranslateUpXML.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/Config.h"
#include "../Foundation/TypeDef.h"
#include "../DeviceAccess/ChanScanThread.h"
#include "../Foundation/StrUtil.h"

EPGQuery::EPGQuery() : DeviceIndependentTask()
{
	
}

EPGQuery::EPGQuery(std::string strXML) : DeviceIndependentTask(strXML)
{
	XmlParser parser(strXML.c_str());
	pXMLNODE queryNode = parser.GetNodeFromPath("Msg/EPGQuery");
	parser.GetAttrNode(queryNode,"ScanTime",m_strScanTime);           
}

EPGQuery::~EPGQuery()
{

}

string EPGQuery::GetTaskName()
{
	return std::string("EPG��ѯ����");
}

void EPGQuery::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]EPG��ѯ����ִ�� !\n",DeviceID));

	bRun = true;
	SetRunning();
	std::string strFileName = "EPGQuery.xml";    //EPG�ļ���

	std::string strTablePath;
	PROPMANAGER::instance()->GetXmlTablePath(GetDVBType(), strTablePath);
	std::string strFullName = strTablePath + strFileName;
	cout<<"EPG Query FullName:"<<strFullName<<endl;
	XmlParser epgParser;
	epgParser.LoadFromFile( strFullName.c_str() );

	pXMLNODE rootNode = epgParser.GetRootNode();
	std::string strEpgXml;
	//�������л�ȡMsgId����Ϣ;ͬʱMsgId�����ҳ���·���MsgId��ͬ;
	epgParser.SetAttrNode( "Version",this->GetVersion(),rootNode );//�汾
	epgParser.SetAttrNode( "MsgID",this->GetMsgID(),rootNode );//��Ϣid����
	std::string strType;
	if (this->GetDVBType() == DVBC)
		strType = "DVBCUp";
	else if (this->GetDVBType() == CTTB)
		strType = "DTMBUp";
	else if (this->GetDVBType() == DVBS)
		strType = "DVBSUp";
	else
		strType = "UnKnown";
	epgParser.SetAttrNode( "Type",strType,rootNode );//��Ϣ����
	epgParser.SetAttrNode( "DateTime",TimeUtil::GetCurDateTime(),rootNode );//��ǰʱ��
	epgParser.SetAttrNode( "SrcCode",this->GetDstCode(),rootNode );//������ʶ����ͨ���ӿڻ��
	epgParser.SetAttrNode( "DstCode",this->GetSrcCode(),rootNode );//Ŀ�������ʶ
	epgParser.SetAttrNode( "DstURL",strSrcURL,rootNode);//Ŀ��URL
	epgParser.SetAttrNode( "ReplyID",this->GetMsgID(),rootNode );//�ظ�����Ϣid

	epgParser.SaveToString( strEpgXml );
	//����&��;
	while(true)
	{
		if(strEpgXml.find("&")!=-1||strEpgXml.find(";")!=-1)
		{
			int posdz = strEpgXml.find("&");
			if(posdz != -1)
			{
				strEpgXml.replace(posdz,1,"A");
			}
			int posfh = strEpgXml.find(";");
			if(posfh != -1)
			{
				strEpgXml.replace(posfh,1,"B");
			}
		}
		else
		{
			break;
		}
	}
	//
	XmlParser validParser( strEpgXml.c_str() );
	validParser.SaveAsFile("C:/vscttb/EpgLastReport.xml");
	string temfile = OSFunction::GetStrDVBType(this->GetDVBType())+"_"+"EpgLastReport"+TimeUtil::DateTimeToString(time(0))+".zip";
	string path = "c:\\vscttb\\temp\\";
	OSFunction::CompressFile(path+temfile,"C:/vscttb/EpgLastReport.xml");
	string RequireURL =  "http://" + PROPMANAGER::instance()->GetHttpServerIP() + ":" + \
			PROPMANAGER::instance()->GetHttpServerPort() + PROPMANAGER::instance()->GetTempSharePath() + temfile;
	SetRetValue(RUN_SUCCESS);
	SendXML(TranslateUpXML::TranslateEPGQuery(this,RequireURL));
	//
	SetFinised();
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]EPG��ѯ����ֹͣ !\n",DeviceID));
}

std::string EPGQuery::GetObjectName()
{
	return std::string("EPGScanQuery");
}

string EPGQuery::CreateEpgScanRet()
{
	string retXml;
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
	parser.SetAttrNode("Type",string("2"),channelScanQueryNode);
	parser.SetAttrNode("NoticeTime",scantime,channelScanQueryNode);
	parser.SaveToString(retXml);
	cout<<"EPGQuery::CreateEpgScanRet():"<<retXml<<endl;
	return retXml;
}