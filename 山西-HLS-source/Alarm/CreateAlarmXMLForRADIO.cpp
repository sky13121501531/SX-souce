
#include "CreateAlarmXMLForRADIO.h"
#include "../BusinessProcess/AlarmParamInfoMgr.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/XmlParser.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/AppLog.h"
#include "../Foundation/PropManager.h"
#include "../DBAccess/DBManager.h"
#include "../BusinessProcess/ChannelInfoMgr.h"

std::string CreateAlarmXMLForRADIO::CreateFreqAlarmXML(const sCheckParam& checkparam,std::string alarmvalue,std::string alarmid)
{
	std::string radioxml ="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\" ?> ";//����xmlͷ
	radioxml+="<Msg><QualityAlarmReport></QualityAlarmReport></Msg>";
	XmlParser radioparser(radioxml.c_str());

// 	string  radioMsgID ;
// 	DBMANAGER::instance()->GetMsgID(radioMsgID);
// 	int radiomsgid=StrUtil::Str2Int(radioMsgID);
// 	DBMANAGER::instance()->UpdateMsgID(StrUtil::Int2Str(++radiomsgid));

	pXMLNODE radioroot=radioparser.GetRootNode();
	radioparser.SetAttrNode( "Version",string("1"),radioroot );//�汾

	string strDateTime = TimeUtil::GetCurDateTime();
	string  MsgID = OSFunction::GetXmlMsgID(strDateTime);
	radioparser.SetAttrNode( "MsgID",MsgID,radioroot );//��Ϣid����
	radioparser.SetAttrNode( "DateTime",strDateTime/*TimeUtil::GetCurDateTime()*/,radioroot );//��ǰʱ��
	//radioparser.SetAttrNode( "MsgID",radiomsgid,radioroot );//��Ϣid����
	//radioparser.SetAttrNode( "DateTime",TimeUtil::GetCurDateTime(),radioroot );//��ǰʱ��

	radioparser.SetAttrNode( "Type",string("RadioUp"),radioroot );//��Ϣ����
	radioparser.SetAttrNode( "SrcCode",PROPMANAGER::instance()->GetDefSrcCode(RADIO),radioroot );//������ʶ����ͨ���ӿڻ��
	radioparser.SetAttrNode( "DstCode",PROPMANAGER::instance()->GetDefDstCode(RADIO),radioroot );//Ŀ�������ʶ
	radioparser.SetAttrNode( "ReplyID",string("-1"),radioroot );//�ظ�����Ϣid

	pXMLNODE radioreportnode=radioparser.GetNodeFromPath("Msg/QualityAlarmReport");
	pXMLNODE radioalarmnode=radioparser.CreateNodePtr(radioreportnode,"QualityAlarm");
	radioparser.SetAttrNode("EquCode",string(""),radioalarmnode);
	radioparser.SetAttrNode("AlarmID",alarmid,radioalarmnode);
	radioparser.SetAttrNode("Mode",alarmvalue,radioalarmnode);
	string chcode;
	CHANNELINFOMGR::instance()->GetChannelCodeByFreq(RADIO,checkparam.Freq,chcode);
	radioparser.SetAttrNode("ChCode",chcode,radioalarmnode);
	radioparser.SetAttrNode("Type",checkparam.TypeID,radioalarmnode);
	radioparser.SetAttrNode("Desc",checkparam.TypeDesc,radioalarmnode);
	radioparser.SetAttrNode("Reason",string(""),radioalarmnode);
	radioparser.SetAttrNode("Value",checkparam.TypedValue,radioalarmnode);
	radioparser.SetAttrNode("CheckTime",TimeUtil::GetCurDateTime(),radioalarmnode);
	radioparser.SetAttrNode("InRunPlan",string("1"),radioalarmnode);

	std::string retXML;
	radioparser.SaveToString(retXML);
	return retXML;
}
std::string CreateAlarmXMLForRADIO::CreateProgramAlarmXML(const sCheckParam& checkparam,std::string alarmvalue,std::string alarmid)
{
	std::string radioxml ="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\" ?> ";//����xmlͷ
	radioxml+="<Msg><QualityAlarmReport></QualityAlarmReport></Msg>";
	XmlParser radioparser(radioxml.c_str());

// 	string  radioMsgID ;
// 	DBMANAGER::instance()->GetMsgID(radioMsgID);
// 	int radiomsgid=StrUtil::Str2Int(radioMsgID);
// 	DBMANAGER::instance()->UpdateMsgID(StrUtil::Int2Str(++radiomsgid));

	pXMLNODE radioroot=radioparser.GetRootNode();
	radioparser.SetAttrNode( "Version",string("1"),radioroot );//�汾

	string strDateTime = TimeUtil::GetCurDateTime();
	string  MsgID = OSFunction::GetXmlMsgID(strDateTime);
	radioparser.SetAttrNode( "MsgID",MsgID,radioroot );//��Ϣid����
	radioparser.SetAttrNode( "DateTime",strDateTime/*TimeUtil::GetCurDateTime()*/,radioroot );//��ǰʱ��
	//radioparser.SetAttrNode( "MsgID",radiomsgid,radioroot );//��Ϣid����
	//radioparser.SetAttrNode( "DateTime",TimeUtil::GetCurDateTime(),radioroot );//��ǰʱ��

	radioparser.SetAttrNode( "Type",string("RadioUp"),radioroot );//��Ϣ����
	radioparser.SetAttrNode( "SrcCode",PROPMANAGER::instance()->GetDefSrcCode(RADIO),radioroot );//������ʶ����ͨ���ӿڻ��
	radioparser.SetAttrNode( "DstCode",PROPMANAGER::instance()->GetDefDstCode(RADIO),radioroot );//Ŀ�������ʶ
	radioparser.SetAttrNode( "ReplyID",string("-1"),radioroot );//�ظ�����Ϣid

	pXMLNODE radioreportnode=radioparser.GetNodeFromPath("Msg/QualityAlarmReport");
	pXMLNODE radioalarmnode=radioparser.CreateNodePtr(radioreportnode,"QualityAlarm");
	//radioparser.SetAttrNode("EquCODE",string(""),radioalarmnode);
	radioparser.SetAttrNode("AlarmID",alarmid,radioalarmnode);
	radioparser.SetAttrNode("Mode",alarmvalue,radioalarmnode);
	string channelcode;
	CHANNELINFOMGR::instance()->GetChannelCodeByFreq(RADIO,checkparam.ChannelID,channelcode);
	radioparser.SetAttrNode("ChCode",channelcode,radioalarmnode);
	radioparser.SetAttrNode("Type",checkparam.TypeID,radioalarmnode);
	radioparser.SetAttrNode("Desc",checkparam.TypeDesc,radioalarmnode);
	radioparser.SetAttrNode("Reason",string(""),radioalarmnode);
	radioparser.SetAttrNode("Value",string("1"),radioalarmnode);
	radioparser.SetAttrNode("CheckTime",TimeUtil::DateTimeToStr(checkparam.CheckTime),radioalarmnode);
	radioparser.SetAttrNode("InRunPlan",string("1"),radioalarmnode);

	std::string retXML;
	radioparser.SaveToString(retXML);
	return retXML;
}
std::string CreateAlarmXMLForRADIO::CreateEnvironmentAlarmXML(const sCheckParam& checkparam,std::string alarmvalue)
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

	parser.SetAttrNode( "Type",string("RadioUp"),root );//��Ϣ����
	parser.SetAttrNode( "SrcCode",PROPMANAGER::instance()->GetDefSrcCode(RADIO),root );//������ʶ����ͨ���ӿڻ��
	parser.SetAttrNode( "DstCode",PROPMANAGER::instance()->GetDefDstCode(RADIO),root );//Ŀ�������ʶ
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