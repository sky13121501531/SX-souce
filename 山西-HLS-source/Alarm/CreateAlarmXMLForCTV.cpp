
#include "CreateAlarmXMLForCTV.h"
#include "../BusinessProcess/AlarmParamInfoMgr.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/XmlParser.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/AppLog.h"
#include "../Foundation/PropManager.h"
#include "../DBAccess/DBManager.h"
#include "../BusinessProcess/ChannelInfoMgr.h"

std::string CreateAlarmXMLForCTV::CreateFreqAlarmXML(const sCheckParam& checkparam,std::string alarmvalue,std::string alarmid)
{
	std::string atvxml ="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\" ?> ";//����xmlͷ
	atvxml+="<Msg><QualityAlarmReport></QualityAlarmReport></Msg>";
	XmlParser atvparser(atvxml.c_str());

// 	string  atvMsgID ;
// 	DBMANAGER::instance()->GetMsgID(atvMsgID);
// 	int atvmsgid=StrUtil::Str2Int(atvMsgID);
// 	DBMANAGER::instance()->UpdateMsgID(StrUtil::Int2Str(++atvmsgid));

	pXMLNODE atvroot=atvparser.GetRootNode();
	atvparser.SetAttrNode( "Version",string("1"),atvroot );//�汾

	string strDateTime = TimeUtil::GetCurDateTime();
	string  MsgID = OSFunction::GetXmlMsgID(strDateTime);
	atvparser.SetAttrNode( "MsgID",MsgID,atvroot );//��Ϣid����
	atvparser.SetAttrNode( "DateTime",strDateTime/*TimeUtil::GetCurDateTime()*/,atvroot );//��ǰʱ��
	//atvparser.SetAttrNode( "MsgID",atvmsgid,atvroot );//��Ϣid����
	//atvparser.SetAttrNode( "DateTime",TimeUtil::GetCurDateTime(),atvroot );//��ǰʱ��

	atvparser.SetAttrNode( "Type",string("TVMonUp"),atvroot );//��Ϣ����
	atvparser.SetAttrNode( "SrcCode",PROPMANAGER::instance()->GetDefSrcCode(CTV),atvroot );//������ʶ����ͨ���ӿڻ��
	atvparser.SetAttrNode( "DstCode",PROPMANAGER::instance()->GetDefDstCode(CTV),atvroot );//Ŀ�������ʶ
	atvparser.SetAttrNode( "ReplyID",string("-1"),atvroot );//�ظ�����Ϣid

	pXMLNODE atvreportnode=atvparser.GetNodeFromPath("Msg/QualityAlarmReport");
	pXMLNODE atvalarmnode=atvparser.CreateNodePtr(atvreportnode,"QualityAlarm");
	atvparser.SetAttrNode("EquCODE",string(""),atvalarmnode);
	atvparser.SetAttrNode("AlarmID",alarmid,atvalarmnode);
	atvparser.SetAttrNode("Mode",alarmvalue,atvalarmnode);
	string chcode;
	CHANNELINFOMGR::instance()->GetChannelCodeByFreq(CTV,checkparam.Freq,chcode);
	atvparser.SetAttrNode("ChannelCode",chcode,atvalarmnode);
	atvparser.SetAttrNode("Type",checkparam.TypeID,atvalarmnode);
	atvparser.SetAttrNode("Desc",checkparam.TypeDesc,atvalarmnode);
	atvparser.SetAttrNode("Reason",string(""),atvalarmnode);
	atvparser.SetAttrNode("Value",checkparam.TypedValue,atvalarmnode);
	atvparser.SetAttrNode("CheckTime",TimeUtil::GetCurDateTime(),atvalarmnode);
	atvparser.SetAttrNode("InRunPlan",string("1"),atvalarmnode);

	std::string retXML;
	atvparser.SaveToString(retXML);
	return retXML;
}
std::string CreateAlarmXMLForCTV::CreateProgramAlarmXML(const sCheckParam& checkparam,std::string alarmvalue,std::string alarmid)
{
	std::string atvxml ="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\" ?> ";//����xmlͷ
	atvxml+="<Msg><QualityAlarmReport></QualityAlarmReport></Msg>";
	XmlParser atvparser(atvxml.c_str());

// 	string  atvMsgID ;
// 	DBMANAGER::instance()->GetMsgID(atvMsgID);
// 	int atvmsgid=StrUtil::Str2Int(atvMsgID);
// 	DBMANAGER::instance()->UpdateMsgID(StrUtil::Int2Str(++atvmsgid));

	pXMLNODE atvroot=atvparser.GetRootNode();
	atvparser.SetAttrNode( "Version",string("1"),atvroot );//�汾

	string strDateTime = TimeUtil::GetCurDateTime();
	string  MsgID = OSFunction::GetXmlMsgID(strDateTime);
	atvparser.SetAttrNode( "MsgID",MsgID,atvroot );//��Ϣid����
	atvparser.SetAttrNode( "DateTime",strDateTime/*TimeUtil::GetCurDateTime()*/,atvroot );//��ǰʱ��
	//atvparser.SetAttrNode( "MsgID",atvmsgid,atvroot );//��Ϣid����
	//atvparser.SetAttrNode( "DateTime",TimeUtil::GetCurDateTime(),atvroot );//��ǰʱ��

	atvparser.SetAttrNode( "Type",string("TVMonUp"),atvroot );//��Ϣ����
	atvparser.SetAttrNode( "SrcCode",PROPMANAGER::instance()->GetDefSrcCode(CTV),atvroot );//������ʶ����ͨ���ӿڻ��
	atvparser.SetAttrNode( "DstCode",PROPMANAGER::instance()->GetDefDstCode(CTV),atvroot );//Ŀ�������ʶ
	atvparser.SetAttrNode( "ReplyID",string("-1"),atvroot );//�ظ�����Ϣid

	pXMLNODE atvreportnode=atvparser.GetNodeFromPath("Msg/QualityAlarmReport");
	pXMLNODE atvalarmnode=atvparser.CreateNodePtr(atvreportnode,"QualityAlarm");
	atvparser.SetAttrNode("EquCODE",string(""),atvalarmnode);
	atvparser.SetAttrNode("AlarmID",alarmid,atvalarmnode);
	atvparser.SetAttrNode("Mode",alarmvalue,atvalarmnode);
	string channelcode;
	CHANNELINFOMGR::instance()->GetChannelCodeByFreq(CTV,checkparam.ChannelID,channelcode);
	atvparser.SetAttrNode("ChannelCode",channelcode,atvalarmnode);
	atvparser.SetAttrNode("Type",checkparam.TypeID,atvalarmnode);
	atvparser.SetAttrNode("Desc",checkparam.TypeDesc,atvalarmnode);
	atvparser.SetAttrNode("Reason",string(""),atvalarmnode);
	atvparser.SetAttrNode("Value",string("1"),atvalarmnode);
	atvparser.SetAttrNode("CheckTime",TimeUtil::DateTimeToStr(checkparam.CheckTime),atvalarmnode);
	atvparser.SetAttrNode("InRunPlan",string("1"),atvalarmnode);

	std::string retXML;
	atvparser.SaveToString(retXML);
	return retXML;
}
std::string CreateAlarmXMLForCTV::CreateEnvironmentAlarmXML(const sCheckParam& checkparam,std::string alarmvalue)
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

	parser.SetAttrNode( "Type",string("TVMonUp"),root );//��Ϣ����
	parser.SetAttrNode( "SrcCode",PROPMANAGER::instance()->GetDefSrcCode(CTV),root );//������ʶ����ͨ���ӿڻ��
	parser.SetAttrNode( "DstCode",PROPMANAGER::instance()->GetDefDstCode(CTV),root );//Ŀ�������ʶ
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