#include "TableQueryTask.h"
#include "ace/Task.h"
#include "TranslateDownXML.h"
#include "TranslateUpXML.h"
#include "../Foundation/OSFunction.h"
#include "../DeviceAccess/ChanScanThread.h"
#include "../Foundation/StrUtil.h"
TableQueryTask::TableQueryTask(void)
{
}

TableQueryTask::TableQueryTask( std::string strXML ) : DeviceIndependentTask(strXML)
{
}
TableQueryTask::~TableQueryTask(void)
{
}

void TableQueryTask::Run( void )
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]���ѯ����ִ�� !\n",DeviceID));
	bRun = true;

	SetRunning();
	
	std::string strFileName;
	std::string STD,Freq,SymbolRate;
	std::string strScanTime,Devid;
    XmlParser parser(strStandardXML.c_str());
	if(this->GetVersion() == "3.0")
	{
		pXMLNODE orgRootNode=parser.GetRootNode();
		pXMLNODELIST nodeList=parser.GetNodeList(orgRootNode);
		for(int i=0;i<nodeList->Size();++i)
		{
			pXMLNODE orgTableNode=parser.GetNextNode(nodeList);
			parser.GetAttrNode(orgTableNode,"ScanDate",strScanTime);
			pXMLNODELIST chilenodeList=parser.GetNodeList(orgTableNode);
			for(int j=0;j<chilenodeList->Size();j++)
			{
				pXMLNODE subTableNode=parser.GetNextNode(chilenodeList);
				std::string strNodeName = parser.GetNodeName(subTableNode);
				if(strNodeName == "DemodulateInfo")
				{
					string strChassisID,strPosition,strChanId;
					parser.GetAttrNode( subTableNode,"ChassisID",strChassisID );
					parser.GetAttrNode( subTableNode,"Position",strPosition );
					parser.GetAttrNode( subTableNode,"Demodulate",strChanId );
					Devid = StrUtil::Int2Str(PROPMANAGER::instance()->GetTunerDeviceID(DVBC,StrUtil::Str2Int(strChassisID),StrUtil::Str2Int(strPosition),StrUtil::Str2Int(strChanId)));
				}
				else if(strNodeName == "FreqInfo")
				{
					parser.GetAttrNode( subTableNode,"Freq",Freq );
				}
			}
		}
		//
		if(strScanTime.size() > 0)
		{
			//
			string Path;
			PROPMANAGER::instance()->GetXmlTablePath(DVBC,Path);
			string tablequeryfilename = Path + string("TableQuery.xml");
			//
			string strRtnXml;
			XmlParser parser;
			parser.LoadFromFile(tablequeryfilename.c_str());
			parser.Get_xml(strRtnXml); //��ȡƵ��ɨ����
			if (strRtnXml.empty())
			{
				SetRetValue(RUN_FAILED);
				cout<<"�����ļ��쳣:"<<strFileName.c_str()<<endl;
			}
			else
			{
				pXMLNODE headRootNode = parser.GetNodeFromPath( "Msg" );//���ڵ�msg
				parser.SetAttrNode( "Version",this->GetVersion(),headRootNode);//Ŀ��URL
				parser.SetAttrNode( "MsgID",this->GetMsgID(),headRootNode);//Ŀ��URL
				parser.SetAttrNode( "Type",string("MonUp"),headRootNode);//Ŀ��URL
				parser.SetAttrNode( "DateTime",TimeUtil::GetCurDateTime(),headRootNode);//Ŀ��URL
				parser.SetAttrNode( "SrcCode",this->GetDstCode(),headRootNode);//Ŀ��URL
				parser.SetAttrNode( "DstCode",this->GetSrcCode(),headRootNode);//Ŀ��URL
				parser.SetAttrNode( "ReplyID",this->GetMsgID(),headRootNode);//Ŀ��URL
				parser.SetAttrNode( "DstURL",this->GetSrcURL(),headRootNode);//Ŀ��URL
				parser.Get_xml(strRtnXml); //��ȡƵ��ɨ����
				SetRetValue(RUN_SUCCESS);
			}
			SendXML(strRtnXml);
		}
		else
		{
			if(this->GetVersion() == "3.0")
			{
				CHANSCANTHREAD::instance()->SetChanScanNow(strStandardXML,Freq,Devid);
			}
		}
	}
	else
	{
		pXMLNODE tableNode = parser.GetNodeFromPath("Msg/TableQuery/Table");
		parser.GetAttrNode( tableNode,"STD",STD );						//��ȡSTD����ֵ
		parser.GetAttrNode( tableNode,"Freq",Freq );					//��ȡFreq����ֵ
		parser.GetAttrNode( tableNode,"SymbolRate",SymbolRate );		//��ȡSymbolRate����ֵ
		//��ȡTABLE��·��
		OSFunction::GetFileName(DVBType,GetObjectName(),strFileName);
		RetValue = RUN_SUCCESS;

		SendXML(TranslateUpXML::TranslateTableQuery(this,strFileName)); //���͵�ǰ��
	}
	
	SetFinised();

	bRun = false;
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]���ѯ����ֹͣ !\n",DeviceID));
}
bool TableQueryTask::TableQuery(const std::string STD, const std::string Freq, const std::string SymbolRate, std::string &strFileName)
{
	strFileName = "Table.rar";
	return true;
}

std::string TableQueryTask::GetObjectName()
{
	return"TableQueryTask";
}

std::string TableQueryTask::GetTaskName()
{
	return "���ѯ����";
}

string TableQueryTask::CreatePreTableQueryRet()
{
	string retXml;
	if(this->GetVersion() == "3.0")
	{
		char * cheader= "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
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
		headParser.SaveToString( retXml );	
		//
		XmlParser parser;
		parser.Set_xml(retXml);
		pXMLNODE channelScanQueryNode=parser.CreateNodePtr(parser.GetRootNode(),"TableScanQueryNoticeReport");//Ϊ���ص�xml����ChannelScanReport�ڵ�
		std::string scantime = TimeUtil::GetCurDateTime();
		parser.SetAttrNode("Type",string("1"),channelScanQueryNode);
		parser.SetAttrNode("NoticeTime",scantime,channelScanQueryNode);
		parser.SaveToString(retXml);
	}
	return retXml;
}