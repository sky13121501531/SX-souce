
#include "CreateAlarmXMLForCTTB.h"
#include "../BusinessProcess/AlarmParamInfoMgr.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/XmlParser.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/AppLog.h"
#include "../Foundation/PropManager.h"
#include "../DBAccess/DBManager.h"
#include "../BusinessProcess/ChannelInfoMgr.h"

std::string CreateAlarmXMLForCTTB::CreateFreqAlarmXML(const sCheckParam& checkparam,std::string alarmvalue,std::string alarmid)
{
	std::string xml ="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\" ?> ";//����xmlͷ
	xml+="<Msg></Msg>";
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

	parser.SetAttrNode( "Type",string("DVBTHUp"),root );//��Ϣ����
	parser.SetAttrNode( "SrcCode",PROPMANAGER::instance()->GetDefSrcCode(CTTB),root );//������ʶ����ͨ���ӿڻ��
	parser.SetAttrNode( "DstCode",PROPMANAGER::instance()->GetDefDstCode(CTTB),root );//Ŀ�������ʶ
	parser.SetAttrNode( "ReplyID",string("-1"),root );//�ظ�����Ϣid
	pXMLNODE node=NULL;
	pXMLNODE childnode=NULL;

	node=parser.CreateNodePtr(root,"AlarmFreqReport");
	childnode=parser.CreateNodePtr(node,"AlarmFreq");
	
	parser.SetAttrNode("EquCode",string(""),childnode);
	parser.SetAttrNode("AlarmID",alarmid,childnode);
	parser.SetAttrNode("Mode",alarmvalue,childnode);
	parser.SetAttrNode("SDT",checkparam.STD,childnode);
	parser.SetAttrNode("Freq",checkparam.Freq,childnode);
	parser.SetAttrNode("SymbolRate",checkparam.SymbolRate,childnode);
	parser.SetAttrNode("Type",checkparam.TypeID,childnode);
	parser.SetAttrNode("Desc",checkparam.TypeDesc,childnode);
	parser.SetAttrNode("Value",checkparam.TypedValue,childnode);

	parser.SetAttrNode("CheckTime",TimeUtil::DateTimeToStr(checkparam.CheckTime),childnode);
	parser.SetAttrNode("InRunPlan",string("1"),childnode);
	std::string error;
	if (alarmvalue=="0")
	{
		error=checkparam.TypeDesc+"����";
	}
	else
	{
		error=checkparam.TypeDesc+"�������";
	}
	parser.SetAttrNode("Reason",error,childnode);

	std::string retXML;
	parser.SaveToString(retXML);
	return retXML;
}
std::string CreateAlarmXMLForCTTB::CreateProgramAlarmXML(const sCheckParam& checkparam,std::string alarmvalue,std::string alarmid)
{
	std::string xml ="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\" ?> ";//����xmlͷ
	xml+="<Msg></Msg>";

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

	parser.SetAttrNode( "Type",string("DVBTHUp"),root );//��Ϣ����
	parser.SetAttrNode( "SrcCode",PROPMANAGER::instance()->GetDefSrcCode(CTTB),root );//������ʶ����ͨ���ӿڻ��
	parser.SetAttrNode( "DstCode",PROPMANAGER::instance()->GetDefDstCode(CTTB),root );//Ŀ�������ʶ
	parser.SetAttrNode( "ReplyID",string("-1"),root );//�ظ�����Ϣid
	pXMLNODE node=NULL;			
	pXMLNODE childnode=NULL;

	node=parser.CreateNodePtr(root,"AlarmProgramReport");
	childnode=parser.CreateNodePtr(node,"AlarmProgram");
	std::string apid,vpid,serviceid,prgname,tsid,netid;
	CHANNELINFOMGR::instance()->GetChannelInfo(CTTB,checkparam.ChannelID,vpid,apid,serviceid,prgname);
	CHANNELINFOMGR::instance()->GetTsID(CTTB,checkparam.Freq,apid,vpid,serviceid,tsid);
	CHANNELINFOMGR::instance()->GetOrgNetID(CTTB,checkparam.Freq,apid,vpid,serviceid,netid);

	int logindex;
	PROPMANAGER::instance()->GetDeviceLogIndex(StrUtil::Str2Int(checkparam.DeviceID),logindex);
	parser.SetAttrNode("Index",logindex,childnode);
	parser.SetAttrNode("EquCode",string(""),childnode);
	parser.SetAttrNode("AlarmID",alarmid,childnode);
	parser.SetAttrNode("Mode",alarmvalue,childnode);
	parser.SetAttrNode("OrgNetID",netid,childnode);
	parser.SetAttrNode("TsID",tsid,childnode);
	parser.SetAttrNode("ServiceID",serviceid,childnode);
	parser.SetAttrNode("Type",checkparam.TypeID,childnode);
	parser.SetAttrNode("Desc",checkparam.TypeDesc,childnode);
	parser.SetAttrNode("Value",checkparam.TypedValue,childnode);
	parser.SetAttrNode("CheckTime",TimeUtil::DateTimeToStr(checkparam.CheckTime),childnode);
	parser.SetAttrNode("InRunPlan",string("1"),childnode);

	std::string error;
	if (alarmvalue=="0")
	{
		error=checkparam.TypeDesc+"����";
	}
	else
	{
		error=checkparam.TypeDesc+"�������";
	}
	parser.SetAttrNode("Reason",error,childnode);

	string retXML = "";
	parser.SaveToString(retXML);
	return retXML;
}
std::string CreateAlarmXMLForCTTB::CreateEnvironmentAlarmXML(const sCheckParam& checkparam,std::string alarmvalue)
{
	std::string xml ="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\" ?> ";//����xmlͷ
	xml+="<Msg></Msg>";
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

	parser.SetAttrNode( "Type",string("DVBTHUp"),root );//��Ϣ����
	parser.SetAttrNode( "SrcCode",PROPMANAGER::instance()->GetDefSrcCode(CTTB),root );//������ʶ����ͨ���ӿڻ��
	parser.SetAttrNode( "DstCode",PROPMANAGER::instance()->GetDefDstCode(CTTB),root );//Ŀ�������ʶ
	parser.SetAttrNode( "ReplyID",string("-1"),root );//�ظ�����Ϣid
	pXMLNODE node=NULL;			
	pXMLNODE childnode=NULL;

	node=parser.CreateNodePtr(root,"AlarmEnvironmentReport");
	childnode=parser.CreateNodePtr(node,"AlarmEnvironment");
	parser.SetAttrNode("EquCode",string(""),childnode);
	parser.SetAttrNode("AlarmID",string(""),childnode);
	parser.SetAttrNode("Mode",alarmvalue,childnode);
	parser.SetAttrNode("Type",checkparam.TypeID,childnode);
	parser.SetAttrNode("Value",checkparam.TypedValue,childnode);
	parser.SetAttrNode("CheckTime",TimeUtil::DateTimeToStr(checkparam.CheckTime),childnode);
	parser.SetAttrNode("Desc",checkparam.TypeDesc,childnode);
	parser.SetAttrNode("Reason",checkparam.TypeDesc,childnode);
	parser.SetAttrNode("InRunPlan",string("1"),childnode);
	std::string retXML;
	parser.SaveToString(retXML);
	return retXML;
}