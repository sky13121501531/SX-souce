///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����TranslateDownXMLForTHREED.cpp
// �����ߣ�jiangcheng
// ����ʱ�䣺2009-06-04
// �������������·�XMLת��Ϊԭ�����������XML
///////////////////////////////////////////////////////////////////////////////////////////
#include "TranslateDownXMLForTHREED.h"
#include "../Foundation/XmlParser.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include "../Foundation/TypeDef.h"
#include "../Foundation/AppLog.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/OSFunction.h"

//���ر�׼��Ƶ��ɨ������XML
bool TranslateDownXMLForTHREED::TranslateChannelScan(const std::string& strOriginalXML,std::string& strStandardXML)
{
	//Ƶ��ɨ���׼��XML�ṹ
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><ChannelScanQuery></ChannelScanQuery></Msg>";
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

	if(!strcmp(downType.c_str(),"3DDown"))
	{
		downType="THREED";
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
	tempParser.SetAttrNode( "TaskType",string("ChannelScanQuery"),tempRootNode );
	tempParser.SetAttrNode( "Protocol",protocol,tempRootNode );
	tempParser.SetAttrNode( "DateTime",dateTime,tempRootNode );
	tempParser.SetAttrNode( "SrcCode",srccode,tempRootNode );
	tempParser.SetAttrNode( "DstCode",dstcode,tempRootNode );
	tempParser.SetAttrNode( "SrcURL",srcurl,tempRootNode );
	tempParser.SetAttrNode( "Priority",priority,tempRootNode );
	pXMLNODE childNode=parser.GetNodeFirstChild(rootNode);
	pXMLNODE tempQueryNode = tempParser.GetNodeFromPath("Msg/ChannelScanQuery");

	pXMLNODELIST nodelist=parser.GetNodeList(childNode);
	
	int count=nodelist->Size();
	string ScanTime;
	string STD, Freq, SymbolRate, StartFreq, EndFreq, StepFreq,QAM;

	parser.GetAttrNode( childNode,"ScanTime",ScanTime );
	if (count==0)
	{
		
		//��ȡƵ��ɨ�������
	
		parser.GetAttrNode( childNode,"SymbolRate",SymbolRate );
		parser.GetAttrNode( childNode,"QAM",QAM );
		//����Ƶ��ɨ������
		pXMLNODE chScanNode=tempParser.CreateNodePtr(tempQueryNode,"ChannelScan");
		tempParser.SetAttrNode( "ScanTime",ScanTime,chScanNode );
		tempParser.SetAttrNode( "STD",STD,chScanNode );
		tempParser.SetAttrNode( "Freq",Freq,chScanNode );
		tempParser.SetAttrNode( "SymbolRate",SymbolRate,chScanNode );
		tempParser.SetAttrNode( "QAM",QAM,chScanNode );
		tempParser.SetAttrNode( "StartFreq",StartFreq,chScanNode );
		tempParser.SetAttrNode( "EndFreq",EndFreq,chScanNode );
		tempParser.SetAttrNode( "StepFreq",StepFreq,chScanNode );
		tempParser.SaveToString(strStandardXML);//���浽�ַ���
	}
	else
	{
		for (int i=0;i!=count;++i)
		{
			pXMLNODE childnode=nodelist->GetNextNode();
			
			parser.GetAttrNode( childnode,"SymbolRate",SymbolRate );
			parser.GetAttrNode( childnode,"QAM",QAM );

			//����Ƶ��ɨ������
			pXMLNODE chScanNode=tempParser.CreateNodePtr(tempQueryNode,"ChannelScan");
			tempParser.SetAttrNode( "ScanTime",ScanTime,chScanNode );
			tempParser.SetAttrNode( "STD",STD,chScanNode );
			tempParser.SetAttrNode( "Freq",Freq,chScanNode );
			tempParser.SetAttrNode( "SymbolRate",SymbolRate,chScanNode );
			tempParser.SetAttrNode( "QAM",QAM,chScanNode );
			tempParser.SetAttrNode( "StartFreq",StartFreq,chScanNode );
			tempParser.SetAttrNode( "EndFreq",EndFreq,chScanNode );
			tempParser.SaveToString(strStandardXML);//���浽�ַ���
		}
	}
	return true;
}
//���ر�׼��ʵʱ��Ƶ����XML
bool TranslateDownXMLForTHREED::TranslateStreamRealtimeQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	//��Ƶ��ѯ��׼��XML�ṹ
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

	if(!strcmp(downType.c_str(),"3DDown"))
	{
		downType="THREED";
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
	string Width, Height,Fps,Bps,ThreeDSet;
	string dataType;

	//��ȡ������
	pXMLNODE tempChildNode = tempParser.GetNodeFirstChild( tempRootNode );
	pXMLNODE realTimeNode = tempParser.GetNodeFirstChild( tempChildNode );


	//��ȡ��Ƶ��ѯ������
	parser.GetAttrNode( realStreamNode,"Freq",freq );
	parser.GetAttrNode( realStreamNode,"ServiceID",serviceID );
	parser.GetAttrNode( realStreamNode,"VideoPID",videoPID );
	parser.GetAttrNode( realStreamNode,"AudioPID",audioPID);

	parser.GetAttrNode(realStreamNode,"Width",Width);
	parser.GetAttrNode(realStreamNode,"Height",Height);
	parser.GetAttrNode(realStreamNode,"Fps",Fps);
	parser.GetAttrNode(realStreamNode,"Bps",Bps);
	parser.GetAttrNode(realStreamNode,"ThreeDSet",ThreeDSet);
	//��ȡOrgNetID,tsID
	CHANNELINFOMGR::instance()->GetOrgNetID(THREED,freq,audioPID,videoPID,serviceID,OrgNetID );
	CHANNELINFOMGR::instance()->GetTsID(THREED,freq,audioPID,videoPID,serviceID,tsID);

	tempParser.SetAttrNode( "DeviceID",deviceID,realTimeNode );
	tempParser.SetAttrNode( "Freq",freq,realTimeNode );
	tempParser.SetAttrNode( "OrgNetID",OrgNetID,realTimeNode );
	tempParser.SetAttrNode( "TsID",tsID,realTimeNode );
	tempParser.SetAttrNode( "ServiceID",serviceID,realTimeNode );
	tempParser.SetAttrNode( "VideoPID",videoPID,realTimeNode );
	tempParser.SetAttrNode( "AudioPID",audioPID,realTimeNode );
	tempParser.SetAttrNode( "DataType",dataType,realTimeNode );
	tempParser.SetAttrNode( "Width",Width,realTimeNode );
	tempParser.SetAttrNode( "Height",Height,realTimeNode );
	tempParser.SetAttrNode( "Fps",Fps,realTimeNode );
	tempParser.SetAttrNode( "Bps",Bps,realTimeNode);
	tempParser.SetAttrNode( "ThreeDSet",ThreeDSet,realTimeNode);

	//���浽�ַ���
	tempParser.SaveToString( strStandardXML );
	return true;
}
//���ر�׼����Ƶ�ֲ�����XML
bool TranslateDownXMLForTHREED::TranslateStreamRoundQuery(const std::string& strOriginalXML,std::string& strStandardXML,bool multi)
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
	if(!strcmp(downType.c_str(),"3DDown"))
	{
		downType="THREED";
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
	string Width, Height,Fps,Bps,ThreeDSet;
	string dataType;

	pXMLNODE childNode=parser.GetNodeFromPath("Msg/StreamRoundInfoQuery/RoundStream");
	parser.GetAttrNode(childNode,"RoundTime",RoundTime);

	pXMLNODE tempNode=tempParser.GetNodeFromPath("Msg/StreamRoundQuery");
	tempParser.SetAttrNode("DeviceID",deviceID,tempNode);
	tempParser.SetAttrNode("RoundTime",RoundTime,tempNode);

	pXMLNODELIST roundStreamList=parser.GetNodeList(childNode);
	for(int i=0;i<roundStreamList->Size();++i)
	{
		pXMLNODE tempChannelNode=tempParser.CreateNodePtr("Msg/StreamRoundQuery","RoundChannel");
		pXMLNODE channelNode=parser.GetNextNode(roundStreamList);
		parser.GetAttrNode( channelNode,"Freq",freq );
		parser.GetAttrNode( channelNode,"ServiceID",serviceID );
		parser.GetAttrNode( channelNode,"VideoPID",videoPID );
		parser.GetAttrNode( channelNode,"AudioPID",audioPID);
		parser.GetAttrNode(childNode,"Width",Width);
		parser.GetAttrNode(childNode,"Height",Height);
		parser.GetAttrNode(childNode,"Fps",Fps);
		parser.GetAttrNode(childNode,"Bps",Bps);
		parser.GetAttrNode(childNode,"ThreeDSet",ThreeDSet);
		//��ȡOrgNetID,tsID
		CHANNELINFOMGR::instance()->GetOrgNetID(THREED,freq,audioPID,videoPID,serviceID,OrgNetID );
		CHANNELINFOMGR::instance()->GetTsID(THREED,freq,audioPID,videoPID,serviceID,tsID);


		tempParser.SetAttrNode( "Freq",freq,tempChannelNode );
		tempParser.SetAttrNode("Code",code,tempChannelNode);
		tempParser.SetAttrNode( "OrgNetID",OrgNetID,tempChannelNode );
		tempParser.SetAttrNode( "TsID",tsID,tempChannelNode );
		tempParser.SetAttrNode( "ServiceID",serviceID,tempChannelNode );
		tempParser.SetAttrNode( "VideoPID",videoPID,tempChannelNode );
		tempParser.SetAttrNode( "AudioPID",audioPID,tempChannelNode );
		tempParser.SetAttrNode( "DataType",dataType,tempChannelNode );
		tempParser.SetAttrNode( "Width",Width,tempChannelNode );
		tempParser.SetAttrNode( "Height",Height,tempChannelNode );
		tempParser.SetAttrNode( "Fps",Fps,tempChannelNode );
		tempParser.SetAttrNode( "Bps",Bps,tempChannelNode );
		tempParser.SetAttrNode( "ThreeDSet",ThreeDSet,tempChannelNode);

	}
	//���浽�ַ���
	tempParser.SaveToString( strStandardXML );
	return true;
}
//���ر�׼��¼����������XML
bool TranslateDownXMLForTHREED::TranslateRecordSet(const std::string& strOriginalXML,std::string& strStandardXML)
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
	if (!strcmp(downType.c_str(),"3DDown") )
	{
		downType = "THREED";
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
				string DeviceID,OrgNetID,TsID,ServiceID,Width,Height,Fps,Bps,Code,TaskID="0",VideoPID,AudioPID,Freq,DataType,Index,ThreeDSet;
				string DayofWeek,StartTime,EndTime,StartDateTime,EndDateTime;
				pXMLNODE recordNode=tempParser.CreateNodePtr(autoRecordSetNode,"Record");
				pXMLNODE childNode=parser.GetNextNode(nodeList);

				parser.GetAttrNode(childNode,"Index",Index);
				parser.GetAttrNode(childNode,"Freq",Freq);
				parser.GetAttrNode(childNode,"VideoPID",VideoPID);
				parser.GetAttrNode(childNode,"AudioPID",AudioPID);
				parser.GetAttrNode(childNode,"ServiceID",ServiceID);
				parser.GetAttrNode(childNode,"Width",Width);
				parser.GetAttrNode(childNode,"Height",Height);
				parser.GetAttrNode(childNode,"Fps",Fps);
				parser.GetAttrNode(childNode,"Bps",Bps);
				parser.GetAttrNode(childNode,"ThreeDSet",ThreeDSet);
				
				PROPMANAGER::instance()->GetDeviceIndex(DeviceID,Index,"THREED");

				CHANNELINFOMGR::instance()->GetTsID(THREED,Freq,AudioPID,VideoPID,ServiceID,TsID);
				CHANNELINFOMGR::instance()->GetOrgNetID(THREED,Freq,AudioPID,VideoPID,ServiceID,OrgNetID);

				tempParser.SetAttrNode("TaskID",TaskID,recordNode);
				tempParser.SetAttrNode("Action",action,recordNode);
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
				tempParser.SetAttrNode("ThreeDSet",ThreeDSet,recordNode);


				tempParser.SetAttrNode("DayofWeek",DayofWeek,recordNode);
				tempParser.SetAttrNode("StartTime",StartTime,recordNode);
				tempParser.SetAttrNode("EndTime",EndTime,recordNode);
				tempParser.SetAttrNode("StartDateTime",StartDateTime,recordNode);
				tempParser.SetAttrNode("EndDateTime",EndDateTime,recordNode);
			}

		}
	}
	else if(strOriginalXML.find("ProvisionalRecordTaskSet")!=string::npos)
	{
		pXMLNODE taskRecordSetNode=tempParser.CreateNodePtr("Msg","TaskRecordSet");
		string Action;

		pXMLNODE proRecordTaskSetNode=parser.GetNodeFromPath("Msg/ProvisionalRecordTaskSet");
		pXMLNODELIST proRecordTaskSetList=parser.GetNodeList(proRecordTaskSetNode);

		for (int j=0;j<proRecordTaskSetList->Size();++j)
		{
			string DeviceID,Width,Height,Fps,Bps,TaskID,Index,ThreeDSet;
			pXMLNODE taskRecordNode=parser.GetNextNode(proRecordTaskSetList);
			parser.GetAttrNode(taskRecordNode,"TaskID",TaskID);
			parser.GetAttrNode(taskRecordNode,"Action",Action);
			parser.GetAttrNode(taskRecordNode,"Index",Index);
			parser.GetAttrNode(taskRecordNode,"Width",Width);
			parser.GetAttrNode(taskRecordNode,"Height",Height);
			parser.GetAttrNode(taskRecordNode,"Fps",Fps);
			parser.GetAttrNode(taskRecordNode,"Bps",Bps);
			parser.GetAttrNode(taskRecordNode,"ThreeDSet",ThreeDSet);
			PROPMANAGER::instance()->GetDeviceIndex(DeviceID,Index,"THREED");

			if (Action=="Del" || Action=="del")
			{
				pXMLNODE recordNode=tempParser.CreateNodePtr(taskRecordSetNode,"Record");
				tempParser.SetAttrNode("TaskID",TaskID,recordNode);
				tempParser.SetAttrNode("Action",Action,recordNode);
				continue;
			}

			pXMLNODELIST taskRecordList=parser.GetNodeList(taskRecordNode);
			for (int k=0;k<taskRecordList->Size();++k)
			{
				string OrgNetID,TsID,ServiceID,VideoPID,AudioPID,Freq,Code,DataType;
				string DayofWeek,StartTime,EndTime,StartDateTime,EndDateTime;
				pXMLNODE recordNode=tempParser.CreateNodePtr(taskRecordSetNode,"Record");
				pXMLNODE childNode=parser.GetNextNode(taskRecordList);

				parser.GetAttrNode(childNode,"Freq",Freq);
				parser.GetAttrNode(childNode,"VideoPID",VideoPID);
				parser.GetAttrNode(childNode,"AudioPID",AudioPID);
				parser.GetAttrNode(childNode,"ServiceID",ServiceID);

				CHANNELINFOMGR::instance()->GetTsID(THREED,Freq,AudioPID,VideoPID,ServiceID,TsID);
				CHANNELINFOMGR::instance()->GetOrgNetID(THREED,Freq,AudioPID,VideoPID,ServiceID,OrgNetID);
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
				tempParser.SetAttrNode("ThreeDSet",ThreeDSet,recordNode);

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
bool TranslateDownXMLForTHREED::TranslateRecordTask(const std::string& strOriginalXML,std::vector<std::string>& strStandardXMLVec)
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
		string Action,DeviceID,OrgNetID,TsID,ServiceID,Width,Height,Fps,Bps,Code,TaskID,VideoPID,AudioPID,Freq,DataType,ThreeDSet;
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
		parser.GetAttrNode(childNode,"ThreeDSet",ThreeDSet);

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
		tempParser.SetAttrNode("ThreeDSet",ThreeDSet,recordNode);

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
bool TranslateDownXMLForTHREED::TranslateRecordQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	//��ʼ������ֵ
	std::string strVersion,strMsgID,strType,strProtocol,strSetDateTime,strSrcCode,strDstCode,strSrcURL,strPriority;
	std::string TaskID, OrgNetID, tsID, serviceID,VideoID,AudioID,code, StartDateTime, EndDateTime, DeviceID;
	std::string finalXml;

	//��Ƶ��ѯ��׼��XML�ṹ
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser( strOriginalXML.c_str());
	XmlParser tempParser( source );

	//��ȡԭʼXML��Ԫ��
	pXMLNODE rootNode = parser.GetNodeFromPath("Msg");
	parser.GetAttrNode(rootNode,"Version",strVersion);         //��ȡMsg�ڵ��Version����ֵ
	parser.GetAttrNode(rootNode,"MsgID",strMsgID);		       //��ȡMsg�ڵ��MsgID����ֵ
	parser.GetAttrNode(rootNode,"Type",strType);		       //��ȡMsg�ڵ��Type����ֵ
	parser.GetAttrNode(rootNode,"DateTime",strSetDateTime);    //��ȡMsg�ڵ��DateTime����ֵ��
	parser.GetAttrNode(rootNode,"SrcCode",strSrcCode);         //��ȡMsg�ڵ��StrCode����ֵ
	parser.GetAttrNode(rootNode,"DstCode",strDstCode);         //��ȡMsg�ڵ��DstCode����ֵ
	parser.GetAttrNode(rootNode,"SrcURL",strSrcURL);		   //��ȡMsg�ڵ��SrcUrl����ֵ	   
	parser.GetAttrNode(rootNode,"Priority",strPriority);       //��ȡMsg�ڵ��Priority����ֵ

	pXMLNODE childNode = parser.GetNodeFirstChild( rootNode );		   
	std::string nodeName = parser.GetNodeName( childNode );      //��ȡMsg���ӽڵ�����
	if (nodeName =="NVRVideoHistoryDownInquiry")
	{
		nodeName = "AutoRecordFileQuery";                        //�޸Ĳ�ѯ���ͽڵ�����
	}
	else if (nodeName =="NVRVideoHistoryInquiry")
	{
		nodeName = "AutoRecordQuery";                            //�޸Ĳ�ѯ���ͽڵ�����
	}
	else if (nodeName =="NVRTaskRecordInquiry")
	{
		nodeName = "TaskRecordQuery";                            //�޸Ĳ�ѯ���ͽڵ�����
	}
	else if (nodeName =="NVRTaskRecordDownInquiry")
	{
		nodeName = "TaskRecordFileQuery";                        //�޸Ĳ�ѯ���ͽڵ�����
	}
	char * taskType = const_cast<char *>(nodeName.c_str());
	pXMLNODE recordNode = parser.GetNodeFirstChild( childNode );

	//���ø�����
	pXMLNODE tempRootNode = tempParser.GetNodeFromPath( "Msg" );
	tempParser.SetAttrNode("Version",strVersion,tempRootNode);               //����Msg�ڵ��Version����ֵ
	tempParser.SetAttrNode("MsgID",strMsgID,tempRootNode);                   //����Msg�ڵ��MsgID����ֵ
	if (strType =="3DDown")
	{
		tempParser.SetAttrNode("DVBType",string("THREED"),tempRootNode);           //����Msg�ڵ��DVBType����ֵ
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
		parser.GetAttrNode( recordNode,"Index",DeviceID);                    //��ȡIndex����ֵ
		PROPMANAGER::instance()->GetDeviceIndex(DeviceID,DeviceID,"THREED");	//���߼�ͨ��ת��Ϊʵ��ͨ��
		parser.GetAttrNode( recordNode,"StartDateTime",StartDateTime );       //��ȡStartDateTime����ֵ
		parser.GetAttrNode( recordNode,"EndDateTime",EndDateTime );           //��ȡEndDateTime����ֵ
	}
	else if (nodeName.compare("TaskRecordQuery") == 0)
	{
		parser.GetAttrNode( recordNode,"TaskID",TaskID );                    //��ȡTaskID����ֵ
		parser.GetAttrNode( recordNode,"StartDateTime",StartDateTime );       //��ȡStartDateTime����ֵ
		parser.GetAttrNode( recordNode,"EndDateTime",EndDateTime );           //��ȡEndDateTime����ֵ
	}
	else if (nodeName.compare("TaskRecordFileQuery")==0)
	{
		parser.GetAttrNode( recordNode,"TaskID",TaskID );                    //��ȡTaskID����ֵ

	}
	//����¼����������
	try
	{
		tempParser.SetAttrNode( "TaskID",TaskID,recordTaskNode );               //����RecordQueryTask�ڵ��TaskID����ֵ
		tempParser.SetAttrNode( "OrgNetID",OrgNetID,recordTaskNode );           //����RecordQueryTask�ڵ��OrgNetID����ֵ
		tempParser.SetAttrNode( "TsID",tsID,recordTaskNode );                   //����RecordQueryTask�ڵ��TsID����ֵ
		tempParser.SetAttrNode( "ServiceID",serviceID,recordTaskNode );         //����RecordQueryTask�ڵ��ServiceID����ֵ
		tempParser.SetAttrNode( "VideoPID",VideoID,recordTaskNode );             //����RecordQueryTask�ڵ��VideoID����ֵ
		tempParser.SetAttrNode( "AudioPID",AudioID,recordTaskNode);              //����RecordQueryTask�ڵ��AudioID����ֵ
		tempParser.SetAttrNode( "Code",code,recordTaskNode);                    //����RecordQueryTask�ڵ��Code����ֵ
		tempParser.SetAttrNode( "DeviceID",DeviceID,recordTaskNode );           //����RecordQueryTask�ڵ��DeviceID����ֵ
		tempParser.SetAttrNode( "StartDateTime",StartDateTime,recordTaskNode ); //����RecordQueryTask�ڵ��StartDateTime����ֵ
		tempParser.SetAttrNode( "EndDateTime",EndDateTime,recordTaskNode );     //����RecordQueryTask�ڵ��EndDateTime����ֵ
	}
	catch(...)
	{
		string error ="���ñ�׼XML�ĵ�Ԫ������ʧ��";
		APPLOG::instance()->WriteLog( OTHER,LOG_EVENT_ERROR,error,LOG_OUTPUT_BOTH );
		return false;
	}
	//���浽�ַ���
	tempParser.SaveToString( strStandardXML);
	return true;
}

//���ر�׼�Ă���¼����������XML
bool TranslateDownXMLForTHREED::TranslateRoundRecordSet(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser originalParser( strOriginalXML.c_str() );
	XmlParser tempParser(source);

	//��ȡ��Ԫ��
	pXMLNODE orgRootNode=originalParser.GetRootNode();
	pXMLNODE tempRootNode=tempParser.GetRootNode();

	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	originalParser.GetAttrNode(orgRootNode,"Version",version);
	originalParser.GetAttrNode(orgRootNode,"MsgID",msgid);
	originalParser.GetAttrNode(orgRootNode,"Type",downtype);
	originalParser.GetAttrNode(orgRootNode,"Protocol",protocol);
	originalParser.GetAttrNode(orgRootNode,"DateTime",datetime);
	originalParser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	originalParser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	originalParser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	originalParser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="3DDown")
	{
		downtype="THREED";
	}
	else
	{
		return false;
	}

	string tasktype="RoundRecordSet";
	tempParser.SetAttrNode("Version",	version,	tempRootNode);
	tempParser.SetAttrNode("MsgID",		msgid,		tempRootNode);
	tempParser.SetAttrNode("DVBType",	downtype,	tempRootNode);
	tempParser.SetAttrNode("TaskType",	tasktype,	tempRootNode);
	tempParser.SetAttrNode("Protocol",	protocol,	tempRootNode);
	tempParser.SetAttrNode("DateTime",	datetime,	tempRootNode);
	tempParser.SetAttrNode("SrcCode",	srccode,	tempRootNode);
	tempParser.SetAttrNode("DstCode",	dstcode,	tempRootNode);
	tempParser.SetAttrNode("SrcURL",	srcurl,		tempRootNode);
	tempParser.SetAttrNode("Priority",	priority,	tempRootNode);

	//TaskID="100" Action="Set" Index="1" StartDateTime="" EndDateTime="" RoundTime=""
	std::string taskid,action,index,startdatetime,enddatetime,roundtime;
	int orgRdRecSetCount = originalParser.GetChildCount(orgRootNode);	//RoundRecordSet�ڵ�ĸ���
	pXMLNODELIST RdRecSetList = originalParser.GetNodeList(orgRootNode);//RoundRecordSet�ڵ��б�
	for (int nSet=0; nSet<orgRdRecSetCount; ++nSet)
	{
		pXMLNODE RdRecSetNode = originalParser.GetNextNode(RdRecSetList);	//<RoundRecordSet/>
		originalParser.GetAttrNode(RdRecSetNode, "TaskID",			taskid);
		originalParser.GetAttrNode(RdRecSetNode, "Action",			action);
		originalParser.GetAttrNode(RdRecSetNode, "Index",			index);
		originalParser.GetAttrNode(RdRecSetNode, "StartDateTime",	startdatetime);
		originalParser.GetAttrNode(RdRecSetNode, "EndDateTime",		enddatetime);
		originalParser.GetAttrNode(RdRecSetNode, "RoundTime",		roundtime);

		//TaskID="100" Action="Set" DeviceID="1" StartDateTime="" EndDateTime="" RoundTime=""
		pXMLNODE tempRdRecSetNode = tempParser.CreateNodePtr(tempRootNode, "RoundRecordSet");	//����RoundRecordSet�ڵ�
		tempParser.SetAttrNode("TaskID",		taskid,			tempRdRecSetNode);
		tempParser.SetAttrNode("Action",		action,			tempRdRecSetNode);
		tempParser.SetAttrNode("DeviceID",		index,			tempRdRecSetNode);
		tempParser.SetAttrNode("StartDateTime", startdatetime,	tempRdRecSetNode);
		tempParser.SetAttrNode("EndDateTime",	enddatetime,	tempRdRecSetNode);
		tempParser.SetAttrNode("RoundTime",		roundtime,		tempRdRecSetNode);
		if ("Del" == action)	//�����del������������;  ������!!!!!!!!!!!
		{
			continue;
		}
		//<Channel Freq="395000" SymbolRate="" QAM="" ServiceID="2012" VideoPID="108" AudioPID="109" />
		std::string freq,symbolrate,qam,serviceid,videopid,audiopid;
		int orgChanCount = originalParser.GetChildCount(RdRecSetNode);
		pXMLNODELIST orgChanList = originalParser.GetNodeList(RdRecSetNode);
		for (int nChan=0; nChan<orgChanCount; ++nChan)
		{
			pXMLNODE orgChanNode = originalParser.GetNextNode(orgChanList);
			originalParser.GetAttrNode(orgChanNode,	"Freq",			freq);
			originalParser.GetAttrNode(orgChanNode,	"SymbolRate",	symbolrate);
			originalParser.GetAttrNode(orgChanNode,	"QAM",			qam);
			originalParser.GetAttrNode(orgChanNode,	"ServiceID",	serviceid);
			originalParser.GetAttrNode(orgChanNode,	"VideoPID",		videopid);
			originalParser.GetAttrNode(orgChanNode,	"AudioPID",		audiopid);

			//<Channel Freq="395000" SymbolRate="" QAM="" ServiceID="2012" VideoPID="108" AudioPID="109" />
			pXMLNODE tempChanNode = tempParser.CreateNodePtr(tempRdRecSetNode, "Channel");
			tempParser.SetAttrNode("Freq",			freq,		tempChanNode);
			tempParser.SetAttrNode("SymbolRate",	symbolrate,	tempChanNode);
			tempParser.SetAttrNode("QAM",			qam,		tempChanNode);
			tempParser.SetAttrNode("ServiceID",		serviceid,	tempChanNode);
			tempParser.SetAttrNode("VideoPID",		videopid,	tempChanNode);
			tempParser.SetAttrNode("AudioPID",		audiopid,	tempChanNode);
		}
	}

	tempParser.SaveToString(strStandardXML);
	return true;
}
//���ر�׼�Ă���¼��ִ������XML
bool TranslateDownXMLForTHREED::TranslateRoundRecordTask(const std::string& strOriginalXML,std::vector<std::string>& strStandardXMLVec)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><RoundRecord></RoundRecord></Msg>";

	XmlParser orgXmlParser( strOriginalXML.c_str() );
	pXMLNODE orgRootNode = orgXmlParser.GetRootNode();
	std::string version,msgid,dvbtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	orgXmlParser.GetAttrNode(orgRootNode, "Version", version);
	orgXmlParser.GetAttrNode(orgRootNode, "MsgID", msgid);
	orgXmlParser.GetAttrNode(orgRootNode, "DVBType", dvbtype);
	orgXmlParser.GetAttrNode(orgRootNode, "Protocol", protocol);
	orgXmlParser.GetAttrNode(orgRootNode, "DateTime", datetime);
	orgXmlParser.GetAttrNode(orgRootNode, "SrcCode", srccode);
	orgXmlParser.GetAttrNode(orgRootNode, "DstCode", dstcode);
	orgXmlParser.GetAttrNode(orgRootNode, "SrcURL", srcurl);
	orgXmlParser.GetAttrNode(orgRootNode, "Priority", priority);
	//std::string tasktype = "RoundRecord";

	int RdRecSetCount = orgXmlParser.GetChildCount(orgRootNode);
	pXMLNODELIST RdRecSetList = orgXmlParser.GetNodeList(orgRootNode);
	for (int nSet=0;nSet<RdRecSetCount;++nSet)
	{
		XmlParser tempparser( source );
		pXMLNODE tempMsgNode = tempparser.GetRootNode();
		tempparser.SetAttrNode("Version", version, tempMsgNode);
		tempparser.SetAttrNode("MsgID", msgid, tempMsgNode);
		tempparser.SetAttrNode("DVBType", dvbtype, tempMsgNode);
		tempparser.SetAttrNode("TaskType",string("RoundRecordTask"),tempMsgNode);
		tempparser.SetAttrNode("Protocol", protocol, tempMsgNode);
		tempparser.SetAttrNode("DateTime", datetime, tempMsgNode);
		tempparser.SetAttrNode("SrcCode", srccode, tempMsgNode);
		tempparser.SetAttrNode("DstCode", dstcode, tempMsgNode);
		tempparser.SetAttrNode("SrcURL", srcurl, tempMsgNode);
		tempparser.SetAttrNode("Priority", priority, tempMsgNode);
		if (protocol.length()<1)
		{
			protocol = "HTTP";
		}

		pXMLNODE RdRecSetNode = orgXmlParser.GetNextNode(RdRecSetList);

		//<RoundRecordSet TaskID="100" Action="Set" DeviceID="1" StartDateTime="" EndDateTime="" RoundTime="">    
		std::string taskid,action,startdatetime,enddatetime,roundtime,deviceid;
		orgXmlParser.GetAttrNode(RdRecSetNode, "TaskID", taskid);
		orgXmlParser.GetAttrNode(RdRecSetNode, "Action", action);
		orgXmlParser.GetAttrNode(RdRecSetNode, "DeviceID", deviceid);
		orgXmlParser.GetAttrNode(RdRecSetNode, "StartDateTime", startdatetime);
		orgXmlParser.GetAttrNode(RdRecSetNode, "EndDateTime", enddatetime);
		orgXmlParser.GetAttrNode(RdRecSetNode, "RoundTime", roundtime);

		pXMLNODE tempSetNode = tempparser.GetNodeFromPath("Msg/RoundRecord");
		tempparser.SetAttrNode("TaskID", taskid, tempSetNode);
		tempparser.SetAttrNode("Action", action, tempSetNode);
		tempparser.SetAttrNode("DeviceID", deviceid, tempSetNode);
		tempparser.SetAttrNode("StartDateTime", startdatetime, tempSetNode);
		tempparser.SetAttrNode("EndDateTime", enddatetime, tempSetNode);
		tempparser.SetAttrNode("RoundTime", roundtime, tempSetNode);

		int ChanCount = orgXmlParser.GetChildCount(RdRecSetNode);
		pXMLNODELIST ChanList = orgXmlParser.GetNodeList(RdRecSetNode);
		for (int nChan=0;nChan<ChanCount;++nChan)
		{
			//<Channel Freq="395000" SymbolRate="" QAM="" ServiceID="2012" VideoPID="108" AudioPID="109" />   
			std::string freq,symbolrate,qam,serviceid,videopid,audiopid,OrgNetID,TsID;
			pXMLNODE orgChanNode = orgXmlParser.GetNextNode(ChanList);
			orgXmlParser.GetAttrNode(orgChanNode, "Freq", freq);
			orgXmlParser.GetAttrNode(orgChanNode, "SymbolRate", symbolrate);
			orgXmlParser.GetAttrNode(orgChanNode, "QAM", qam);
			orgXmlParser.GetAttrNode(orgChanNode, "ServiceID", serviceid);
			orgXmlParser.GetAttrNode(orgChanNode, "VideoPID", videopid);
			orgXmlParser.GetAttrNode(orgChanNode, "AudioPID", audiopid);

			enumDVBType Downtype = OSFunction::GetEnumDVBType(dvbtype);
			CHANNELINFOMGR::instance()->GetOrgNetID(Downtype,freq,audiopid,videopid,serviceid,OrgNetID);
			CHANNELINFOMGR::instance()->GetTsID(Downtype,freq,audiopid,videopid,serviceid,TsID);
			pXMLNODE tempChanNode = orgXmlParser.CreateNodePtr(tempSetNode, "Channel");
			tempparser.SetAttrNode("Freq", freq, tempChanNode);
			tempparser.SetAttrNode("OrgNetID", OrgNetID, tempChanNode);
			tempparser.SetAttrNode("TsID", TsID, tempChanNode);
			tempparser.SetAttrNode("SymbolRate", symbolrate, tempChanNode);
			tempparser.SetAttrNode("QAM", qam, tempChanNode);
			tempparser.SetAttrNode("ServiceID", serviceid, tempChanNode);
			tempparser.SetAttrNode("VideoPID", videopid, tempChanNode);
			tempparser.SetAttrNode("AudioPID", audiopid, tempChanNode);
		}

		std::string tempxml;
		tempparser.SaveToString(tempxml);
		strStandardXMLVec.push_back(tempxml);
	}
	return true;
}


//���ر�׼��ʵʱָ���ѯXML
bool TranslateDownXMLForTHREED::TranslateQualityRealtimeQuery( const std::string& strOriginalXML,std::string& strStandardXML)
{
	string downType;//����������

	//��Ƶ��ѯ��׼��XML�ṹ
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><QualityRealtimeQuery><QualityQueryTask/><QualityParam></QualityParam></QualityRealtimeQuery></Msg>";

	XmlParser parser( strOriginalXML.c_str());
	XmlParser tempParser( source );

	//��ȡ��Ԫ��
	pXMLNODE rootNode = parser.GetNodeFromPath("Msg");
	pXMLNODE childNode = parser.GetNodeFirstChild(rootNode);;
	parser.GetAttrNode(rootNode,"Type",downType);

	//��ʼ������ֵ
	string dateTime, priority;
	string taskID,deviceID, freq,STD,symbolRate,action,msgid,version,protocol,srccode,dstcode,srcurl;
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
	parser.GetAttrNode(rootNode,"Protocol",protocol);
	parser.GetAttrNode( rootNode,"DateTime",dateTime );
	parser.GetAttrNode(rootNode,"SrcCode",srccode);
	parser.GetAttrNode(rootNode,"DstCode",dstcode);
	parser.GetAttrNode(rootNode,"SrcURL",srcurl);
	parser.GetAttrNode( rootNode,"Priority",priority );

	if(downType=="3DDown")
	{
		downType="THREED";
	}
	else
	{
		return false;
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
	tempParser.SetAttrNode( "Priority",priority,tempRootNode );

	//��ȡ��Ԫ������
	parser.GetAttrNode( childNode,"Freq",freq );
	parser.GetAttrNode(childNode,"SymbolRate",symbolRate);


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
		tempParser.SetAttrNode( "SymbolRate",symbolRate,qualityTaskNode);

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
	tempParser.SaveToString( strStandardXML );

	return true;
}
//���ر�׼�Ķ�ʱָ����������xml
bool TranslateDownXMLForTHREED::TranslateQualityTaskSet(const std::string& strOriginalXML,std::string& strStandardXML)
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
	if (strType =="3DDown")
	{
		tempParser.SetAttrNode("DVBType",string("THREED"),tempRootNode);           //����Msg�ڵ��DVBType����ֵ
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
	pXMLNODE channelNode = parser.GetNodeFromPath("Msg/QualityTaskSet/QualityReport/Channel");      //��ȡԭʼXML��Channel�ڵ�
	pXMLNODE qualityNode = parser.GetNodeFromPath("Msg/QualityTaskSet/QualityReport/QualityParam"); //��ȡԭʼXML��QualityParam�ڵ�

	pXMLNODE querySetNode = tempParser.CreateNodePtr(tempRootNode,"QualityTaskSet");  //����QualityTaskSet�ڵ�
	pXMLNODE taskParmNode = tempParser.CreateNodePtr(querySetNode,"TaskParm");         //����TaskParm�ڵ�
	pXMLNODE qualityParamNode = tempParser.CreateNodePtr(querySetNode,"QualityParam"); //����QualityParam�ڵ�
	pXMLNODE timeParamNode =  tempParser.CreateNodePtr(querySetNode,"TimeParam");      //����TimeParam�ڵ�
	parser.GetAttrNode(qualityReportNode,"TaskID",TaskID);							   //��ȡԭʼXML��QualityReport�ڵ��TaskID����
	parser.GetAttrNode(qualityReportNode,"Action",Action);                             //��ȡԭʼXML��QualityReport�ڵ��Action����
	tempParser.SetAttrNode("TaskID",TaskID,querySetNode);                              //����QualityQuerySet�ڵ��TaskID����
	tempParser.SetAttrNode("Action",Action,querySetNode);                              //����QualityQuerySet�ڵ��Action����

	std::string STD,Freq,SymbolRate;
	pXMLNODELIST ChanneList=parser.GetNodeList(channelNode);
	for ( int i = 0;i<parser.GetChildCount(channelNode);i++)
	{  
		pXMLNODE channelInfo = parser.GetNextNode(ChanneList);
		parser.GetAttrNode(channelInfo,"STD",STD);                                     //��ȡChannel�ڵ��STD����
		parser.GetAttrNode(channelInfo,"Freq",Freq);								   //��ȡChannel�ڵ��Freq����
		parser.GetAttrNode(channelInfo,"SymbolRate",SymbolRate);					   //��ȡChannel�ڵ��SymbolRate����
		pXMLNODE taskNode =tempParser.CreateNodePtr(taskParmNode,"QualityQueryTask");  //����TaskParm�ڵ���ӽڵ�
		tempParser.SetAttrNode("STD",STD,taskNode);                                    //����QualityQueryTask�ڵ��STD����
		tempParser.SetAttrNode("Freq",Freq,taskNode);                                  //����QualityQueryTask�ڵ��Freq����
		tempParser.SetAttrNode("SymbolRate",SymbolRate,taskNode);                      //����QualityQueryTask�ڵ��SymbolRate����
	}
	std::string Type,Desc;
	pXMLNODELIST QualityList=parser.GetNodeList(qualityNode);
	for (int j = 0;j<parser.GetChildCount(qualityNode);j++)
	{
		pXMLNODE qualityIndex = parser.GetNextNode(QualityList);
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
	tempParser.SaveToString(strStandardXML);
	return true;
}
//���ر�׼�Ķ�ʱָ��ִ������xml
bool TranslateDownXMLForTHREED::TranslateQualityTask(const std::string& strOriginalXML,std::vector<std::string>& strStandardXMLVec)
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
		tempParser.SetAttrNode("DVBType",string("THREED"),tempRootNode);               //����Msg�ڵ��DVBType����ֵ
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
		pXMLNODELIST taskList=parser.GetNodeList(taskNode);
		for ( int i = 0;i<parser.GetChildCount(taskNode);i++)
		{  
			pXMLNODE taskInfo = parser.GetNextNode(taskList);
			parser.GetAttrNode(taskInfo,"STD",STD);                                     //��ȡTaskParm�ڵ��STD����
			parser.GetAttrNode(taskInfo,"Freq",Freq);								   //��ȡTaskParm�ڵ��Freq����
			parser.GetAttrNode(taskInfo,"SymbolRate",SymbolRate);					   //��ȡTaskParm�ڵ��SymbolRate����
			pXMLNODE taskNode =tempParser.CreateNodePtr(taskParmNode,"QualityQueryTask");  //����TaskParm�ڵ���ӽڵ�
			tempParser.SetAttrNode("STD",STD,taskNode);                                    //����QualityQueryTask�ڵ��STD����
			tempParser.SetAttrNode("Freq",Freq,taskNode);                                  //����QualityQueryTask�ڵ��Freq����
			tempParser.SetAttrNode("SymbolRate",SymbolRate,taskNode);                      //����QualityQueryTask�ڵ��SymbolRate����
		}
		std::string Type,Desc;
		pXMLNODELIST QualityList=parser.GetNodeList(qualityNode);
		for (int j = 0;j<parser.GetChildCount(qualityNode);j++)
		{
			pXMLNODE qualityIndex = parser.GetNextNode(QualityList);
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
bool TranslateDownXMLForTHREED::TranslateQualityQuery(const std::string& strOriginalXML,std::string& strStandardXML)
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
	parser.GetAttrNode(rootNode,"SrcURL",strSrcURL);		   //��ȡMsg�ڵ��SrcUrl����ֵ


	pXMLNODE tempRootNode = tempParser.GetNodeFromPath( "Msg" );         
	tempParser.SetAttrNode("Version",strVersion,tempRootNode);               //����Msg�ڵ��Version����ֵ
	tempParser.SetAttrNode("MsgID",strMsgID,tempRootNode);                   //����Msg�ڵ��MsgID����ֵ
	if (strType =="3DDown")
	{
		tempParser.SetAttrNode("DVBType",string("THREED"),tempRootNode);           //����Msg�ڵ��DVBType����ֵ
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
bool TranslateDownXMLForTHREED::TranslateChannelSet(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser( strOriginalXML.c_str());
	XmlParser tempParser( source );

	std::string strVersion,strMsgID,strType,strProtocol,strSetDateTime,strSrcCode,
		strDstCode,strSrcURL,strPriority;

	pXMLNODE rootNode = parser.GetNodeFromPath("Msg");         //��ȡԭʼXML��Ԫ��
	parser.GetAttrNode(rootNode,"Version",strVersion);         //��ȡMsg�ڵ��Version����ֵ
	parser.GetAttrNode(rootNode,"MsgID",strMsgID);		       //��ȡMsg�ڵ��MsgID����ֵ
	parser.GetAttrNode(rootNode,"Type",strType);		       //��ȡMsg�ڵ��Type����ֵ
	parser.GetAttrNode(rootNode,"DateTime",strSetDateTime);    //��ȡMsg�ڵ��DateTime����ֵ��
	parser.GetAttrNode(rootNode,"SrcCode",strSrcCode);         //��ȡMsg�ڵ��StrCode����ֵ
	parser.GetAttrNode(rootNode,"DstCode",strDstCode);         //��ȡMsg�ڵ��DstCode����ֵ
	parser.GetAttrNode(rootNode,"Priority",strPriority);       //��ȡMsg�ڵ��Priority����ֵ
	parser.GetAttrNode(rootNode,"SrcURL",strSrcURL);           //��ȡMsg�ڵ��SrcUrl����ֵ
	if (strType =="3DDown")
	{
		 strType = "THREED";          //����Msg�ڵ��DVBType����ֵ
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
bool TranslateDownXMLForTHREED::TranslateClientInfoQuery(const std::string& strOriginalXML,std::string& strStandardXML)
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


	if(downtype=="3DDown")
	{
		downtype="THREED";
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
bool TranslateDownXMLForTHREED::TranslateDeviceStatusQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><DeviceStatusQuery></DeviceStatusQuery></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//��ȡ��Ԫ��
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	string tasktype="DeviceStatusQuery";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="3DDown")
	{
		downtype="THREED";
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
		temParser.CreateNode("Msg/DeviceStatusQuery","DeviceStatus");
	}

	pXMLNODE    StatusInfo=temParser.GetNodeFromPath("Msg/DeviceStatusQuery");
	string index,equcode;
	temParser.SetAttrNode("Index",index,StatusInfo);
	temParser.SetAttrNode("EquCode",equcode,StatusInfo);
	//��ȡStatusIndex��Ԫ���б�
	pXMLNODELIST stdStatusIndexList = temParser.GetNodeList( StatusInfo );
	pXMLNODELIST orgStatusIndexList = parser.GetNodeList( childNode );
	string deviceid;

	try
	{
		//���÷���XML��StatusIndexԪ������
		for ( int j = 0;j< count;j++ )
		{
			pXMLNODE  orglistNode = parser.GetNextNode( orgStatusIndexList );
			pXMLNODE  stdListNode = temParser.GetNextNode( stdStatusIndexList );
			parser.GetAttrNode( orglistNode,"Index",deviceid );
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
//���ر�׼�ı��ѯ����xml
bool TranslateDownXMLForTHREED::TranslateTableQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><TableQuery></TableQuery></Msg>";
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
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="3DDown")
	{
		downtype="THREED";
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

	pXMLNODELIST nodeList=parser.GetNodeList(orgRootNode);
	for(int i=0;i<nodeList->Size();++i)
	{
		pXMLNODE stdTabelNode=temParser.CreateNodePtr("Msg/TableQuery","Table");
		pXMLNODE orgTableNode=parser.GetNextNode(nodeList);
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
	}

	//���浽�ַ���
	temParser.SaveToString( strStandardXML);
	return true;
}

//���ر�׼������ͼ��������xml
bool TranslateDownXMLForTHREED::TranslateRunPlanSet(const std::string& strOriginalXML,std::string& strStandardXML)
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

	if(downtype=="3DDown")
	{
		downtype="THREED";
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

	pXMLNODE stdRunSetNode=temParser.GetNodeFromPath("Msg/RunPlanSet");

	pXMLNODELIST orgRunPlanList = parser.GetNodeList( orgRootNode );
	int runPlanCount = parser.GetChildCount( orgRootNode );	//how many RunPlanSet node?
	try
	{
		for ( int runPlanIndex=0; runPlanIndex< runPlanCount; runPlanIndex++ )
		{
			pXMLNODE RunPlanSetNode = parser.GetNextNode( orgRunPlanList );

			string EquCode,Freq,OrgNetID,TsID,ServiceID,VideoPID, AudioPID;
			parser.GetAttrNode(RunPlanSetNode,"EquCode",EquCode);
			parser.GetAttrNode(RunPlanSetNode,"Freq",Freq);
			parser.GetAttrNode(RunPlanSetNode,"ServiceID",ServiceID);
			parser.GetAttrNode(RunPlanSetNode,"VideoPID",VideoPID);
			parser.GetAttrNode(RunPlanSetNode,"AudioPID",AudioPID);

			enumDVBType dvbtype = OSFunction::GetEnumDVBType(downtype);
			CHANNELINFOMGR::instance()->GetOrgNetID(dvbtype,Freq,AudioPID,VideoPID,ServiceID,OrgNetID);
			CHANNELINFOMGR::instance()->GetTsID(dvbtype,Freq,AudioPID,VideoPID,ServiceID,TsID);

			int timeParamCount = parser.GetChildCount(RunPlanSetNode);
			pXMLNODELIST timeParamList = parser.GetNodeList( RunPlanSetNode );
			for(int timeIndex=0; timeIndex<timeParamCount; timeIndex++)
			{
				pXMLNODE timeParamNode = parser.GetNextNode( timeParamList );

				string Month,Day,DayOfWeek,StartTime,EndTime,StartDateTime,EndDateTime;
				if(parser.GetNodeName(timeParamNode)=="MonthTime")
				{
					parser.GetAttrNode(timeParamNode,"Month",Month);
					parser.GetAttrNode(timeParamNode,"Day",Day);
					parser.GetAttrNode(timeParamNode,"StartTime",StartTime);
					parser.GetAttrNode(timeParamNode,"EndTime",EndTime);
				}
				else if(parser.GetNodeName(timeParamNode)=="WeeklyTime")
				{
					parser.GetAttrNode(timeParamNode,"DayofWeek",DayOfWeek);
					parser.GetAttrNode(timeParamNode,"StartTime",StartTime);
					parser.GetAttrNode(timeParamNode,"EndTime",EndTime);
				}
				else if(parser.GetNodeName(timeParamNode)=="DayTime")
				{
					parser.GetAttrNode(timeParamNode,"StartDateTime",StartDateTime);
					parser.GetAttrNode(timeParamNode,"EndDateTime",EndDateTime);
				}

				string RunPlanType, AlarmEndTime;
				parser.GetAttrNode(timeParamNode, "Type", RunPlanType);
				parser.GetAttrNode(timeParamNode, "AlarmEndTime", AlarmEndTime);

				string ValidStartDateTime,ValidEndDateTime;
				pXMLNODE stdListNode = temParser.CreateNodePtr(stdRunSetNode, "RunPlanChannel");
				temParser.SetAttrNode("EquCode",EquCode,stdListNode);
				temParser.SetAttrNode("OrgNetID",OrgNetID,stdListNode);
				temParser.SetAttrNode("TsID",TsID,stdListNode);
				temParser.SetAttrNode("ServiceID",ServiceID,stdListNode);
				temParser.SetAttrNode("VideoPID",VideoPID,stdListNode);
				temParser.SetAttrNode("AudioPID",AudioPID,stdListNode);
				temParser.SetAttrNode("Month",Month,stdListNode);
				temParser.SetAttrNode("DayOfMonth",Day,stdListNode);
				temParser.SetAttrNode("DayOfWeek",DayOfWeek,stdListNode);
				temParser.SetAttrNode("StartTime",StartTime,stdListNode);
				temParser.SetAttrNode("EndTime",EndTime,stdListNode);
				temParser.SetAttrNode("ValidStartDateTime",ValidStartDateTime,stdListNode);

				//��AlarmEndTime�ŵ�ValidEndDateTime��
				temParser.SetAttrNode("ValidEndDateTime",AlarmEndTime,stdListNode);
				temParser.SetAttrNode("StartDateTime",StartDateTime,stdListNode);
				temParser.SetAttrNode("EndDateTime",EndDateTime,stdListNode);
				temParser.SetAttrNode("Type", RunPlanType, stdListNode);
			}
		}
	}
	catch(...)
	{
		string error = "��������ͼ����ʧ��";
		APPLOG::instance()->WriteLog( OTHER,LOG_EVENT_ERROR,error,LOG_OUTPUT_BOTH );
		return false;

	}

	//���浽�ַ���
	temParser.SaveToString( strStandardXML);
	return true;
}
//���ر�׼���ֶ�¼������xml
bool TranslateDownXMLForTHREED::TranslateManualRecordSet(const std::string &strOriginalXML, std::string &strStandardXML)
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

	if(downtype=="3DDown")
	{
		downtype="THREED";
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
//���ر�׼������ҵ�����ʱ����������xml
bool TranslateDownXMLForTHREED::TranslateAutoAnalysisTimeSet(const std::string& strOriginalXML,std::string& strStandardXML)
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

	if(downtype=="3DDown")
	{
		downtype="THREED";
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

bool TranslateDownXMLForTHREED::TranslateEPGQuery( const std::string& strOriginalXML,std::string& strStandardXML )
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

	if(downtype=="3DDown")
	{
		downtype="THREED";
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

bool TranslateDownXMLForTHREED::TranslateMHPQuery( const std::string& strOriginalXML,std::string& strStandardXML )
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

	if(downtype=="3DDown")
	{
		downtype="THREED";
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

//���ر�׼�Ļ���ָ���ѯ����xml
bool TranslateDownXMLForTHREED::TranslateEnvironmentQualityQuery(const std::string& strOriginalXML,std::string& strStandardXML)
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

	if(downtype=="3DDown")
	{
		downtype="THREED";
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
//���ر�׼��Ƶ��ɨ������xml
bool TranslateDownXMLForTHREED::TranslateSpectrumScan(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><SpectrumScanQuery/></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//��ȡ��Ԫ��
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	string ScanTime,STD,SymbolRate,QAM,StartFreq,EndFreq,StepFreq;
	string tasktype="SpectrumScanQuery";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="3DDown")
	{
		downtype="THREED";
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

	pXMLNODE orgchildNode=parser.GetNodeFirstChild(orgRootNode);
	pXMLNODE stdchildNode=temParser.GetNodeFromPath("Msg/SpectrumScanQuery");
	parser.GetAttrNode(orgchildNode,"ScanTime",ScanTime);
	parser.GetAttrNode(orgchildNode,"STD",STD);
	parser.GetAttrNode(orgchildNode,"SymbolRate",SymbolRate);
	parser.GetAttrNode(orgchildNode,"QAM",QAM);
	parser.GetAttrNode(orgchildNode,"StartFreq",StartFreq);
	parser.GetAttrNode(orgchildNode,"EndFreq",EndFreq);
	parser.GetAttrNode(orgchildNode,"StepFreq",StepFreq);

	temParser.SetAttrNode("ScanTime",ScanTime,stdchildNode);
	temParser.SetAttrNode("STD",STD,stdchildNode);
	temParser.SetAttrNode("SymbolRate",SymbolRate,stdchildNode);
	temParser.SetAttrNode("QAM",QAM,stdchildNode);
	temParser.SetAttrNode("StartFreq",StartFreq,stdchildNode);
	temParser.SetAttrNode("EndFreq",EndFreq,stdchildNode);
	temParser.SetAttrNode("StepFreq",StepFreq,stdchildNode);

	//���浽�ַ���
	temParser.SaveToString( strStandardXML);
	return true;
}
//����ָ�겹����׼xml
bool TranslateDownXMLForTHREED::TranslateQualityCompensationSet(const std::string& strOriginalXML,std::string& strStandardXML)
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

	if(downtype=="3DDown")
	{
		downtype="THREED";
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
//���ر����������ñ�׼xml
bool TranslateDownXMLForTHREED::TranslateAlarmParamSet(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><AlarmParamSet></AlarmParamSet></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//��ȡ��Ԫ��
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	string ScanTime,QAM,StartFreq,EndFreq,StepFreq;
	string tasktype="AlarmParamSet";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="3DDown")
	{
		downtype="THREED";
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

	string STD, Freq,SymbolRate;
	pXMLNODE FreqNode=temParser.GetNodeFromPath("Msg/AlarmParamSet");
	temParser.SetAttrNode("STD",STD,FreqNode);
	temParser.SetAttrNode("Freq",Freq,FreqNode);
	temParser.SetAttrNode("SymbolRate",SymbolRate,FreqNode);
	int count=parser.GetChildCount(orgRootNode);
	pXMLNODELIST HoldList=parser.GetNodeList(orgRootNode);
	for(int n=0;n<count;n++)
	{
		pXMLNODE orgSetNode =parser.GetNextNode(HoldList);
		pXMLNODE stdSetNode =temParser.GetNodeFirstChild(stdRootNode);
		string SetName=parser.GetNodeName(orgSetNode);
		pXMLNODELIST SetList=parser.GetNodeList(orgSetNode);

		parser.GetAttrNode(orgSetNode,"Freq",Freq);
		for(int i=0;i<SetList->Size();i++)
		{
			string Type,Desc,Switch,Duration,ServiceID,VideoPID,AudioPID;
			pXMLNODE HoldNode = parser.GetNextNode(SetList);
			if(parser.GetNodeName(HoldNode)=="AlarmProgram")
			{
				pXMLNODELIST childSetList=parser.GetNodeList(HoldNode);
				parser.GetAttrNode(HoldNode,"ServiceID",ServiceID);
				parser.GetAttrNode(HoldNode,"VideoPID",VideoPID);
				parser.GetAttrNode(HoldNode,"AudioPID",AudioPID);
				string Tsid,Netid,Channelid;

				CHANNELINFOMGR::instance()->GetOrgNetID(THREED,Freq,AudioPID,VideoPID,ServiceID,Netid);
				CHANNELINFOMGR::instance()->GetTsID(THREED,Freq,AudioPID,VideoPID,ServiceID,Tsid);
				CHANNELINFOMGR::instance()->GetChannelID(THREED,Netid,Tsid,ServiceID,VideoPID,AudioPID,"",Channelid);
				for(int k=0;k<childSetList->Size();k++)
				{
					pXMLNODE childHoldNode = parser.GetNextNode(childSetList);
					pXMLNODE stdHoldNode = temParser.CreateNodePtr(FreqNode,"AlarmParam");
					
					temParser.SetAttrNode("Freq",Freq,stdHoldNode);
					temParser.SetAttrNode("ChannelID",Channelid,stdHoldNode);

					parser.GetAttrNode(childHoldNode,string("Type"),Type);//���Type����
					parser.GetAttrNode(childHoldNode,string("Desc"),Desc);//���Desc����
					temParser.SetAttrNode("Type",Type,stdHoldNode);
					temParser.SetAttrNode("Desc",Desc,stdHoldNode);
					string Num,TimeInterval;
					parser.GetAttrNode(childHoldNode,string("Num"),Num);//�������
					parser.GetAttrNode(childHoldNode,string("TimeInterval"),TimeInterval);//ʱ����

					temParser.SetAttrNode("Num",Num,stdHoldNode);
					temParser.SetAttrNode("TimeInterval",TimeInterval,stdHoldNode);
					string DownThreshold,UpThreshold;

					parser.GetAttrNode(childHoldNode,string("DownThreshold"),DownThreshold);//����
					parser.GetAttrNode(childHoldNode,string("UpThreshold"),UpThreshold);//����

					temParser.SetAttrNode("DownThreshold",DownThreshold,stdHoldNode);
					temParser.SetAttrNode("UpThreshold",UpThreshold,stdHoldNode);

					parser.GetAttrNode(childHoldNode,string("Duration"),Duration);
					temParser.SetAttrNode("Duration",Duration,stdHoldNode);
					parser.GetAttrNode(childHoldNode,string("Switch"),Switch);
					temParser.SetAttrNode("Switch",Switch,stdHoldNode);
				}
			}
			else
			{
				pXMLNODE stdHoldNode = temParser.CreateNodePtr(FreqNode,"AlarmParam");
				parser.GetAttrNode(HoldNode,string("Switch"),Switch);
				if(Freq.length()<1||Freq=="All")
				{
					Freq="All";
					Switch="1";
				}
				temParser.SetAttrNode("Freq",Freq,stdHoldNode);
				temParser.SetAttrNode("ServiceID",ServiceID,stdHoldNode);
				temParser.SetAttrNode("VideoPID",VideoPID,stdHoldNode);
				temParser.SetAttrNode("AudioPID",AudioPID,stdHoldNode);
				parser.GetAttrNode(HoldNode,string("Type"),Type);//���Type����
				parser.GetAttrNode(HoldNode,string("Desc"),Desc);//���Desc����
				temParser.SetAttrNode("Type",Type,stdHoldNode);
				temParser.SetAttrNode("Desc",Desc,stdHoldNode);
				string Num,TimeInterval;
				parser.GetAttrNode(HoldNode,string("Num"),Num);//�������
				parser.GetAttrNode(HoldNode,string("TimeInterval"),TimeInterval);//ʱ����

				temParser.SetAttrNode("Num",Num,stdHoldNode);
				temParser.SetAttrNode("TimeInterval",TimeInterval,stdHoldNode);
				string DownThreshold,UpThreshold;

				parser.GetAttrNode(HoldNode,string("DownThreshold"),DownThreshold);//����
				parser.GetAttrNode(HoldNode,string("UpThreshold"),UpThreshold);//����

				temParser.SetAttrNode("DownThreshold",DownThreshold,stdHoldNode);
				temParser.SetAttrNode("UpThreshold",UpThreshold,stdHoldNode);

				parser.GetAttrNode(HoldNode,string("Duration"),Duration);
				temParser.SetAttrNode("Duration",Duration,stdHoldNode);

				
				temParser.SetAttrNode("Switch",Switch,stdHoldNode);
			}

		}
	}
	//���浽�ַ���
	temParser.SaveToString( strStandardXML);
	return true;
}

//wz_110107
bool TranslateDownXMLForTHREED::TranslateAlarmReportSet( const std::string& strOriginalXML,std::string& strStandardXML )
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//��ȡ��Ԫ��
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	//string freq,equcode,stdtype,rate,querytime;
	string tasktype="AlarmTypeSet";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="3DDown")
	{
		downtype="THREED";
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

	temParser.CreateNode("Msg", "AlarmTypeSet");
	pXMLNODE alarmTypeSetNode = parser.GetNodeFromPath("Msg/AlarmTypeSet");

	string TypeSetType, TypeSetURL, TypeSetTimeInterval;
	parser.GetAttrNode(alarmTypeSetNode, "Type", TypeSetType);
	parser.GetAttrNode(alarmTypeSetNode, "URL", TypeSetURL);
	parser.GetAttrNode(alarmTypeSetNode, "TimeInterval", TypeSetTimeInterval);

	pXMLNODE tempTypeSetNode = temParser.GetNodeFromPath("Msg/AlarmTypeSet");
	temParser.SetAttrNode("Type", TypeSetType, tempTypeSetNode);
	temParser.SetAttrNode("URL", TypeSetURL, tempTypeSetNode);
	temParser.SetAttrNode("TimeInterval", TypeSetTimeInterval, tempTypeSetNode);

	//string tempString;
	temParser.SaveToString(strStandardXML);
	return true;

	/*
	strStandardXML=strOriginalXML;
		size_t pos1=strStandardXML.find("Type=");
		strStandardXML.replace(pos1,4,"DVBType");
		size_t firPos=strStandardXML.find("\"",pos1);
		strStandardXML.replace(firPos+1,7,"THREED");
		size_t secPos=strStandardXML.find(" ",firPos+1);
		strStandardXML.insert(secPos+1,std::string("TaskType=\"AlarmTypeSet\" "));
	
	return true;
	*/
}

bool TranslateDownXMLForTHREED::TranslateAlarmQuery( const std::string& strOriginalXML,std::string& strStandardXML )
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><AlarmQuerySet></AlarmQuerySet></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//��ȡ��Ԫ��
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	string tasktype="AlarmQuerySet";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="3DDown")
	{
		downtype="THREED";
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

	pXMLNODE orgchildNode=parser.GetNodeFirstChild(orgRootNode);
	pXMLNODE stdchildNode=temParser.GetNodeFromPath("Msg/AlarmQuerySet");
	string Index,Freq, ServiceID,VideoPID, AudioPID;
	parser.GetAttrNode(orgchildNode,"Index",Index);
	parser.GetAttrNode(orgchildNode,"Freq",Freq);
	parser.GetAttrNode(orgchildNode,"ServiceID",ServiceID);
	parser.GetAttrNode(orgchildNode,"VideoPID",VideoPID);
	parser.GetAttrNode(orgchildNode,"AudioPID",AudioPID);

	temParser.SetAttrNode("Index",Index,stdchildNode);
	temParser.SetAttrNode("Freq",Freq,stdchildNode);
	temParser.SetAttrNode("ServiceID",ServiceID,stdchildNode);
	temParser.SetAttrNode("VideoPID",VideoPID,stdchildNode);
	temParser.SetAttrNode("AudioPID",AudioPID,stdchildNode);
	string AlarmType=parser.GetNodeName(orgchildNode);//���ֱ�������  Ƶ�� ��Ŀ ����
	temParser.SetAttrNode("AlarmType",AlarmType,stdchildNode);
	pXMLNODELIST orgList=parser.GetNodeList(orgchildNode);
	int count=parser.GetChildCount(orgchildNode);
	for(int i=0;i<count;i++)
	{   
		string Type,Desc;
		pXMLNODE orgNode=parser.GetNextNode(orgList);
		pXMLNODE stdNode=temParser.CreateNodePtr(stdchildNode,"AlarmQuery");
		parser.GetAttrNode(orgNode,"Type",Type);
		parser.GetAttrNode(orgNode,"Desc",Desc);

		temParser.SetAttrNode("Type",Type,stdNode);
		temParser.SetAttrNode("Desc",Desc,stdNode);
	}
	//���浽�ַ���
	temParser.SaveToString( strStandardXML);
	return true;
}

//wz_110107
bool TranslateDownXMLForTHREED::TranslateOSDSet( const std::string& strOriginalXML,std::string& strStandardXML )
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//��ȡ��Ԫ��
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	//string freq,equcode,stdtype,rate,querytime;
	string tasktype="OSDSet";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="3DDown")
	{
		downtype="THREED";
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

	string index, infoOSD, infoOSDx, infoOSDy, timeOSDType, timeOSDx, timeOSDy;
	pXMLNODE OSDSetNode = parser.GetNodeFromPath("Msg/OSDSet");
	parser.GetAttrNode(OSDSetNode, "Index", index);
	parser.GetAttrNode(OSDSetNode, "InfoOSD", infoOSD);
	parser.GetAttrNode(OSDSetNode, "InfoOSDX", infoOSDx);
	parser.GetAttrNode(OSDSetNode, "InfoOSDY", infoOSDy);
	parser.GetAttrNode(OSDSetNode, "TimeOSDType", timeOSDType);
	parser.GetAttrNode(OSDSetNode, "TimeOSDX", timeOSDx);
	parser.GetAttrNode(OSDSetNode, "TimeOSDY", timeOSDy);

	temParser.CreateNode("Msg", "OSDSet");
	pXMLNODE tempOSDSetNode = temParser.GetNodeFromPath("Msg/OSDSet");
	temParser.SetAttrNode("Index", index, tempOSDSetNode);
	temParser.SetAttrNode("InfoOSD", infoOSD, tempOSDSetNode);
	temParser.SetAttrNode("InfoOSDX", infoOSDx, tempOSDSetNode);
	temParser.SetAttrNode("InfoOSDY", infoOSDy, tempOSDSetNode);
	temParser.SetAttrNode("TimeOSDType", timeOSDType, tempOSDSetNode);
	temParser.SetAttrNode("TimeOSDX", timeOSDx, tempOSDSetNode);
	temParser.SetAttrNode("TimeOSDY", timeOSDy, tempOSDSetNode);

	//string tempString;
	temParser.SaveToString(strStandardXML);
	return true;
}

//wz_110107
bool TranslateDownXMLForTHREED::TranslateStreamRealtimeRate(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//��ȡ��Ԫ��
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	//string freq,equcode,stdtype,rate,querytime;
	string tasktype="NVRSteamRateSet";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="3DDown")
	{
		downtype="THREED";
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

	string index, width, height, fps, bps;
	pXMLNODE rateSetNode = parser.GetNodeFromPath("Msg/NVRSteamRateSet");
	parser.GetAttrNode(rateSetNode, "Index", index);
	parser.GetAttrNode(rateSetNode, "Width", width);
	parser.GetAttrNode(rateSetNode, "Height", height);
	parser.GetAttrNode(rateSetNode, "Fps", fps);
	parser.GetAttrNode(rateSetNode, "Bps", bps);

	temParser.CreateNode("Msg", "NVRSteamRateSet");
	pXMLNODE tempRateSetNode = temParser.GetNodeFromPath("Msg/NVRSteamRateSet");
	temParser.SetAttrNode("Index", index, tempRateSetNode);
	temParser.SetAttrNode("Width", width, tempRateSetNode);
	temParser.SetAttrNode("Height", height, tempRateSetNode);
	temParser.SetAttrNode("Fps", fps, tempRateSetNode);
	temParser.SetAttrNode("Bps", bps, tempRateSetNode);

	//string tempString;
	temParser.SaveToString(strStandardXML);
	return true;
}

//wz_110107
bool TranslateDownXMLForTHREED::TranslateRecordCapabilityQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//��ȡ��Ԫ��
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;

	string tasktype="RecordCapabilityQuery";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="3DDown")
	{
		downtype="THREED";
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

	temParser.CreateNode("Msg", "RecordCapabilityQuery");
	pXMLNODE tempCapabilityNode = temParser.GetNodeFromPath("Msg/RecordCapabilityQuery");

	string freq, serviceId, programId;
	pXMLNODE CapabilityNode = parser.GetNodeFromPath("Msg/RecordCapabilityQuery");
	pXMLNODELIST channelInfolist = parser.GetNodeList(CapabilityNode);
	int channelInfoCount = 0;
	channelInfoCount = parser.GetChildCount(CapabilityNode);
	for (int i=0; i!=channelInfoCount; i++)
	{
		pXMLNODE channelInfoNode = parser.GetNextNode(channelInfolist);
		parser.GetAttrNode(channelInfoNode, "Freq", freq);
		parser.GetAttrNode(channelInfoNode, "ServicID", serviceId);
		parser.GetAttrNode(channelInfoNode, "ProgramID", programId);
		
		pXMLNODE tempChanInfoNode = temParser.CreateNodePtr(tempCapabilityNode, "ChannelInfo");
		temParser.SetAttrNode("Freq", freq, tempChanInfoNode);
		temParser.SetAttrNode("ServicID", serviceId, tempChanInfoNode);
		temParser.SetAttrNode("ProgramID", programId, tempChanInfoNode);
	}

	temParser.SaveToString(strStandardXML);
	return true;

	/*
	strStandardXML=strOriginalXML;
	size_t pos1=strStandardXML.find("Type=");
	strStandardXML.replace(pos1,4,"DVBType");
	size_t firPos=strStandardXML.find("\"",pos1);
	strStandardXML.replace(firPos+1,7,"THREED");
	size_t secPos=strStandardXML.find(" ",firPos+1);
	strStandardXML.insert(secPos+1,std::string("TaskType=\"RecordCapabilityQuery\" "));
	return true;
	*/
}

//wz_110107
bool TranslateDownXMLForTHREED::TranslateHDDefAudioParamSet(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//��ȡ��Ԫ��
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;

	string tasktype="HDDefAudioParamSet";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="3DDown")
	{
		downtype="THREED";
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

	string right, left, leftSurround, rightSurround, center, subWoofer, audioEncode;
	pXMLNODE HDAudioParamSetNode = parser.GetNodeFromPath("Msg/HDDefAudioParamSet");
	parser.GetAttrNode(HDAudioParamSetNode, "Right", right);
	parser.GetAttrNode(HDAudioParamSetNode, "Left", left);
	parser.GetAttrNode(HDAudioParamSetNode, "LeftSurround", leftSurround);
	parser.GetAttrNode(HDAudioParamSetNode, "RightSurround", rightSurround);
	parser.GetAttrNode(HDAudioParamSetNode, "Center", center);
	parser.GetAttrNode(HDAudioParamSetNode, "Subwoofer", subWoofer);
	parser.GetAttrNode(HDAudioParamSetNode, "AudioEncode", audioEncode);

	temParser.CreateNode("Msg", "HDDefAudioParamSet");
	pXMLNODE tempHDAParamSetNode = temParser.GetNodeFromPath("Msg/HDDefAudioParamSet");
	temParser.SetAttrNode("Right", right, tempHDAParamSetNode);
	temParser.SetAttrNode("Left", left, tempHDAParamSetNode);
	temParser.SetAttrNode("LeftSurround", leftSurround, tempHDAParamSetNode);
	temParser.SetAttrNode("RightSurround", rightSurround, tempHDAParamSetNode);
	temParser.SetAttrNode("Center", center, tempHDAParamSetNode);
	temParser.SetAttrNode("Subwoofer", subWoofer, tempHDAParamSetNode);
	temParser.SetAttrNode("AudioEncode", audioEncode, tempHDAParamSetNode);

	//string tempString;
	temParser.SaveToString(strStandardXML);
	return true;

	/*
	strStandardXML=strOriginalXML;
	size_t pos1=strStandardXML.find("Type=");
	strStandardXML.replace(pos1,4,"DVBType");
	size_t firPos=strStandardXML.find("\"",pos1);
	strStandardXML.replace(firPos+1,7,"THREED");
	size_t secPos=strStandardXML.find(" ",firPos+1);
	strStandardXML.insert(secPos+1,std::string("TaskType=\"HDDefAudioParamSet\" "));
	return true;
	*/
}

//wz_110107
bool TranslateDownXMLForTHREED::TranslateAudioParamSet(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//��ȡ��Ԫ��
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;

	string tasktype="AudioParamSet";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="3DDown")
	{
		downtype="THREED";
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

	string viderPid, audioPid, freq, right, left, leftSurround, rightSurround, center, subWoofer, serviceId;
	pXMLNODE audioParamSetNode = parser.GetNodeFromPath("Msg/AudioParamSet");
	parser.GetAttrNode(audioParamSetNode, "VideoPID", viderPid);
	parser.GetAttrNode(audioParamSetNode, "AudioPID", audioPid);
	parser.GetAttrNode(audioParamSetNode, "Freq", freq);
	parser.GetAttrNode(audioParamSetNode, "Right", right);
	parser.GetAttrNode(audioParamSetNode, "Left", left);
	parser.GetAttrNode(audioParamSetNode, "LeftSurround", leftSurround);
	parser.GetAttrNode(audioParamSetNode, "RightSurround", rightSurround);
	parser.GetAttrNode(audioParamSetNode, "Center", center);
	parser.GetAttrNode(audioParamSetNode, "Subwoofer", subWoofer);
	parser.GetAttrNode(audioParamSetNode, "ServiceID", serviceId);

	temParser.CreateNode("Msg", "AudioParamSet");
	pXMLNODE tempAParamSetNode = temParser.GetNodeFromPath("Msg/AudioParamSet");
	temParser.SetAttrNode("VideoPID", viderPid,tempAParamSetNode);
	temParser.SetAttrNode("AudioPID", audioPid,tempAParamSetNode);
	temParser.SetAttrNode("Freq", freq,tempAParamSetNode);
	temParser.SetAttrNode("Right", right, tempAParamSetNode);
	temParser.SetAttrNode("Left", left, tempAParamSetNode);
	temParser.SetAttrNode("LeftSurround", leftSurround, tempAParamSetNode);
	temParser.SetAttrNode("RightSurround", rightSurround, tempAParamSetNode);
	temParser.SetAttrNode("Center", center, tempAParamSetNode);
	temParser.SetAttrNode("Subwoofer", subWoofer, tempAParamSetNode);
	temParser.SetAttrNode("ServiceID", serviceId, tempAParamSetNode);

	//string tempString;
	temParser.SaveToString(strStandardXML);
	return true;

	/*
	strStandardXML=strOriginalXML;
	size_t pos1=strStandardXML.find("Type=");
	strStandardXML.replace(pos1,4,"DVBType");
	size_t firPos=strStandardXML.find("\"",pos1);
	strStandardXML.replace(firPos+1,7,"THREED");
	size_t secPos=strStandardXML.find(" ",firPos+1);
	strStandardXML.insert(secPos+1,std::string("TaskType=\"AudioParamSet\" "));
	return true;
	*/
}

//wz_110107
bool TranslateDownXMLForTHREED::TranslateClearAlarmState(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//��ȡ��Ԫ��
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	string tasktype="ClearAlarmState";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="3DDown")
	{
		downtype="THREED";
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

	// <ClearAlarmState> 
	//   <Target Freq="482000"/>
	// </ClearAlarmState>

	temParser.CreateNode("Msg", "ClearAlarmState");
	pXMLNODE tempClearAlarmStateNode = temParser.GetNodeFromPath("Msg/ClearAlarmState");

	string freq;
	pXMLNODE cleanAlarmStatNode = parser.GetNodeFromPath("Msg/ClearAlarmState");
	pXMLNODELIST cleanAlarmStatList = parser.GetNodeList(cleanAlarmStatNode);
	int targetCount = parser.GetChildCount(cleanAlarmStatNode);
	for (int i=0; i!=targetCount; i++)
	{
		pXMLNODE targetNode = parser.GetNextNode(cleanAlarmStatList);
		parser.GetAttrNode(targetNode, "Freq", freq);

		pXMLNODE tempTargerNode = temParser.CreateNodePtr(tempClearAlarmStateNode, "Target");
		temParser.SetAttrNode("Freq", freq, tempTargerNode);
	}

	temParser.SaveToString(strStandardXML);
	return true;
}

//wz_110107
bool TranslateDownXMLForTHREED::TranslateMatrixQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//��ȡ��Ԫ��
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;

	string tasktype="MatrixQuery";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="3DDown")
	{
		downtype="THREED";
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

	//<MatrixQuery >		
	//  <Matrix Type="Set" Index1="1" Index2="14"/>
	//	<Matrix Type="Set" Index1="4" Index2="5"/>
	//</MatrixQuery>

	temParser.CreateNode("Msg", "MatrixQuery");
	pXMLNODE tempMatrixQueryNode = temParser.GetNodeFromPath("Msg/MatrixQuery");

	string matrixType, index1, index2;
	pXMLNODE MatrixQueryNode = parser.GetNodeFromPath("Msg/MatrixQuery");
	pXMLNODELIST MatrixQueryList = parser.GetNodeList(MatrixQueryNode);
	int matrixCount = parser.GetChildCount(MatrixQueryNode);
	for (int i=0; i!=matrixCount; i++)
	{
		pXMLNODE matrixNode = parser.GetNextNode(MatrixQueryList);
		parser.GetAttrNode(matrixNode, "type", matrixType);
		parser.GetAttrNode(matrixNode, "Index1", index1);
		parser.GetAttrNode(matrixNode, "Index2", index2);

		pXMLNODE tempMatrixNode = temParser.CreateNodePtr(tempMatrixQueryNode, "Matrix");
		temParser.SetAttrNode("Type", matrixType, tempMatrixNode);
		temParser.SetAttrNode("Index1", index1, tempMatrixNode);
		temParser.SetAttrNode("Index2", index2, tempMatrixNode);
	}

	temParser.SaveToString(strStandardXML);
	return true;
}

//wz_110107
bool TranslateDownXMLForTHREED::TranslateChangeQAMQuery(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//��ȡ��Ԫ��
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	//string freq,equcode,stdtype,rate,querytime;
	string tasktype="ChangeQAMQuery";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="3DDown")
	{
		downtype="THREED";
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

	// <ChangeQAMQuery>
	//		<ChangeQAM Index="1" QAM="16" SymbolRate="1" />
	//		<ChangeQAM Index="5" QAM="128" SymbolRate="4" />
	// </ChangeQAMQuery>

	temParser.CreateNode("Msg", "ChangeQAMQuery");
	pXMLNODE tempChangeQAMNode = temParser.GetNodeFromPath("Msg/ChangeQAMQuery");

	string changeIndex, qam, symbolRate;
	pXMLNODE ChangeQAMNode = parser.GetNodeFromPath("Msg/ChangeQAMQuery");
	pXMLNODELIST ChangeQAMList = parser.GetNodeList(ChangeQAMNode);
	int changeCount = parser.GetChildCount(ChangeQAMNode);
	for (int i=0; i!=changeCount; i++)
	{
		pXMLNODE changeNode = parser.GetNextNode(ChangeQAMList);
		parser.GetAttrNode(changeNode, "Index", changeIndex);
		parser.GetAttrNode(changeNode, "QAM", qam);
		parser.GetAttrNode(changeNode, "SymbolRate", symbolRate);

		pXMLNODE tempChangeNode = temParser.CreateNodePtr(tempChangeQAMNode, "ChangeQAM");
		temParser.SetAttrNode("Index", changeIndex, tempChangeNode);
		temParser.SetAttrNode("QAM", qam, tempChangeNode);
		temParser.SetAttrNode("SymbolRate", symbolRate, tempChangeNode);
	}

	temParser.SaveToString(strStandardXML);
	return true;
}

bool TranslateDownXMLForTHREED::TranslateLoopAlarmInfo(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//��ȡ��Ԫ��
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	//string freq,equcode,stdtype,rate,querytime;
	string tasktype="LoopAlaInf";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="3DDown")
	{
		downtype="THREED";
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

	//	<LoopAlaInf>
	//		<RoundStream Index="1" Switch="1" DelayTime="15" AlaType="1">
	//			<Channel Freq="530000" SymbolRate="6875" QAM="QAM64"/>
	//			<Channel Freq="538000" SymbolRate="6875" QAM="QAM64"/>
	//			<!--SymbolRate="" QAM=""��ʾĬ��ֵ-->   	
	//		</RoundStream>
	//		<RoundStream Index="1" Switch="1" DelayTime="15" AlaType="1">
	//			<Channel Freq="520000" SymbolRate="6875" QAM="QAM64"/>
	//			<Channel Freq="528000" SymbolRate="6875" QAM="QAM64"/>
	//			<!--SymbolRate="" QAM=""��ʾĬ��ֵ-->   	
	//		</RoundStream>
	//	</LoopAlaInf>

	string streamIndex, streamSwitch, delayTime, alarmType;
	string freq, symbolRate, qam;
	pXMLNODE tempLoopNode = temParser.CreateNodePtr(stdRootNode, "LoopAlaInf");
	pXMLNODE loopNode = parser.GetNodeFromPath("Msg/LoopAlaInf");
	pXMLNODELIST roundStreamList = parser.GetNodeList(loopNode);
	int streamCount = parser.GetChildCount(loopNode);
	for (int n=0; n!=streamCount; n++)
	{
		pXMLNODE RoundStreamNode = parser.GetNextNode(roundStreamList);
		parser.GetAttrNode(RoundStreamNode, "Index", streamIndex);
		parser.GetAttrNode(RoundStreamNode, "Switch", streamSwitch);
		parser.GetAttrNode(RoundStreamNode, "DelayTime", delayTime);
		parser.GetAttrNode(RoundStreamNode, "AlaType", alarmType);

		pXMLNODE tempRoundStreamNode = temParser.CreateNodePtr(tempLoopNode, "RoundStream");
		temParser.SetAttrNode("Index", streamIndex, tempRoundStreamNode);
		temParser.SetAttrNode("Switch", streamSwitch, tempRoundStreamNode);
		temParser.SetAttrNode("DelayTime", delayTime, tempRoundStreamNode);
		temParser.SetAttrNode("AlaType", alarmType, tempRoundStreamNode);

		pXMLNODELIST channelList = parser.GetNodeList(RoundStreamNode);
		int channelCount = parser.GetChildCount(RoundStreamNode);
		for (int i=0; i!=channelCount; i++)
		{
			pXMLNODE channelNode = parser.GetNextNode(channelList);
			parser.GetAttrNode(channelNode, "Freq", freq);
			parser.GetAttrNode(channelNode, "SymbolRate", symbolRate);
			parser.GetAttrNode(channelNode, "QAM", qam);

			pXMLNODE tempChannelNode = temParser.CreateNodePtr(tempRoundStreamNode, "Channel");
			temParser.SetAttrNode("Freq", freq, tempChannelNode);
			temParser.SetAttrNode("SymbolRate", symbolRate, tempChannelNode);
			temParser.SetAttrNode("QAM", qam, tempChannelNode);
		}
	}

	temParser.SaveToString(strStandardXML);
	return true;
}



bool TranslateDownXMLForTHREED::TranslateRecordParamSet(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(strOriginalXML.c_str());
	XmlParser temParser( source );
	//��ȡ��Ԫ��
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,downtype,protocol,datetime,srccode,dstcode,srcurl,priority;
	//string freq,equcode,stdtype,rate,querytime;
	string tasktype="RecordParamSet";
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"Type",downtype);
	parser.GetAttrNode(orgRootNode,"Protocol",protocol);
	parser.GetAttrNode(orgRootNode,"DateTime",datetime);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
	parser.GetAttrNode(orgRootNode,"SrcURL",srcurl);
	parser.GetAttrNode(orgRootNode,"Priority",priority);

	if(downtype=="3DDown")
	{
		downtype="THREED";
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

	//  <RecordParamSet>
	//   <RecordParam Type="HD" Width="1" Height="2" Fps="3" Bps="4" />
	//	 <RecordParam Type="SD" Width="5" Height="6" Fps="7" Bps="8" />
	//	</RecordParamSet>


	temParser.CreateNode("Msg", "RecordParamSet");
	pXMLNODE tempRecordParamSetNode = temParser.GetNodeFromPath("Msg/RecordParamSet");

	string paramType, width, height, fps, bps;
	pXMLNODE RecordParamSetNode = parser.GetNodeFromPath("Msg/RecordParamSet");
	pXMLNODELIST RecordParamList = parser.GetNodeList(RecordParamSetNode);
	int RecordParamCount = parser.GetChildCount(RecordParamSetNode);
	for (int i=0; i!=RecordParamCount; i++)
	{
		pXMLNODE paramNode = parser.GetNextNode(RecordParamList);
		parser.GetAttrNode(paramNode, "Type", paramType);
		parser.GetAttrNode(paramNode, "Width", width);
		parser.GetAttrNode(paramNode, "Height", height);
		parser.GetAttrNode(paramNode, "Fps", fps);
		parser.GetAttrNode(paramNode, "Bps", bps);

		pXMLNODE tempParamNode = temParser.CreateNodePtr(tempRecordParamSetNode, "RecordParam");
		temParser.SetAttrNode("Type", paramType, tempParamNode);
		temParser.SetAttrNode("Width", width, tempParamNode);
		temParser.SetAttrNode("Height", height, tempParamNode);
		temParser.SetAttrNode("Fps", fps, tempParamNode);
		temParser.SetAttrNode("Bps", bps, tempParamNode);
	}

	temParser.SaveToString(strStandardXML);
	return true;
}

// <?xml version="1.0" encoding="GB2312" standalone="yes"?>
// <Msg Version="1" MsgID="2" Type="RadioDown" DateTime="2006-08-17 15:30:00" SrcCode="110000X01" 
// DstCode="110000R01" Priority="1">
// <VideoClientStop EquCode="110000N00001" URL=""/>
// </Msg>


bool TranslateDownXMLForTHREED::TranslateStopClient(const std::string& strOriginalXML,std::string& strStandardXML)
{
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser retparser(source);

	XmlParser parser(strOriginalXML.c_str());
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
	retparser.SetAttrNode("DVBType",string("THREED"),retRootNode);               //����Msg�ڵ��DVBType����ֵ
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