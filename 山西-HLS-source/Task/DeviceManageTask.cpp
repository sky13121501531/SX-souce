#include "DeviceManageTask.h"
#include "ace/Task.h"
#include "TranslateDownXML.h"
#include "TranslateUpXML.h"
#include "../DeviceAccess/TsFetcherMgr.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/StrUtil.h"
#include "../DBAccess/DBManager.h"

DeviceManageTask::DeviceManageTask(void)
{
}

DeviceManageTask::DeviceManageTask( std::string strXML ) : DeviceIndependentTask(strXML)
{
}
DeviceManageTask::~DeviceManageTask(void)
{
}

void DeviceManageTask::Run( void )
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]Ӳ����������ִ�� !\n",DeviceID));
	bRun = true;

	SetRunning();

	RetValue = RUN_SUCCESS;

	SendXML(CreateRetXml()); //���͵�ǰ��
	std::string strFileName;
	std::string STD,Freq,SymbolRate;
	XmlParser parser(strStandardXML.c_str());
	pXMLNODE Node = parser.GetNodeFromPath("Msg/DeviceManage");
	pXMLNODELIST NodeList =parser.GetNodeList(Node);
	int count=parser.GetChildCount(Node);

	for(int i=0;i<count;i++)
	{
		string DType,Index,device,Ip;
		pXMLNODE childNode=parser.GetNextNode(NodeList);
		parser.GetAttrNode( childNode,"type",DType );
		parser.GetAttrNode(childNode,"Index",Index);
		if(Index=="All")
		{
			int devnum = PROPMANAGER::instance()->GetCoderDevNum();
			//std::list<int> devicedlist;
			//PROPMANAGER::instance()->GetDVBDeviceList(OSFunction::GetEnumDVBType(DType),devicedlist);	
			//std::list<int>::iterator ptr=devicedlist.begin();

			//for(;ptr!=devicedlist.end();ptr++)
			//{
			//	TSFETCHERMGR::instance()->RebootCard(*ptr);
			//}
		}
		else
		{
			PROPMANAGER::instance()->GetDeviceIndex(device,Index,DType);
			TSFETCHERMGR::instance()->RebootCard(StrUtil::Str2Int(device));
		}
	}

	SetFinised();

	bRun = false;
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]Ӳ����������ֹͣ !\n",DeviceID));
}


std::string DeviceManageTask::CreateRetXml()
{
	char * cheader= "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><Return/></Msg>";

//	string  MsgID ;
//	DBMANAGER::instance()->GetMsgID(MsgID);
//	int msgid=StrUtil::Str2Int(MsgID);
//	DBMANAGER::instance()->UpdateMsgID(StrUtil::Int2Str(++msgid));

	string strDateTime = TimeUtil::GetCurDateTime();
	string  MsgID = OSFunction::GetXmlMsgID(strDateTime);

	string Desc="�ɹ�";
	std::string msgType = "CommonUp";//��Ϣ����
	std::string retXml="";
	XmlParser headParser(cheader);
	pXMLNODE headRootNode = headParser.GetNodeFromPath( "Msg" );//���ڵ�msg

	headParser.SetAttrNode( "Version",GetVersion(),headRootNode );//�汾
	headParser.SetAttrNode( "MsgID",MsgID,headRootNode );//��Ϣid����
	headParser.SetAttrNode( "Type",msgType,headRootNode );//��Ϣ����
//	headParser.SetAttrNode( "DateTime",TimeUtil::GetCurDateTime(),headRootNode );//��ǰʱ��
	headParser.SetAttrNode( "DateTime",strDateTime,headRootNode );//��ǰʱ��
	headParser.SetAttrNode( "SrcCode",GetDstCode(),headRootNode );//������ʶ����ͨ���ӿڻ��
	headParser.SetAttrNode( "DstCode",GetSrcCode(),headRootNode );//Ŀ�������ʶ
	headParser.SetAttrNode( "DstURL",GetSrcURL(),headRootNode);//Ŀ��URL
	headParser.SetAttrNode( "ReplyID",GetMsgID(),headRootNode );//�ظ�����Ϣid

	pXMLNODE retNode = headParser.GetNodeFromPath("Msg/Return");
	headParser.SetAttrNode( "Type",string("DeviceManage"),retNode );
	headParser.SetAttrNode( "Value",string("0"),retNode );//return�ڵ��value����
	headParser.SetAttrNode( "Desc",Desc,retNode);//return�ڵ��Desc����
	headParser.SaveToString( retXml );

	return retXml;
}

std::string DeviceManageTask::GetObjectName()
{
	return std::string("DeviceManageTask");
}

std::string DeviceManageTask::GetTaskName()
{
	return std::string("Ӳ����������");
}