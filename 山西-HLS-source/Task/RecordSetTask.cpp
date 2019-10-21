///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：RecordSetTask.cpp
// 创建者：jiangcheng
// 创建时间：2009-06-01
// 内容描述：自动录像设置任务类
///////////////////////////////////////////////////////////////////////////////////////////
#include "RecordSetTask.h"
#include "TranslateDownXML.h"
#include "TranslateUpXML.h"
#include "RecordTask.h"
#include "../BusinessProcess/BusinessLayoutMgr.h"
#include "../DBAccess/DBManager.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/XmlParser.h"
#include <vector>
#include <iostream>
#include "ace/Task.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include "./QualityTask.h"
#ifdef ZONG_JU_ZHAO_BIAO
#include "../Foundation/AppLog.h"
#include "../Foundation/StrUtil.h"
#endif
//ACE_Thread_Mutex g_RecordSetTaskMutex;
using namespace std;

RecordSetTask::RecordSetTask() : DeviceIndependentTask()
{

}

RecordSetTask::RecordSetTask(std::string strXML) : DeviceIndependentTask(strXML)
{
	
}

RecordSetTask::~RecordSetTask()
{

}
void RecordSetTask::Run(void)
{
	//串行处理录像
	//ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]设置录像任务准备执行 !\n",DeviceID));
	//ACE_Guard<ACE_Thread_Mutex> guard(g_RecordSetTaskMutex);
	//ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]设置录像任务开始执行 !\n",DeviceID));
	
	bRun = true;
	SetRunning();
	std::string Action;
	std::vector<std::string> TaskXMLVec;
	std::vector<XMLTask*> vecTask;

#ifdef ZONG_JU_ZHAO_BIAO
	std::vector<sRecordSetInfo> vecRecSetInfo;	//存储任务信息以及任务是否正常执行起来
#endif

	RetValue = RUN_SUCCESS;
	TranslateDownXML::TranslateRecordTask(strStandardXML,TaskXMLVec);
	BUSINESSLAYOUTMGR::instance()->DelRecInfo(GetDVBType(),"0");	//删除所有其他自动录像
	PROPMANAGER::instance()->FreeRecTunerCoder(GetDVBType());
	//Sleep(2000);
#ifdef ZONG_JU_ZHAO_BIAO
	std::string strFreq, strServiceId, strAudioPid, strVideoPid, strChanId;
#endif
	std::string strTdevid = "";
	//
	std::vector<std::string>  vctXML;
	bool rnt = DBMANAGER::instance()->QueryTask(GetDVBType(),vctXML);
	for (int i=0;i<vctXML.size();i++)
	{
		DBMANAGER::instance()->DeleteTask(GetDVBType(),vctXML[i]);
	}
	//
	bool bEffective = TestTaskIsEffective(TaskXMLVec);
	for (size_t i=0; i< TaskXMLVec.size();i++)
	{
		XmlParser psr;
		int deviceid = -1;
		psr.Set_xml(TaskXMLVec[i]);
		pXMLNODE recordNode;
		if(TaskXMLVec[i].find("AutoRecord")!=string::npos)
		{
			recordNode=psr.GetNodeFromPath("Msg/AutoRecord/Record");
#ifdef ZONG_JU_ZHAO_BIAO
			//******************************
			//总局入网：自动录像删除时，添加获取频道信息代码
			psr.GetAttrNode(recordNode, "Freq", strFreq);
			psr.GetAttrNode(recordNode, "ServiceID", strServiceId);
			psr.GetAttrNode(recordNode, "VideoPID", strVideoPid);
			psr.GetAttrNode(recordNode, "AudioPID", strAudioPid);
			//******************************
#endif
		}
		else
		{
			recordNode=psr.GetNodeFromPath("Msg/TaskRecord/Record");
		}
		psr.GetAttrNode(recordNode,"Action",Action);
		psr.GetAttrNode(recordNode,"TaskID",TaskID);
		if((GetDVBType() == DVBC)&&(GetVersion() == "3.0"))
		{
			psr.GetAttrNode(recordNode,"CDeviceID",deviceid);
			psr.GetAttrNode(recordNode,"TDeviceID",strTdevid);  
			DeviceID = deviceid;
		}
		else
		{
			psr.GetAttrNode(recordNode,"DeviceID",deviceid);
			DeviceID = deviceid;
		}
		//
		string IndexSet = "",Step = "";
		psr.GetAttrNode(recordNode,"IndexSet",IndexSet);//指标报警标志
		psr.GetAttrNode(recordNode,"Step",Step);

		if(Action=="Set"||Action=="set")
		{
			//if (strFreq != strHDMIFreq)
			//{
			//	if (!bEffective)
			//		continue;
			//}
			RecordTask* recordtask = new RecordTask(TaskXMLVec[i]);

			//如果向数据库中添加任务失败了，就认为没有设置失败，跳出循环
			bool ret = BUSINESSLAYOUTMGR::instance()->AddRecTask(recordtask);
			//if (!ret)
			//{
			//	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 数据库中添加任务失败:%s !\n",recordtask->GetTaskName()));
			//	break;
			//}
			vecTask.push_back(recordtask);

#ifdef ZONG_JU_ZHAO_BIAO
			sRecordSetInfo recSetInfo;
			recSetInfo.strTaskXml = TaskXMLVec[i];
			recSetInfo.bSetAction = ret;
			recSetInfo.nDeviceId = recordtask->GetDeviceID();
			vecRecSetInfo.push_back( recSetInfo );
#endif
			//********************添加录像指标**********************
			if((IndexSet == "1" || ((IndexSet == "2")&&(Step.size() > 0)))&&(strFreq.size() > 0))
			{
				string CheckInterval = "";
				if(IndexSet == "1")
				{
					//指标监测报警
					CheckInterval = "33:33:33";//这种时间格式表示按报警门限录制指标
				}
				else
				{
					CheckInterval = TimeUtil::TimeToStr(StrUtil::Str2Int(Step));
				}
				//if(!(BUSINESSLAYOUTMGR::instance()->IsExistTaskByTaskid(GetDVBType(),strFreq)))
				//{
				//	CreateQualityTask(strFreq,CheckInterval,strTdevid);//以频点作为任务ID,时间是当前时间的一年
				//}
			}
			//********************添加录像指标**********************
			OSFunction::Sleep(0,5);
		 }
		else if(Action=="Del"||Action=="del")
		{	
#ifdef ZONG_JU_ZHAO_BIAO
			deviceid = PROPMANAGER::instance()->FreeDev(strFreq,strServiceId);
			//**********************************
#endif
			if(deviceid != -1 && !BUSINESSLAYOUTMGR::instance()->DelTask(deviceid,TaskID))
			{
				RetValue=DATABASEERROR;
			}
			else if(deviceid == 0 && !BUSINESSLAYOUTMGR::instance()->DelTask(TaskID))
			{
				RetValue=DATABASEERROR;
			}
			RetValue=RUN_SUCCESS;
			//********************删除录像指标**********************
			//if((IndexSet == "1" || ((IndexSet == "2")&&(Step.size() > 0)))&&(strFreq.size() > 0))
			if (strFreq.size() > 0)
			{
				if(!(BUSINESSLAYOUTMGR::instance()->IsExistRecTaskByFreq(GetDVBType(),strFreq)))
				{
					BUSINESSLAYOUTMGR::instance()->DelTask(strFreq);//以频点作为任务ID,时间是当前时间的一年
				}
			}
			//********************删除录像指标**********************
		}
	}

#ifdef ZONG_JU_ZHAO_BIAO
	//如果所有录像任务都设置失败，那么认为这个设置任务失败.
	if(Action=="Set"||Action=="set")
	{
		bool bSetRetValue = false;
		for (int n=0; n<vecRecSetInfo.size(); n++)
		{
			if (vecRecSetInfo[n].bSetAction)
			{
				bSetRetValue = true;
				break;
			}
		}
		if (!bSetRetValue)
		{
			this->SetRetValue( RUN_FAILED );
		}
	}

	if (Action == "Set" || Action == "set")
		SendXML(TranslateUpXML::TranslateRecord(this,vecRecSetInfo));//发送到前端
	else
#endif
		SendXML(TranslateUpXML::TranslateRecord(this,vecTask));//发送到前端
	
	SetFinised();
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) 通道[%d]设置录像任务停止 !\n",DeviceID));
}

std::string RecordSetTask::GetObjectName()
{
	return std::string("RecordSetTask");
}
string RecordSetTask::GetTaskName()
{
	return "设置录像任务";
}
bool RecordSetTask::CreateQualityTask(string freq,string CheckInterval,string TunerIndex)
{
	//***************************************
	/*<?xml version="1.0" encoding="GB2312" standalone="yes" ?> 
	<Msg Version="1" MsgID="2" DVBType="DVBC" TaskType="QualityTaskSet" Protocol="" DateTime="2006-08-17 15:30:00" SrcCode="110000X01" DstCode="110000N01" SrcURL="" Priority="1">
	<QualityTaskSet TaskID="100" Action="Set">
	<TaskParm>
	<QualityQueryTask STD="" Freq="698000" SymbolRate="" /> 
	</TaskParm>
	<QualityParam>
	<QualityIndex Type="0" Desc="ALL" /> 
	<QualityIndex Type="1" Desc="Level" /> 
	<QualityIndex Type="2" Desc="BER" /> 
	<QualityIndex Type="4" Desc="MER" /> 
	</QualityParam>
	<TimeParam>
	<TimeIndex DayOfWeek="" StartTime="" EndTime="" StartDateTime="2006-09-03 14:08:33" EndDateTime="2006-09-04 16:08:33" CheckInterval="00:00:10" /> 
	</TimeParam>
	</QualityTaskSet>
	</Msg>*/
	//***************************************
	bool ret = true;
	std::string xml ="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\" ?> ";//返回xml头
	xml+="<Msg><QualityTaskSet TaskID=\"";
	xml+=freq;
	xml+="\" TunerIndex=\"";
	xml+=TunerIndex;
	xml+="\" Action=\"Set\"><TaskParm><QualityQueryTask Freq=\"";
	xml+=freq;
	xml+="\" STD=\"\" SymbolRate=\"\"/></TaskParm><QualityParam><QualityIndex Type=\"0\" Desc=\"ALL\"/></QualityParam><TimeParam><TimeIndex/></TimeParam></QualityTaskSet></Msg>";
	XmlParser parser(xml.c_str());
	pXMLNODE root=parser.GetRootNode();
	parser.SetAttrNode( "Version",string("3.0"),root );//版本
	string strDateTime = TimeUtil::GetCurDateTime();
	string  MsgID = OSFunction::GetXmlMsgID(strDateTime);
	parser.SetAttrNode( "MsgID",MsgID,root );//消息id属性
	parser.SetAttrNode( "DateTime",strDateTime/*TimeUtil::GetCurDateTime()*/,root );//当前时间
	string TypeDvb;
	if(GetDVBType()==CTTB)
	{
		TypeDvb="CTTB";
	}
	else if(GetDVBType()==DVBC)
	{
		TypeDvb="DVBC";
	}
	else if(GetDVBType()==RADIO)
	{
		TypeDvb="RADIO";
	}
	else if(GetDVBType()==AM)
	{
		TypeDvb="AM";
	}
	else if(GetDVBType()==DVBS)
	{
		TypeDvb="DVBS";
	}
	else
	{
		TypeDvb="";
	}
	parser.SetAttrNode("DVBType",TypeDvb,root);               //设置Msg节点的DVBType属性值
	parser.SetAttrNode("TaskType",string("QualityTask"),root);    //设置Msg节点的TaskType属性值
	parser.SetAttrNode( "SrcCode",PROPMANAGER::instance()->GetDefSrcCode(GetDVBType()),root );//本机标识，可通过接口获得
	parser.SetAttrNode( "DstCode",PROPMANAGER::instance()->GetDefDstCode(GetDVBType()),root );//目标机器标识
	parser.SetAttrNode( "Priority",string("1"),root );//回复的消息id
	//
	pXMLNODE SReportTimeNode = parser.GetNodeFromPath("Msg/QualityTaskSet/TimeParam/TimeIndex");
	parser.SetAttrNode( "StartDateTime",strDateTime,SReportTimeNode );
	parser.SetAttrNode( "EndDateTime",TimeUtil::DateTimeToStr(time(0)+365*24*3600),SReportTimeNode );
	parser.SetAttrNode( "CheckInterval",CheckInterval,SReportTimeNode );
	string xmltask;
	parser.SaveToString(xmltask);
	//
	std::vector<std::string> strVecTask;
	TranslateDownXML::TranslateQualityTask(xmltask,strVecTask);
	for (size_t i=0; i!= strVecTask.size();++i)
	{
		QualityTask* qualitytask = new QualityTask(strVecTask[i]);//新的指标任务
		//添加新任务
		if( BUSINESSLAYOUTMGR::instance()->AddTask(qualitytask) == true )
		{

		}
	}
	return ret;
}

bool RecordSetTask::TestTaskIsEffective(std::vector<std::string> vecTaskXml)
{
	map<std::string, vector<std::string>> mapFreqServiceID;
	for (size_t i=0; i< vecTaskXml.size();++i)
	{
		XmlParser psr;
		psr.Set_xml(vecTaskXml[i]);
		pXMLNODE recordNode;
		if(vecTaskXml[i].find("AutoRecord")!=string::npos)
		{
			recordNode=psr.GetNodeFromPath("Msg/AutoRecord/Record");
			std::string strFreq, strServiceId, Action;
			psr.GetAttrNode(recordNode, "Freq", strFreq);
			psr.GetAttrNode(recordNode, "ServiceID", strServiceId);
			psr.GetAttrNode(recordNode,"Action",Action);
			if (Action == "Del" || Action == "del")
				break;
			vector<std::string> vecServiceID;
			if (mapFreqServiceID.size() > 0)
			{
				map<std::string, vector<std::string>>::iterator ite = mapFreqServiceID.find(strFreq);
				if (ite != mapFreqServiceID.end())
				{
					vecServiceID = (*ite).second;
					mapFreqServiceID.erase(ite);
				}
			}
			vecServiceID.push_back(strServiceId);
			mapFreqServiceID.insert(make_pair(strFreq, vecServiceID));
		}
	}
	if (mapFreqServiceID.size() == 0)
		return false;
	int iUnUsedCoderNum = PROPMANAGER::instance()->GetUnUsedCoderDevNum();
	if (iUnUsedCoderNum < vecTaskXml.size())
		return false;
	bool bRet = PROPMANAGER::instance()->IsHadUnUsedTunerOrCoder(this->GetDVBType(), mapFreqServiceID);

	return bRet;
}
