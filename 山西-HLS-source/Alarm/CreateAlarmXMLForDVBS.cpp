
#include "CreateAlarmXMLForDVBS.h"
#include "../BusinessProcess/AlarmParamInfoMgr.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/XmlParser.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/AppLog.h"
#include "../Foundation/PropManager.h"
#include "../DBAccess/DBManager.h"
#include "../BusinessProcess/ChannelInfoMgr.h"

std::string CreateAlarmXMLForDVBS::CreateFreqAlarmXML(const sCheckParam& checkparam,std::string alarmvalue,std::string alarmid)
{
	std::string xml ="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\" ?> ";//����xmlͷ
	xml+="<Msg><Return/><ReturnInfo></ReturnInfo></Msg>";
	XmlParser parser(xml.c_str());

// 	string  MsgID ;
// 	DBMANAGER::instance()->GetMsgID(MsgID);
// 	int msgid=StrUtil::Str2Int(MsgID);
// 	DBMANAGER::instance()->UpdateMsgID(StrUtil::Int2Str(++msgid));

	pXMLNODE root=parser.GetRootNode();
	parser.SetAttrNode( "Version",string("1"),root );//�汾

	string strDateTime = TimeUtil::GetCurDateTime();
	string  MsgID = OSFunction::GetXmlMsgID(strDateTime);
	parser.SetAttrNode( "MsgID",MsgID,root );//��Ϣid����
	parser.SetAttrNode( "DateTime",strDateTime/*TimeUtil::GetCurDateTime()*/,root );//��ǰʱ��
	//parser.SetAttrNode( "MsgID",msgid,root );//��Ϣid����
	//parser.SetAttrNode( "DateTime",TimeUtil::GetCurDateTime(),root );//��ǰʱ��

	parser.SetAttrNode( "Type",string("SatUp"),root );//��Ϣ����
	parser.SetAttrNode( "SrcCode",PROPMANAGER::instance()->GetDefSrcCode(DVBS),root );//������ʶ����ͨ���ӿڻ��
	parser.SetAttrNode( "DstCode",PROPMANAGER::instance()->GetDefDstCode(DVBS),root );//Ŀ�������ʶ
	parser.SetAttrNode( "ReplyID",string("-1"),root );//�ظ�����Ϣid
	pXMLNODE node=NULL;
	pXMLNODE childnode=NULL;

	pXMLNODE RetNode=parser.GetNodeFromPath("Msg/Return");
	parser.SetAttrNode("Type",string("AlarmSearchFSet"),RetNode);
	parser.SetAttrNode("Value",string("0"),RetNode);
	parser.SetAttrNode("Desc",string("�ɹ�"),RetNode);
	node=parser.GetNodeFromPath("Msg/ReturnInfo");
	pXMLNODE SetNode=parser.CreateNodePtr(node,"AlarmSearchFSet");
	int logindex;
	PROPMANAGER::instance()->GetDeviceLogIndex(StrUtil::Str2Int(checkparam.DeviceID),logindex);
	parser.SetAttrNode("Index",logindex,SetNode);
	parser.SetAttrNode("Freq",checkparam.Freq,SetNode);
	parser.SetAttrNode("Alias",string(""),SetNode);
	childnode=parser.CreateNodePtr(SetNode,"AlarmSearchF");
	parser.SetAttrNode("Type",checkparam.TypeID,childnode);

	std::string error;
	if (alarmvalue=="0")
	{
		parser.SetAttrNode("Value",(string)"1",childnode);
		error=checkparam.TypeDesc+"����";
	}
	else
	{
		parser.SetAttrNode("Value",(string)"2",childnode);
		error=checkparam.TypeDesc+"�������";
	}
	parser.SetAttrNode("Time",TimeUtil::DateTimeToStr(checkparam.CheckTime),childnode);
	parser.SetAttrNode("Desc",error,childnode);

	std::string retXML;
	parser.SaveToString(retXML);
	return retXML;

}
std::string CreateAlarmXMLForDVBS::CreateProgramAlarmXML(const sCheckParam& checkparam,std::string alarmvalue,std::string alarmid)
{
	std::string xml ="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\" ?> ";//����xmlͷ
	xml+="<Msg><Return/><ReturnInfo></ReturnInfo></Msg>";

// 	string  MsgID ;
// 	DBMANAGER::instance()->GetMsgID(MsgID);
// 	int msgid=StrUtil::Str2Int(MsgID);
// 	DBMANAGER::instance()->UpdateMsgID(StrUtil::Int2Str(++msgid));
	XmlParser parser(xml.c_str());
	pXMLNODE root=parser.GetRootNode();
	parser.SetAttrNode( "Version",string("1"),root );//�汾

	string strDateTime = TimeUtil::GetCurDateTime();
	string  MsgID = OSFunction::GetXmlMsgID(strDateTime);
	parser.SetAttrNode( "MsgID",MsgID,root );//��Ϣid����
	parser.SetAttrNode( "DateTime",strDateTime/*TimeUtil::GetCurDateTime()*/,root );//��ǰʱ��
	//parser.SetAttrNode( "MsgID",msgid,root );//��Ϣid����
	//parser.SetAttrNode( "DateTime",TimeUtil::GetCurDateTime(),root );//��ǰʱ��

	parser.SetAttrNode( "Type",string("SatUp"),root );//��Ϣ����
	parser.SetAttrNode( "SrcCode",PROPMANAGER::instance()->GetDefSrcCode(DVBS),root );//������ʶ����ͨ���ӿڻ��
	parser.SetAttrNode( "DstCode",PROPMANAGER::instance()->GetDefDstCode(DVBS),root );//Ŀ�������ʶ
	parser.SetAttrNode( "ReplyID",string("-1"),root );//�ظ�����Ϣid
	pXMLNODE node=NULL;			
	pXMLNODE childnode=NULL;

	pXMLNODE RetNode=parser.GetNodeFromPath("Msg/Return");
	parser.SetAttrNode("Type",string("AlarmSearchPSet"),RetNode);
	parser.SetAttrNode("Value",string("0"),RetNode);
	parser.SetAttrNode("Desc",string("�ɹ�"),RetNode);
	node=parser.GetNodeFromPath("Msg/ReturnInfo");
	pXMLNODE SetNode=parser.CreateNodePtr(node,"AlarmSearchPSet");
	int logindex;
	PROPMANAGER::instance()->GetDeviceLogIndex(StrUtil::Str2Int(checkparam.DeviceID),logindex);
	parser.SetAttrNode("Index",logindex,SetNode);
	parser.SetAttrNode("Freq",checkparam.Freq,SetNode);
	std::string apid,vpid,serviceid,prgname;
	CHANNELINFOMGR::instance()->GetChannelInfo(DVBS,checkparam.ChannelID,vpid,apid,serviceid,prgname);
	parser.SetAttrNode("ServiceID",serviceid,SetNode);
	parser.SetAttrNode("VideoPID",vpid,SetNode);
	parser.SetAttrNode("AudioPID",apid,SetNode);
	parser.SetAttrNode("PrgName",prgname,SetNode);
	childnode=parser.CreateNodePtr(SetNode,"AlarmSearchP");
	parser.SetAttrNode("Type",checkparam.TypeID,childnode);

	std::string error;
	if (alarmvalue=="0")
	{
		parser.SetAttrNode("Value",(string)"1",childnode);
		error=checkparam.TypeDesc+"����";
	}
	else
	{
		parser.SetAttrNode("Value",(string)"2",childnode);
		error=checkparam.TypeDesc+"�������";
	}
	parser.SetAttrNode("Desc",error,childnode);
	parser.SetAttrNode("Time",TimeUtil::DateTimeToStr(checkparam.CheckTime),childnode);

	string retXML = "";
	parser.SaveToString(retXML);
	return retXML;
}
std::string CreateAlarmXMLForDVBS::CreateEnvironmentAlarmXML(const sCheckParam& checkparam,std::string alarmvalue)
{
	std::string xml ="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\" ?> ";//����xmlͷ
	xml+="<Msg><Return/><ReturnInfo></ReturnInfo></Msg>";
	XmlParser parser(xml.c_str());
// 	string  MsgID ;
// 	DBMANAGER::instance()->GetMsgID(MsgID);
// 	int msgid=StrUtil::Str2Int(MsgID);
// 	DBMANAGER::instance()->UpdateMsgID(StrUtil::Int2Str(++msgid));

	pXMLNODE root=parser.GetRootNode();
	parser.SetAttrNode( "Version",string("1"),root );//�汾

	string strDateTime = TimeUtil::GetCurDateTime();
	string  MsgID = OSFunction::GetXmlMsgID(strDateTime);
	parser.SetAttrNode( "MsgID",MsgID,root );//��Ϣid����
	parser.SetAttrNode( "DateTime",strDateTime/*TimeUtil::GetCurDateTime()*/,root );//��ǰʱ��
	//parser.SetAttrNode( "MsgID",msgid,root );//��Ϣid����
	//parser.SetAttrNode( "DateTime",TimeUtil::GetCurDateTime(),root );//��ǰʱ��

	parser.SetAttrNode( "Type",string("SatUp"),root );//��Ϣ����
	parser.SetAttrNode( "SrcCode",PROPMANAGER::instance()->GetDefSrcCode(DVBS),root );//������ʶ����ͨ���ӿڻ��
	parser.SetAttrNode( "DstCode",PROPMANAGER::instance()->GetDefDstCode(DVBS),root );//Ŀ�������ʶ
	parser.SetAttrNode( "ReplyID",string("-1"),root );//�ظ�����Ϣid
	pXMLNODE node=NULL;			
	pXMLNODE childnode=NULL;

	pXMLNODE RetNode=parser.GetNodeFromPath("Msg/Return");
	parser.SetAttrNode("Type",string("AlarmSearchPSet"),RetNode);
	parser.SetAttrNode("Value",string("0"),RetNode);
	parser.SetAttrNode("Desc",string("�ɹ�"),RetNode);
	node=parser.GetNodeFromPath("Msg/ReturnInfo");
	pXMLNODE SetNode=parser.CreateNodePtr(node,"AlarmSearchPSet");
	parser.SetAttrNode("Index",checkparam.DeviceID,SetNode);
	parser.SetAttrNode("Freq",checkparam.Freq,SetNode);
	parser.SetAttrNode("ServiceID",string(""),SetNode);
	parser.SetAttrNode("VideoPID",string(""),SetNode);
	parser.SetAttrNode("AudioPID",string(""),SetNode);
	parser.SetAttrNode("PrgName",string(""),SetNode);
	childnode=parser.CreateNodePtr(SetNode,"AlarmSearchP");
	parser.SetAttrNode("Type",checkparam.TypeID,childnode);
	parser.SetAttrNode("Value",checkparam.TypedValue,childnode);
	parser.SetAttrNode("Time",TimeUtil::DateTimeToStr(checkparam.CheckTime),childnode);
	parser.SetAttrNode("Desc",checkparam.TypeDesc,childnode);
	std::string retXML;
	parser.SaveToString(retXML);
	return retXML;
}