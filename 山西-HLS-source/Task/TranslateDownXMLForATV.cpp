///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����TranslateDownXMLForATV.cpp
// �����ߣ�gaoxd
// ����ʱ�䣺2010-04-09
// �������������·�XMLת��Ϊԭ�����������XML
///////////////////////////////////////////////////////////////////////////////////////////
#include "TranslateDownXMLForATV.h"
#include "../Foundation/XmlParser.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include "../Foundation/TypeDef.h"
#include "../Foundation/AppLog.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/StrUtil.h"

//���ر�׼��ʵʱ��Ƶ����XML
bool TranslateDownXMLForATV::TranslateStreamRealtimeQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><StreamRealtimeQuery></StreamRealtimeQuery></Msg>";
	XmlParser parser( strOriginalXML.c_str());
	XmlParser tempParser( source );

	string downType,version,msgid,protocol, dateTime, priority,srccode,dstcode,srcurl;//ͷ��Ϣ
	pXMLNODE rootNode = parser.GetNodeFromPath( "Msg" );
	parser.GetAttrNode( rootNode,"Type",downType );
	parser.GetAttrNode( rootNode,"MsgID",msgid );
	parser.GetAttrNode( rootNode,"Version",version );
	parser.GetAttrNode( rootNode,"Protocol",protocol );
	parser.GetAttrNode( rootNode,"SrcCode",srccode );
	parser.GetAttrNode( rootNode,"DstCode",dstcode );
	parser.GetAttrNode( rootNode,"DateTime",dateTime );
	parser.GetAttrNode( rootNode,"Priority",priority );
	parser.GetAttrNode( rootNode,"SrcURL",srcurl );

	if(!strcmp(downType.c_str(),"TVMonDown"))
	{
		downType="ATV";
	}
	else
	{
		return false;
	}

	//����ͷ��Ϣ
	if(protocol.length()<1)
	{
		protocol="HTTP";
	}
	pXMLNODE tempRootNode = tempParser.GetNodeFromPath( "Msg" );
	tempParser.SetAttrNode( "Version",version,tempRootNode );
	tempParser.SetAttrNode( "MsgID",msgid,tempRootNode );
	tempParser.SetAttrNode( "DVBType",downType,tempRootNode );
	tempParser.SetAttrNode( "TaskType",string("StreamRealtimeQuery"),tempRootNode );
	tempParser.SetAttrNode( "Protocol",protocol,tempRootNode );
	tempParser.SetAttrNode( "DateTime",dateTime,tempRootNode );
	tempParser.SetAttrNode( "SrcCode",srccode,tempRootNode );
	tempParser.SetAttrNode( "DstCode",dstcode,tempRootNode );
	tempParser.SetAttrNode( "SrcURL",srcurl,tempRootNode );
	tempParser.SetAttrNode( "Priority",priority,tempRootNode );
	pXMLNODE childNode=parser.GetNodeFirstChild(rootNode);
	pXMLNODE realStreamNode = parser.GetNodeFirstChild( childNode );
	pXMLNODE tempQueryNode = tempParser.GetNodeFromPath("Msg/StreamRealtimeQuery");
	//��ʼ������ֵ
	string Index,EquCode,LastURL,Freq,Width,Height,Fps,Bps;
	try
	{	
		//��ȡʵʱ����Ƶ������
		parser.GetAttrNode( realStreamNode,"Index",Index );
		parser.GetAttrNode( realStreamNode,"EquCode",EquCode );
		parser.GetAttrNode( realStreamNode,"LastURL",LastURL );
		parser.GetAttrNode( realStreamNode,"Freq",Freq );
		parser.GetAttrNode( realStreamNode,"Width",Width );
		parser.GetAttrNode( realStreamNode,"Height",Height );
		parser.GetAttrNode( realStreamNode,"Fps",Fps );
		parser.GetAttrNode( realStreamNode,"Bps",Bps );
		//����ʵʱ����Ƶ����
		pXMLNODE chScanNode=tempParser.CreateNodePtr(tempQueryNode,"RealtimeQueryTask");
		tempParser.SetAttrNode( "Index",Index,chScanNode );
		tempParser.SetAttrNode( "EquCode",EquCode,chScanNode );
		tempParser.SetAttrNode( "LastURL",LastURL,chScanNode );
		tempParser.SetAttrNode( "Freq",Freq,chScanNode );
		tempParser.SetAttrNode( "Width",Width,chScanNode );
		tempParser.SetAttrNode( "Height",Height,chScanNode );
		tempParser.SetAttrNode( "Fps",Fps,chScanNode );
		tempParser.SetAttrNode( "Bps",Bps,chScanNode );

	}
	catch(...)
	{
		string error = "���ñ�׼XML�ĵ�Ԫ�ص����Թ���ʧ��";
		APPLOG::instance()->WriteLog( OTHER,LOG_EVENT_ERROR,error,LOG_OUTPUT_BOTH );
		return false;
	}	
	tempParser.SaveToString(strStandardXML);//���浽�ַ���
	return true;
}
//���ر�׼��¼����������XML
bool TranslateDownXMLForATV::TranslateRecordSet(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser( strOriginalXML.c_str());
	XmlParser tempParser( source );
	string downType,version,msgid,protocol, dateTime, priority,srccode,dstcode,srcurl;//ͷ��Ϣ
	//��ȡ��Ԫ��
	pXMLNODE rootNode= parser.GetNodeFromPath( "Msg" );
	parser.GetAttrNode( rootNode,"Type",downType );
	parser.GetAttrNode( rootNode,"MsgID",msgid );
	parser.GetAttrNode( rootNode,"Version",version );
	parser.GetAttrNode( rootNode,"Protocol",protocol );
	if (protocol.length()<1)
	{
		protocol="HTTP";
	}
	parser.GetAttrNode( rootNode,"SrcCode",srccode );
	parser.GetAttrNode( rootNode,"DstCode",dstcode );
	parser.GetAttrNode( rootNode,"DateTime",dateTime );
	parser.GetAttrNode( rootNode,"Priority",priority );
	parser.GetAttrNode( rootNode,"SrcURL",srcurl );

	if (!strcmp(downType.c_str(),"TVMonDown") )
	{
		downType = "ATV";
	}
	else 
	{
		return false;
	}
	pXMLNODE tempRootNode = tempParser.GetNodeFromPath( "Msg" );
	tempParser.SetAttrNode( "Version",version,tempRootNode );
	tempParser.SetAttrNode( "MsgID",msgid,tempRootNode );
	tempParser.SetAttrNode( "DVBType",downType,tempRootNode );
	tempParser.SetAttrNode( "TaskType",string("RecordTaskSet"),tempRootNode );
	tempParser.SetAttrNode( "Protocol",protocol,tempRootNode );
	tempParser.SetAttrNode( "DateTime",dateTime,tempRootNode );
	tempParser.SetAttrNode( "SrcCode",srccode,tempRootNode );
	tempParser.SetAttrNode( "DstCode",dstcode,tempRootNode );
	tempParser.SetAttrNode( "SrcURL",srcurl,tempRootNode );
	tempParser.SetAttrNode( "Priority",priority,tempRootNode );

	if(strOriginalXML.find("SetAutoRecordChannel")!=string::npos)		//�Զ�¼������
	{
		pXMLNODE autoRecordSetNode=tempParser.CreateNodePtr("Msg","AutoRecordSet");
		pXMLNODE autoRecordNode=parser.GetNodeFromPath("Msg/SetAutoRecordChannel");
		pXMLNODELIST autoRecordList=parser.GetNodeList(autoRecordNode);
		string action;
		for (int i=0;i<autoRecordList->Size();++i)
		{
			pXMLNODE node=parser.GetNextNode(autoRecordList);
			parser.GetAttrNode(node,"Action",action);
			pXMLNODELIST nodeList=parser.GetNodeList(node);
			for (int j=0;j<nodeList->Size();++j)
			{
				string DeviceID,OrgNetID,TsID,ServiceID,Width,Height,Fps,Bps,Code,TaskID="0",VideoPID,AudioPID,Freq,DataType,SubPriority;
				string DayofWeek,StartTime,EndTime,StartDateTime,EndDateTime;
				string Index,ExpireDays;
				pXMLNODE recordNode=tempParser.CreateNodePtr(autoRecordSetNode,"Record");
				pXMLNODE childNode=parser.GetNextNode(nodeList);

				parser.GetAttrNode(childNode,"DeviceID",DeviceID);
				parser.GetAttrNode(childNode,"Code",Code);
				parser.GetAttrNode(childNode,"Bps",Bps);
				parser.GetAttrNode(childNode,"Priority",SubPriority);
				parser.GetAttrNode(childNode,"Freq",Freq);
				parser.GetAttrNode(childNode,"ExpireDays",ExpireDays);
				
				PROPMANAGER::instance()->GetDeviceIndex(Index,DeviceID,"ATV");
				tempParser.SetAttrNode("TaskID",TaskID,recordNode);
				tempParser.SetAttrNode("Action",action,recordNode);
				tempParser.SetAttrNode("DeviceID",Index,recordNode);
				if(Code!="")
					CHANNELINFOMGR::instance()->GetFreqByChannelCode(ATV,Code,Freq);
				
				tempParser.SetAttrNode("Freq",Freq,recordNode);
				tempParser.SetAttrNode("Code",Code,recordNode);
				tempParser.SetAttrNode("OrgNetID",OrgNetID,recordNode);
				tempParser.SetAttrNode("TsID",TsID,recordNode);
				tempParser.SetAttrNode("ServiceID",ServiceID,recordNode);
				tempParser.SetAttrNode("VideoPID",VideoPID,recordNode);
				tempParser.SetAttrNode("AudioPID",AudioPID,recordNode);
				tempParser.SetAttrNode("DataType",DataType,recordNode);
				tempParser.SetAttrNode("Width",Width,recordNode);
				tempParser.SetAttrNode("Height",Height,recordNode);
				tempParser.SetAttrNode("Fps",Fps,recordNode);
				tempParser.SetAttrNode("Bps",Bps,recordNode);
				tempParser.SetAttrNode("DayofWeek",DayofWeek,recordNode);
				tempParser.SetAttrNode("StartTime",StartTime,recordNode);
				tempParser.SetAttrNode("EndTime",EndTime,recordNode);
				tempParser.SetAttrNode("StartDateTime",StartDateTime,recordNode);
				tempParser.SetAttrNode("EndDateTime",EndDateTime,recordNode);
				tempParser.SetAttrNode("ExpireDays",ExpireDays,recordNode);
			}

		}
	}
	else		//����¼������
	{
		pXMLNODE taskRecordSetNode=tempParser.CreateNodePtr("Msg","TaskRecordSet");
		pXMLNODE taskRecord=parser.GetNodeFromPath("Msg/TaskSet/Task");
		string EquCode,TaskID,Action, ChCode,Freq, DeviceID, Width, Height, Fps, Bps,Index;
		parser.GetAttrNode(taskRecord,"EquCode",EquCode);
		parser.GetAttrNode(taskRecord,"TaskID",TaskID);
		parser.GetAttrNode(taskRecord,"Action",Action);
		parser.GetAttrNode(taskRecord,"ChCode",ChCode);
		if (ChCode.length() == 0)
		{
			parser.GetAttrNode(taskRecord,"ChannelCode",ChCode);
		}
		parser.GetAttrNode(taskRecord,"Freq",Freq);
		parser.GetAttrNode(taskRecord,"DeviceID",DeviceID);
		parser.GetAttrNode(taskRecord,"Width",Width);
		parser.GetAttrNode(taskRecord,"Height",Height);
		parser.GetAttrNode(taskRecord,"Fps",Fps);
		parser.GetAttrNode(taskRecord,"Bps",Bps);
		PROPMANAGER::instance()->GetDeviceIndex(Index,DeviceID,"ATV");
		if ("Del"==Action||"del"==Action)		//¼������ɾ��
		{
			pXMLNODE recordNode=tempParser.CreateNodePtr(taskRecordSetNode,"Record");
			tempParser.SetAttrNode("TaskID",TaskID,recordNode);
			tempParser.SetAttrNode("Action",Action,recordNode);
			tempParser.SetAttrNode("DeviceID",DeviceID,recordNode);
			tempParser.SetAttrNode("Freq",Freq,recordNode);
			tempParser.SetAttrNode("Code",ChCode,recordNode);
			tempParser.SetAttrNode("OrgNetID",string(""),recordNode);
			tempParser.SetAttrNode("TsID",string(""),recordNode);
			tempParser.SetAttrNode("ServiceID",string(""),recordNode);
			tempParser.SetAttrNode("VideoPID",string(""),recordNode);
			tempParser.SetAttrNode("AudioPID",string(""),recordNode);
			tempParser.SetAttrNode("DataType",string(""),recordNode);
			tempParser.SetAttrNode("Width",Width,recordNode);
			tempParser.SetAttrNode("Height",Height,recordNode);
			tempParser.SetAttrNode("Fps",Fps,recordNode);
			tempParser.SetAttrNode("Bps",Bps,recordNode);

			tempParser.SetAttrNode("DayofWeek",string(""),recordNode);
			tempParser.SetAttrNode("StartTime",string(""),recordNode);
			tempParser.SetAttrNode("EndTime",string(""),recordNode);
			tempParser.SetAttrNode("StartDateTime",string(""),recordNode);
			tempParser.SetAttrNode("EndDateTime",string(""),recordNode);
		}
		else		//¼����������
		{
			pXMLNODELIST nodelist=parser.GetNodeList(taskRecord);
			for (int i=0;i!=nodelist->Size();++i)
			{
				pXMLNODE childnode=parser.GetNextNode(nodelist);
				pXMLNODE recordNode=tempParser.CreateNodePtr(taskRecordSetNode,"Record");
				string   DayOfWeek, StartTime, EndTime, StartDateTime,EndDateTime, ExpireDays;
				parser.GetAttrNode(childnode,"DayOfWeek",DayOfWeek);
				parser.GetAttrNode(childnode,"StartTime",StartTime);
				parser.GetAttrNode(childnode,"EndTime",EndTime);
				parser.GetAttrNode(childnode,"StartDateTime",StartDateTime);
				parser.GetAttrNode(childnode,"EndDateTime",EndDateTime);
				parser.GetAttrNode(childnode,"ExpireDays",ExpireDays);

				tempParser.SetAttrNode("TaskID",TaskID,recordNode);
				tempParser.SetAttrNode("Action",Action,recordNode);
				tempParser.SetAttrNode("DeviceID",Index,recordNode);
				if (ChCode!="")
				{
					CHANNELINFOMGR::instance()->GetFreqByChannelCode(ATV,ChCode,Freq);
				}
				tempParser.SetAttrNode("Freq",Freq,recordNode);
				tempParser.SetAttrNode("Code",ChCode,recordNode);
				tempParser.SetAttrNode("OrgNetID",string(""),recordNode);
				tempParser.SetAttrNode("TsID",string(""),recordNode);
				tempParser.SetAttrNode("ServiceID",string(""),recordNode);
				tempParser.SetAttrNode("VideoPID",string(""),recordNode);
				tempParser.SetAttrNode("AudioPID",string(""),recordNode);
				tempParser.SetAttrNode("DataType",string(""),recordNode);
				tempParser.SetAttrNode("Width",Width,recordNode);
				tempParser.SetAttrNode("Height",Height,recordNode);
				tempParser.SetAttrNode("Fps",Fps,recordNode);
				tempParser.SetAttrNode("Bps",Bps,recordNode);

				tempParser.SetAttrNode("DayofWeek",DayOfWeek,recordNode);
				tempParser.SetAttrNode("StartTime",StartTime,recordNode);
				tempParser.SetAttrNode("EndTime",EndTime,recordNode);
				tempParser.SetAttrNode("StartDateTime",StartDateTime,recordNode);
				tempParser.SetAttrNode("EndDateTime",EndDateTime,recordNode);
				tempParser.SetAttrNode("ExpireDays",ExpireDays,recordNode);
			}
		  }
		}

	tempParser.SaveToString(strStandardXML);
	return true;
}
//���ر�׼��¼��ִ������XML
bool TranslateDownXMLForATV::TranslateRecordTask(const std::string& strOriginalXML,std::vector<std::string>& strStandardXMLVec)
{
	//¼���׼��XML�ṹ
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser( strOriginalXML.c_str());
	XmlParser tempParser( source );
	string downType,version,msgid,protocol, dateTime, priority,srccode,dstcode,srcurl;//ͷ��Ϣ
	//��ȡ��Ԫ��
	pXMLNODE rootNode= parser.GetNodeFromPath( "Msg" );
	parser.GetAttrNode( rootNode,"DVBType",downType );
	parser.GetAttrNode( rootNode,"MsgID",msgid );
	parser.GetAttrNode( rootNode,"Version",version );
	parser.GetAttrNode( rootNode,"Protocol",protocol );
	parser.GetAttrNode( rootNode,"SrcCode",srccode );
	parser.GetAttrNode( rootNode,"DstCode",dstcode );
	parser.GetAttrNode( rootNode,"DateTime",dateTime );
	parser.GetAttrNode( rootNode,"Priority",priority );
	parser.GetAttrNode( rootNode,"SrcURL",srcurl );

	pXMLNODE tempRootNode = tempParser.GetNodeFromPath( "Msg" );
	tempParser.SetAttrNode( "Version",version,tempRootNode );
	tempParser.SetAttrNode( "MsgID",msgid,tempRootNode );
	tempParser.SetAttrNode( "DVBType",downType,tempRootNode );
	tempParser.SetAttrNode( "TaskType",string("RecordTask"),tempRootNode );
	tempParser.SetAttrNode( "Protocol",protocol,tempRootNode );
	tempParser.SetAttrNode( "DateTime",dateTime,tempRootNode );
	tempParser.SetAttrNode( "SrcCode",srccode,tempRootNode );
	tempParser.SetAttrNode( "DstCode",dstcode,tempRootNode );
	tempParser.SetAttrNode( "SrcURL",srcurl,tempRootNode );
	tempParser.SetAttrNode( "Priority",priority,tempRootNode );

	pXMLNODE MyRecordNode;
	pXMLNODE MyRecordSetNode;
	if(strOriginalXML.find("AutoRecordSet")!=string::npos)
	{
		MyRecordNode=tempParser.CreateNodePtr(tempRootNode,"AutoRecord");
		MyRecordSetNode=parser.GetNodeFromPath("Msg/AutoRecordSet");
	}
	else if(strOriginalXML.find("TaskRecordSet")!=string::npos)
	{
	   MyRecordNode=tempParser.CreateNodePtr(tempRootNode,"TaskRecord");
	   MyRecordSetNode=parser.GetNodeFromPath("Msg/TaskRecordSet");
	}
	else 
	{
		return false;
	}
	pXMLNODELIST recordSetList=parser.GetNodeList(MyRecordSetNode);
	for(int i=0;i<recordSetList->Size();++i)
	{
		string xml;
		string Action,DeviceID,OrgNetID,TsID,ServiceID,Width,Height,Fps,Bps,Code,TaskID,VideoPID,AudioPID,Freq,DataType;
		string DayofWeek,StartTime,EndTime,StartDateTime,EndDateTime,ExpireDays;
		pXMLNODE recordNode=tempParser.CreateNodePtr(MyRecordNode,"Record");
		pXMLNODE childNode=parser.GetNextNode(recordSetList);
		parser.GetAttrNode(childNode,"TaskID",TaskID);
		parser.GetAttrNode(childNode,"Action",Action);
		parser.GetAttrNode(childNode,"DeviceID",DeviceID);
		parser.GetAttrNode(childNode,"Freq",Freq);
		parser.GetAttrNode(childNode,"Code",Code);
		parser.GetAttrNode(childNode,"OrgNetID",OrgNetID);
		parser.GetAttrNode(childNode,"TsID",TsID);
		parser.GetAttrNode(childNode,"ServiceID",ServiceID);
		parser.GetAttrNode(childNode,"VideoPID",VideoPID);
		parser.GetAttrNode(childNode,"AudioPID",AudioPID);
		parser.GetAttrNode(childNode,"DataType",DataType);
		parser.GetAttrNode(childNode,"Width",Width);
		parser.GetAttrNode(childNode,"Height",Height);
		parser.GetAttrNode(childNode,"Fps",Fps);
		parser.GetAttrNode(childNode,"Bps",Bps);
		parser.GetAttrNode(childNode,"DayofWeek",DayofWeek);
		parser.GetAttrNode(childNode,"StartTime",StartTime);
		parser.GetAttrNode(childNode,"EndTime",EndTime);
		parser.GetAttrNode(childNode,"StartDateTime",StartDateTime);
		parser.GetAttrNode(childNode,"EndDateTime",EndDateTime);
		parser.GetAttrNode(childNode,"Priority",priority);
		parser.GetAttrNode(childNode,"ExpireDays",ExpireDays);

		tempParser.SetAttrNode( "Priority",priority,tempRootNode );
		tempParser.SetAttrNode("TaskID",TaskID,recordNode);
		tempParser.SetAttrNode("Action",Action,recordNode);
		tempParser.SetAttrNode("DeviceID",DeviceID,recordNode);
		tempParser.SetAttrNode("Freq",Freq,recordNode);
		tempParser.SetAttrNode("Code",Code,recordNode);
		tempParser.SetAttrNode("OrgNetID",OrgNetID,recordNode);
		tempParser.SetAttrNode("TsID",TsID,recordNode);
		tempParser.SetAttrNode("ServiceID",ServiceID,recordNode);
		tempParser.SetAttrNode("VideoPID",VideoPID,recordNode);
		tempParser.SetAttrNode("AudioPID",AudioPID,recordNode);
		tempParser.SetAttrNode("DataType",DataType,recordNode);
		tempParser.SetAttrNode("Width",Width,recordNode);
		tempParser.SetAttrNode("Height",Height,recordNode);
		tempParser.SetAttrNode("Fps",Fps,recordNode);
		tempParser.SetAttrNode("Bps",Bps,recordNode);
		tempParser.SetAttrNode("DayofWeek",DayofWeek,recordNode);
		tempParser.SetAttrNode("StartTime",StartTime,recordNode);
		tempParser.SetAttrNode("EndTime",EndTime,recordNode);
		tempParser.SetAttrNode("StartDateTime",StartDateTime,recordNode);
		tempParser.SetAttrNode("EndDateTime",EndDateTime,recordNode);
		tempParser.SetAttrNode("ExpireDays",ExpireDays,recordNode);

		tempParser.SaveToString(xml);
		tempParser.DeleteNode(MyRecordNode,recordNode);
		strStandardXMLVec.push_back(xml);
	}
	return true;
}
//���ر�׼��¼���ѯ������XML
bool TranslateDownXMLForATV::TranslateRecordQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	//��ʼ������ֵ
	std::string strVersion,strMsgID,strType,strProtocol,strSetDateTime,strSrcCode,strDstCode,strSrcURL,strPriority;
	std::string Freq,TaskID, OrgNetID, tsID, serviceID,videoID,audioID,code, StartDateTime, EndDateTime, DeviceID;

	//��Ƶ��ѯ��׼��XML�ṹ
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser( strOriginalXML.c_str());
	XmlParser tempParser( source );

	//��ȡԭʼXML��Ԫ��
	pXMLNODE rootNode = parser.GetNodeFromPath("Msg");
	parser.GetAttrNode(rootNode,"Version",strVersion);         //��ȡMsg�ڵ��Version����ֵ
	parser.GetAttrNode(rootNode,"MsgID",strMsgID);		       //��ȡMsg�ڵ��MsgID����ֵ
	parser.GetAttrNode(rootNode,"Type",strType);		       //��ȡMsg�ڵ��Type����ֵ
	parser.GetAttrNode(rootNode,"Protocol",strProtocol);       //��ȡMsg�ڵ��protocol����ֵ
	parser.GetAttrNode(rootNode,"DateTime",strSetDateTime);    //��ȡMsg�ڵ��DateTime����ֵ��
	parser.GetAttrNode(rootNode,"SrcCode",strSrcCode);         //��ȡMsg�ڵ��StrCode����ֵ
	parser.GetAttrNode(rootNode,"DstCode",strDstCode);         //��ȡMsg�ڵ��DstCode����ֵ
	parser.GetAttrNode(rootNode,"SrcURL",strSrcURL);		   //��ȡMsg�ڵ��SrcUrl����ֵ	   
	parser.GetAttrNode(rootNode,"Priority",strPriority);       //��ȡMsg�ڵ��Priority����ֵ

	pXMLNODE childNode = parser.GetNodeFirstChild( rootNode );		   
	std::string nodeName = parser.GetNodeName( childNode );    //��ȡMsg���ӽڵ�����
	
	std::string  taskType;
	if (nodeName=="StreamSimpleQuery")//��ͬ��¼���ѯ��������
	{
		taskType="AutoRecordQuery";
	}
	else if (nodeName=="TaskRecordQuery")
	{
		taskType="TaskRecordQuery";
	}
	else if(nodeName=="TaskRecordFileQuery")
	{
		taskType="TaskRecordFileQuery";
	}
	pXMLNODE recordNode = parser.GetNodeFirstChild( childNode );

	//���ø�����
	pXMLNODE tempRootNode = tempParser.GetNodeFromPath( "Msg" );
	tempParser.SetAttrNode("Version",strVersion,tempRootNode);               //����Msg�ڵ��Version����ֵ
	tempParser.SetAttrNode("MsgID",strMsgID,tempRootNode);                   //����Msg�ڵ��MsgID����ֵ
	if (strType =="TVMonDown")
	{
		tempParser.SetAttrNode("DVBType",string("ATV"),tempRootNode);           //����Msg�ڵ��DVBType����ֵ
	}
	else
	{
		return false;
	}
	tempParser.SetAttrNode("TaskType",string("RecordQuery"),tempRootNode);   //����Msg�ڵ��TaskType����ֵ
	if (strProtocol.length()<1)
	{
		strProtocol="HTTP";
	}
	tempParser.SetAttrNode("Protocol",strProtocol,tempRootNode);             //����Msg�ڵ��Protocol����ֵ
	tempParser.SetAttrNode("DateTime",strSetDateTime,tempRootNode);          //����Msg�ڵ��DateTime����ֵ
	tempParser.SetAttrNode("SrcCode",strSrcCode,tempRootNode);               //����Msg�ڵ��StrCode����ֵ
	tempParser.SetAttrNode("DstCode",strDstCode,tempRootNode);               //����Msg�ڵ��DstCode����ֵ
	tempParser.SetAttrNode("SrcURL",strSrcURL,tempRootNode);                 //����Msg�ڵ��SrcUrl����ֵ
	tempParser.SetAttrNode("Priority",strPriority,tempRootNode);             //����Msg�ڵ��Priority����ֵ
	
	 //������ѯ���ͽڵ�
	
	pXMLNODE query= tempParser.CreateNodePtr(tempRootNode,(char*)taskType.c_str());
		        
	pXMLNODE recordTaskNode = tempParser.CreateNodePtr(query,"RecordQueryTask"); //����RecordQueryTask�ڵ�

	//��ȡ¼�����������
	if ( (taskType.compare("AutoRecordFileQuery") == 0)||(taskType.compare("AutoRecordQuery") == 0) )
	{
		parser.GetAttrNode( recordNode,"OrgNetID",OrgNetID );
		parser.GetAttrNode( recordNode,"TsID",tsID );
		parser.GetAttrNode( recordNode,"ServiceID",serviceID );
		if ( nodeName.compare("AutoRecordQuery") == 0)
		{
			parser.GetAttrNode( recordNode,"DeviceID",DeviceID );

		}
		parser.GetAttrNode(recordNode,"Freq",Freq);
		parser.GetAttrNode(recordNode,"ChannelCode",code);
		parser.GetAttrNode( recordNode,"StartDateTime",StartDateTime );
		parser.GetAttrNode( recordNode,"EndDateTime",EndDateTime );
	}
	else
	{
		parser.GetAttrNode( recordNode,"TaskID",TaskID );
		parser.GetAttrNode(recordNode,"ChCode",code);
		parser.GetAttrNode(recordNode,"Freq",Freq);
		parser.GetAttrNode( recordNode,"StartTime",StartDateTime );
		parser.GetAttrNode( recordNode,"EndTime",EndDateTime );
	}


	//����¼����������
	try
	{
		tempParser.SetAttrNode( "TaskID",TaskID,recordTaskNode );
		//Ӧ��ͨ��ChCode�����Ӧ��Freq
		if(code!="")
			CHANNELINFOMGR::instance()->GetFreqByChannelCode(ATV,code,Freq);
		tempParser.SetAttrNode( "Freq",Freq,recordTaskNode);
		tempParser.SetAttrNode( "OrgNetID",OrgNetID,recordTaskNode );
		tempParser.SetAttrNode( "TsID",tsID,recordTaskNode );
		tempParser.SetAttrNode( "ServiceID",serviceID,recordTaskNode );
		tempParser.SetAttrNode( "VideoPID",videoID,recordTaskNode );
		tempParser.SetAttrNode( "AudioPID",audioID,recordTaskNode );
		tempParser.SetAttrNode( "Code",code,recordTaskNode );
		tempParser.SetAttrNode( "DeviceID",DeviceID,recordTaskNode );
		tempParser.SetAttrNode( "StartDateTime",StartDateTime,recordTaskNode );
		tempParser.SetAttrNode( "EndDateTime",EndDateTime,recordTaskNode );
	}
	catch(...)
	{
		string error ="���ñ�׼XML�ĵ�Ԫ������ʧ��";
		APPLOG::instance()->WriteLog( OTHER,LOG_EVENT_ERROR,error,LOG_OUTPUT_BOTH );
		return false;
	}
	//���浽�ַ���
	tempParser.SaveToString( strStandardXML );
	return true;
}
//���ر�׼��Ƶ��ɨ��XML
bool TranslateDownXMLForATV::TranslateSpectrumScanQuery( const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser( strOriginalXML.c_str());
	XmlParser tempParser( source );

	string downType,version,msgid,protocol, dateTime, priority,srccode,dstcode,strSrcURL;//ͷ��Ϣ
	pXMLNODE rootNode = parser.GetNodeFromPath( "Msg" );
	parser.GetAttrNode( rootNode,"Type",downType );
	parser.GetAttrNode( rootNode,"MsgID",msgid );
	parser.GetAttrNode( rootNode,"Version",version );
	parser.GetAttrNode( rootNode,"Protocol",protocol );
	parser.GetAttrNode( rootNode,"SrcCode",srccode );
	parser.GetAttrNode( rootNode,"DstCode",dstcode );
	parser.GetAttrNode( rootNode,"DateTime",dateTime );
	parser.GetAttrNode( rootNode,"Priority",priority );
	parser.GetAttrNode(rootNode,"SrcURL",strSrcURL);

	if(!strcmp(downType.c_str(),"TVMonDown"))
	{
		downType="ATV";
	}
	else
	{
		return false;
	}

	//����ͷ��Ϣ
	if(protocol.length()<1)
	{
		protocol="HTTP";
	}
	pXMLNODE tempRootNode = tempParser.GetNodeFromPath( "Msg" );
	tempParser.SetAttrNode( "Version",version,tempRootNode );
	tempParser.SetAttrNode( "MsgID",msgid,tempRootNode );
	tempParser.SetAttrNode( "DVBType",downType,tempRootNode );
	tempParser.SetAttrNode( "TaskType",string("SpectrumScanQuery"),tempRootNode );
	tempParser.SetAttrNode( "Protocol",protocol,tempRootNode );
	tempParser.SetAttrNode( "DateTime",dateTime,tempRootNode );
	tempParser.SetAttrNode( "SrcCode",srccode,tempRootNode );
	tempParser.SetAttrNode( "DstCode",dstcode,tempRootNode );
	tempParser.SetAttrNode( "Priority",priority,tempRootNode );
	tempParser.SetAttrNode( "SrcURL",strSrcURL,tempRootNode );
	
	pXMLNODE realStreamNode = parser.GetNodeFirstChild( rootNode );
	//��ʼ������ֵ
	string EquCode,StartFreq,EndFreq,StepFreq,Steps,Action,TaskType,ReferDbu,VBW,RBW;
	try
	{	
		//��ȡƵ��ɨ�������
		parser.GetAttrNode( realStreamNode,"EquCode",EquCode );
		parser.GetAttrNode( realStreamNode,"StartFreq",StartFreq);
		parser.GetAttrNode( realStreamNode,"EndFreq",EndFreq );
		parser.GetAttrNode( realStreamNode,"TaskType",TaskType );
		parser.GetAttrNode( realStreamNode,"StepFreq",StepFreq );
		parser.GetAttrNode( realStreamNode,"Steps",Steps);
		parser.GetAttrNode( realStreamNode,"Action",Action );
		parser.GetAttrNode( realStreamNode,"ReferDbu",ReferDbu );
		parser.GetAttrNode( realStreamNode,"VBW",VBW);
		parser.GetAttrNode( realStreamNode,"RBW",RBW );

		//����Ƶ��ɨ������
		pXMLNODE chScanNode=tempParser.CreateNodePtr(tempRootNode,"SpectrumScanQuery");
		tempParser.SetAttrNode( "EquCode",EquCode,chScanNode );
		tempParser.SetAttrNode( "StartFreq",StartFreq,chScanNode );
		tempParser.SetAttrNode( "EndFreq",EndFreq,chScanNode );
		tempParser.SetAttrNode( "TaskType",TaskType,chScanNode );
		tempParser.SetAttrNode( "StepFreq",StepFreq,chScanNode );
		tempParser.SetAttrNode( "Steps",Steps,chScanNode );
		tempParser.SetAttrNode("Action",Action,chScanNode);
		tempParser.SetAttrNode( "ReferDbu",ReferDbu,chScanNode );
		tempParser.SetAttrNode( "VBW",VBW,chScanNode );
		tempParser.SetAttrNode("RBW",RBW,chScanNode);

	}
	catch(...)
	{
		string error = "���ñ�׼XML�ĵ�Ԫ�ص����Թ���ʧ��";
		APPLOG::instance()->WriteLog( OTHER,LOG_EVENT_ERROR,error,LOG_OUTPUT_BOTH );
		return false;
	}	
	tempParser.SaveToString(strStandardXML);//���浽�ַ���
	return true;
}
//���ر�׼��ʵʱָ���ѯXML
bool TranslateDownXMLForATV::TranslateQualityRealtimeQuery( const std::string& strOriginalXML,std::string& strStandardXML)
{
	string downType;//����������

	//��Ƶ��ѯ��׼��XML�ṹ
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><QualityRealtimeQuery><QualityQueryTask/><QualityParam></QualityParam></QualityRealtimeQuery></Msg>";

	XmlParser parser( strOriginalXML.c_str());
	XmlParser tempParser( source );

	//��ȡ��Ԫ��
	pXMLNODE rootNode ;
	pXMLNODE childNode ;
	string nodeName;
	rootNode=parser.GetRootNode();
	childNode=parser.GetNodeFirstChild(rootNode);

	//��ʼ������ֵ
	string dateTime, priority;
	string taskID,deviceID, freq,STD,symbolRate,action,version,msgid,protocol,srccode,dstcode,srcurl;
	string dayOfWeek,startTime,endTime,startDateTime,endDateTime,checkInterval;
	string Type,Desc,Value;
	int count = 0;
	string tasktype="QualityRealtimeQuery";
	//	string periodicity="Cycle";
	string periodicity="Cycle";
	//���ø�����
	pXMLNODE tempRootNode = tempParser.GetNodeFromPath( "Msg" );


	pXMLNODE tempChildNode = tempParser.GetNodeFirstChild( tempRootNode );
	parser.GetAttrNode(rootNode,"Version",version);
	parser.GetAttrNode(rootNode,"MsgID",msgid);
	parser.GetAttrNode(rootNode,"Type",downType);
	parser.GetAttrNode(rootNode,"Protocol",protocol);
	parser.GetAttrNode(rootNode,"DateTime",dateTime );
	parser.GetAttrNode(rootNode,"Priority",priority );
	parser.GetAttrNode(rootNode,"Protocol",protocol);
	parser.GetAttrNode(rootNode,"SrcCode",srccode);
	parser.GetAttrNode(rootNode,"DstCode",dstcode);
	parser.GetAttrNode(rootNode,"SrcURL",srcurl);

	if(downType=="TVMonDown")
	{
		downType="ATV";
	}
	else
	{
		return false;
	}
	if(protocol.length()<1)
		protocol="HTTP";
	tempParser.SetAttrNode("Version",version,tempRootNode);
	tempParser.SetAttrNode("MsgID",msgid,tempRootNode);
	tempParser.SetAttrNode("DVBType",downType,tempRootNode );
	tempParser.SetAttrNode("TaskType",tasktype,tempRootNode );
	tempParser.SetAttrNode("Protocol",protocol,tempRootNode);
	tempParser.SetAttrNode("DateTime",dateTime,tempRootNode );
	tempParser.SetAttrNode("SrcCode",srccode,tempRootNode);
	tempParser.SetAttrNode("DstCode",dstcode,tempRootNode);
	tempParser.SetAttrNode("Priority",priority,tempRootNode );
	tempParser.SetAttrNode("SrcURL",srcurl,tempRootNode);

	//��ȡ��Ԫ������
	parser.GetAttrNode( childNode,"Freq",freq );
	parser.GetAttrNode( childNode,"STD",STD );
	parser.GetAttrNode( childNode,"SymbolRate",symbolRate );
	parser.GetAttrNode( childNode,"Action",action );

	tempParser.SetAttrNode( "TaskID",taskID,tempChildNode );
	tempParser.SetAttrNode( "Action",action,tempChildNode );
	tempParser.SetAttrNode("Periodicity",periodicity,tempChildNode);

	pXMLNODELIST childNodeList = tempParser.GetNodeList( tempChildNode );

	//��ȡQualityQueryTaskԪ��
	pXMLNODE qualityTaskNode = tempParser.GetFirstNode( childNodeList );


	try
	{
		//����QualityQueryTaskԪ������
		tempParser.SetAttrNode( "DeviceID",deviceID,qualityTaskNode );
		tempParser.SetAttrNode( "STD",STD,qualityTaskNode );
		tempParser.SetAttrNode( "Freq",freq,qualityTaskNode );
		tempParser.SetAttrNode( "SymbolRate",symbolRate,qualityTaskNode) ;
	}
	catch(...)
	{
		string error = "����ָ���ѯ����ʧ��";
		APPLOG::instance()->WriteLog( OTHER,LOG_EVENT_ERROR,error,LOG_OUTPUT_BOTH );

	}
	pXMLNODE qualityParamNode = tempParser.GetNextNode( childNodeList );

	count = parser.GetChildCount( childNode );

	//��������XML��QualityIndexԪ���б�
	for ( int i = 0;i< count;i++ )
	{
		tempParser.CreateNode("Msg/QualityRealtimeQuery/QualityParam","QualityIndex");
	}
	//��ȡQualityIndex��Ԫ���б�
	pXMLNODELIST tempQualityIndexList = tempParser.GetNodeList( qualityParamNode );
	pXMLNODELIST qualityIndexList = parser.GetNodeList( childNode );
	pXMLNODE  listNode;
	pXMLNODE  tempListNode;

	try
	{
		//���÷���XML��QualityIndexԪ������
		for ( int j = 0;j< count;j++ )
		{
			if ( j == 0 )
			{
				listNode = parser.GetFirstNode( qualityIndexList );
				tempListNode = tempParser.GetFirstNode( tempQualityIndexList );
			}
			else
			{
				listNode = parser.GetNextNode( qualityIndexList );
				tempListNode = tempParser.GetNextNode( tempQualityIndexList );
			}

			parser.GetAttrNode( listNode,"Type",Type );
			parser.GetAttrNode( listNode,"Desc",Desc );
			tempParser.SetAttrNode( "Type",Type,tempListNode );
			tempParser.SetAttrNode( "Desc",Desc,tempListNode );
			tempParser.SetAttrNode( "Value",Value,tempListNode );
		}
	}
	catch(...)
	{
		string error = "����ָ������ʧ��";
		APPLOG::instance()->WriteLog( OTHER,LOG_EVENT_ERROR,error,LOG_OUTPUT_BOTH );

	}
	//���浽�ַ���
	tempParser.SaveToString( strStandardXML);
	return true;
}

bool TranslateDownXMLForATV::TranslateChannelScanQuery( const std::string& strOriginalXML,std::string& strStandardXML )
{
	string downType;//����������

	//��Ƶ��ѯ��׼��XML�ṹ
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><ChannelScanQuery/></Msg>";

	XmlParser parser( strOriginalXML.c_str());
	XmlParser tempParser( source );

	//��ȡ��Ԫ��
	pXMLNODE rootNode ;
	pXMLNODE childNode ;
	string nodeName;
	rootNode=parser.GetRootNode();
	childNode=parser.GetNodeFromPath("Msg/ChannelScanQuery/ChannelScan");

	//��ʼ������ֵ
	string dateTime, priority;
	string taskID,deviceID, freq,STD,symbolRate,action,version,msgid,protocol,srccode,dstcode,srcurl;
	string StartFreq,EndFreq,StepFreq;
	string Type,Desc,Value;
	int count = 0;
	string tasktype="ChannelScanQuery";
	
	//���ø�����
	pXMLNODE tempRootNode = tempParser.GetNodeFromPath( "Msg" );


	pXMLNODE tempChildNode = tempParser.GetNodeFromPath( "Msg/ChannelScanQuery" );
	pXMLNODE channelscannode=tempParser.CreateNodePtr(tempChildNode,"Channel");
	parser.GetAttrNode(rootNode,"Version",version);
	parser.GetAttrNode(rootNode,"MsgID",msgid);
	parser.GetAttrNode(rootNode,"Type",downType);
	parser.GetAttrNode(rootNode,"Protocol",protocol);
	parser.GetAttrNode(rootNode,"DateTime",dateTime );
	parser.GetAttrNode(rootNode,"Priority",priority );
	parser.GetAttrNode(rootNode,"Protocol",protocol);
	parser.GetAttrNode(rootNode,"SrcCode",srccode);
	parser.GetAttrNode(rootNode,"DstCode",dstcode);
	parser.GetAttrNode(rootNode,"SrcURL",srcurl);

	if(downType=="TVMonDown")
	{
		downType="ATV";
	}
	else
	{
		return false;
	}
	if(protocol.length()<1)
		protocol="HTTP";
	tempParser.SetAttrNode("Version",version,tempRootNode);
	tempParser.SetAttrNode("MsgID",msgid,tempRootNode);
	tempParser.SetAttrNode("DVBType",downType,tempRootNode );
	tempParser.SetAttrNode("TaskType",tasktype,tempRootNode );
	tempParser.SetAttrNode("Protocol",protocol,tempRootNode);
	tempParser.SetAttrNode("DateTime",dateTime,tempRootNode );
	tempParser.SetAttrNode("SrcCode",srccode,tempRootNode);
	tempParser.SetAttrNode("DstCode",dstcode,tempRootNode);
	tempParser.SetAttrNode("Priority",priority,tempRootNode );
	tempParser.SetAttrNode("SrcURL",srcurl,tempRootNode);

	parser.GetAttrNode(childNode,"StartFreq",StartFreq);
	parser.GetAttrNode(childNode,"EndFreq",EndFreq);
	parser.GetAttrNode(childNode,"StepFreq",StepFreq);

	tempParser.SetAttrNode("StartFreq",StartFreq,channelscannode);
	tempParser.SetAttrNode("EndFreq",EndFreq,channelscannode);
	tempParser.SetAttrNode("StepFreq",StepFreq,channelscannode);

	tempParser.SaveToString(strStandardXML);
	return true;

}

//wz_110125
bool TranslateDownXMLForATV::TranslateChannelSet( const std::string& strOriginalXML,std::string& strStandardXML )
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser( strOriginalXML.c_str());
	XmlParser tempParser( source );

	std::string strVersion,strMsgID,strType,strProtocol,strSetDateTime,
		strSrcCode,strDstCode,strSrcURL,strPriority;

	pXMLNODE rootNode = parser.GetNodeFromPath("Msg");         //��ȡԭʼXML��Ԫ��
	parser.GetAttrNode(rootNode,"Version",strVersion);         //��ȡMsg�ڵ��Version����ֵ
	parser.GetAttrNode(rootNode,"MsgID",strMsgID);		       //��ȡMsg�ڵ��MsgID����ֵ
	parser.GetAttrNode(rootNode,"Type",strType);		       //��ȡMsg�ڵ��Type����ֵ
	parser.GetAttrNode(rootNode,"DateTime",strSetDateTime);    //��ȡMsg�ڵ��DateTime����ֵ��
	parser.GetAttrNode(rootNode,"SrcCode",strSrcCode);         //��ȡMsg�ڵ��StrCode����ֵ
	parser.GetAttrNode(rootNode,"DstCode",strDstCode);         //��ȡMsg�ڵ��DstCode����ֵ
	parser.GetAttrNode(rootNode,"Priority",strPriority);       //��ȡMsg�ڵ��Priority����ֵ
	parser.GetAttrNode(rootNode,"SrcURL",strSrcURL);

	if (strType =="TVMonDown")
	{
		strType = "ATV";          //����Msg�ڵ��DVBType����ֵ
	}
	else
	{
		return false;
	}
	string strTaskType = "ChannelSet";
	pXMLNODE tempRootNode = tempParser.GetNodeFromPath( "Msg" );         
	tempParser.SetAttrNode("Version",strVersion,tempRootNode);               //����Msg�ڵ��Version����ֵ
	tempParser.SetAttrNode("MsgID",strMsgID,tempRootNode);                   //����Msg�ڵ��MsgID����ֵ
	tempParser.SetAttrNode("DVBType",strType,tempRootNode);
	tempParser.SetAttrNode("TaskType",strTaskType,tempRootNode);   //����Msg�ڵ��TaskType����ֵ
	tempParser.SetAttrNode("Protocol",strProtocol,tempRootNode);                 //����Msg�ڵ��Protocol����ֵ
	tempParser.SetAttrNode("DateTime",strSetDateTime,tempRootNode);              //����Msg�ڵ��DateTime����ֵ
	tempParser.SetAttrNode("SrcCode",strSrcCode,tempRootNode);                   //����Msg�ڵ��StrCode����ֵ
	tempParser.SetAttrNode("DstCode",strDstCode,tempRootNode);                   //����Msg�ڵ��DstCode����ֵ
	tempParser.SetAttrNode("SrcURL",strSrcURL,tempRootNode);                     //����Msg�ڵ��SrcUrl����ֵ
	tempParser.SetAttrNode("Priority",strPriority,tempRootNode);                 //����Msg�ڵ��Priority����ֵ

	pXMLNODE channelSetNode = parser.GetNodeFromPath("Msg/ChannelSet");
	pXMLNODELIST channelNodeList = parser.GetNodeList(channelSetNode);
	pXMLNODE tempChannelSetNode =tempParser.CreateNodePtr(tempRootNode,"ChannelSet");  //����QualityQuery�ڵ�

	std::string strCode, strName, strFreq;
	for (int i = 0;i<parser.GetChildCount(channelSetNode);i++)
	{
		pXMLNODE channelNode = parser.GetNextNode(channelNodeList);

		parser.GetAttrNode(channelNode,"CODE",strCode);                                //��ȡԭʼXML��Channel�ڵ��CODE����ֵ
		parser.GetAttrNode(channelNode,"Name",strName);                              //��ȡԭʼXML��Channel�ڵ��Name����ֵ
		parser.GetAttrNode(channelNode,"VideoFreq",strFreq);                                //��ȡԭʼXML��Channel�ڵ��VideoFreq����ֵ
		

		pXMLNODE tempChannelNode = tempParser.CreateNodePtr(tempChannelSetNode,"Channel");//������׼XML��Channel�ڵ�
		tempParser.SetAttrNode("CODE",strCode,tempChannelNode);                                //���ñ�׼XML��Channel�ڵ��CODE����ֵ
		tempParser.SetAttrNode("Name",strName,tempChannelNode);                              //���ñ�׼XML��Channel�ڵ��Name����ֵ
		tempParser.SetAttrNode("VideoFreq",strFreq,tempChannelNode);                                //���ñ�׼XML��Channel�ڵ��VideoFreq����ֵ
	}
	tempParser.SaveToString(strStandardXML);   //���浽�ַ���
	return true;
}

bool TranslateDownXMLForATV::TranslateStreamRound( const std::string& strOriginalXML,std::string& strStandardXML )
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><StreamRoundQuery></StreamRoundQuery></Msg>";
	XmlParser parser( strOriginalXML.c_str());
	XmlParser tempParser( source );

	string downType,version,msgid,protocol, dateTime, priority,srccode,dstcode,srcurl;//ͷ��Ϣ
	pXMLNODE rootNode = parser.GetNodeFromPath( "Msg" );
	parser.GetAttrNode( rootNode,"Type",downType );
	parser.GetAttrNode( rootNode,"MsgID",msgid );
	parser.GetAttrNode( rootNode,"Version",version );
	parser.GetAttrNode( rootNode,"Protocol",protocol );
	parser.GetAttrNode( rootNode,"SrcCode",srccode );
	parser.GetAttrNode( rootNode,"DstCode",dstcode );
	parser.GetAttrNode( rootNode,"DateTime",dateTime );
	parser.GetAttrNode( rootNode,"Priority",priority );
	parser.GetAttrNode( rootNode,"SrcURL",srcurl );

	if(!strcmp(downType.c_str(),"TVMonDown"))
	{
		downType="ATV";
	}
	else
	{
		return false;
	}

	//����ͷ��Ϣ
	if(protocol.length()<1)
	{
		protocol="HTTP";
	}
	pXMLNODE tempRootNode = tempParser.GetNodeFromPath( "Msg" );
	tempParser.SetAttrNode( "Version",version,tempRootNode );
	tempParser.SetAttrNode( "MsgID",msgid,tempRootNode );
	tempParser.SetAttrNode( "DVBType",downType,tempRootNode );
	tempParser.SetAttrNode( "TaskType",string("StreamRoundQuery"),tempRootNode );
	tempParser.SetAttrNode( "Protocol",protocol,tempRootNode );
	tempParser.SetAttrNode( "DateTime",dateTime,tempRootNode );
	tempParser.SetAttrNode( "SrcCode",srccode,tempRootNode );
	tempParser.SetAttrNode( "DstCode",dstcode,tempRootNode );
	tempParser.SetAttrNode( "SrcURL",srcurl,tempRootNode );
	tempParser.SetAttrNode( "Priority",priority,tempRootNode );
	pXMLNODE childNode=parser.GetNodeFromPath("Msg/StreamRoundQuery/RoundStream");

	pXMLNODE tempQueryNode = tempParser.GetNodeFromPath("Msg/StreamRoundQuery");
	//��ʼ������ֵ
	string Index,EquCode,LastURL,Freq,Width,Height,Fps,Bps,RoundTime,DeviceID;
	
		//��ȡʵʱ����Ƶ������
	parser.GetAttrNode( childNode,"Index",Index );
	parser.GetAttrNode( childNode,"EquCode",EquCode );
	parser.GetAttrNode( childNode,"LastURL",LastURL );
	parser.GetAttrNode( childNode,"Freq",Freq );
	parser.GetAttrNode( childNode,"Width",Width );
	parser.GetAttrNode( childNode,"RoundTime",RoundTime );
	parser.GetAttrNode( childNode,"Height",Height );
	parser.GetAttrNode( childNode,"Fps",Fps );
	parser.GetAttrNode( childNode,"Bps",Bps );
		//����ʵʱ����Ƶ����
	tempParser.SetAttrNode("DeviceID",DeviceID,tempQueryNode);
	tempParser.SetAttrNode("RoundTime",RoundTime,tempQueryNode);

	pXMLNODELIST nodelist=parser.GetNodeList(childNode);
	for (int k=0;k!=nodelist->Size();++k)
	{
		pXMLNODE chScanNode=tempParser.CreateNodePtr(tempQueryNode,"RoundChannel");
		pXMLNODE child=parser.GetNextNode(nodelist);
		parser.GetAttrNode(child,"Freq",Freq);

		tempParser.SetAttrNode( "Freq",Freq,chScanNode );
		tempParser.SetAttrNode( "Code",string(""),chScanNode );
		tempParser.SetAttrNode( "OrgNetID",string(""),chScanNode );
		tempParser.SetAttrNode( "TsID",string(""),chScanNode );
		tempParser.SetAttrNode( "ServiceID",string(""),chScanNode );
		tempParser.SetAttrNode( "VideoPID",string(""),chScanNode );
		tempParser.SetAttrNode( "AudioPID",string(""),chScanNode );
		tempParser.SetAttrNode( "DataType",string(""),chScanNode );
		tempParser.SetAttrNode( "Width",Width,chScanNode );
		tempParser.SetAttrNode( "Height",Height,chScanNode );
		tempParser.SetAttrNode( "Fps",Fps,chScanNode );
		tempParser.SetAttrNode( "Bps",Bps,chScanNode );
	}
		
	tempParser.SaveToString(strStandardXML);//���浽�ַ���
	return true;
}

//���ر�׼�Ķ�ʱָ����������xml
bool TranslateDownXMLForATV::TranslateQualityTaskSet(const std::string& strOriginalXML,std::string& strStandardXML)
{  
	std::string strVersion,strMsgID,strType,strTaskType,strProtocol,strSetDateTime,strSrcCode,strDstCode,strSrcURL,strPriority;
	std::string TaskID, Action,EquCode,InRunPlan,Code;

	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser( strOriginalXML.c_str());
	XmlParser tempParser( source );

	pXMLNODE rootNode = parser.GetNodeFromPath("Msg");         //��ȡԭʼXML��Ԫ��
	parser.GetAttrNode(rootNode,"Version",strVersion);         //��ȡMsg�ڵ��Version����ֵ
	parser.GetAttrNode(rootNode,"MsgID",strMsgID);		       //��ȡMsg�ڵ��MsgID����ֵ
	parser.GetAttrNode(rootNode,"Type",strType);		       //��ȡMsg�ڵ��Type����ֵ
	parser.GetAttrNode(rootNode,"Protocol",strProtocol);       //��ȡMsg�ڵ��protocol����ֵ
	parser.GetAttrNode(rootNode,"DateTime",strSetDateTime);    //��ȡMsg�ڵ��DateTime����ֵ��
	parser.GetAttrNode(rootNode,"SrcCode",strSrcCode);         //��ȡMsg�ڵ��StrCode����ֵ
	parser.GetAttrNode(rootNode,"DstCode",strDstCode);         //��ȡMsg�ڵ��DstCode����ֵ
	parser.GetAttrNode(rootNode,"SrcURL",strSrcURL);		   //��ȡMsg�ڵ��SrcUrl����ֵ	   
	parser.GetAttrNode(rootNode,"Priority",strPriority);       //��ȡMsg�ڵ��Priority����ֵ


	pXMLNODE tempRootNode = tempParser.GetNodeFromPath( "Msg" );         
	tempParser.SetAttrNode("Version",strVersion,tempRootNode);               //����Msg�ڵ��Version����ֵ
	tempParser.SetAttrNode("MsgID",strMsgID,tempRootNode);                   //����Msg�ڵ��MsgID����ֵ
	if (strType =="TVMonDown")
	{
		tempParser.SetAttrNode("DVBType",string("ATV"),tempRootNode);           //����Msg�ڵ��DVBType����ֵ
	}
	else
	{
		return false;
	}

	tempParser.SetAttrNode("TaskType",string("QualityTaskSet"),tempRootNode);   //����Msg�ڵ��TaskType����ֵ
	tempParser.SetAttrNode("Protocol",strProtocol,tempRootNode);                 //����Msg�ڵ��Protocol����ֵ
	tempParser.SetAttrNode("DateTime",strSetDateTime,tempRootNode);              //����Msg�ڵ��DateTime����ֵ
	tempParser.SetAttrNode("SrcCode",strSrcCode,tempRootNode);                   //����Msg�ڵ��StrCode����ֵ
	tempParser.SetAttrNode("DstCode",strDstCode,tempRootNode);                   //����Msg�ڵ��DstCode����ֵ
	tempParser.SetAttrNode("SrcURL",strSrcURL,tempRootNode);                     //����Msg�ڵ��SrcUrl����ֵ
	tempParser.SetAttrNode("Priority",strPriority,tempRootNode);                 //����Msg�ڵ��Priority����ֵ

	pXMLNODE childNode = parser.GetNodeFirstChild( rootNode );		   
	pXMLNODE qualityReportNode = parser.GetNodeFirstChild( childNode );


	pXMLNODE querySetNode = tempParser.CreateNodePtr(tempRootNode,"QualityTaskSet");  //����QualityTaskSet�ڵ�
	pXMLNODE taskParmNode = tempParser.CreateNodePtr(querySetNode,"TaskParm");         //����TaskParm�ڵ�
	pXMLNODE qualityParamNode = tempParser.CreateNodePtr(querySetNode,"QualityParam"); //����QualityParam�ڵ�
	pXMLNODE timeParamNode =  tempParser.CreateNodePtr(querySetNode,"TimeParam");      //����TimeParam�ڵ�
	parser.GetAttrNode(qualityReportNode,"TaskID",TaskID);							   //��ȡԭʼXML��QualityReport�ڵ��TaskID����
	parser.GetAttrNode(qualityReportNode,"Action",Action);                             //��ȡԭʼXML��QualityReport�ڵ��Action����
	parser.GetAttrNode(qualityReportNode,"EquCode",EquCode);
	parser.GetAttrNode(qualityReportNode,"InRunPlan",InRunPlan);
	tempParser.SetAttrNode("TaskID",TaskID,querySetNode);                              //����QualityQuerySet�ڵ��TaskID����
	tempParser.SetAttrNode("Action",Action,querySetNode);                              //����QualityQuerySet�ڵ��Action����
	tempParser.SetAttrNode("EquCode",EquCode,querySetNode);
	tempParser.SetAttrNode("InRunPlan",InRunPlan,querySetNode);

	if(Action=="Set"||Action=="set")
	{
		pXMLNODE channelNode = parser.GetNodeFromPath("Msg/QualityReportTaskSet/QualityReport/Channel");      //��ȡԭʼXML��Channel�ڵ�
		pXMLNODE qualityNode = parser.GetNodeFromPath("Msg/QualityReportTaskSet/QualityReport/QualityParam"); //��ȡԭʼXML��QualityParam�ڵ�
		std::string STD,Freq,SymbolRate;
		pXMLNODELIST channelNodeList=parser.GetNodeList(channelNode);
		for ( int i = 0;i<parser.GetChildCount(channelNode);i++)
		{  
			pXMLNODE channelInfo = parser.GetNextNode(channelNodeList);
			parser.GetAttrNode(channelInfo,"STD",STD);                                     //��ȡChannel�ڵ��STD����
			parser.GetAttrNode(channelInfo,"Freq",Freq);								   //��ȡChannel�ڵ��Freq����
			parser.GetAttrNode(channelInfo,"SymbolRate",SymbolRate);					   //��ȡChannel�ڵ��SymbolRate����
			parser.GetAttrNode(channelInfo,"Code",Code);

			//���°���ȫƵ��ָ����������������⴦��
			if (Code == "All" || Code == "all")
			{
				std::vector<sChannelInfo> vecChannelInfo;
				CHANNELINFOMGR::instance()->GetChannelInfoByDvbtype(ATV, vecChannelInfo);	//��ȡ������Ƶ����
				if (vecChannelInfo.empty())
				{
					string msg = "��·����ָ�����������ѯƵ����ʧ��!!";
					//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);
				}

				//��Ƶ��ѭ�����뵽ת�����xml��
				std::vector<sChannelInfo>::iterator chanIter = vecChannelInfo.begin();
				for (; chanIter!=vecChannelInfo.end(); chanIter++)
				{
					pXMLNODE taskNode =tempParser.CreateNodePtr(taskParmNode,"QualityQueryTask");  //����TaskParm�ڵ���ӽڵ�
					tempParser.SetAttrNode("STD", STD, taskNode);                                    //����QualityQueryTask�ڵ��STD����
					tempParser.SetAttrNode("Freq", (*chanIter).Freq, taskNode);                                  //����QualityQueryTask�ڵ��Freq����
					tempParser.SetAttrNode("SymbolRate", SymbolRate, taskNode);                      //����QualityQueryTask�ڵ��SymbolRate����
					tempParser.SetAttrNode("Code", (*chanIter).ChannelCode, taskNode);
				}
				break;	//ȫƵ��������ɣ�������ѭ��
			}
			if(Code!="")
				CHANNELINFOMGR::instance()->GetFreqByChannelCode(ATV,Code,Freq);
			pXMLNODE taskNode =tempParser.CreateNodePtr(taskParmNode,"QualityQueryTask");  //����TaskParm�ڵ���ӽڵ�
			tempParser.SetAttrNode("STD",STD,taskNode);                                    //����QualityQueryTask�ڵ��STD����
			tempParser.SetAttrNode("Freq",Freq,taskNode);                                  //����QualityQueryTask�ڵ��Freq����
			tempParser.SetAttrNode("SymbolRate",SymbolRate,taskNode);                      //����QualityQueryTask�ڵ��SymbolRate����
			tempParser.SetAttrNode("Code",Code,taskNode);
		}
		std::string Type,Desc;
		pXMLNODELIST qualityNodeList=parser.GetNodeList(qualityNode);
		for (int j = 0;j<parser.GetChildCount(qualityNode);j++)
		{
			pXMLNODE qualityIndex = parser.GetNextNode(qualityNodeList);
			parser.GetAttrNode(qualityIndex,"Type",Type);                                          //��ȡԭʼXML��QualityIndex�ڵ��Type����
			parser.GetAttrNode(qualityIndex,"Desc",Desc);										   //��ȡԭʼXML��QualityIndex�ڵ��Desc����
			pXMLNODE qualityIndexNode = tempParser.CreateNodePtr(qualityParamNode,"QualityIndex"); //����QualityParam�ڵ���ӽڵ�
			tempParser.SetAttrNode("Type",Type,qualityIndexNode);                                  //����QualityIndex�ڵ��Type����
			tempParser.SetAttrNode("Desc",Desc,qualityIndexNode);								   //����QualityIndex�ڵ��Desc����
		}
		std::string DayOfWeek,StartTime,EndTime,StartDateTime,EndDateTime,CheckInterval;
		pXMLNODELIST paramNodeList = parser.GetNodeList(qualityReportNode);
		for (int p = 0;p<parser.GetChildCount(qualityReportNode);p++)
		{
			pXMLNODE checkTimeNode = parser.GetNextNode(paramNodeList);
			std::string strNodeName = parser.GetNodeName(checkTimeNode);
			if (strNodeName =="CycleReportTime"||strNodeName =="SingleReportTime")
			{
				if (strNodeName =="CycleReportTime")
				{
					parser.GetAttrNode(checkTimeNode,"DayOfWeek",DayOfWeek);                   //��ȡԭʼXML��CycleReportTime�ڵ��DayOfWeek����
					parser.GetAttrNode(checkTimeNode,"StartTime",StartTime);                   //��ȡԭʼXML��CycleReportTime�ڵ��StartTime����
					parser.GetAttrNode(checkTimeNode,"EndTime",EndTime);                       //��ȡԭʼXML��CycleReportTime�ڵ��EndTime����
					parser.GetAttrNode(checkTimeNode,"CheckInterval",CheckInterval);           //��ȡԭʼXML��CycleReportTime�ڵ��CheckInterval����
				}
				else if (strNodeName =="SingleReportTime")
				{
					parser.GetAttrNode(checkTimeNode,"StartDateTime",StartDateTime);          //��ȡԭʼXML��SingleReportTime�ڵ��StartDateTime����
					parser.GetAttrNode(checkTimeNode,"EndDateTime",EndDateTime);			  //��ȡԭʼXML��SingleReportTime�ڵ��EndDateTime����
					parser.GetAttrNode(checkTimeNode,"CheckInterval",CheckInterval);          //��ȡԭʼXML��SingleReportTime�ڵ��CheckInterval����
				}
				pXMLNODE timeIndexNode = tempParser.CreateNodePtr(timeParamNode,"TimeIndex");   //����TimeParam�ڵ���ӽڵ�

				tempParser.SetAttrNode("DayOfWeek",DayOfWeek,timeIndexNode);                    //����TimeIndex�ڵ��DayOfWeek����
				tempParser.SetAttrNode("StartTime",StartTime,timeIndexNode);                    //����TimeIndex�ڵ��StartTime����
				tempParser.SetAttrNode("EndTime",EndTime,timeIndexNode);                        //����TimeIndex�ڵ��EndTime����
				tempParser.SetAttrNode("StartDateTime",StartDateTime,timeIndexNode);            //����TimeIndex�ڵ��StartDateTime����
				tempParser.SetAttrNode("EndDateTime",EndDateTime,timeIndexNode);                //����TimeIndex�ڵ��EndDateTime����
				tempParser.SetAttrNode("CheckInterval",CheckInterval,timeIndexNode);            //����TimeIndex�ڵ��CheckInterval����
				//���³�ʼ��ָ����������ʱ����ر���
				DayOfWeek = "";
				StartTime = "";
				EndTime ="";
				StartDateTime ="";
				EndDateTime = "";
				CheckInterval = "";
			}
		}
	}
	tempParser.SaveToString(strStandardXML);
	return true;
}

//���ر�׼�Ķ�ʱָ��ִ������xml
bool TranslateDownXMLForATV::TranslateQualityTask(const std::string& strOriginalXML,std::vector<std::string>& strStandardXMLVec)
{
	size_t pos;
	std::string orgXML = strOriginalXML;
	std::vector<std::string> timeIndexVct;
	while ((pos=orgXML.find("<TimeIndex"))!=std::string::npos&&orgXML.length()>0)//����xml���Ƿ���TimeIndex
	{
		size_t pos2=orgXML.find(">",pos);
		if (pos2>0)
		{
			timeIndexVct.push_back(orgXML.substr(pos,pos2-pos+1));//TimeIndex�ڵ���Ϣ����
			orgXML.erase(pos,pos2-pos+1);//ɾ��xml�е�TimeIndex�ڵ�
		}
	}

	for (size_t p=0;p<timeIndexVct.size();++p)
	{
		std::string tmpstr=orgXML;
		std::string strVersion,strMsgID,strType,strTaskType,strProtocol,strSetDateTime,strSrcCode,strDstCode,strSrcURL,strPriority;
		std::string TaskID, Action,DevideID;

		char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
		XmlParser parser( tmpstr.c_str());
		XmlParser tempParser( source );

		pXMLNODE rootNode = parser.GetNodeFromPath("Msg");         //��ȡԭʼXML��Ԫ��
		parser.GetAttrNode(rootNode,"Version",strVersion);         //��ȡMsg�ڵ��Version����ֵ
		parser.GetAttrNode(rootNode,"MsgID",strMsgID);		       //��ȡMsg�ڵ��MsgID����ֵ
		parser.GetAttrNode(rootNode,"Protocol",strProtocol);       //��ȡMsg�ڵ��protocol����ֵ
		parser.GetAttrNode(rootNode,"DateTime",strSetDateTime);    //��ȡMsg�ڵ��DateTime����ֵ��
		parser.GetAttrNode(rootNode,"SrcCode",strSrcCode);         //��ȡMsg�ڵ��StrCode����ֵ
		parser.GetAttrNode(rootNode,"DstCode",strDstCode);         //��ȡMsg�ڵ��DstCode����ֵ
		parser.GetAttrNode(rootNode,"SrcURL",strSrcURL);		   //��ȡMsg�ڵ��SrcUrl����ֵ	   
		parser.GetAttrNode(rootNode,"Priority",strPriority);       //��ȡMsg�ڵ��Priority����ֵ

		pXMLNODE tempRootNode = tempParser.GetNodeFromPath( "Msg" );         
		tempParser.SetAttrNode("Version",strVersion,tempRootNode);                   //����Msg�ڵ��Version����ֵ
		tempParser.SetAttrNode("MsgID",strMsgID,tempRootNode);                       //����Msg�ڵ��MsgID����ֵ
		tempParser.SetAttrNode("DVBType",string("ATV"),tempRootNode);               //����Msg�ڵ��DVBType����ֵ
		tempParser.SetAttrNode("TaskType",string("QualityTask"),tempRootNode);    //����Msg�ڵ��TaskType����ֵ
		tempParser.SetAttrNode("Protocol",strProtocol,tempRootNode);                 //����Msg�ڵ��Protocol����ֵ
		tempParser.SetAttrNode("DateTime",strSetDateTime,tempRootNode);              //����Msg�ڵ��DateTime����ֵ
		tempParser.SetAttrNode("SrcCode",strSrcCode,tempRootNode);                   //����Msg�ڵ��StrCode����ֵ
		tempParser.SetAttrNode("DstCode",strDstCode,tempRootNode);                   //����Msg�ڵ��DstCode����ֵ
		tempParser.SetAttrNode("SrcURL",strSrcURL,tempRootNode);                     //����Msg�ڵ��SrcUrl����ֵ
		tempParser.SetAttrNode("Priority",strPriority,tempRootNode);                 //����Msg�ڵ��Priority����ֵ

		pXMLNODE childNode = parser.GetNodeFirstChild( rootNode );		   
		pXMLNODE taskNode = parser.GetNodeFromPath("Msg/QualityTaskSet/TaskParm");      //��ȡԭʼXML��TaskParm�ڵ�
		pXMLNODE qualityNode = parser.GetNodeFromPath("Msg/QualityTaskSet/QualityParam"); //��ȡԭʼXML��QualityParam�ڵ�

		pXMLNODE qualityTaskNode = tempParser.CreateNodePtr(tempRootNode,"QualityTask");  //����QualityTask�ڵ�
		pXMLNODE taskParmNode = tempParser.CreateNodePtr(qualityTaskNode,"TaskParm");         //����TaskParm�ڵ�
		pXMLNODE qualityParamNode = tempParser.CreateNodePtr(qualityTaskNode,"QualityParam"); //����QualityParam�ڵ�
		parser.GetAttrNode(childNode,"TaskID",TaskID);				     			  //��ȡԭʼXML��QualityTaskSet�ڵ��TaskID����
		parser.GetAttrNode(childNode,"Action",Action);                                //��ȡԭʼXML��QualityTaskSet�ڵ��Action����
		tempParser.SetAttrNode("TaskID",TaskID,qualityTaskNode);                              //����QualityTask�ڵ��TaskID����
		tempParser.SetAttrNode("Action",Action,qualityTaskNode);                              //����QualityTask�ڵ��Action����
		tempParser.SetAttrNode("DeviceID",DevideID,qualityTaskNode);                          //����QualityTask�ڵ��DevieceID����

		std::string STD,Freq,SymbolRate;
		pXMLNODELIST taskNodeList=parser.GetNodeList(taskNode);
		for ( int i = 0;i<taskNodeList->Size();i++)
		{  
			pXMLNODE taskInfo = parser.GetNextNode(taskNodeList);
			parser.GetAttrNode(taskInfo,"STD",STD);                                     //��ȡTaskParm�ڵ��STD����
			parser.GetAttrNode(taskInfo,"Freq",Freq);								   //��ȡTaskParm�ڵ��Freq����
			parser.GetAttrNode(taskInfo,"SymbolRate",SymbolRate);					   //��ȡTaskParm�ڵ��SymbolRate����
			pXMLNODE taskNode =tempParser.CreateNodePtr(taskParmNode,"QualityQueryTask");  //����TaskParm�ڵ���ӽڵ�
			tempParser.SetAttrNode("STD",STD,taskNode);                                    //����QualityQueryTask�ڵ��STD����
			tempParser.SetAttrNode("Freq",Freq,taskNode);                                  //����QualityQueryTask�ڵ��Freq����
			tempParser.SetAttrNode("SymbolRate",SymbolRate,taskNode);                      //����QualityQueryTask�ڵ��SymbolRate����
		}
		std::string Type,Desc;
		pXMLNODELIST qualityNodeList =parser.GetNodeList(qualityNode);
		for (int j = 0;j<qualityNodeList->Size();j++)
		{
			pXMLNODE qualityIndex = parser.GetNextNode(qualityNodeList);
			parser.GetAttrNode(qualityIndex,"Type",Type);                                          //��ȡԭʼXML��QualityIndex�ڵ��Type����
			parser.GetAttrNode(qualityIndex,"Desc",Desc);										   //��ȡԭʼXML��QualityIndex�ڵ��Desc����
			pXMLNODE qualityIndexNode = tempParser.CreateNodePtr(qualityParamNode,"QualityIndex"); //����QualityParam�ڵ���ӽڵ�
			tempParser.SetAttrNode("Type",Type,qualityIndexNode);                                  //����QualityIndex�ڵ��Type����
			tempParser.SetAttrNode("Desc",Desc,qualityIndexNode);								   //����QualityIndex�ڵ��Desc����
		}
		tempParser.SaveToString(tmpstr);

		//����timeIndexVct�е�TimeIndex�ڵ���Ϣ��������
		size_t pos4=tmpstr.find("</QualityParam>");
		if (pos4>0)
		{
			std::string timeParam = "<TimeParam>"+ timeIndexVct[p]+"</TimeParam>";
			tmpstr.insert(pos4+15,timeParam);
			strStandardXMLVec.push_back(tmpstr);
		}
	}
	return true;
}

bool TranslateDownXMLForATV::TranslateAlarmParamSet( const std::string& strOriginalXML,std::string& strStandardXML )
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><AlarmParamSet></AlarmParamSet></Msg>";
	XmlParser parser( strOriginalXML.c_str());
	XmlParser tempParser( source );

	string downType,version,msgid,protocol, dateTime, priority,srccode,dstcode,srcurl;//ͷ��Ϣ
	pXMLNODE rootNode = parser.GetNodeFromPath( "Msg" );
	parser.GetAttrNode( rootNode,"Type",downType );
	parser.GetAttrNode( rootNode,"MsgID",msgid );
	parser.GetAttrNode( rootNode,"Version",version );
	parser.GetAttrNode( rootNode,"Protocol",protocol );
	parser.GetAttrNode( rootNode,"SrcCode",srccode );
	parser.GetAttrNode( rootNode,"DstCode",dstcode );
	parser.GetAttrNode( rootNode,"DateTime",dateTime );
	parser.GetAttrNode( rootNode,"Priority",priority );
	parser.GetAttrNode( rootNode,"SrcURL",srcurl );

	if(!strcmp(downType.c_str(),"TVMonDown"))
	{
		downType="ATV";
	}
	else
	{
		return false;
	}

	//����ͷ��Ϣ
	if(protocol.length()<1)
	{
		protocol="HTTP";
	}
	pXMLNODE tempRootNode = tempParser.GetNodeFromPath( "Msg" );
	tempParser.SetAttrNode( "Version",version,tempRootNode );
	tempParser.SetAttrNode( "MsgID",msgid,tempRootNode );
	tempParser.SetAttrNode( "DVBType",downType,tempRootNode );
	tempParser.SetAttrNode( "TaskType",string("AlarmParamSet"),tempRootNode );
	tempParser.SetAttrNode( "Protocol",protocol,tempRootNode );
	tempParser.SetAttrNode( "DateTime",dateTime,tempRootNode );
	tempParser.SetAttrNode( "SrcCode",srccode,tempRootNode );
	tempParser.SetAttrNode( "DstCode",dstcode,tempRootNode );
	tempParser.SetAttrNode("SrcURL",srcurl,tempRootNode);   

	pXMLNODE alarmset=tempParser.GetNodeFromPath("Msg/AlarmParamSet");
	pXMLNODE QualityAlarmSet=parser.GetNodeFromPath("Msg/QualityAlarmParamSet");
	int count=parser.GetChildCount(QualityAlarmSet);
	pXMLNODELIST AlarmList=parser.GetNodeList(QualityAlarmSet);
	for(int i=0;i<count;i++)
	{
		pXMLNODE alarmparam=parser.GetNextNode(AlarmList);
		std::string channelcode,freq;
		parser.GetAttrNode(alarmparam,"ChannelCode",channelcode);   //ChannelCODE����
		if ("" == channelcode)
		{
			parser.GetAttrNode(alarmparam,"ChannelCODE",channelcode);
			if ("" == channelcode)
			{
				//APPLOG::instance()->WriteLog(OTHER,LOG_EVENT_ERROR,
				//	"TranslateDownXMLForATV::TranslateAlarmParamSet --> channelcode�޷�����!!!");
				channelcode = "All";
			}
		}
		if (channelcode!="all"&&channelcode!="All")
		{
			CHANNELINFOMGR::instance()->GetFreqByChannelCode(ATV,channelcode,freq);
		}
		else
		{
			freq="All";
		}
		if(freq.length()<1)
		{
			parser.GetAttrNode(alarmparam,"Freq",freq);
		}
		//tempParser.SetAttrNode("STD",string(""),alarmset);
		//tempParser.SetAttrNode("Freq",freq,alarmset);
		//tempParser.SetAttrNode("SymbolRate",string(""),alarmset);

		pXMLNODELIST alarmlist=parser.GetNodeList(alarmparam);
		for (int i=0;i!=alarmlist->Size();++i)
		{
			pXMLNODE childnode=parser.GetNextNode(alarmlist);
			std::string Type,Desc,Value,UpThreshold,DownThreshold,Duration,Num,Switch,AlarmThreshold;
			parser.GetAttrNode(childnode,"type",Type);
			parser.GetAttrNode(childnode,"Desc",Desc);
			parser.GetAttrNode(childnode,"TimeThreshold",Duration);
			parser.GetAttrNode(childnode,"UpThreshold",UpThreshold);
			parser.GetAttrNode(childnode,"AlarmThreshold",AlarmThreshold);
			if (StrUtil::Str2Int(Type)<10)
			{
				parser.GetAttrNode(childnode,"DownThreshold",DownThreshold);
			}
			else
			{
				parser.GetAttrNode(childnode,"DownThreshold",Num);
			}
			parser.GetAttrNode(childnode,"Switch",Switch);
			if(Switch.length()<1)
				Switch="1";
			pXMLNODE AlarmParamNode=tempParser.CreateNodePtr(alarmset,"AlarmParam");
			tempParser.SetAttrNode("Freq",freq,AlarmParamNode);
			tempParser.SetAttrNode("ChannelID",freq,AlarmParamNode);
			tempParser.SetAttrNode("Type",Type,AlarmParamNode);
			tempParser.SetAttrNode("Desc",Desc,AlarmParamNode);
			tempParser.SetAttrNode("Duration",Duration,AlarmParamNode);
			tempParser.SetAttrNode("TimeInterval",string(""),AlarmParamNode);
			tempParser.SetAttrNode("DownThreshold",DownThreshold,AlarmParamNode);
			tempParser.SetAttrNode("UpThreshold",UpThreshold,AlarmParamNode);
			tempParser.SetAttrNode("AlarmThreshold",AlarmThreshold,AlarmParamNode);
			tempParser.SetAttrNode("Num",Num,AlarmParamNode);
			tempParser.SetAttrNode("Switch",Switch,AlarmParamNode);

		}
	}
	tempParser.SaveToString(strStandardXML);

	return true;

}

//���ر�׼�Ķ�ʱָ����������xml
bool TranslateDownXMLForATV::TranslateSpectrumTaskSet(const std::string& strOriginalXML,std::string& strStandardXML)
{  
	std::string strVersion,strMsgID,strType,strTaskType,strProtocol,strSetDateTime,strSrcCode,strDstCode,strSrcURL,strPriority;
	std::string TaskID, Action,EquCode,IsFreqScan,StartFreq,EndFreq,StepFreq,Code,TaskType,SpectrumAnalyse,ReferDbu,VBW,RBW;

	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser( strOriginalXML.c_str());
	XmlParser tempParser( source );

	pXMLNODE rootNode = parser.GetNodeFromPath("Msg");         //��ȡԭʼXML��Ԫ��
	parser.GetAttrNode(rootNode,"Version",strVersion);         //��ȡMsg�ڵ��Version����ֵ
	parser.GetAttrNode(rootNode,"MsgID",strMsgID);		       //��ȡMsg�ڵ��MsgID����ֵ
	parser.GetAttrNode(rootNode,"Type",strType);		       //��ȡMsg�ڵ��Type����ֵ
	parser.GetAttrNode(rootNode,"Protocol",strProtocol);       //��ȡMsg�ڵ��protocol����ֵ
	parser.GetAttrNode(rootNode,"DateTime",strSetDateTime);    //��ȡMsg�ڵ��DateTime����ֵ��
	parser.GetAttrNode(rootNode,"SrcCode",strSrcCode);         //��ȡMsg�ڵ��StrCode����ֵ
	parser.GetAttrNode(rootNode,"DstCode",strDstCode);         //��ȡMsg�ڵ��DstCode����ֵ
	parser.GetAttrNode(rootNode,"SrcURL",strSrcURL);		   //��ȡMsg�ڵ��SrcUrl����ֵ	   
	parser.GetAttrNode(rootNode,"Priority",strPriority);       //��ȡMsg�ڵ��Priority����ֵ


	pXMLNODE tempRootNode = tempParser.GetNodeFromPath( "Msg" );         
	tempParser.SetAttrNode("Version",strVersion,tempRootNode);               //����Msg�ڵ��Version����ֵ
	tempParser.SetAttrNode("MsgID",strMsgID,tempRootNode);                   //����Msg�ڵ��MsgID����ֵ
	if (strType =="TVMonDown")
	{
		tempParser.SetAttrNode("DVBType",string("ATV"),tempRootNode);           //����Msg�ڵ��DVBType����ֵ
	}
	else
	{
		return false;
	}

	tempParser.SetAttrNode("TaskType",string("SpectrumTaskSet"),tempRootNode);   //����Msg�ڵ��TaskType����ֵ
	tempParser.SetAttrNode("Protocol",strProtocol,tempRootNode);                 //����Msg�ڵ��Protocol����ֵ
	tempParser.SetAttrNode("DateTime",strSetDateTime,tempRootNode);              //����Msg�ڵ��DateTime����ֵ
	tempParser.SetAttrNode("SrcCode",strSrcCode,tempRootNode);                   //����Msg�ڵ��StrCode����ֵ
	tempParser.SetAttrNode("DstCode",strDstCode,tempRootNode);                   //����Msg�ڵ��DstCode����ֵ
	tempParser.SetAttrNode("SrcURL",strSrcURL,tempRootNode);                     //����Msg�ڵ��SrcUrl����ֵ
	tempParser.SetAttrNode("Priority",strPriority,tempRootNode);                 //����Msg�ڵ��Priority����ֵ

	pXMLNODE childNode = parser.GetNodeFirstChild( rootNode );		   
	pXMLNODE SpectrumNode = parser.GetNodeFirstChild( childNode );

	pXMLNODE querySetNode = tempParser.CreateNodePtr(tempRootNode,"SpectrumTaskSet");  //����QualityTaskSet�ڵ�
	pXMLNODE TaskNode = tempParser.CreateNodePtr(querySetNode,"TaskParam");
	pXMLNODE SpecqueryNode = tempParser.CreateNodePtr(TaskNode,"SpectrumQueryIndex");
	pXMLNODE timeParamNode =  tempParser.CreateNodePtr(querySetNode,"TimeParam");      //����TimeParam�ڵ�
	parser.GetAttrNode(SpectrumNode,"TaskID",TaskID);							   //��ȡԭʼXML��QualityReport�ڵ��TaskID����
	parser.GetAttrNode(SpectrumNode,"Action",Action);                             //��ȡԭʼXML��QualityReport�ڵ��Action����
	parser.GetAttrNode(SpectrumNode,"EquCode",EquCode);
	parser.GetAttrNode(SpectrumNode,"StartFreq",StartFreq);
	parser.GetAttrNode(SpectrumNode,"EndFreq",EndFreq);
	parser.GetAttrNode(SpectrumNode,"StepFreq",StepFreq);
	parser.GetAttrNode(SpectrumNode,"TaskType",TaskType);
	parser.GetAttrNode(SpectrumNode,"IsFreqScan",IsFreqScan);
	parser.GetAttrNode(SpectrumNode,"SpectrumAnalyse",SpectrumAnalyse);
	parser.GetAttrNode(SpectrumNode,"ReferDbu",ReferDbu);
	parser.GetAttrNode(SpectrumNode,"VBW",VBW);
	parser.GetAttrNode(SpectrumNode,"RBW",RBW);

	tempParser.SetAttrNode("TaskID",TaskID,querySetNode);                              //����QualityQuerySet�ڵ��TaskID����
	tempParser.SetAttrNode("Action",Action,querySetNode);                              //����QualityQuerySet�ڵ��Action����
	tempParser.SetAttrNode("EquCode",EquCode,SpecqueryNode );
	tempParser.SetAttrNode("StartFreq",StartFreq,SpecqueryNode );
	tempParser.SetAttrNode("EndFreq",EndFreq,SpecqueryNode) ;
	tempParser.SetAttrNode("StepFreq",StepFreq,SpecqueryNode );
	tempParser.SetAttrNode("TaskType",TaskType,SpecqueryNode);
	tempParser.SetAttrNode("IsFreqScan",IsFreqScan,SpecqueryNode );
	tempParser.SetAttrNode("SpectrumAnalyse",SpectrumAnalyse,SpecqueryNode );
	tempParser.SetAttrNode("ReferDbu",ReferDbu,SpecqueryNode) ;
	tempParser.SetAttrNode("VBW",VBW,SpecqueryNode );
	tempParser.SetAttrNode("RBW",RBW,SpecqueryNode );
	
	std::string DayOfWeek,StartTime,StartDateTime;
	pXMLNODELIST paramNodeList = parser.GetNodeList(SpectrumNode);
	for (int p = 0;p<paramNodeList->Size();p++)
	{
		pXMLNODE checkTimeNode = parser.GetNextNode(paramNodeList);
		std::string strNodeName = parser.GetNodeName(checkTimeNode);
		if (strNodeName =="CycleScanTime"||strNodeName =="SingleScanTime")
		{
			if (strNodeName =="CycleScanTime")
			{
				parser.GetAttrNode(checkTimeNode,"DayofWeek",DayOfWeek);                   //��ȡԭʼXML��CycleReportTime�ڵ��DayOfWeek����
				parser.GetAttrNode(checkTimeNode,"StartTime",StartTime);                   //��ȡԭʼXML��CycleReportTime�ڵ��StartTime����
			}
			else if (strNodeName =="SingleScanTime")
			{
				parser.GetAttrNode(checkTimeNode,"StartDateTime",StartDateTime);          //��ȡԭʼXML��SingleReportTime�ڵ��StartDateTime����

			}
			pXMLNODE timeIndexNode = tempParser.CreateNodePtr(timeParamNode,"TimeIndex");   //����TimeParam�ڵ���ӽڵ�

			tempParser.SetAttrNode("DayOfWeek",DayOfWeek,timeIndexNode);                    //����TimeIndex�ڵ��DayOfWeek����
			tempParser.SetAttrNode("StartTime",StartTime,timeIndexNode);                    //����TimeIndex�ڵ��StartTime����
			tempParser.SetAttrNode("StartDateTime",StartDateTime,timeIndexNode);            //����TimeIndex�ڵ��StartDateTime����               //����TimeIndex�ڵ��EndDateTime����

			//���³�ʼ��ָ����������ʱ����ر���
			DayOfWeek = "";
			StartTime = "";
			StartDateTime ="";
		}
	}
	tempParser.SaveToString(strStandardXML);
	return true;
}

bool TranslateDownXMLForATV::TranslateSpectrumTask(const std::string& strOriginalXML,std::vector<std::string>& strStandardXMLVec)
{
	size_t pos;
	std::string orgXML = strOriginalXML;
	std::vector<std::string> timeIndexVct;
	while ((pos=orgXML.find("<TimeIndex"))!=std::string::npos&&orgXML.length()>0)//����xml���Ƿ���TimeIndex
	{
		size_t pos2=orgXML.find(">",pos);
		if (pos2>0)
		{
			timeIndexVct.push_back(orgXML.substr(pos,pos2-pos+1));//TimeIndex�ڵ���Ϣ����
			orgXML.erase(pos,pos2-pos+1);//ɾ��xml�е�TimeIndex�ڵ�
		}
	}

	for (size_t p=0;p<timeIndexVct.size();++p)
	{
		std::string tmpstr=orgXML;
		std::string strVersion,strMsgID,strType,strTaskType,strProtocol,strSetDateTime,strSrcCode,strDstCode,strSrcURL,strPriority;
		std::string TaskID, Action,DevideID,EquCode,IsFreqScan,StartFreq,EndFreq,StepFreq,TaskType,SpectrumAnalyse,ReferDbu,VBW,RBW;

		char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
		XmlParser parser( tmpstr.c_str());
		XmlParser tempParser( source );

		pXMLNODE rootNode = parser.GetNodeFromPath("Msg");         //��ȡԭʼXML��Ԫ��
		parser.GetAttrNode(rootNode,"Version",strVersion);         //��ȡMsg�ڵ��Version����ֵ
		parser.GetAttrNode(rootNode,"MsgID",strMsgID);		       //��ȡMsg�ڵ��MsgID����ֵ
		parser.GetAttrNode(rootNode,"Protocol",strProtocol);       //��ȡMsg�ڵ��protocol����ֵ
		parser.GetAttrNode(rootNode,"DateTime",strSetDateTime);    //��ȡMsg�ڵ��DateTime����ֵ��
		parser.GetAttrNode(rootNode,"SrcCode",strSrcCode);         //��ȡMsg�ڵ��StrCode����ֵ
		parser.GetAttrNode(rootNode,"DstCode",strDstCode);         //��ȡMsg�ڵ��DstCode����ֵ
		parser.GetAttrNode(rootNode,"SrcURL",strSrcURL);		   //��ȡMsg�ڵ��SrcUrl����ֵ	   
		parser.GetAttrNode(rootNode,"Priority",strPriority);       //��ȡMsg�ڵ��Priority����ֵ


		pXMLNODE tempRootNode = tempParser.GetNodeFromPath( "Msg" );         
		tempParser.SetAttrNode("Version",strVersion,tempRootNode);                   //����Msg�ڵ��Version����ֵ
		tempParser.SetAttrNode("MsgID",strMsgID,tempRootNode);                       //����Msg�ڵ��MsgID����ֵ
		tempParser.SetAttrNode("DVBType",string("ATV"),tempRootNode);               //����Msg�ڵ��DVBType����ֵ
		tempParser.SetAttrNode("TaskType",string("SpectrumTask"),tempRootNode);    //����Msg�ڵ��TaskType����ֵ
		tempParser.SetAttrNode("Protocol",strProtocol,tempRootNode);                 //����Msg�ڵ��Protocol����ֵ
		tempParser.SetAttrNode("DateTime",strSetDateTime,tempRootNode);              //����Msg�ڵ��DateTime����ֵ
		tempParser.SetAttrNode("SrcCode",strSrcCode,tempRootNode);                   //����Msg�ڵ��StrCode����ֵ
		tempParser.SetAttrNode("DstCode",strDstCode,tempRootNode);                   //����Msg�ڵ��DstCode����ֵ
		tempParser.SetAttrNode("SrcURL",strSrcURL,tempRootNode);                     //����Msg�ڵ��SrcUrl����ֵ
		tempParser.SetAttrNode("Priority",strPriority,tempRootNode);                 //����Msg�ڵ��Priority����ֵ

		pXMLNODE childNode = parser.GetNodeFirstChild( rootNode );		   
		pXMLNODE SpecNode = parser.GetNodeFromPath("Msg/SpectrumTaskSet/TaskParam/SpectrumQueryIndex");      //��ȡԭʼXML��TaskParm�ڵ�
		
		pXMLNODE qualityTaskNode = tempParser.CreateNodePtr(tempRootNode,"SpectrumTask");  //����QualityTask�ڵ�
		pXMLNODE qualityParamNode = tempParser.CreateNodePtr(qualityTaskNode,"TaskParam"); //����QualityParam�ڵ�
		pXMLNODE TaskQueryNode = tempParser.CreateNodePtr(qualityParamNode,"SpectrumQueryIndex");
		parser.GetAttrNode(childNode,"TaskID",TaskID);				     			  //��ȡԭʼXML��QualityTaskSet�ڵ��TaskID����
		parser.GetAttrNode(childNode,"Action",Action);                                //��ȡԭʼXML��QualityTaskSet�ڵ��Action����
		parser.GetAttrNode(SpecNode,"EquCode",EquCode);
		parser.GetAttrNode(SpecNode,"StartFreq",StartFreq);
		parser.GetAttrNode(SpecNode,"EndFreq",EndFreq);
		parser.GetAttrNode(SpecNode,"StepFreq",StepFreq);
		parser.GetAttrNode(SpecNode,"TaskType",TaskType);
		parser.GetAttrNode(SpecNode,"IsFreqScan",IsFreqScan);
		parser.GetAttrNode(SpecNode,"SpectrumAnalyse",SpectrumAnalyse);
		parser.GetAttrNode(SpecNode,"ReferDbu",ReferDbu);
		parser.GetAttrNode(SpecNode,"VBW",VBW);
		parser.GetAttrNode(SpecNode,"RBW",RBW);

		tempParser.SetAttrNode("TaskID",TaskID,qualityTaskNode);                              //����QualityTask�ڵ��TaskID����
		tempParser.SetAttrNode("Action",Action,qualityTaskNode);                              //����QualityTask�ڵ��Action����
		tempParser.SetAttrNode("EquCode",EquCode,TaskQueryNode);  
		tempParser.SetAttrNode("StartFreq",StartFreq,TaskQueryNode);  
		tempParser.SetAttrNode("EndFreq",EndFreq,TaskQueryNode);  
		tempParser.SetAttrNode("StepFreq",StepFreq,TaskQueryNode);  
		tempParser.SetAttrNode("TaskType",TaskType,TaskQueryNode);
		tempParser.SetAttrNode("IsFreqScan",IsFreqScan,TaskQueryNode); 
		tempParser.SetAttrNode("SpectrumAnalyse",SpectrumAnalyse,TaskQueryNode );
		tempParser.SetAttrNode("ReferDbu",ReferDbu,TaskQueryNode) ;
		tempParser.SetAttrNode("VBW",VBW,TaskQueryNode );
		tempParser.SetAttrNode("RBW",RBW,TaskQueryNode );
		
		tempParser.SaveToString(tmpstr);

		//����timeIndexVct�е�TimeIndex�ڵ���Ϣ��������
		size_t pos4=tmpstr.find("</TaskParam>");
		if (pos4>0)
		{
			std::string timeParam = "<TimeParam>"+ timeIndexVct[p]+"</TimeParam>";
			tmpstr.insert(pos4+12,timeParam);
			strStandardXMLVec.push_back(tmpstr);
		}
	}
	return true;
}

bool TranslateDownXMLForATV::TranslateSpectrumQueryTask(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser retparser(source);
	std::string strVersion,strMsgID,strType,strProtocol,strSetDateTime,strSrcCode,strDstCode,strSrcURL,strPriority;
	pXMLNODE rootNode = parser.GetRootNode();         //��ȡԭʼXML��Ԫ��
	parser.GetAttrNode(rootNode,"Version",strVersion);         //��ȡMsg�ڵ��Version����ֵ
	parser.GetAttrNode(rootNode,"MsgID",strMsgID);		       //��ȡMsg�ڵ��MsgID����ֵ
	parser.GetAttrNode(rootNode,"Type",strType);		       //��ȡMsg�ڵ��Type����ֵ
	parser.GetAttrNode(rootNode,"Protocol",strProtocol);       //��ȡMsg�ڵ��protocol����ֵ
	parser.GetAttrNode(rootNode,"DateTime",strSetDateTime);    //��ȡMsg�ڵ��DateTime����ֵ��
	parser.GetAttrNode(rootNode,"SrcCode",strSrcCode);         //��ȡMsg�ڵ��StrCode����ֵ
	parser.GetAttrNode(rootNode,"DstCode",strDstCode);         //��ȡMsg�ڵ��DstCode����ֵ
	parser.GetAttrNode(rootNode,"SrcURL",strSrcURL);		   //��ȡMsg�ڵ��SrcUrl����ֵ	   
	parser.GetAttrNode(rootNode,"Priority",strPriority);       //��ȡMsg�ڵ��Priority����ֵ


	pXMLNODE retRootNode = retparser.GetRootNode(); 
	retparser.SetAttrNode("Version",strVersion,retRootNode);                   //����Msg�ڵ��Version����ֵ
	retparser.SetAttrNode("MsgID",strMsgID,retRootNode);                       //����Msg�ڵ��MsgID����ֵ
	retparser.SetAttrNode("DVBType",string("ATV"),retRootNode);               //����Msg�ڵ��DVBType����ֵ
	retparser.SetAttrNode("TaskType",string("SpectrumQuery"),retRootNode);   //����Msg�ڵ��TaskType����ֵ
	retparser.SetAttrNode("Protocol",strProtocol,retRootNode);                 //����Msg�ڵ��Protocol����ֵ
	retparser.SetAttrNode("DateTime",strSetDateTime,retRootNode);              //����Msg�ڵ��DateTime����ֵ
	retparser.SetAttrNode("SrcCode",strSrcCode,retRootNode);                   //����Msg�ڵ��StrCode����ֵ
	retparser.SetAttrNode("DstCode",strDstCode,retRootNode);                   //����Msg�ڵ��DstCode����ֵ
	retparser.SetAttrNode("SrcURL",strSrcURL,retRootNode);                     //����Msg�ڵ��SrcUrl����ֵ
	retparser.SetAttrNode("Priority",strPriority,retRootNode);   

	pXMLNODE QueryNode=parser.GetNodeFirstChild(rootNode);
	std::string EquCode,TaskID,StartDateTime,EndDateTime;
	parser.GetAttrNode(QueryNode,"EquCode",EquCode);
	parser.GetAttrNode(QueryNode,"TaskID",TaskID);
	parser.GetAttrNode(QueryNode,"StartDateTime",StartDateTime);
	parser.GetAttrNode(QueryNode,"EndDateTime",EndDateTime);

	pXMLNODE retQueryNode =retparser.CreateNodePtr(retRootNode,"SpectrumQuery");

	retparser.SetAttrNode("EquCode",EquCode,retQueryNode);
	retparser.SetAttrNode("TaskID",TaskID,retQueryNode);
	retparser.SetAttrNode("StartDateTime",StartDateTime,retQueryNode);
	retparser.SetAttrNode("EndDateTime",EndDateTime,retQueryNode);

	retparser.SaveToString(strStandardXML);
	return true;
}

bool TranslateDownXMLForATV::TranslateQualityQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser retparser(source);
	std::string strVersion,strMsgID,strType,strProtocol,strSetDateTime,strSrcCode,strDstCode,strSrcURL,strPriority;
	pXMLNODE rootNode = parser.GetRootNode();         //��ȡԭʼXML��Ԫ��
	parser.GetAttrNode(rootNode,"Version",strVersion);         //��ȡMsg�ڵ��Version����ֵ
	parser.GetAttrNode(rootNode,"MsgID",strMsgID);		       //��ȡMsg�ڵ��MsgID����ֵ
	parser.GetAttrNode(rootNode,"Type",strType);		       //��ȡMsg�ڵ��Type����ֵ
	parser.GetAttrNode(rootNode,"Protocol",strProtocol);       //��ȡMsg�ڵ��protocol����ֵ
	parser.GetAttrNode(rootNode,"DateTime",strSetDateTime);    //��ȡMsg�ڵ��DateTime����ֵ��
	parser.GetAttrNode(rootNode,"SrcCode",strSrcCode);         //��ȡMsg�ڵ��StrCode����ֵ
	parser.GetAttrNode(rootNode,"DstCode",strDstCode);         //��ȡMsg�ڵ��DstCode����ֵ
	parser.GetAttrNode(rootNode,"SrcURL",strSrcURL);		   //��ȡMsg�ڵ��SrcUrl����ֵ	   
	parser.GetAttrNode(rootNode,"Priority",strPriority);       //��ȡMsg�ڵ��Priority����ֵ


	pXMLNODE retRootNode = retparser.GetRootNode(); 
	retparser.SetAttrNode("Version",strVersion,retRootNode);                   //����Msg�ڵ��Version����ֵ
	retparser.SetAttrNode("MsgID",strMsgID,retRootNode);                       //����Msg�ڵ��MsgID����ֵ
	retparser.SetAttrNode("DVBType",string("ATV"),retRootNode);               //����Msg�ڵ��DVBType����ֵ
	retparser.SetAttrNode("TaskType",string("QualityQuery"),retRootNode);   //����Msg�ڵ��TaskType����ֵ
	retparser.SetAttrNode("Protocol",strProtocol,retRootNode);                 //����Msg�ڵ��Protocol����ֵ
	retparser.SetAttrNode("DateTime",strSetDateTime,retRootNode);              //����Msg�ڵ��DateTime����ֵ
	retparser.SetAttrNode("SrcCode",strSrcCode,retRootNode);                   //����Msg�ڵ��StrCode����ֵ
	retparser.SetAttrNode("DstCode",strDstCode,retRootNode);                   //����Msg�ڵ��DstCode����ֵ
	retparser.SetAttrNode("SrcURL",strSrcURL,retRootNode);                     //����Msg�ڵ��SrcUrl����ֵ
	retparser.SetAttrNode("Priority",strPriority,retRootNode);   

	pXMLNODE QueryNode=parser.GetNodeFirstChild(rootNode);
	std::string EquCode,TaskID,StartDateTime,EndDateTime,ReportTime,ChannelCode,Freq;
	parser.GetAttrNode(QueryNode,"EquCode",EquCode);
	parser.GetAttrNode(QueryNode,"TaskID",TaskID);
	parser.GetAttrNode(QueryNode,"StartDateTime",StartDateTime);
	parser.GetAttrNode(QueryNode,"EndDateTime",EndDateTime);
	parser.GetAttrNode(QueryNode,"ReportTime",ReportTime);
	parser.GetAttrNode(QueryNode,"ChannelCode",ChannelCode);
	parser.GetAttrNode(QueryNode,"Freq",Freq);

	pXMLNODE retNode =retparser.CreateNodePtr(retRootNode,"QualityQuery");
	pXMLNODE retQueryNode =retparser.CreateNodePtr(retNode,"Quality");
	retparser.SetAttrNode("EquCode",EquCode,retQueryNode);
	retparser.SetAttrNode("TaskID",TaskID,retQueryNode);
	retparser.SetAttrNode("StartDateTime",StartDateTime,retQueryNode);
	retparser.SetAttrNode("EndDateTime",EndDateTime,retQueryNode);
	retparser.SetAttrNode("ReportTime",ReportTime,retQueryNode);
	retparser.SetAttrNode("ChannelCode",ChannelCode,retQueryNode);
	retparser.SetAttrNode("Freq",Freq,retQueryNode);
	retparser.SaveToString(strStandardXML);
	return true;
}

bool TranslateDownXMLForATV::TranslateClientInfoQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser retparser(source);
	std::string strVersion,strMsgID,strType,strProtocol,strSetDateTime,strSrcCode,strDstCode,strSrcURL,strPriority;
	pXMLNODE rootNode = parser.GetRootNode();         //��ȡԭʼXML��Ԫ��
	parser.GetAttrNode(rootNode,"Version",strVersion);         //��ȡMsg�ڵ��Version����ֵ
	parser.GetAttrNode(rootNode,"MsgID",strMsgID);		       //��ȡMsg�ڵ��MsgID����ֵ
	parser.GetAttrNode(rootNode,"Type",strType);		       //��ȡMsg�ڵ��Type����ֵ
	parser.GetAttrNode(rootNode,"Protocol",strProtocol);       //��ȡMsg�ڵ��protocol����ֵ
	parser.GetAttrNode(rootNode,"DateTime",strSetDateTime);    //��ȡMsg�ڵ��DateTime����ֵ��
	parser.GetAttrNode(rootNode,"SrcCode",strSrcCode);         //��ȡMsg�ڵ��StrCode����ֵ
	parser.GetAttrNode(rootNode,"DstCode",strDstCode);         //��ȡMsg�ڵ��DstCode����ֵ
	parser.GetAttrNode(rootNode,"SrcURL",strSrcURL);		   //��ȡMsg�ڵ��SrcUrl����ֵ	   
	parser.GetAttrNode(rootNode,"Priority",strPriority);       //��ȡMsg�ڵ��Priority����ֵ


	pXMLNODE retRootNode = retparser.GetRootNode(); 
	retparser.SetAttrNode("Version",strVersion,retRootNode);                   //����Msg�ڵ��Version����ֵ
	retparser.SetAttrNode("MsgID",strMsgID,retRootNode);                       //����Msg�ڵ��MsgID����ֵ
	retparser.SetAttrNode("DVBType",string("ATV"),retRootNode);               //����Msg�ڵ��DVBType����ֵ
	retparser.SetAttrNode("TaskType",string("ClientInfoQuery"),retRootNode);   //����Msg�ڵ��TaskType����ֵ
	retparser.SetAttrNode("Protocol",strProtocol,retRootNode);                 //����Msg�ڵ��Protocol����ֵ
	retparser.SetAttrNode("DateTime",strSetDateTime,retRootNode);              //����Msg�ڵ��DateTime����ֵ
	retparser.SetAttrNode("SrcCode",strSrcCode,retRootNode);                   //����Msg�ڵ��StrCode����ֵ
	retparser.SetAttrNode("DstCode",strDstCode,retRootNode);                   //����Msg�ڵ��DstCode����ֵ
	retparser.SetAttrNode("SrcURL",strSrcURL,retRootNode);                     //����Msg�ڵ��SrcUrl����ֵ
	retparser.SetAttrNode("Priority",strPriority,retRootNode);  
	std::string Index;
	pXMLNODE ClientNode =parser.GetNodeFromPath("Msg/ClientInfoQuery/HeadendInfo");
	parser.GetAttrNode(ClientNode,"Index",Index);

	pXMLNODE retClientInfoNode = retparser.CreateNodePtr(retRootNode,"ClientInfoQuery");

	pXMLNODE retClientNode = retparser.CreateNodePtr(retClientInfoNode,"HeadendInfo");

	retparser.SetAttrNode("Index",Index,retClientNode);

	retparser.SaveToString(strStandardXML);	
	return true;
}
//���ر�׼������ͼ��������xml
bool TranslateDownXMLForATV::TranslateRunPlanSet(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><RunPlanSet></RunPlanSet></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//��ȡ��Ԫ��
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	string freq,equcode,stdtype,rate;
	string tasktype="RunPlanSet";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="TVMonDown")
	{
		downtype="ATV";
	}
	else
	{
		return false;
	}

	temParser.SetAttrNode("Version",version,stdRootNode);
	temParser.SetAttrNode("MsgID",msgid,stdRootNode);
	temParser.SetAttrNode("DVBType",downtype,stdRootNode);
	temParser.SetAttrNode("TaskType",tasktype,stdRootNode);
	temParser.SetAttrNode("Protocol",protocol,stdRootNode);
	temParser.SetAttrNode("DateTime",datetime,stdRootNode);
	temParser.SetAttrNode("SrcCode",srccode,stdRootNode);
	temParser.SetAttrNode("DstCode",dstcode,stdRootNode);
	temParser.SetAttrNode("SrcURL",srcurl,stdRootNode);
	temParser.SetAttrNode("Priority",priority,stdRootNode);

	pXMLNODE retRunNode = temParser.GetNodeFromPath("Msg/RunPlanSet");

	pXMLNODE childNode=parser.GetNodeFirstChild(orgRootNode);	//runplanset node
	pXMLNODELIST ChannelList = parser.GetNodeList(childNode);	//runplanchannel node list
	for(int count=0;count<parser.GetChildCount(childNode);count++)
	{
		pXMLNODE ChannelIndexNode =parser.GetNextNode(ChannelList);

		std::string EquCode,ChannelCode;
		parser.GetAttrNode(ChannelIndexNode,"EquCode",EquCode);
		parser.GetAttrNode(ChannelIndexNode,"ChCode",ChannelCode);

		/**	���ܻ��ж��ʱ����Ϣ��ͨ��ѭ��ȥ��ȡ���ֱ𴴽�RunPlanChannel�ڵ�
		*/
		pXMLNODELIST timeParamlist = parser.GetNodeList( ChannelIndexNode );
		int timeParamCount = parser.GetChildCount( ChannelIndexNode );
		for (int timeIndex=0; timeIndex<timeParamCount; timeIndex++)
		{
			//��ȡCycleWorkTime����SingleWorkTime�ڵ�
			pXMLNODE timeParamNode = parser.GetNextNode( timeParamlist );	

			std::string DayOfWeek,StartTime,EndTime,ValidStartDateTime,ValidEndDateTime,StartDateTime,EndDateTime;
			parser.GetAttrNode(timeParamNode,"DayOfWeek",DayOfWeek);
			parser.GetAttrNode(timeParamNode,"StartTime",StartTime);
			parser.GetAttrNode(timeParamNode,"EndTime",EndTime);
			parser.GetAttrNode(timeParamNode,"StartDateTime",StartDateTime);
			parser.GetAttrNode(timeParamNode,"EndDateTime",EndDateTime);
			parser.GetAttrNode(timeParamNode,"ValidStartDateTime",ValidStartDateTime);
			parser.GetAttrNode(timeParamNode,"ValidEndDateTime",ValidEndDateTime);

			pXMLNODE retIndexNode = temParser.CreateNodePtr(retRunNode,"RunPlanChannel");
			temParser.SetAttrNode("EquCode",EquCode,retIndexNode);
			temParser.SetAttrNode("ChannelCode",ChannelCode,retIndexNode);
			temParser.SetAttrNode("DayOfWeek",DayOfWeek,retIndexNode);
			temParser.SetAttrNode("StartTime",StartTime,retIndexNode);
			temParser.SetAttrNode("EndTime",EndTime,retIndexNode);
			temParser.SetAttrNode("StartDateTime",StartDateTime,retIndexNode);
			temParser.SetAttrNode("EndDateTime",EndDateTime,retIndexNode);
			temParser.SetAttrNode("ValidStartDateTime",ValidStartDateTime,retIndexNode);
			temParser.SetAttrNode("ValidEndDateTime",ValidEndDateTime,retIndexNode);
		}
	}

	//���浽�ַ���
	temParser.SaveToString( strStandardXML);
	return true;
}
bool TranslateDownXMLForATV::TranslateStopClient(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser retparser(source);
	std::string strVersion,strMsgID,strType,strProtocol,strSetDateTime,strSrcCode,strDstCode,strSrcURL,strPriority;
	pXMLNODE rootNode = parser.GetRootNode();					//��ȡԭʼXML��Ԫ��
	parser.GetAttrNode(rootNode,"Version",strVersion);         //��ȡMsg�ڵ��Version����ֵ
	parser.GetAttrNode(rootNode,"MsgID",strMsgID);		       //��ȡMsg�ڵ��MsgID����ֵ
	parser.GetAttrNode(rootNode,"Type",strType);		       //��ȡMsg�ڵ��Type����ֵ
	parser.GetAttrNode(rootNode,"Protocol",strProtocol);       //��ȡMsg�ڵ��protocol����ֵ
	parser.GetAttrNode(rootNode,"DateTime",strSetDateTime);    //��ȡMsg�ڵ��DateTime����ֵ��
	parser.GetAttrNode(rootNode,"SrcCode",strSrcCode);         //��ȡMsg�ڵ��StrCode����ֵ
	parser.GetAttrNode(rootNode,"DstCode",strDstCode);         //��ȡMsg�ڵ��DstCode����ֵ
	parser.GetAttrNode(rootNode,"SrcURL",strSrcURL);		   //��ȡMsg�ڵ��SrcUrl����ֵ	   
	parser.GetAttrNode(rootNode,"Priority",strPriority);       //��ȡMsg�ڵ��Priority����ֵ

	pXMLNODE retRootNode = retparser.GetRootNode(); 
	retparser.SetAttrNode("Version",strVersion,retRootNode);                   //����Msg�ڵ��Version����ֵ
	retparser.SetAttrNode("MsgID",strMsgID,retRootNode);                       //����Msg�ڵ��MsgID����ֵ
	retparser.SetAttrNode("DVBType",string("ATV"),retRootNode);               //����Msg�ڵ��DVBType����ֵ
	retparser.SetAttrNode("TaskType",string("StopClient"),retRootNode);			//����Msg�ڵ��TaskType����ֵ
	retparser.SetAttrNode("Protocol",strProtocol,retRootNode);                 //����Msg�ڵ��Protocol����ֵ
	retparser.SetAttrNode("DateTime",strSetDateTime,retRootNode);              //����Msg�ڵ��DateTime����ֵ
	retparser.SetAttrNode("SrcCode",strSrcCode,retRootNode);                   //����Msg�ڵ��StrCode����ֵ
	retparser.SetAttrNode("DstCode",strDstCode,retRootNode);                   //����Msg�ڵ��DstCode����ֵ
	retparser.SetAttrNode("SrcURL",strSrcURL,retRootNode);                     //����Msg�ڵ��SrcUrl����ֵ
	retparser.SetAttrNode("Priority",strPriority,retRootNode);   

	pXMLNODE QueryNode=parser.GetNodeFirstChild(rootNode);
	std::string EquCode,URL;
	parser.GetAttrNode(QueryNode,"EquCode",EquCode);
	parser.GetAttrNode(QueryNode,"URl",URL);

	pXMLNODE retNode =retparser.CreateNodePtr(retRootNode,"StopClient");

	retparser.SetAttrNode("EquCode",EquCode,retNode);
	retparser.SetAttrNode("URL",URL,retNode);

	retparser.SaveToString(strStandardXML);
	return true;
}

bool TranslateDownXMLForATV::TranslateTaskSchedulerQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser retparser(source);
	std::string strVersion,strMsgID,strType,strProtocol,strSetDateTime,strSrcCode,strDstCode,strSrcURL,strPriority;
	pXMLNODE rootNode = parser.GetRootNode();         //��ȡԭʼXML��Ԫ��
	parser.GetAttrNode(rootNode,"Version",strVersion);         //��ȡMsg�ڵ��Version����ֵ
	parser.GetAttrNode(rootNode,"MsgID",strMsgID);		       //��ȡMsg�ڵ��MsgID����ֵ
	parser.GetAttrNode(rootNode,"Type",strType);		       //��ȡMsg�ڵ��Type����ֵ
	parser.GetAttrNode(rootNode,"Protocol",strProtocol);       //��ȡMsg�ڵ��protocol����ֵ
	parser.GetAttrNode(rootNode,"DateTime",strSetDateTime);    //��ȡMsg�ڵ��DateTime����ֵ��
	parser.GetAttrNode(rootNode,"SrcCode",strSrcCode);         //��ȡMsg�ڵ��StrCode����ֵ
	parser.GetAttrNode(rootNode,"DstCode",strDstCode);         //��ȡMsg�ڵ��DstCode����ֵ
	parser.GetAttrNode(rootNode,"SrcURL",strSrcURL);		   //��ȡMsg�ڵ��SrcUrl����ֵ	   
	parser.GetAttrNode(rootNode,"Priority",strPriority);       //��ȡMsg�ڵ��Priority����ֵ


	pXMLNODE retRootNode = retparser.GetRootNode(); 
	retparser.SetAttrNode("Version",strVersion,retRootNode);                   //����Msg�ڵ��Version����ֵ
	retparser.SetAttrNode("MsgID",strMsgID,retRootNode);                       //����Msg�ڵ��MsgID����ֵ
	retparser.SetAttrNode("DVBType",string("ATV"),retRootNode);               //����Msg�ڵ��DVBType����ֵ
	retparser.SetAttrNode("TaskType",string("TaskSchedulerQuery"),retRootNode);   //����Msg�ڵ��TaskType����ֵ
	retparser.SetAttrNode("Protocol",strProtocol,retRootNode);                 //����Msg�ڵ��Protocol����ֵ
	retparser.SetAttrNode("DateTime",strSetDateTime,retRootNode);              //����Msg�ڵ��DateTime����ֵ
	retparser.SetAttrNode("SrcCode",strSrcCode,retRootNode);                   //����Msg�ڵ��StrCode����ֵ
	retparser.SetAttrNode("DstCode",strDstCode,retRootNode);                   //����Msg�ڵ��DstCode����ֵ
	retparser.SetAttrNode("SrcURL",strSrcURL,retRootNode);                     //����Msg�ڵ��SrcUrl����ֵ
	retparser.SetAttrNode("Priority",strPriority,retRootNode);  

	std::string EquCode,StartDateTime,EndDateTime;
	pXMLNODE QueryNode =parser.GetNodeFromPath("Msg/TaskSchedulerQuery");
	parser.GetAttrNode(QueryNode,"EquCode",EquCode);
	parser.GetAttrNode(QueryNode,"StartDateTime",StartDateTime);
	parser.GetAttrNode(QueryNode,"EndDateTime",EndDateTime);

	pXMLNODE retQueryNode = retparser.CreateNodePtr(retRootNode,"TaskSchedulerQuery");

	retparser.SetAttrNode("EquCode",EquCode,retQueryNode);
	retparser.SetAttrNode("StartDateTime",StartDateTime,retQueryNode);
	retparser.SetAttrNode("EndDateTime",EndDateTime,retQueryNode);

	retparser.SaveToString(strStandardXML);	
	return true;
}

bool TranslateDownXMLForATV::TranslateManualTaskSchedulerQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser retparser(source);
	std::string strVersion,strMsgID,strType,strProtocol,strSetDateTime,strSrcCode,strDstCode,strSrcURL,strPriority;
	pXMLNODE rootNode = parser.GetRootNode();         //��ȡԭʼXML��Ԫ��
	parser.GetAttrNode(rootNode,"Version",strVersion);         //��ȡMsg�ڵ��Version����ֵ
	parser.GetAttrNode(rootNode,"MsgID",strMsgID);		       //��ȡMsg�ڵ��MsgID����ֵ
	parser.GetAttrNode(rootNode,"Type",strType);		       //��ȡMsg�ڵ��Type����ֵ
	parser.GetAttrNode(rootNode,"Protocol",strProtocol);       //��ȡMsg�ڵ��protocol����ֵ
	parser.GetAttrNode(rootNode,"DateTime",strSetDateTime);    //��ȡMsg�ڵ��DateTime����ֵ��
	parser.GetAttrNode(rootNode,"SrcCode",strSrcCode);         //��ȡMsg�ڵ��StrCode����ֵ
	parser.GetAttrNode(rootNode,"DstCode",strDstCode);         //��ȡMsg�ڵ��DstCode����ֵ
	parser.GetAttrNode(rootNode,"SrcURL",strSrcURL);		   //��ȡMsg�ڵ��SrcUrl����ֵ	   
	parser.GetAttrNode(rootNode,"Priority",strPriority);       //��ȡMsg�ڵ��Priority����ֵ


	pXMLNODE retRootNode = retparser.GetRootNode(); 
	retparser.SetAttrNode("Version",strVersion,retRootNode);                   //����Msg�ڵ��Version����ֵ
	retparser.SetAttrNode("MsgID",strMsgID,retRootNode);                       //����Msg�ڵ��MsgID����ֵ
	retparser.SetAttrNode("DVBType",string("ATV"),retRootNode);               //����Msg�ڵ��DVBType����ֵ
	retparser.SetAttrNode("TaskType",string("ManualTaskSchedulerQuery"),retRootNode);   //����Msg�ڵ��TaskType����ֵ
	retparser.SetAttrNode("Protocol",strProtocol,retRootNode);                 //����Msg�ڵ��Protocol����ֵ
	retparser.SetAttrNode("DateTime",strSetDateTime,retRootNode);              //����Msg�ڵ��DateTime����ֵ
	retparser.SetAttrNode("SrcCode",strSrcCode,retRootNode);                   //����Msg�ڵ��StrCode����ֵ
	retparser.SetAttrNode("DstCode",strDstCode,retRootNode);                   //����Msg�ڵ��DstCode����ֵ
	retparser.SetAttrNode("SrcURL",strSrcURL,retRootNode);                     //����Msg�ڵ��SrcUrl����ֵ
	retparser.SetAttrNode("Priority",strPriority,retRootNode);  

	std::string EquCode,StartDateTime,EndDateTime;
	pXMLNODE QueryNode =parser.GetNodeFromPath("Msg/ManualTaskSchedulerQuery");
	parser.GetAttrNode(QueryNode,"EquCode",EquCode);
	parser.GetAttrNode(QueryNode,"StartDateTime",StartDateTime);
	parser.GetAttrNode(QueryNode,"EndDateTime",EndDateTime);

	pXMLNODE retQueryNode = retparser.CreateNodePtr(retRootNode,"MualTaskSchedulerQuery");

	retparser.SetAttrNode("EquCode",EquCode,retQueryNode);
	retparser.SetAttrNode("StartDateTime",StartDateTime,retQueryNode);
	retparser.SetAttrNode("EndDateTime",EndDateTime,retQueryNode);

	retparser.SaveToString(strStandardXML);	
	return true;
}

bool TranslateDownXMLForATV::TranslateEquipmentAlarmParamSet(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><AlarmParamSet></AlarmParamSet></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser retparser(source);
	std::string strVersion,strMsgID,strType,strProtocol,strSetDateTime,strSrcCode,strDstCode,strSrcURL,strPriority;
	pXMLNODE rootNode = parser.GetRootNode();         //��ȡԭʼXML��Ԫ��
	parser.GetAttrNode(rootNode,"Version",strVersion);         //��ȡMsg�ڵ��Version����ֵ
	parser.GetAttrNode(rootNode,"MsgID",strMsgID);		       //��ȡMsg�ڵ��MsgID����ֵ
	parser.GetAttrNode(rootNode,"Type",strType);		       //��ȡMsg�ڵ��Type����ֵ
	parser.GetAttrNode(rootNode,"Protocol",strProtocol);       //��ȡMsg�ڵ��protocol����ֵ
	parser.GetAttrNode(rootNode,"DateTime",strSetDateTime);    //��ȡMsg�ڵ��DateTime����ֵ��
	parser.GetAttrNode(rootNode,"SrcCode",strSrcCode);         //��ȡMsg�ڵ��StrCode����ֵ
	parser.GetAttrNode(rootNode,"DstCode",strDstCode);         //��ȡMsg�ڵ��DstCode����ֵ
	parser.GetAttrNode(rootNode,"SrcURL",strSrcURL);		   //��ȡMsg�ڵ��SrcUrl����ֵ	   
	parser.GetAttrNode(rootNode,"Priority",strPriority);       //��ȡMsg�ڵ��Priority����ֵ


	pXMLNODE retRootNode = retparser.GetRootNode(); 
	retparser.SetAttrNode("Version",strVersion,retRootNode);                   //����Msg�ڵ��Version����ֵ
	retparser.SetAttrNode("MsgID",strMsgID,retRootNode);                       //����Msg�ڵ��MsgID����ֵ
	retparser.SetAttrNode("DVBType",string("ATV"),retRootNode);               //����Msg�ڵ��DVBType����ֵ
	retparser.SetAttrNode("TaskType",string("AlarmParamSet"),retRootNode);   //����Msg�ڵ��TaskType����ֵ
	retparser.SetAttrNode("Protocol",strProtocol,retRootNode);                 //����Msg�ڵ��Protocol����ֵ
	retparser.SetAttrNode("DateTime",strSetDateTime,retRootNode);              //����Msg�ڵ��DateTime����ֵ
	retparser.SetAttrNode("SrcCode",strSrcCode,retRootNode);                   //����Msg�ڵ��StrCode����ֵ
	retparser.SetAttrNode("DstCode",strDstCode,retRootNode);                   //����Msg�ڵ��DstCode����ֵ
	retparser.SetAttrNode("SrcURL",strSrcURL,retRootNode);                     //����Msg�ڵ��SrcUrl����ֵ
	retparser.SetAttrNode("Priority",strPriority,retRootNode);  


	pXMLNODE alarmset=retparser.GetNodeFromPath("Msg/AlarmParamSet");
	pXMLNODE EquipAlarmSet=parser.GetNodeFromPath("Msg/EquipmentAlarmParamSet");
	int count=parser.GetChildCount(EquipAlarmSet);
	pXMLNODELIST AlarmList=parser.GetNodeList(EquipAlarmSet);
	
	for(int i=0;i<count;i++)
	{
		string Type,Desc,UpThreshold,DownThreshold,TimeThreshold,Switch;
		pXMLNODE childNode=parser.GetNextNode(AlarmList);
		parser.GetAttrNode(childNode,"type",Type);
		parser.GetAttrNode(childNode,"Desc",Desc);
		parser.GetAttrNode(childNode,"UpThreshold",UpThreshold);
		parser.GetAttrNode(childNode,"DownThreshold",DownThreshold);
		parser.GetAttrNode(childNode,"TimeThreshold",TimeThreshold);
		parser.GetAttrNode(childNode,"Switch",Switch);
		if(Switch.length()<1)
			Switch="1";

		StrUtil::ConvertEquipTypeIn(Type);
		pXMLNODE AlarmParamNode=retparser.CreateNodePtr(alarmset,"AlarmParam");
		retparser.SetAttrNode("Type",Type,AlarmParamNode);
		retparser.SetAttrNode("Desc",Desc,AlarmParamNode);
		retparser.SetAttrNode("UpThreshold",UpThreshold,AlarmParamNode);
		retparser.SetAttrNode("DownThreshold",DownThreshold,AlarmParamNode);
		retparser.SetAttrNode("TimeThreshold",TimeThreshold,AlarmParamNode);
		retparser.SetAttrNode("Switch",Switch,AlarmParamNode);

	}
	retparser.SaveToString(strStandardXML);
	return true;
}


bool TranslateDownXMLForATV::TranslateInfoCheckRequest(const std::string& strOriginalXml, std::string& strStandardXml)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><InfoCheckRequest></InfoCheckRequest></Msg>";

	XmlParser parser(strOriginalXml.c_str());
	XmlParser retparser(source);

	std::string strVersion,strMsgID,strType,strProtocol,strSetDateTime,strSrcCode,strDstCode,strSrcURL,strPriority;
	pXMLNODE rootNode = parser.GetRootNode();         
	parser.GetAttrNode(rootNode,"Version",strVersion);         
	parser.GetAttrNode(rootNode,"MsgID",strMsgID);		       
	parser.GetAttrNode(rootNode,"Type",strType);		       
	parser.GetAttrNode(rootNode,"Protocol",strProtocol);       
	parser.GetAttrNode(rootNode,"DateTime",strSetDateTime);    
	parser.GetAttrNode(rootNode,"SrcCode",strSrcCode);         
	parser.GetAttrNode(rootNode,"DstCode",strDstCode);         
	parser.GetAttrNode(rootNode,"SrcURL",strSrcURL);		      
	parser.GetAttrNode(rootNode,"Priority",strPriority);      


	pXMLNODE retRootNode = retparser.GetRootNode(); 
	retparser.SetAttrNode("Version",strVersion,retRootNode);                   
	retparser.SetAttrNode("MsgID",strMsgID,retRootNode);                       
	retparser.SetAttrNode("DVBType",string("ATV"),retRootNode);              
	retparser.SetAttrNode("TaskType",string("InfoCheckRequest"),retRootNode);  
	retparser.SetAttrNode("Protocol",strProtocol,retRootNode);                 
	retparser.SetAttrNode("DateTime",strSetDateTime,retRootNode);              
	retparser.SetAttrNode("SrcCode",strSrcCode,retRootNode);                   
	retparser.SetAttrNode("DstCode",strDstCode,retRootNode);                   
	retparser.SetAttrNode("SrcURL",strSrcURL,retRootNode);                     
	retparser.SetAttrNode("Priority",strPriority,retRootNode); 

	pXMLNODE oriInfoCheckNode = parser.GetNodeFromPath("Msg/InfoCheckRequest");
	pXMLNODE InfoCheckNode = retparser.GetNodeFromPath("Msg/InfoCheckRequest");

	string taskType;
	parser.GetAttrNode(oriInfoCheckNode, "TaskType", taskType);
	retparser.SetAttrNode("TaskType", taskType, InfoCheckNode);

	retparser.SaveToString(strStandardXml);
	return true;
}


bool TranslateDownXMLForATV::TranslateRunPlanCheck(const std::string& strOriginalXml, std::string& strStandardXml)
{
	char* source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><RunPlanCheckRequest></RunPlanCheckRequest></Msg>";

	XmlParser orgParser( strOriginalXml.c_str() );
	XmlParser stdParser(source);
	pXMLNODE oriMsgNode = orgParser.GetRootNode();
	pXMLNODE stdMsgNode = stdParser.GetRootNode();

	std::string strVersion,strMsgID,strType,strProtocol,strSetDateTime,strSrcCode,strDstCode,strSrcURL,strPriority;        
	orgParser.GetAttrNode(oriMsgNode,"Version",strVersion);         
	orgParser.GetAttrNode(oriMsgNode,"MsgID",strMsgID);		       
	orgParser.GetAttrNode(oriMsgNode,"Type",strType);		       
	orgParser.GetAttrNode(oriMsgNode,"Protocol",strProtocol);       
	orgParser.GetAttrNode(oriMsgNode,"DateTime",strSetDateTime);    
	orgParser.GetAttrNode(oriMsgNode,"SrcCode",strSrcCode);         
	orgParser.GetAttrNode(oriMsgNode,"DstCode",strDstCode);         
	orgParser.GetAttrNode(oriMsgNode,"SrcURL",strSrcURL);		      
	orgParser.GetAttrNode(oriMsgNode,"Priority",strPriority);      

	stdParser.SetAttrNode("Version",strVersion,stdMsgNode);                   
	stdParser.SetAttrNode("MsgID",strMsgID,stdMsgNode);                       
	stdParser.SetAttrNode("DVBType",string("ATV"),stdMsgNode);              
	stdParser.SetAttrNode("TaskType",string("RunPlanCheckRequest"),stdMsgNode);  
	stdParser.SetAttrNode("Protocol",strProtocol,stdMsgNode);                 
	stdParser.SetAttrNode("DateTime",strSetDateTime,stdMsgNode);              
	stdParser.SetAttrNode("SrcCode",strSrcCode,stdMsgNode);                   
	stdParser.SetAttrNode("DstCode",strDstCode,stdMsgNode);                   
	stdParser.SetAttrNode("SrcURL",strSrcURL,stdMsgNode);                     
	stdParser.SetAttrNode("Priority",strPriority,stdMsgNode);

	pXMLNODE orgCheckNode = orgParser.GetNodeFromPath("Msg/RunPlanCheckRequest");
	pXMLNODE stdCheckNode = stdParser.GetNodeFromPath("Msg/RunPlanCheckRequest");

	string strEquCode = "";
	orgParser.GetAttrNode(orgCheckNode, "EquCode", strEquCode);
	stdParser.SetAttrNode("EquCode", strEquCode, stdCheckNode);

	stdParser.SaveToString( strStandardXml );
	return true;
}

bool TranslateDownXMLForATV::TranslateAlarmExpireTask(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser retparser(source);
	std::string strVersion,strMsgID,strType,strProtocol,strSetDateTime,strSrcCode,strDstCode,strSrcURL,strPriority;
	pXMLNODE rootNode = parser.GetRootNode();         //��ȡԭʼXML��Ԫ��
	parser.GetAttrNode(rootNode,"Version",strVersion);         //��ȡMsg�ڵ��Version����ֵ
	parser.GetAttrNode(rootNode,"MsgID",strMsgID);		       //��ȡMsg�ڵ��MsgID����ֵ
	parser.GetAttrNode(rootNode,"Type",strType);		       //��ȡMsg�ڵ��Type����ֵ
	parser.GetAttrNode(rootNode,"Protocol",strProtocol);       //��ȡMsg�ڵ��protocol����ֵ
	parser.GetAttrNode(rootNode,"DateTime",strSetDateTime);    //��ȡMsg�ڵ��DateTime����ֵ��
	parser.GetAttrNode(rootNode,"SrcCode",strSrcCode);         //��ȡMsg�ڵ��StrCode����ֵ
	parser.GetAttrNode(rootNode,"DstCode",strDstCode);         //��ȡMsg�ڵ��DstCode����ֵ
	parser.GetAttrNode(rootNode,"SrcURL",strSrcURL);		   //��ȡMsg�ڵ��SrcUrl����ֵ	   
	parser.GetAttrNode(rootNode,"Priority",strPriority);       //��ȡMsg�ڵ��Priority����ֵ


	pXMLNODE retRootNode = retparser.GetRootNode(); 
	retparser.SetAttrNode("Version",strVersion,retRootNode);                   //����Msg�ڵ��Version����ֵ
	retparser.SetAttrNode("MsgID",strMsgID,retRootNode);                       //����Msg�ڵ��MsgID����ֵ
	retparser.SetAttrNode("DVBType",string("ATV"),retRootNode);               //����Msg�ڵ��DVBType����ֵ
	retparser.SetAttrNode("TaskType",string("TaskAndAlarmExpireRequest"),retRootNode);   //����Msg�ڵ��TaskType����ֵ
	retparser.SetAttrNode("Protocol",strProtocol,retRootNode);                 //����Msg�ڵ��Protocol����ֵ
	retparser.SetAttrNode("DateTime",strSetDateTime,retRootNode);              //����Msg�ڵ��DateTime����ֵ
	retparser.SetAttrNode("SrcCode",strSrcCode,retRootNode);                   //����Msg�ڵ��StrCode����ֵ
	retparser.SetAttrNode("DstCode",strDstCode,retRootNode);                   //����Msg�ڵ��DstCode����ֵ
	retparser.SetAttrNode("SrcURL",strSrcURL,retRootNode);                     //����Msg�ڵ��SrcUrl����ֵ
	retparser.SetAttrNode("Priority",strPriority,retRootNode);  
	std::string EquCode,ExpireDay;
	pXMLNODE RequestNode =parser.GetNodeFromPath("Msg/TaskAndAlarmExpireRequest");
	parser.GetAttrNode(RequestNode,"EquCode",EquCode);
	parser.GetAttrNode(RequestNode,"ExpireDay",ExpireDay);

	pXMLNODE retNode=retparser.CreateNodePtr(retRootNode,"TaskAndAlarmExpireRequest");

	retparser.SetAttrNode("EquCode",EquCode,retNode);
	retparser.SetAttrNode("ExpireDay",ExpireDay,retNode);

	retparser.SaveToString(strStandardXML);	
	return true;
}

bool TranslateDownXMLForATV::TranslateServerStatusSet(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser retparser(source);
	
	std::string strVersion,strMsgID,strType,strProtocol,strSetDateTime,strSrcCode,strDstCode,strSrcURL,strPriority;
	pXMLNODE rootNode = parser.GetRootNode();         //��ȡԭʼXML��Ԫ��
	parser.GetAttrNode(rootNode,"Version",strVersion);         //��ȡMsg�ڵ��Version����ֵ
	parser.GetAttrNode(rootNode,"MsgID",strMsgID);		       //��ȡMsg�ڵ��MsgID����ֵ
	parser.GetAttrNode(rootNode,"Type",strType);		       //��ȡMsg�ڵ��Type����ֵ
	parser.GetAttrNode(rootNode,"Protocol",strProtocol);       //��ȡMsg�ڵ��protocol����ֵ
	parser.GetAttrNode(rootNode,"DateTime",strSetDateTime);    //��ȡMsg�ڵ��DateTime����ֵ��
	parser.GetAttrNode(rootNode,"SrcCode",strSrcCode);         //��ȡMsg�ڵ��StrCode����ֵ
	parser.GetAttrNode(rootNode,"DstCode",strDstCode);         //��ȡMsg�ڵ��DstCode����ֵ
	parser.GetAttrNode(rootNode,"SrcURL",strSrcURL);		   //��ȡMsg�ڵ��SrcUrl����ֵ	   
	parser.GetAttrNode(rootNode,"Priority",strPriority);       //��ȡMsg�ڵ��Priority����ֵ



	pXMLNODE retRootNode = retparser.GetRootNode(); 
	retparser.SetAttrNode("Version",strVersion,retRootNode);                   //����Msg�ڵ��Version����ֵ
	retparser.SetAttrNode("MsgID",strMsgID,retRootNode);   
	retparser.SetAttrNode("DVBType",string("ATV"),retRootNode);   
	retparser.SetAttrNode("Type",string("TVMonDown"),retRootNode);               //����Msg�ڵ��DVBType����ֵ
	retparser.SetAttrNode("TaskType",string("ServerStatusSet"),retRootNode);   //����Msg�ڵ��TaskType����ֵ
	retparser.SetAttrNode("Protocol",strProtocol,retRootNode);                 //����Msg�ڵ��Protocol����ֵ
	retparser.SetAttrNode("DateTime",strSetDateTime,retRootNode);              //����Msg�ڵ��DateTime����ֵ
	retparser.SetAttrNode("SrcCode",strSrcCode,retRootNode);                   //����Msg�ڵ��StrCode����ֵ
	retparser.SetAttrNode("DstCode",strDstCode,retRootNode);                   //����Msg�ڵ��DstCode����ֵ
	retparser.SetAttrNode("SrcURL",strSrcURL,retRootNode);                     //����Msg�ڵ��SrcUrl����ֵ
	retparser.SetAttrNode("Priority",strPriority,retRootNode); 


	std::string EquCode,Action;
	pXMLNODE ServerNode = parser.GetNodeFromPath("Msg/ServerStatusSet");
	pXMLNODE TaskNode =parser.findNode(ServerNode,"Task");
	if(TaskNode==NULL)
	{
		strStandardXML=strOriginalXML;	
		return true;
	}
	parser.GetAttrNode(TaskNode,"EquCode",EquCode);
	parser.GetAttrNode(TaskNode,"Action",Action);

	pXMLNODE retNode=retparser.CreateNodePtr(retRootNode,"ServerStatusSet");

	retparser.SetAttrNode("EquCode",EquCode,retNode);
	retparser.SetAttrNode("Action",Action,retNode);

	pXMLNODELIST NodeList=parser.GetNodeList(TaskNode);

	int count=parser.GetChildCount(TaskNode);

	for(int i=0;i<count;i++)
	{
		pXMLNODE ChildNode=parser.GetNextNode(NodeList);

		if(parser.GetNodeName(ChildNode)=="CycleRecordTime")
		{
			std::string DayOfWeek,StartTime;
			parser.GetAttrNode(ChildNode,"DayOfWeek",DayOfWeek);
			parser.GetAttrNode(ChildNode,"StartTime",StartTime);

			pXMLNODE ParamNode=retparser.CreateNodePtr(retNode,"CycleRecordTime");

			retparser.SetAttrNode("DayOfWeek",DayOfWeek,ParamNode);
			retparser.SetAttrNode("StartTime",StartTime,ParamNode);
		}
		else if(parser.GetNodeName(ChildNode)=="SingleReportTime")
		{
			std::string StartDateTime;
			parser.GetAttrNode(ChildNode,"StartDateTime",StartDateTime);

			pXMLNODE ParamNode=retparser.CreateNodePtr(retNode,"SingleReportTime");

			retparser.SetAttrNode("StartDateTime",StartDateTime,ParamNode);
		}
		else if(parser.GetNodeName(ChildNode)=="FtpServer")
		{
			std::string URL;
			parser.GetAttrNode(ChildNode,"URL",URL);

			pXMLNODE ParamNode=retparser.CreateNodePtr(retNode,"FtpServer");

			retparser.SetAttrNode("URL",URL,ParamNode);

		}
	}

	retparser.SaveToString(strStandardXML);	
	return true;
}
