#include "TranslateDownXMLForCTTB.h"
#include "../Foundation/XmlParser.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include "../Foundation/TypeDef.h"
#include "../Foundation/AppLog.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/OSFunction.h"	//wz_110319

//���ر�׼��Ƶ��ɨ������XML
bool TranslateDownXMLForCTTB::TranslateChannelScan(const std::string& strOriginalXML,std::string& strStandardXML)
{
	if(strOriginalXML.empty())
		return "";
	//Ƶ��ɨ���׼��XML�ṹ
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><ChannelScanQuery></ChannelScanQuery></Msg>";
	XmlParser orgParser( strOriginalXML.c_str());
	XmlParser retParser( source );

	string downType,version,msgid,protocol, dateTime, priority,srccode,dstcode,srcurl;//ͷ��Ϣ
	//��ȡ��Ԫ��
	pXMLNODE rootNode= orgParser.GetNodeFromPath( "Msg" );
	orgParser.GetAttrNode( rootNode,"Type",downType );
	orgParser.GetAttrNode( rootNode,"MsgID",msgid );
	orgParser.GetAttrNode( rootNode,"Version",version );
	orgParser.GetAttrNode( rootNode,"Protocol",protocol );
	orgParser.GetAttrNode( rootNode,"SrcCode",srccode );
	orgParser.GetAttrNode( rootNode,"DstCode",dstcode );
	orgParser.GetAttrNode( rootNode,"DateTime",dateTime );
	orgParser.GetAttrNode( rootNode,"Priority",priority );
	orgParser.GetAttrNode( rootNode,"SrcURL",srcurl );
	if (!strcmp(downType.c_str(),"DVBTHDown") )
	{
		downType = "CTTB";
	}
	else 
	{
		return false;
	}
	//��ȡ��Ԫ��
	pXMLNODE childNode = NULL;
	
	childNode=orgParser.GetNodeFirstChild(rootNode);
	
	pXMLNODE retRootNode = retParser.GetNodeFromPath( "Msg" );
	
	retParser.SetAttrNode( "Version",version,retRootNode );
	retParser.SetAttrNode( "MsgID",msgid,retRootNode );
	retParser.SetAttrNode( "DVBType",downType,retRootNode );
	retParser.SetAttrNode( "TaskType",string("ChannelScanQuery"),retRootNode );
	retParser.SetAttrNode( "Protocol",protocol,retRootNode );
	retParser.SetAttrNode( "DateTime",dateTime,retRootNode );
	retParser.SetAttrNode( "SrcCode",srccode,retRootNode );
	retParser.SetAttrNode( "DstCode",dstcode,retRootNode );
	retParser.SetAttrNode( "SrcURL",srcurl,retRootNode );
	retParser.SetAttrNode( "Priority",priority,retRootNode );

	//��ȡ��Ԫ�ظ���
	int count = orgParser.GetChildCount( childNode );
	//��ȡԪ���б�
	pXMLNODELIST nodeList = orgParser.GetNodeList( childNode );
	pXMLNODE node = NULL; //�б�Ԫ��
	for ( int i = 0; i< count; i++ )
	{ 
		//��ʼ������ֵ
		string ScanTime, STD, Freq, SymbolRate, StartFreq, EndFreq, StepFreq;
		try
		{	
			//��ȡƵ��ɨ�������
			node=orgParser.GetNextNode(nodeList);
			orgParser.GetAttrNode( node,"ScanTime",ScanTime );
			orgParser.GetAttrNode( node,"STD",STD );
			orgParser.GetAttrNode( node,"Freq",Freq );
			orgParser.GetAttrNode( node,"SymbolRate",SymbolRate );
			//����Ƶ��ɨ������
			pXMLNODE chScanNode=retParser.CreateNodePtr(retParser.GetNodeFromPath("Msg/ChannelScanQuery"),"ChannelScan");
			retParser.SetAttrNode( "ScanTime",ScanTime,chScanNode );
			retParser.SetAttrNode( "STD",STD,chScanNode );
			retParser.SetAttrNode( "Freq",Freq,chScanNode );
			retParser.SetAttrNode( "SymbolRate",SymbolRate,chScanNode );
			retParser.SetAttrNode( "StartFreq",StartFreq,chScanNode );
			retParser.SetAttrNode( "EndFreq",EndFreq,chScanNode );
			retParser.SetAttrNode( "StepFreq",StepFreq,chScanNode );
		}
		catch(...)
		{
			string error = "���ñ�׼XML�ĵ�Ԫ�ص����Թ���ʧ��";
			APPLOG::instance()->WriteLog( OTHER,LOG_EVENT_ERROR,error,LOG_OUTPUT_BOTH );
			return false;
		}	
	}
	retParser.SaveToString(strStandardXML);//���浽�ַ���
	return true;
}
//���ر�׼��ʵʱ��Ƶ����XML
bool TranslateDownXMLForCTTB::TranslateStreamRealtimeQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><StreamRealtimeQuery><RealtimeQueryTask/></StreamRealtimeQuery></Msg>";

	XmlParser parser( strOriginalXML.c_str());
	XmlParser tempParser( source );

	string downType,version,msgid,protocol, dateTime, priority,srccode,dstcode,srcurl;//ͷ��Ϣ
	//��ȡ��Ԫ��
	pXMLNODE rootNode= parser.GetNodeFromPath( "Msg" );
	parser.GetAttrNode( rootNode,"Type",downType );
	parser.GetAttrNode( rootNode,"MsgID",msgid );
	parser.GetAttrNode( rootNode,"Version",version );
	parser.GetAttrNode( rootNode,"Protocol",protocol );
	parser.GetAttrNode( rootNode,"SrcCode",srccode );
	parser.GetAttrNode( rootNode,"DstCode",dstcode );
	parser.GetAttrNode( rootNode,"DateTime",dateTime );
	parser.GetAttrNode( rootNode,"Priority",priority );
	parser.GetAttrNode( rootNode,"SrcURL",srcurl );
	if (!strcmp(downType.c_str(),"DVBTHDown") )
	{
		downType = "CTTB";
	}
	else 
	{
		return false;
	}

	//����ͷ��Ϣ
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


	pXMLNODE childNode = parser.GetNodeFirstChild( rootNode );
	//��ȡ��Ƶ��ѯԪ��
	pXMLNODE realStreamNode = parser.GetNodeFirstChild( childNode );

	//��ʼ������ֵ

	string deviceID, freq, OrgNetID,tsID, serviceID, videoPID, audioPID;
	string width, height,fps,bps;
	string dataType;

	//��ȡ������
	pXMLNODE tempChildNode = tempParser.GetNodeFirstChild( tempRootNode );
	pXMLNODE realTimeNode = tempParser.GetNodeFirstChild( tempChildNode );


	//��ȡ��Ƶ��ѯ������
	parser.GetAttrNode( realStreamNode,"OrgNetID",OrgNetID );
	parser.GetAttrNode( realStreamNode,"TsID",tsID );
	parser.GetAttrNode( realStreamNode,"ServiceID",serviceID );
	parser.GetAttrNode( realStreamNode,"Width",width);
	parser.GetAttrNode( realStreamNode,"Height",height);
	parser.GetAttrNode(realStreamNode,"Bps",bps);
	parser.GetAttrNode(realStreamNode,"Fps",fps);

	//��ȡVideoID�� AudioID�� Freq
	CHANNELINFOMGR::instance()->GetAudioID(CTTB,OrgNetID,tsID,serviceID,audioPID );
	CHANNELINFOMGR::instance()->GetVideoID(CTTB,OrgNetID,tsID,serviceID,videoPID );
	CHANNELINFOMGR::instance()->GetFreq(CTTB,OrgNetID,tsID,serviceID,freq );

	tempParser.SetAttrNode( "DeviceID",deviceID,realTimeNode );
	tempParser.SetAttrNode( "Freq",freq,realTimeNode );
	tempParser.SetAttrNode( "OrgNetID",OrgNetID,realTimeNode );
	tempParser.SetAttrNode( "TsID",tsID,realTimeNode );
	tempParser.SetAttrNode( "ServiceID",serviceID,realTimeNode );
	tempParser.SetAttrNode( "VideoPID",videoPID,realTimeNode );
	tempParser.SetAttrNode( "AudioPID",audioPID,realTimeNode );
	tempParser.SetAttrNode( "DataType",dataType,realTimeNode );
	tempParser.SetAttrNode( "Width",width,realTimeNode );
	tempParser.SetAttrNode( "Height",height,realTimeNode );
	tempParser.SetAttrNode( "Fps",fps,realTimeNode );
	tempParser.SetAttrNode( "Bps",bps,realTimeNode );

	//���浽�ַ���
	tempParser.SaveToString( strStandardXML );
	return true;
}
//���ر�׼����Ƶ�ֲ�����XML
bool TranslateDownXMLForCTTB::TranslateStreamRoundQuery(const std::string& strOriginalXML,std::string& strStandardXML,bool multi)
{
	//��Ƶ��ѯ��׼��XML�ṹ
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><StreamRoundQuery></StreamRoundQuery></Msg>";

	XmlParser parser( strOriginalXML.c_str());
	XmlParser tempParser( source );
	string downType,version,msgid,protocol, dateTime, priority,srccode,dstcode,srcurl;//ͷ��Ϣ
	//��ȡ��Ԫ��
	pXMLNODE rootNode= parser.GetNodeFromPath( "Msg" );
	parser.GetAttrNode( rootNode,"Type",downType );
	parser.GetAttrNode( rootNode,"MsgID",msgid );
	parser.GetAttrNode( rootNode,"Version",version );
	parser.GetAttrNode( rootNode,"Protocol",protocol );
	parser.GetAttrNode( rootNode,"SrcCode",srccode );
	parser.GetAttrNode( rootNode,"DstCode",dstcode );
	parser.GetAttrNode( rootNode,"DateTime",dateTime );
	parser.GetAttrNode( rootNode,"Priority",priority );
	parser.GetAttrNode( rootNode,"SrcURL",srcurl );
	if (!strcmp(downType.c_str(),"DVBTHDown") )
	{
		downType = "CTTB";
	}
	else 
	{
		return false;
	}

	//����ͷ��Ϣ
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

	string deviceID, freq,code, RoundTime, OrgNetID,tsID, serviceID, videoPID, audioPID;
	string width, height,fps,bps;
	string dataType;

	pXMLNODE childNode=parser.GetNodeFromPath("Msg/StreamRoundQuery/RoundStream");
	parser.GetAttrNode(childNode,"RoundTime",RoundTime);

	pXMLNODE tempNode=tempParser.GetNodeFromPath("Msg/StreamRoundQuery");
	tempParser.SetAttrNode("DeviceID",deviceID,tempNode);
	tempParser.SetAttrNode("RoundTime",RoundTime,tempNode);

	pXMLNODELIST roundStreamList=parser.GetNodeList(childNode);
	for(int i=0;i<roundStreamList->Size();++i)
	{
		pXMLNODE tempChannelNode=tempParser.CreateNodePtr("Msg/StreamRoundQuery","RoundChannel");
		pXMLNODE channelNode=parser.GetNextNode(roundStreamList);
		parser.GetAttrNode( channelNode,"OrgNetID",OrgNetID );
		parser.GetAttrNode( channelNode,"TsID",tsID );
		parser.GetAttrNode( channelNode,"ServiceID",serviceID );
		parser.GetAttrNode( channelNode,"Width",width);
		parser.GetAttrNode( channelNode,"Height",height);
		parser.GetAttrNode(channelNode,"Bps",bps);
		parser.GetAttrNode(channelNode,"Fps",fps);

		CHANNELINFOMGR::instance()->GetAudioID(CTTB,OrgNetID,tsID,serviceID,audioPID );
		CHANNELINFOMGR::instance()->GetVideoID(CTTB,OrgNetID,tsID,serviceID,videoPID );
		CHANNELINFOMGR::instance()->GetFreq(CTTB,OrgNetID,tsID,serviceID,freq );


		tempParser.SetAttrNode( "Freq",freq,tempChannelNode );
		tempParser.SetAttrNode("Code",code,tempChannelNode);
		tempParser.SetAttrNode( "OrgNetID",OrgNetID,tempChannelNode );
		tempParser.SetAttrNode( "TsID",tsID,tempChannelNode );
		tempParser.SetAttrNode( "ServiceID",serviceID,tempChannelNode );
		tempParser.SetAttrNode( "VideoPID",videoPID,tempChannelNode );
		tempParser.SetAttrNode( "AudioPID",audioPID,tempChannelNode );
		tempParser.SetAttrNode( "DataType",dataType,tempChannelNode );
		tempParser.SetAttrNode( "Width",width,tempChannelNode );
		tempParser.SetAttrNode( "Height",height,tempChannelNode );
		tempParser.SetAttrNode( "Fps",fps,tempChannelNode );
		tempParser.SetAttrNode( "Bps",bps,tempChannelNode );

	}
	//���浽�ַ���
	tempParser.SaveToString( strStandardXML );
	return true;
}
//���ر�׼��¼����������XML
bool TranslateDownXMLForCTTB::TranslateRecordSet(const std::string& strOriginalXML,std::string& strStandardXML)
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
	parser.GetAttrNode( rootNode,"SrcCode",srccode );
	parser.GetAttrNode( rootNode,"DstCode",dstcode );
	parser.GetAttrNode( rootNode,"DateTime",dateTime );
	parser.GetAttrNode( rootNode,"Priority",priority );
	parser.GetAttrNode( rootNode,"SrcURL",srcurl );
	if (!strcmp(downType.c_str(),"DVBTHDown") )
	{
		downType = "CTTB";
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


	if(strOriginalXML.find("SetAutoRecordChannel")!=string::npos)
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
				string DeviceID,OrgNetID,TsID,ServiceID,Width,Height,Fps,Bps,Code,TaskID="0",VideoPID,AudioPID,Freq,DataType,Index;
				string DayofWeek,StartTime,EndTime,StartDateTime,EndDateTime;
				pXMLNODE recordNode=tempParser.CreateNodePtr(autoRecordSetNode,"Record");
				pXMLNODE childNode=parser.GetNextNode(nodeList);

				parser.GetAttrNode(childNode,"DeviceID",DeviceID);
				parser.GetAttrNode(childNode,"OrgNetID",OrgNetID);
				parser.GetAttrNode(childNode,"TsID",TsID);
				parser.GetAttrNode(childNode,"ServiceID",ServiceID);
				parser.GetAttrNode(childNode,"Width",Width);
				parser.GetAttrNode(childNode,"Height",Height);
				parser.GetAttrNode(childNode,"Fps",Fps);
				parser.GetAttrNode(childNode,"Bps",Bps);

				PROPMANAGER::instance()->GetDeviceIndex(Index,DeviceID,"CTTB");
				CHANNELINFOMGR::instance()->GetFreq(CTTB,OrgNetID,TsID,ServiceID,Freq);
				CHANNELINFOMGR::instance()->GetAudioID(CTTB,OrgNetID,TsID,ServiceID,AudioPID);
				CHANNELINFOMGR::instance()->GetVideoID(CTTB,OrgNetID,TsID,ServiceID,VideoPID);
				tempParser.SetAttrNode("TaskID",TaskID,recordNode);
				tempParser.SetAttrNode("Action",action,recordNode);
				tempParser.SetAttrNode("DeviceID",Index,recordNode);
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
			}

		}
	}
	else if(strOriginalXML.find("TaskRecordSet")!=string::npos)
	{
		pXMLNODE taskRecordSetNode=tempParser.CreateNodePtr("Msg","TaskRecordSet");
		string Action;

		pXMLNODE TaskRecordSetNode=parser.GetNodeFromPath("Msg/TaskRecordSet");
		pXMLNODELIST TaskRecordList=parser.GetNodeList(TaskRecordSetNode);
		for (int i=0;i<TaskRecordList->Size();++i)
		{
			string Width,Height,Fps,Bps,Code,TaskID="0";
			pXMLNODE taskRecordNode=parser.GetNextNode(TaskRecordList);
			parser.GetAttrNode(taskRecordNode,"TaskID",TaskID);
			parser.GetAttrNode(taskRecordNode,"Action",Action);
			parser.GetAttrNode(taskRecordNode,"Width",Width);
			parser.GetAttrNode(taskRecordNode,"Height",Height);
			parser.GetAttrNode(taskRecordNode,"Fps",Fps);
			parser.GetAttrNode(taskRecordNode,"Bps",Bps);
			pXMLNODELIST taskRecordList=parser.GetNodeList(taskRecordNode);
			for (int k=0;k<taskRecordList->Size();++k)
			{
				string DeviceID,OrgNetID,TsID,ServiceID,VideoPID,AudioPID,Freq,DataType,Index;
				string DayofWeek,StartTime,EndTime,StartDateTime,EndDateTime;
				pXMLNODE recordNode=tempParser.CreateNodePtr(taskRecordSetNode,"Record");
				pXMLNODE childNode=parser.GetNextNode(taskRecordList);
				parser.GetAttrNode(childNode,"DeviceID",DeviceID);
				parser.GetAttrNode(childNode,"OrgNetID",OrgNetID);
				parser.GetAttrNode(childNode,"TsID",TsID);
				parser.GetAttrNode(childNode,"ServiceID",ServiceID);
				
				PROPMANAGER::instance()->GetDeviceIndex(Index,DeviceID,"CTTB");
				CHANNELINFOMGR::instance()->GetFreq(CTTB,OrgNetID,TsID,ServiceID,Freq);
				CHANNELINFOMGR::instance()->GetAudioID(CTTB,OrgNetID,TsID,ServiceID,AudioPID);
				CHANNELINFOMGR::instance()->GetVideoID(CTTB,OrgNetID,TsID,ServiceID,VideoPID);
				parser.GetAttrNode(childNode,"DayofWeek",DayofWeek);
				if (DayofWeek.length()<1)
				{
					parser.GetAttrNode(childNode,"DayOfWeek",DayofWeek);
				}
				parser.GetAttrNode(childNode,"StartTime",StartTime);
				parser.GetAttrNode(childNode,"EndTime",EndTime);
				parser.GetAttrNode(childNode,"StartDateTime",StartDateTime);
				parser.GetAttrNode(childNode,"EndDateTime",EndDateTime);

				tempParser.SetAttrNode("TaskID",TaskID,recordNode);
				tempParser.SetAttrNode("Action",Action,recordNode);
				tempParser.SetAttrNode("DeviceID",Index,recordNode);
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
			}
		}
	}
	else
	{
		return false;
	}
	tempParser.SaveToString(strStandardXML);
	return true;
}
//���ر�׼��¼��ִ������XML
bool TranslateDownXMLForCTTB::TranslateRecordTask(const std::string& strOriginalXML,std::vector<std::string>& strStandardXMLVec)
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
		string DayofWeek,StartTime,EndTime,StartDateTime,EndDateTime;
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

		tempParser.SaveToString(xml);
		tempParser.DeleteNode(MyRecordNode,recordNode);
		strStandardXMLVec.push_back(xml);
	}
	return true;
}
//���ر�׼��¼���ѯ������XML
bool TranslateDownXMLForCTTB::TranslateRecordQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	//��ʼ������ֵ
	std::string strVersion,strMsgID,strType,strProtocol,strSetDateTime,strSrcCode,strDstCode,strSrcURL,strPriority;
	std::string TaskID, OrgNetID, tsID, serviceID,videoID,audioID,code, StartDateTime, EndDateTime, DeviceID;

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
	char * taskType = const_cast<char *>(nodeName.c_str());
	pXMLNODE recordNode = parser.GetNodeFirstChild( childNode );

	//���ø�����
	pXMLNODE tempRootNode = tempParser.GetNodeFromPath( "Msg" );
	tempParser.SetAttrNode("Version",strVersion,tempRootNode);               //����Msg�ڵ��Version����ֵ
	tempParser.SetAttrNode("MsgID",strMsgID,tempRootNode);                   //����Msg�ڵ��MsgID����ֵ
	if (strType =="DVBTHDown")
	{
		tempParser.SetAttrNode("DVBType",string("CTTB"),tempRootNode);           //����Msg�ڵ��DVBType����ֵ
	}
	else
	{
		return false;
	}
	tempParser.SetAttrNode("TaskType",string("RecordQuery"),tempRootNode);   //����Msg�ڵ��TaskType����ֵ
	tempParser.SetAttrNode("Protocol",strProtocol,tempRootNode);             //����Msg�ڵ��Protocol����ֵ
	tempParser.SetAttrNode("DateTime",strSetDateTime,tempRootNode);          //����Msg�ڵ��DateTime����ֵ
	tempParser.SetAttrNode("SrcCode",strSrcCode,tempRootNode);               //����Msg�ڵ��StrCode����ֵ
	tempParser.SetAttrNode("DstCode",strDstCode,tempRootNode);               //����Msg�ڵ��DstCode����ֵ
	tempParser.SetAttrNode("SrcURL",strSrcURL,tempRootNode);                 //����Msg�ڵ��SrcUrl����ֵ
	tempParser.SetAttrNode("Priority",strPriority,tempRootNode);             //����Msg�ڵ��Priority����ֵ

	pXMLNODE query = tempParser.CreateNodePtr(tempRootNode,taskType);	         //������ѯ���ͽڵ�
	pXMLNODE recordTaskNode = tempParser.CreateNodePtr(query,"RecordQueryTask"); //����RecordQueryTask�ڵ�

	//��ȡ¼�����������
	if ( (nodeName.compare("AutoRecordFileQuery") == 0)||(nodeName.compare("AutoRecordQuery") == 0) )
	{
		parser.GetAttrNode( recordNode,"OrgNetID",OrgNetID );
		parser.GetAttrNode( recordNode,"TsID",tsID );
		parser.GetAttrNode( recordNode,"ServiceID",serviceID );
		parser.GetAttrNode(recordNode,"Index",DeviceID);
		if ( nodeName.compare("AutoRecordQuery") == 0)
		{
			parser.GetAttrNode( recordNode,"DeviceID",DeviceID );
		}
		if(DeviceID!="")
		PROPMANAGER::instance()->GetDeviceIndex(DeviceID,DeviceID,"CTTB");	//���߼�ͨ��ת��Ϊʵ��ͨ��
	}
	else
	{
		parser.GetAttrNode( recordNode,"TaskID",TaskID );
	}

	parser.GetAttrNode( recordNode,"StartDateTime",StartDateTime );
	parser.GetAttrNode( recordNode,"EndDateTime",EndDateTime );

	//����¼����������
	try
	{
		tempParser.SetAttrNode( "TaskID",TaskID,recordTaskNode );
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
//���ر�׼��ʵʱָ���ѯXML
bool TranslateDownXMLForCTTB::TranslateQualityRealtimeQuery( const std::string& strOriginalXML,std::string& strStandardXML)
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

	string periodicity="Once";
	//���ø�����
	pXMLNODE tempRootNode = tempParser.GetNodeFromPath( "Msg" );


	pXMLNODE tempChildNode = tempParser.GetNodeFirstChild( tempRootNode );
	parser.GetAttrNode(rootNode,"Version",version);
	parser.GetAttrNode(rootNode,"MsgID",msgid);
	parser.GetAttrNode(rootNode,"Type",downType);
	parser.GetAttrNode(rootNode,"DateTime",dateTime );
	parser.GetAttrNode(rootNode,"Priority",priority );
	parser.GetAttrNode(rootNode,"Protocol",protocol);
	parser.GetAttrNode(rootNode,"SrcCode",srccode);
	parser.GetAttrNode(rootNode,"DstCode",dstcode);
	parser.GetAttrNode(rootNode,"SrcURL",srcurl);

	if(downType=="DVBTHDown")
	{
		downType="CTTB";
	}
	else
	{
		return false;
	}
	if (protocol.length()<1)
	{
		protocol = "HTTP";
	}
	if (protocol == "HTTP")
	{
		periodicity="Once";
	}
	else if(protocol == "TCP")
	{
		periodicity="Cycle";
	}

	tempParser.SetAttrNode("Version",version,tempRootNode);
	tempParser.SetAttrNode("MsgID",msgid,tempRootNode);
	tempParser.SetAttrNode("DVBType",downType,tempRootNode );
	tempParser.SetAttrNode("TaskType",tasktype,tempRootNode );
	tempParser.SetAttrNode("Protocol",protocol,tempRootNode);
	tempParser.SetAttrNode("DateTime",dateTime,tempRootNode );
	tempParser.SetAttrNode("SrcCode",srccode,tempRootNode);
	tempParser.SetAttrNode("DstCode",dstcode,tempRootNode);
	tempParser.SetAttrNode("SrcURL",srcurl,tempRootNode);
	tempParser.SetAttrNode("Priority",priority,tempRootNode );

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
//���ر�׼�Ķ�ʱָ����������xml
bool TranslateDownXMLForCTTB::TranslateQualityTaskSet(const std::string& strOriginalXML,std::string& strStandardXML)
{
	std::string strVersion,strMsgID,strType,strTaskType,strProtocol,strSetDateTime,strSrcCode,strDstCode,strSrcURL,strPriority;
	std::string TaskID, Action;

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
	if (strType =="DVBTHDown")
	{
		tempParser.SetAttrNode("DVBType",string("CTTB"),tempRootNode);           //����Msg�ڵ��DVBType����ֵ
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

	parser.GetAttrNode(qualityReportNode,"TaskID",TaskID);							   //��ȡԭʼXML��QualityReport�ڵ��TaskID����
	parser.GetAttrNode(qualityReportNode,"Action",Action);                             //��ȡԭʼXML��QualityReport�ڵ��Action����
	tempParser.SetAttrNode("TaskID",TaskID,querySetNode);                              //����QualityQuerySet�ڵ��TaskID����
	tempParser.SetAttrNode("Action",Action,querySetNode);                              //����QualityQuerySet�ڵ��Action����
	if(Action=="Del"||Action=="del")
	{
		
	}
	else if(Action=="Set"||Action=="set")
	{
		pXMLNODE channelNode = parser.GetNodeFromPath("Msg/QualityTaskSet/QualityReport/Channel");      //��ȡԭʼXML��Channel�ڵ�
		pXMLNODE qualityNode = parser.GetNodeFromPath("Msg/QualityTaskSet/QualityReport/QualityParam"); //��ȡԭʼXML��QualityParam�ڵ�


		pXMLNODE taskParmNode = tempParser.CreateNodePtr(querySetNode,"TaskParm");         //����TaskParm�ڵ�
		pXMLNODE qualityParamNode = tempParser.CreateNodePtr(querySetNode,"QualityParam"); //����QualityParam�ڵ�
		pXMLNODE timeParamNode =  tempParser.CreateNodePtr(querySetNode,"TimeParam");      //����TimeParam�ڵ�

		std::string STD,Freq,SymbolRate;
		pXMLNODELIST channelNodeList=parser.GetNodeList(channelNode);
		for ( int i = 0;i<parser.GetChildCount(channelNode);i++)
		{  
			pXMLNODE channelInfo = parser.GetNextNode(channelNodeList);
			parser.GetAttrNode(channelInfo,"STD",STD);                                     //��ȡChannel�ڵ��STD����
			parser.GetAttrNode(channelInfo,"Freq",Freq);								   //��ȡChannel�ڵ��Freq����
			parser.GetAttrNode(channelInfo,"SymbolRate",SymbolRate);					   //��ȡChannel�ڵ��SymbolRate����
			pXMLNODE taskNode =tempParser.CreateNodePtr(taskParmNode,"QualityQueryTask");  //����TaskParm�ڵ���ӽڵ�
			tempParser.SetAttrNode("STD",STD,taskNode);                                    //����QualityQueryTask�ڵ��STD����
			tempParser.SetAttrNode("Freq",Freq,taskNode);                                  //����QualityQueryTask�ڵ��Freq����
			tempParser.SetAttrNode("SymbolRate",SymbolRate,taskNode);                      //����QualityQueryTask�ڵ��SymbolRate����
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
bool TranslateDownXMLForCTTB::TranslateQualityTask(const std::string& strOriginalXML,std::vector<std::string>& strStandardXMLVec)
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
		tempParser.SetAttrNode("DVBType",string("CTTB"),tempRootNode);               //����Msg�ڵ��DVBType����ֵ
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
		pXMLNODELIST taskNodelist=parser.GetNodeList(taskNode);
		for ( int i = 0;i<parser.GetChildCount(taskNode);i++)
		{  
			pXMLNODE taskInfo = parser.GetNextNode(taskNodelist);
			parser.GetAttrNode(taskInfo,"STD",STD);                                     //��ȡTaskParm�ڵ��STD����
			parser.GetAttrNode(taskInfo,"Freq",Freq);								   //��ȡTaskParm�ڵ��Freq����
			parser.GetAttrNode(taskInfo,"SymbolRate",SymbolRate);					   //��ȡTaskParm�ڵ��SymbolRate����
			pXMLNODE taskNode =tempParser.CreateNodePtr(taskParmNode,"QualityQueryTask");  //����TaskParm�ڵ���ӽڵ�
			tempParser.SetAttrNode("STD",STD,taskNode);                                    //����QualityQueryTask�ڵ��STD����
			tempParser.SetAttrNode("Freq",Freq,taskNode);                                  //����QualityQueryTask�ڵ��Freq����
			tempParser.SetAttrNode("SymbolRate",SymbolRate,taskNode);                      //����QualityQueryTask�ڵ��SymbolRate����
		}
		std::string Type,Desc;
		pXMLNODELIST qualityNodelist=parser.GetNodeList(qualityNode);
		for (int j = 0;j<parser.GetChildCount(qualityNode);j++)
		{
			pXMLNODE qualityIndex = parser.GetNextNode(qualityNodelist);
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
//���ر�׼�Ķ�ʱָ������ѯ����xml
bool TranslateDownXMLForCTTB::TranslateQualityQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	std::string strVersion,strMsgID,strType,strTaskType,strProtocol,strSetDateTime,strSrcCode,strDstCode,strSrcURL,strPriority;

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
	parser.GetAttrNode(rootNode,"Priority",strPriority);       //��ȡMsg�ڵ��Priority����ֵ
	parser.GetAttrNode(rootNode,"SrcURL",strSrcURL);


	pXMLNODE tempRootNode = tempParser.GetNodeFromPath( "Msg" );         
	tempParser.SetAttrNode("Version",strVersion,tempRootNode);               //����Msg�ڵ��Version����ֵ
	tempParser.SetAttrNode("MsgID",strMsgID,tempRootNode);                   //����Msg�ڵ��MsgID����ֵ
	if (strType =="DVBTHDown")
	{
		tempParser.SetAttrNode("DVBType",string("CTTB"),tempRootNode);           //����Msg�ڵ��DVBType����ֵ
	}
	else
	{
		return false;
	}
	tempParser.SetAttrNode("TaskType",string("QualityQuery"),tempRootNode);   //����Msg�ڵ��TaskType����ֵ
	tempParser.SetAttrNode("Protocol",strProtocol,tempRootNode);                 //����Msg�ڵ��Protocol����ֵ
	tempParser.SetAttrNode("DateTime",strSetDateTime,tempRootNode);              //����Msg�ڵ��DateTime����ֵ
	tempParser.SetAttrNode("SrcCode",strSrcCode,tempRootNode);                   //����Msg�ڵ��StrCode����ֵ
	tempParser.SetAttrNode("DstCode",strDstCode,tempRootNode);                   //����Msg�ڵ��DstCode����ֵ
	tempParser.SetAttrNode("SrcURL",strSrcURL,tempRootNode);                     //����Msg�ڵ��SrcUrl����ֵ
	tempParser.SetAttrNode("Priority",strPriority,tempRootNode);                 //����Msg�ڵ��Priority����ֵ

	pXMLNODE qualityQueryNode = parser.GetNodeFromPath("Msg/QualityQuery");
	pXMLNODE qualityNode = parser.GetNodeFirstChild(qualityQueryNode);
	pXMLNODE tempQualityQueryNode =tempParser.CreateNodePtr(tempRootNode,"QualityQuery");  //����QualityQuery�ڵ�
	pXMLNODE tempQualityNode = tempParser.CreateNodePtr(tempQualityQueryNode,"Quality");   //����Quality�ڵ�

	std::string TaskID, EquCode,STD,Freq,StartDateTime,EndDateTime,ReportTime;
	parser.GetAttrNode(qualityNode,"TaskID",TaskID);                             //��ȡԭʼXML��Quality�ڵ��TaskID����ֵ
	parser.GetAttrNode(qualityNode,"EquCode",EquCode);                           //��ȡԭʼXML��Quality�ڵ��EquCode����ֵ
	parser.GetAttrNode(qualityNode,"STD",STD);                                   //��ȡԭʼXML��Quality�ڵ��STD����ֵ
	parser.GetAttrNode(qualityNode,"Freq",Freq);                                 //��ȡԭʼXML��Quality�ڵ��Freq����ֵ
	parser.GetAttrNode(qualityNode,"StartDateTime",StartDateTime);               //��ȡԭʼXML��Quality�ڵ��StartDateTime����ֵ
	parser.GetAttrNode(qualityNode,"EndDateTime",EndDateTime);                   //��ȡԭʼXML��Quality�ڵ��EndDateTime����ֵ
	parser.GetAttrNode(qualityNode,"ReportTime",ReportTime);                     //��ȡԭʼXML��Quality�ڵ��ReportTime����ֵ

	tempParser.SetAttrNode("TaskID",TaskID,tempQualityNode);                     //���ñ�׼XML��Quality�ڵ��TaskID����ֵ
	tempParser.SetAttrNode("EquCode",EquCode,tempQualityNode);					 //���ñ�׼XML��Quality�ڵ��EquCode����ֵ
	tempParser.SetAttrNode("STD",STD,tempQualityNode);							 //���ñ�׼XML��Quality�ڵ��STD����ֵ
	tempParser.SetAttrNode("Freq",Freq,tempQualityNode);						 //���ñ�׼XML��Quality�ڵ��Freq����ֵ
	tempParser.SetAttrNode("StartDateTime",StartDateTime,tempQualityNode);		 //���ñ�׼XML��Quality�ڵ��StartDateTime����ֵ
	tempParser.SetAttrNode("EndDateTime",EndDateTime,tempQualityNode);			 //���ñ�׼XML��Quality�ڵ��EndDateTime����ֵ
	tempParser.SetAttrNode("ReportTime",ReportTime,tempQualityNode);			 //���ñ�׼XML��Quality�ڵ��ReportTime����ֵ

	tempParser.SaveToString(strStandardXML);   //���浽�ַ���
	return true;
}
//���ر�׼��Ƶ����������xml
bool TranslateDownXMLForCTTB::TranslateChannelSet(const std::string& strOriginalXML,std::string& strStandardXML)
{
	std::string strVersion,strMsgID,strType,strTaskType,strProtocol,strSetDateTime,strSrcCode,strDstCode,strSrcURL,strPriority;

	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser( strOriginalXML.c_str());
	XmlParser tempParser( source );

	pXMLNODE rootNode = parser.GetNodeFromPath("Msg");         //��ȡԭʼXML��Ԫ��
	parser.GetAttrNode(rootNode,"Version",strVersion);         //��ȡMsg�ڵ��Version����ֵ
	parser.GetAttrNode(rootNode,"MsgID",strMsgID);		       //��ȡMsg�ڵ��MsgID����ֵ
	parser.GetAttrNode(rootNode,"Type",strType);		       //��ȡMsg�ڵ��Type����ֵ
	parser.GetAttrNode(rootNode,"DateTime",strSetDateTime);    //��ȡMsg�ڵ��DateTime����ֵ��
	parser.GetAttrNode(rootNode,"SrcCode",strSrcCode);         //��ȡMsg�ڵ��StrCode����ֵ
	parser.GetAttrNode(rootNode,"DstCode",strDstCode);         //��ȡMsg�ڵ��DstCode����ֵ
	parser.GetAttrNode(rootNode,"Priority",strPriority);       //��ȡMsg�ڵ��Priority����ֵ
	parser.GetAttrNode(rootNode,"SrcURL",strSrcURL);

	pXMLNODE tempRootNode = tempParser.GetNodeFromPath( "Msg" );         
	tempParser.SetAttrNode("Version",strVersion,tempRootNode);               //����Msg�ڵ��Version����ֵ
	tempParser.SetAttrNode("MsgID",strMsgID,tempRootNode);                   //����Msg�ڵ��MsgID����ֵ
	if (strType =="DVBTHDown")
	{
		tempParser.SetAttrNode("DVBType",string("CTTB"),tempRootNode);           //����Msg�ڵ��DVBType����ֵ
	}
	else
	{
		return false;
	}
	tempParser.SetAttrNode("TaskType",string("ChannelSet"),tempRootNode);   //����Msg�ڵ��TaskType����ֵ
	tempParser.SetAttrNode("Protocol",strProtocol,tempRootNode);                 //����Msg�ڵ��Protocol����ֵ
	tempParser.SetAttrNode("DateTime",strSetDateTime,tempRootNode);              //����Msg�ڵ��DateTime����ֵ
	tempParser.SetAttrNode("SrcCode",strSrcCode,tempRootNode);                   //����Msg�ڵ��StrCode����ֵ
	tempParser.SetAttrNode("DstCode",strDstCode,tempRootNode);                   //����Msg�ڵ��DstCode����ֵ
	tempParser.SetAttrNode("SrcURL",strSrcURL,tempRootNode);                     //����Msg�ڵ��SrcUrl����ֵ
	tempParser.SetAttrNode("Priority",strPriority,tempRootNode);                 //����Msg�ڵ��Priority����ֵ

	pXMLNODE channelSetNode = parser.GetNodeFromPath("Msg/ChannelSet");
	pXMLNODELIST channelNodeList = parser.GetNodeList(channelSetNode);
	pXMLNODE tempChannelSetNode =tempParser.CreateNodePtr(tempRootNode,"ChannelSet");  //����QualityQuery�ڵ�

	std::string STD,Freq,QAM,HeaderMode,CarrierNumber,EncodeEfficiency,InterwovenMode;
	std::string DoublePilot,PpnPhase,OrgNetID,TsID,Program,ServiceID,VideoPID,AudioPID,Encryption;
	for (int i = 0;i<parser.GetChildCount(channelSetNode);i++)
	{
		pXMLNODE channelNode = parser.GetNextNode(channelNodeList);

		parser.GetAttrNode(channelNode,"STD",STD);                                //��ȡԭʼXML��Channel�ڵ��STD����ֵ
		parser.GetAttrNode(channelNode,"Freq",Freq);                              //��ȡԭʼXML��Channel�ڵ��Freq����ֵ
		parser.GetAttrNode(channelNode,"QAM",QAM);                                //��ȡԭʼXML��Channel�ڵ��QAM����ֵ
		parser.GetAttrNode(channelNode,"HeaderMode",HeaderMode);                  //��ȡԭʼXML��Channel�ڵ��HeaderMode����ֵ
		parser.GetAttrNode(channelNode,"CarrierNumber",CarrierNumber);            //��ȡԭʼXML��Channel�ڵ��CarrierNumber����ֵ
		parser.GetAttrNode(channelNode,"EncodeEfficiency",EncodeEfficiency);      //��ȡԭʼXML��Channel�ڵ��EncodeEfficiency����ֵ
		parser.GetAttrNode(channelNode,"InterwovenMode",InterwovenMode);          //��ȡԭʼXML��Channel�ڵ��InterwovenMode����ֵ
		parser.GetAttrNode(channelNode,"DoublePilot",DoublePilot);                //��ȡԭʼXML��Channel�ڵ��DoublePilot����ֵ
		parser.GetAttrNode(channelNode,"PpnPhase",PpnPhase);                      //��ȡԭʼXML��Channel�ڵ��PpnPhase����ֵ
		parser.GetAttrNode(channelNode,"OrgNetID",OrgNetID);                      //��ȡԭʼXML��Channel�ڵ��OrgNetID����ֵ
		parser.GetAttrNode(channelNode,"TsID",TsID);                              //��ȡԭʼXML��Channel�ڵ��TsID����ֵ
		parser.GetAttrNode(channelNode,"Program",Program);                        //��ȡԭʼXML��Channel�ڵ��Program����ֵ
		parser.GetAttrNode(channelNode,"ServiceID",ServiceID);                    //��ȡԭʼXML��Channel�ڵ��ServiceID����ֵ
		parser.GetAttrNode(channelNode,"VideoPID",VideoPID);                      //��ȡԭʼXML��Channel�ڵ��VideoPID����ֵ
		parser.GetAttrNode(channelNode,"AudioPID",AudioPID);                      //��ȡԭʼXML��Channel�ڵ��AudioPID����ֵ
		parser.GetAttrNode(channelNode,"Encryption",Encryption);                  //��ȡԭʼXML��Channel�ڵ��Encryption����ֵ


		pXMLNODE tempChannelNode = tempParser.CreateNodePtr(tempChannelSetNode,"Channel");//������׼XML��Channel�ڵ�
		tempParser.SetAttrNode("STD",STD,tempChannelNode);                                //���ñ�׼XML��Channel�ڵ��STD����ֵ
		tempParser.SetAttrNode("Freq",Freq,tempChannelNode);                              //���ñ�׼XML��Channel�ڵ��Freq����ֵ
		tempParser.SetAttrNode("QAM",QAM,tempChannelNode);                                //���ñ�׼XML��Channel�ڵ��QAM����ֵ
		tempParser.SetAttrNode("HeaderMode",HeaderMode,tempChannelNode);                  //���ñ�׼XML��Channel�ڵ��HeaderMode����ֵ
		tempParser.SetAttrNode("CarrierNumber",CarrierNumber,tempChannelNode);            //���ñ�׼XML��Channel�ڵ��CarrierNumber����ֵ
		tempParser.SetAttrNode("EncodeEfficiency",EncodeEfficiency,tempChannelNode);      //���ñ�׼XML��Channel�ڵ��EncodeEfficiency����ֵ
		tempParser.SetAttrNode("InterwovenMode",InterwovenMode,tempChannelNode);          //���ñ�׼XML��Channel�ڵ��InterwovenMode����ֵ
		tempParser.SetAttrNode("DoublePilot",DoublePilot,tempChannelNode);                //���ñ�׼XML��Channel�ڵ��DoublePilot����ֵ
		tempParser.SetAttrNode("PpnPhase",PpnPhase,tempChannelNode);                      //���ñ�׼XML��Channel�ڵ��PpnPhase����ֵ
		tempParser.SetAttrNode("OrgNetID",OrgNetID,tempChannelNode);                      //���ñ�׼XML��Channel�ڵ��OrgNetID����ֵ
		tempParser.SetAttrNode("TsID",TsID,tempChannelNode);                              //���ñ�׼XML��Channel�ڵ��TsID����ֵ
		tempParser.SetAttrNode("Program",Program,tempChannelNode);                        //���ñ�׼XML��Channel�ڵ��Program����ֵ
		tempParser.SetAttrNode("ServiceID",ServiceID,tempChannelNode);                    //���ñ�׼XML��Channel�ڵ��ServiceID����ֵ
		tempParser.SetAttrNode("VideoPID",VideoPID,tempChannelNode);                      //���ñ�׼XML��Channel�ڵ��VideoPID����ֵ
		tempParser.SetAttrNode("AudioPID",AudioPID,tempChannelNode);                      //���ñ�׼XML��Channel�ڵ��AudioPID����ֵ
		tempParser.SetAttrNode("Encryption",Encryption,tempChannelNode);                  //���ñ�׼XML��Channel�ڵ��Encryption����ֵ
	}
	tempParser.SaveToString(strStandardXML);   //���浽�ַ���
	return true;
}
//���ر�׼�Ŀͻ���Ϣ��ѯ����xml
bool TranslateDownXMLForCTTB::TranslateClientInfoQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{	
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><ClientInfoQuery></ClientInfoQuery></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//��ȡ��Ԫ��
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	string tasktype="ClientInfoQuery";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"Priority",priority);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);

	if(downtype=="DVBTHDown")
	{
		downtype="CTTB";
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

	pXMLNODE childNode=parser.GetNodeFirstChild(orgRootNode);
	int count = 0;
	count=parser.GetChildCount( childNode );
	for ( int i = 0;i< count;i++ )
	{
		temParser.CreateNode("Msg/ClientInfoQuery","HeadendInfo");
	}
	//��ȡClientIndex��Ԫ���б�
	pXMLNODE    ClientInfo=temParser.GetNodeFromPath("Msg/ClientInfoQuery");
	pXMLNODELIST stdClientIndexList = temParser.GetNodeList( ClientInfo );
	pXMLNODELIST orgClientIndexList = parser.GetNodeList( childNode );
	pXMLNODE  orglistNode;
	pXMLNODE  stdListNode;
	string index;
	try
	{
		//���÷���XML��ClientIndexԪ������
		for ( int j = 0;j< count;j++ )
		{
			if ( j == 0 )
			{
				orglistNode = parser.GetFirstNode( orgClientIndexList );
				stdListNode = temParser.GetFirstNode( stdClientIndexList );
			}
			else
			{
				orglistNode = parser.GetNextNode( orgClientIndexList );
				stdListNode = temParser.GetNextNode( stdClientIndexList );
			}

			parser.GetAttrNode( orglistNode,"Index",index );
			temParser.SetAttrNode( "Index",index,stdListNode );
		}
	}
	catch(...)
	{
		string error = "�����ÿͻ���Ϣ����ʧ��";
		APPLOG::instance()->WriteLog( OTHER,LOG_EVENT_ERROR,error,LOG_OUTPUT_BOTH );

	}
	//���浽�ַ���
	temParser.SaveToString( strStandardXML);

	return true;
}

//���ر�׼��ͨ��״̬��ѯ����xml
bool TranslateDownXMLForCTTB::TranslateDeviceStatusQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><DeviceStatusQuery></DeviceStatusQuery></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//��ȡ��Ԫ��
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	string index,equcode;
	string tasktype="DeviceStatusQuery";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"Priority",priority);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);

	if(downtype=="DVBTHDown")
	{
		downtype="CTTB";
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

	pXMLNODE childNode=parser.GetNodeFirstChild(orgRootNode);
	int count = 0;
	count=parser.GetChildCount( childNode );
	for ( int i = 0;i< count;i++ )
	{
		temParser.CreateNode("Msg/DeviceStatusQuery","GetDeviceIndexStatus");
	}
	//��ȡStatusIndex��Ԫ���б�
	pXMLNODE    StatusInfo=temParser.GetNodeFromPath("Msg/DeviceStatusQuery");

	parser.GetAttrNode(childNode,"Index",index);
	parser.GetAttrNode(childNode,"EquCode",equcode);
	temParser.SetAttrNode("Index",index,StatusInfo);
	temParser.SetAttrNode("EquCode",equcode,StatusInfo);

	pXMLNODELIST stdStatusIndexList = temParser.GetNodeList( StatusInfo );
	pXMLNODELIST orgStatusIndexList = parser.GetNodeList( childNode );
	pXMLNODE  orglistNode;
	pXMLNODE  stdListNode;
	string deviceid;

	try
	{
		//���÷���XML��StatusIndexԪ������
		for ( int j = 0;j< count;j++ )
		{
			orglistNode = parser.GetNextNode( orgStatusIndexList );
			stdListNode = temParser.GetNextNode( stdStatusIndexList );

			parser.GetAttrNode( orglistNode,"DeviceID",deviceid );
			temParser.SetAttrNode( "DeviceID",deviceid,stdListNode );
		}
	}
	catch(...)
	{
		string error = "����ͨ��״̬����ʧ��";
		APPLOG::instance()->WriteLog( OTHER,LOG_EVENT_ERROR,error,LOG_OUTPUT_BOTH );

	}
	//���浽�ַ���
	temParser.SaveToString( strStandardXML);

	return true;
}
//���ر�׼��PSI/SI��ѯ����xml
bool TranslateDownXMLForCTTB::TranslateTableQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><TableQuery><Table/></TableQuery></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//��ȡ��Ԫ��
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	string freq,equcode,stdtype,rate,querytime;
	string tasktype="TableQuery";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"Priority",priority);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);

	if(downtype=="DVBTHDown")
	{
		downtype="CTTB";
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

	pXMLNODE childNode=parser.GetNodeFirstChild(orgRootNode);
	pXMLNODE orgTableNode=parser.GetNodeFirstChild(childNode);
	pXMLNODE stdTabelNode=temParser.GetNodeFromPath("Msg/TableQuery/Table");
	parser.GetAttrNode(orgTableNode,"EquCode",equcode);
	parser.GetAttrNode(orgTableNode,"STD",stdtype);
	parser.GetAttrNode(orgTableNode,"Freq",freq);
	parser.GetAttrNode(orgTableNode,"SymbolRate",rate);
	parser.GetAttrNode(orgTableNode,"QueryTime",querytime);

	temParser.SetAttrNode("EquCode",equcode,stdTabelNode);
	temParser.SetAttrNode("STD",stdtype,stdTabelNode);
	temParser.SetAttrNode("Freq",freq,stdTabelNode);
	temParser.SetAttrNode("SymbolRate",rate,stdTabelNode);
	temParser.SetAttrNode("QueryTime",querytime,stdTabelNode);

	//���浽�ַ���
	temParser.SaveToString( strStandardXML);
	return true;
}
//���ر�׼������ͼ��������xml
bool TranslateDownXMLForCTTB::TranslateRunPlanSet(const std::string& strOriginalXML,std::string& strStandardXML)
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
	parser.GetAttrNode(orgRootNode,"Priority",priority);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);

	if(downtype=="DVBTHDown")
	{
		downtype="CTTB";
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

	pXMLNODE childNode=parser.GetNodeFirstChild(orgRootNode);
	pXMLNODE stdRunSetNode=temParser.GetNodeFromPath("Msg/RunPlanSet");

	int count = 0;
	count=parser.GetChildCount( childNode );

	pXMLNODELIST stdChannelIndexList = temParser.GetNodeList( stdRunSetNode );
	pXMLNODELIST orgChannelIndexList = parser.GetNodeList( childNode );

	pXMLNODE  orglistNode;
	pXMLNODE  stdListNode;
	string EquCode,OrgNetID,TsID,ServiceID,VideoID,AudioID;
	int childcount=0;
	for ( int j = 0;j< count;j++ )
	{
		orglistNode = parser.GetNextNode( orgChannelIndexList );
		childcount=parser.GetChildCount(orglistNode);
		for(int i=0;i<childcount;i++)
		{
			temParser.CreateNode("Msg/RunPlanSet","RunPlanChannel");
		}
	}
	try
	{
		//���÷���XML��ChannelIndexԪ������
		stdChannelIndexList = temParser.GetNodeList( stdRunSetNode );
		for ( int j = 0;j< count;j++ )
		{	
			orglistNode = parser.GetNextNode( orgChannelIndexList );
		
			parser.GetAttrNode(orglistNode,"EquCode",EquCode);
			parser.GetAttrNode(orglistNode,"OrgNetID",OrgNetID);
			parser.GetAttrNode(orglistNode,"TsID",TsID);
			parser.GetAttrNode(orglistNode,"ServiceID",ServiceID);

			CHANNELINFOMGR::instance()->GetVideoID(CTTB,OrgNetID,TsID,ServiceID,VideoID);
			CHANNELINFOMGR::instance()->GetAudioID(CTTB,OrgNetID,TsID,ServiceID,AudioID);
			childcount=parser.GetChildCount(orglistNode);

			pXMLNODE childListNode;
			pXMLNODELIST childChannelIndexList = parser.GetNodeList( orglistNode );
			for(int k=0;k<childcount;k++)
			{
				string DayOfWeek,StartTime,EndTime,ValidStartDateTime,ValidEndDateTime,StartDateTime,EndDateTime;

				if ( k == 0 )
				{
					childListNode = parser.GetFirstNode( childChannelIndexList );
					if(j==0)
					{
						stdListNode = temParser.GetFirstNode( stdChannelIndexList );
					}
					else
					{
						stdListNode = temParser.GetNextNode( stdChannelIndexList );
					}	

				}
				else
				{
					childListNode = parser.GetNextNode( childChannelIndexList );
					stdListNode = temParser.GetNextNode( stdChannelIndexList );
				}
				if(parser.GetNodeName(childListNode)=="CycleWorkTime")
				{
					parser.GetAttrNode(childListNode,"DayOfWeek",DayOfWeek);
					parser.GetAttrNode(childListNode,"StartTime",StartTime);
					parser.GetAttrNode(childListNode,"EndTime",EndTime);
					parser.GetAttrNode(childListNode,"ValidStartDateTime",ValidStartDateTime);
					parser.GetAttrNode(childListNode,"ValidEndDateTime",ValidEndDateTime);

				}
				else if(parser.GetNodeName(childListNode)=="SingleWorkTime")
				{
					parser.GetAttrNode(childListNode,"StartDateTime",StartDateTime);
					parser.GetAttrNode(childListNode,"EndDateTime",EndDateTime);
				}
				temParser.SetAttrNode("EquCode",EquCode,stdListNode);
				temParser.SetAttrNode("OrgNetID",OrgNetID,stdListNode);
				temParser.SetAttrNode("TsID",TsID,stdListNode);
				temParser.SetAttrNode("ServiceID",ServiceID,stdListNode);
                temParser.SetAttrNode("VideoPID",VideoID,stdListNode);
				temParser.SetAttrNode("AudioPID",AudioID,stdListNode);
				temParser.SetAttrNode("DayOfWeek",DayOfWeek,stdListNode);
				temParser.SetAttrNode("StartTime",StartTime,stdListNode);
				temParser.SetAttrNode("EndTime",EndTime,stdListNode);
				temParser.SetAttrNode("ValidStartDateTime",ValidStartDateTime,stdListNode);
				temParser.SetAttrNode("ValidEndDateTime",ValidEndDateTime,stdListNode);
				temParser.SetAttrNode("StartDateTime",StartDateTime,stdListNode);
				temParser.SetAttrNode("EndDateTime",EndDateTime,stdListNode);
				temParser.SaveToString( strStandardXML);
			}


		}
	}
	catch(...)
	{
		string error = "��������ͼ����ʧ��";
		APPLOG::instance()->WriteLog( OTHER,LOG_EVENT_ERROR,error,LOG_OUTPUT_BOTH );

	}

	//���浽�ַ���
	temParser.SaveToString( strStandardXML);

	return true;
}
//���ر�׼���ֶ�¼������xml
bool TranslateDownXMLForCTTB::TranslateManualRecordSet(const std::string &strOriginalXML, std::string &strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><ManualRecordSet></ManualRecordSet></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//��ȡ��Ԫ��
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	string tasktype="ManualRecordSet";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="DVBTHDown")
	{
		downtype="CTTB";
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

	pXMLNODE  manualRecordQueryNode = parser.GetNodeFromPath("Msg/ManualRecordQuery");
	pXMLNODE  tempRecordSetNode = temParser.GetNodeFromPath("Msg/ManualRecordSet");
	pXMLNODELIST recordNodelist = parser.GetNodeList(manualRecordQueryNode);      //��ȡManualRecordQuery���ӽڵ��б�
	int count = parser.GetChildCount(manualRecordQueryNode);
	for (int i = 0;i<count;i++)
	{
		pXMLNODE recordNode = parser.GetNextNode(recordNodelist);
		pXMLNODE  tempRecordNode = temParser.CreateNodePtr(tempRecordSetNode,"ManualRecord");    //����ManualRecord�ڵ�
		std::string deviceid,time,path;
		parser.GetAttrNode(recordNode,"Index",deviceid);
		parser.GetAttrNode(recordNode,"Time",time);
		parser.GetAttrNode(recordNode,"Path",path);
		temParser.SetAttrNode("DeviceID",deviceid,tempRecordNode);
		temParser.SetAttrNode("Time",time,tempRecordNode);
		temParser.SetAttrNode("Path",path,tempRecordNode);
	}
	temParser.SaveToString(strStandardXML);
	return true;

}
//���ر�׼�Ļ���ָ���ѯ����xml
bool TranslateDownXMLForCTTB::TranslateEnvironmentQualityQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><EnvironmentQuery></EnvironmentQuery></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//��ȡ��Ԫ��
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	string freq,equcode,stdtype,rate,querytime;
	string tasktype="EnvironmentQuery";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="DVBTHDown")
	{
		downtype="CTTB";
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

	pXMLNODE childNode=parser.GetNodeFirstChild(orgRootNode);
	int count = 0;
	count=parser.GetChildCount( childNode );
	for ( int i = 0;i< count;i++ )
	{
		temParser.CreateNode("Msg/EnvironmentQuery","Environment");
	}

	pXMLNODE    IndexDesc=temParser.GetNodeFromPath("Msg/EnvironmentQuery");


	pXMLNODELIST stdIndexList = temParser.GetNodeList( IndexDesc );
	pXMLNODELIST orgIndexList = parser.GetNodeList( childNode );
	pXMLNODE  orglistNode;
	pXMLNODE  stdListNode;
	string type,desc;

	try
	{
		//���÷���XML��StatusIndexԪ������
		for ( int j = 0;j< count;j++ )
		{
			if ( j == 0 )
			{
				orglistNode = parser.GetFirstNode( orgIndexList );
				stdListNode = temParser.GetFirstNode( stdIndexList );
			}
			else
			{
				orglistNode = parser.GetNextNode( orgIndexList );
				stdListNode = temParser.GetNextNode( stdIndexList );
			}

			parser.GetAttrNode( orglistNode,"Type",type );
			parser.GetAttrNode(orglistNode,"Desc",desc);
			temParser.SetAttrNode( "Type",type,stdListNode );
			temParser.SetAttrNode("Desc",desc,stdListNode);
		}
	}
	catch(...)
	{
		string error = "���û���ָ������ʧ��";
		APPLOG::instance()->WriteLog( OTHER,LOG_EVENT_ERROR,error,LOG_OUTPUT_BOTH );

	}


	//���浽�ַ���
	temParser.SaveToString( strStandardXML);
	return true;
}
bool TranslateDownXMLForCTTB::TranslateMHPQuery( const std::string& strOriginalXML,std::string& strStandardXML )
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><MHPQuery></MHPQuery></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//��ȡ��Ԫ��
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	string tasktype="MHPQuery";
	string ScanTime;
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="DVBTHDown")
	{
		downtype="CTTB";
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

	pXMLNODE queryNode=parser.GetNodeFromPath("Msg/MHPQuery");
	parser.GetAttrNode(queryNode,"ScanTime",ScanTime);
	pXMLNODE retQueryNode=temParser.CreateNodePtr(orgRootNode,"MHPQuery");
	temParser.SetAttrNode("ScanTime",ScanTime,retQueryNode);
	temParser.SaveToString(strStandardXML);
	return true;
}
//������������	( jidushan 110321 ) 
bool TranslateDownXMLForCTTB::TranslateAlarmParamSet(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><AlarmParamSet></AlarmParamSet></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//��ȡ��Ԫ��
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	
	string tasktype="AlarmParamSet";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"Priority",priority);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);

	if(downtype=="DVBTHDown")
	{
		downtype="CTTB";
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

	pXMLNODE ParamNode=parser.GetNodeFirstChild(orgRootNode);
	//AlarmParamSet�ڵ�
	pXMLNODE AlarmParamSetNode=temParser.GetNodeFromPath("Msg/AlarmParamSet");
	string std,freq,symbolrate;
	//AlarmParamSet�ڵ� ����std��freq��symbolrate����,Ĭ��Ϊ��
	temParser.SetAttrNode("STD",		std,		AlarmParamSetNode); 
	temParser.SetAttrNode("Freq",		freq,		AlarmParamSetNode);
	temParser.SetAttrNode("SymbolRate", symbolrate, AlarmParamSetNode);

	int  count = parser.GetChildCount(ParamNode);
	pXMLNODELIST NodeList = parser.GetNodeList(ParamNode);
	for(int i=0;i<count;i++)
	{
		pXMLNODE childNode=parser.GetNextNode(NodeList);	//AlarmProgram \AlarmFreq \AlarmEnvironment�ڵ�
		string nodename = parser.GetNodeName(childNode);
		if (nodename == "AlarmProgram")		//program��������
		{
			//�Ȱ�freq��channelID��ȡ�����������ӽڵ��ʱ��ֱ�Ӹ�ֵ
			string EquCode,OrgNetID,TsID,ServiceID;
			parser.GetAttrNode(childNode,"EquCode",		EquCode);
			parser.GetAttrNode(childNode,"OrgNetID",	OrgNetID);
			parser.GetAttrNode(childNode,"TsID",		TsID);
			parser.GetAttrNode(childNode,"ServiceID",	ServiceID);

			//��ȡfreq
			bool ret = CHANNELINFOMGR::instance()->GetFreq(OSFunction::GetEnumDVBType(downtype), OrgNetID, TsID, ServiceID, freq);
			if (freq.length()<1)
			{
				freq = "All";
			}
			//��ȡchannelid
			string VideoPID,AudioPID,Channelid;
			CHANNELINFOMGR::instance()->GetVideoID(OSFunction::GetEnumDVBType(downtype),OrgNetID, TsID,ServiceID,VideoPID);
			CHANNELINFOMGR::instance()->GetAudioID(OSFunction::GetEnumDVBType(downtype),OrgNetID, TsID,ServiceID,AudioPID);
			CHANNELINFOMGR::instance()->GetChannelID(CTTB,OrgNetID,TsID,ServiceID,VideoPID,AudioPID,"",Channelid);
			
			pXMLNODELIST childSetList=parser.GetNodeList(childNode);
			for(int k=0;k<childSetList->Size();k++)
			{
				pXMLNODE childHoldNode = parser.GetNextNode(childSetList);
				pXMLNODE AlarmParamNode = temParser.CreateNodePtr(AlarmParamSetNode,"AlarmParam");

				temParser.SetAttrNode("Freq",freq,AlarmParamNode);
				temParser.SetAttrNode("ChannelID",Channelid,AlarmParamNode);

				//��Ŀ����Ӧ���е�����
				string type,desc,duration,alarmswitch;
				parser.GetAttrNode(childHoldNode,string("Type"),type);//���Type����
				parser.GetAttrNode(childHoldNode,string("Desc"),desc);//���Desc����
				parser.GetAttrNode(childHoldNode,string("Duration"),duration);
				parser.GetAttrNode(childHoldNode,string("Switch"),alarmswitch);

				temParser.SetAttrNode("Type",type,AlarmParamNode);
				temParser.SetAttrNode("Desc",desc,AlarmParamNode);
				temParser.SetAttrNode("Duration",duration,AlarmParamNode);
				temParser.SetAttrNode("Switch",alarmswitch,AlarmParamNode);

				//��Ŀ����Ӧ�û�ȡ����������
				string num,timeinterval,downthreshold,upthreshold;
				parser.GetAttrNode(childHoldNode,string("Num"),num);//�������
				parser.GetAttrNode(childHoldNode,string("TimeInterval"),timeinterval);//ʱ����
				parser.GetAttrNode(childHoldNode,string("DownThreshold"),downthreshold);//����
				parser.GetAttrNode(childHoldNode,string("UpThreshold"),upthreshold);//����

				temParser.SetAttrNode("Num",num,AlarmParamNode);
				temParser.SetAttrNode("TimeInterval",timeinterval,AlarmParamNode);
				temParser.SetAttrNode("DownThreshold",downthreshold,AlarmParamNode);
				temParser.SetAttrNode("UpThreshold",upthreshold,AlarmParamNode);
			}
		}
		else		//freq���ͻ���environment����
		{
			//freq����alarmfreq�ڵ��Դ�std��freq��symbolrate����
			//environment����û�У�Ĭ�ϸ�ֵΪ��
			parser.GetAttrNode(childNode, "STD",		std);
			parser.GetAttrNode(childNode, "Freq",		freq);
			parser.GetAttrNode(childNode, "SymbolRate", symbolrate);
			
			temParser.SetAttrNode("STD",		std,		AlarmParamSetNode);
			temParser.SetAttrNode("Freq",		freq,		AlarmParamSetNode);
			temParser.SetAttrNode("SymbolRate", symbolrate, AlarmParamSetNode);
			
			int lastcount = parser.GetChildCount(childNode);	
			pXMLNODELIST lastNodeList = parser.GetNodeList(childNode);
			for (int j=0;j<lastcount;++j)
			{
				pXMLNODE lastNode = parser.GetNextNode(lastNodeList);
				pXMLNODE AlarmParamNode = temParser.CreateNodePtr(AlarmParamSetNode, "AlarmParam");
				if (freq.length()<1)
				{
					freq = "All";
				}
				temParser.SetAttrNode("Freq",	freq,	AlarmParamNode);

				//freq�����Լ�Environment������������
				string alarmSwitch,type,desc,alarmNum, TimeInterval,DownThreshold,UpThreshold;
				parser.GetAttrNode(lastNode, "Type", type);
				parser.GetAttrNode(lastNode, "Desc", desc);
				parser.GetAttrNode(lastNode, "Switch", alarmSwitch);
				if (alarmSwitch.length()<1)
				{
					alarmSwitch = "1";
				}
				parser.GetAttrNode(lastNode, "Num", alarmNum);
				parser.GetAttrNode(lastNode, "TimeInterval", TimeInterval);
				parser.GetAttrNode(lastNode, "DownThreshold", DownThreshold);
				parser.GetAttrNode(lastNode, "UpThreshold", UpThreshold);
				
				temParser.SetAttrNode("Type",			type,			AlarmParamNode);
				temParser.SetAttrNode("Desc",			desc,			AlarmParamNode);
				temParser.SetAttrNode("Num",			alarmNum,		AlarmParamNode);
				temParser.SetAttrNode("TimeInterval",	TimeInterval,	AlarmParamNode);
				temParser.SetAttrNode("DownThreshold",	DownThreshold,	AlarmParamNode);
				temParser.SetAttrNode("UpThreshold",	UpThreshold,	AlarmParamNode);
				temParser.SetAttrNode("Switch",			alarmSwitch,	AlarmParamNode);

				//����Ĭ��Ϊ������
				string serviceId,videoPid,audioPid,duration;
				parser.GetAttrNode(lastNode, "ServiceID", serviceId);
				parser.GetAttrNode(lastNode, "VideoPID", videoPid);
				parser.GetAttrNode(lastNode, "AudioPID", audioPid);
				parser.GetAttrNode(lastNode, "Duration", duration);
				temParser.SetAttrNode("Duration",		duration,		AlarmParamNode);
				temParser.SetAttrNode("ServiceID",		serviceId,		AlarmParamNode);
				temParser.SetAttrNode("VideoPID",		videoPid,		AlarmParamNode);
				temParser.SetAttrNode("AudioPID",		audioPid,		AlarmParamNode);		
			}
		}
	}
	//���浽�ַ���
	temParser.SaveToString( strStandardXML);
	return true;

}


//����ָ�겹����׼xml
bool TranslateDownXMLForCTTB::TranslateQualityCompensationSet(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//��ȡ��Ԫ��
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	string freq,equcode,stdtype,rate,querytime;
	string tasktype="QualityCompensationSet";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="DVBTHDown")
	{
		downtype="CTTB";
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


	int count = 0;
	count=parser.GetChildCount( orgRootNode );
	for(int j=0;j<count;j++)
	{
		temParser.CreateNode("Msg","QualityCompensationSet");
	}

	pXMLNODELIST stdIndexList = temParser.GetNodeList( stdRootNode );
	pXMLNODELIST orgIndexList = parser.GetNodeList( orgRootNode );
	for (int i=0;i<count;i++)
	{
		pXMLNODE orglistNode;
		pXMLNODE stdListNode;
		string deviceid;
		if ( i == 0 )
		{
			orglistNode = parser.GetFirstNode( orgIndexList );
			stdListNode = temParser.GetFirstNode( stdIndexList );
		}
		else
		{
			orglistNode = parser.GetNextNode( orgIndexList );
			stdListNode = temParser.GetNextNode( stdIndexList );
		}
		parser.GetAttrNode(orglistNode,"Index",deviceid);
		temParser.SetAttrNode("DeviceID",deviceid,stdListNode);

		int childcount=parser.GetChildCount(orglistNode);
		int k=0;
		for(k=0;k<childcount;k++)
		{
			temParser.CreateNodePtr(stdListNode,"Compensation");
		}
		pXMLNODELIST stdChildIndexList = temParser.GetNodeList(stdListNode);
		pXMLNODELIST orgChildIndexList = parser.GetNodeList(orglistNode);
		for(k=0;k<childcount;k++)
		{
			pXMLNODE orgChildlistNode;
			pXMLNODE stdChildListNode;
			string type,desc,val;
			if(k==0)
			{
				orgChildlistNode = parser.GetFirstNode(orgChildIndexList);
				stdChildListNode = parser.GetFirstNode(stdChildIndexList);
			}
			else
			{
				orgChildlistNode = parser.GetNextNode(orgChildIndexList);
				stdChildListNode = parser.GetNextNode(stdChildIndexList);
			}
			parser.GetAttrNode(orgChildlistNode,"Type",type);
			parser.GetAttrNode(orgChildlistNode,"Desc",desc);
			parser.GetAttrNode(orgChildlistNode,"Value",val);

			temParser.SetAttrNode("Type",type,stdChildListNode);
			temParser.SetAttrNode("Desc",desc,stdChildListNode);
			temParser.SetAttrNode("Value",val,stdChildListNode);

		}


	}
	temParser.SaveToString(strStandardXML);

	return true;
}
//���ر�׼������ҵ�����ʱ����������xml
bool TranslateDownXMLForCTTB::TranslateAutoAnalysisTimeSet(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><AutoAnalysisTimeSet></AutoAnalysisTimeSet></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//��ȡ��Ԫ��
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	string tasktype="AutoAnalysisTimeSet";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="DVBTHDown")
	{
		downtype="CTTB";
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

	pXMLNODE  timeQueryNode = parser.GetNodeFromPath("Msg/AutoAnalysisTimeQuery");
	pXMLNODE  tempTimeQueryNode = temParser.GetNodeFromPath("Msg/AutoAnalysisTimeSet");
	pXMLNODELIST queryNodelist = parser.GetNodeList(timeQueryNode);     //��ȡAutoAnalysisTimeQuery���ӽڵ��б�
	int count = parser.GetChildCount(timeQueryNode);
	for (int i = 0;i<count;i++)
	{
		pXMLNODE timeNode = parser.GetNextNode(queryNodelist);
		pXMLNODE tempTimeNode = temParser.CreateNodePtr(tempTimeQueryNode,"AutoAnalysisTime");  //����AutoAnalysisTime�ڵ�
		std::string starttime,type;
		parser.GetAttrNode(timeNode,"StartTime",starttime);
		parser.GetAttrNode(timeNode,"Type",type);
		temParser.SetAttrNode("StartTime",starttime,tempTimeNode);
		temParser.SetAttrNode("Type",type,tempTimeNode);
	}
	temParser.SaveToString(strStandardXML);
	return true;
}

bool TranslateDownXMLForCTTB::TranslateEPGQuery( const std::string& strOriginalXML,std::string& strStandardXML )
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><EPGQuery></EPGQuery></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//��ȡ��Ԫ��
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	string tasktype="EPGQuery";
	string ScanTime;
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);


	if(downtype=="DVBTHDown")
	{
		downtype="CTTB";
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

	pXMLNODE queryNode=parser.GetNodeFromPath("Msg/EPGQuery");
	parser.GetAttrNode(queryNode,"ScanTime",ScanTime);
	pXMLNODE retQueryNode=temParser.CreateNodePtr(orgRootNode,"EPGQuery");
	temParser.SetAttrNode("ScanTime",ScanTime,retQueryNode);
	temParser.SaveToString(strStandardXML);
	return true;
}
