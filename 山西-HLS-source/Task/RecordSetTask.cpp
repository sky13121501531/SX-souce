///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����RecordSetTask.cpp
// �����ߣ�jiangcheng
// ����ʱ�䣺2009-06-01
// �����������Զ�¼������������
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
	//���д���¼��
	//ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]����¼������׼��ִ�� !\n",DeviceID));
	//ACE_Guard<ACE_Thread_Mutex> guard(g_RecordSetTaskMutex);
	//ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]����¼������ʼִ�� !\n",DeviceID));
	
	bRun = true;
	SetRunning();
	std::string Action;
	std::vector<std::string> TaskXMLVec;
	std::vector<XMLTask*> vecTask;

#ifdef ZONG_JU_ZHAO_BIAO
	std::vector<sRecordSetInfo> vecRecSetInfo;	//�洢������Ϣ�Լ������Ƿ�����ִ������
#endif

	RetValue = RUN_SUCCESS;
	TranslateDownXML::TranslateRecordTask(strStandardXML,TaskXMLVec);
	BUSINESSLAYOUTMGR::instance()->DelRecInfo(GetDVBType(),"0");	//ɾ�����������Զ�¼��
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
			//�ܾ��������Զ�¼��ɾ��ʱ����ӻ�ȡƵ����Ϣ����
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
		psr.GetAttrNode(recordNode,"IndexSet",IndexSet);//ָ�걨����־
		psr.GetAttrNode(recordNode,"Step",Step);

		if(Action=="Set"||Action=="set")
		{
			//if (strFreq != strHDMIFreq)
			//{
			//	if (!bEffective)
			//		continue;
			//}
			RecordTask* recordtask = new RecordTask(TaskXMLVec[i]);

			//��������ݿ����������ʧ���ˣ�����Ϊû������ʧ�ܣ�����ѭ��
			bool ret = BUSINESSLAYOUTMGR::instance()->AddRecTask(recordtask);
			//if (!ret)
			//{
			//	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ���ݿ����������ʧ��:%s !\n",recordtask->GetTaskName()));
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
			//********************���¼��ָ��**********************
			if((IndexSet == "1" || ((IndexSet == "2")&&(Step.size() > 0)))&&(strFreq.size() > 0))
			{
				string CheckInterval = "";
				if(IndexSet == "1")
				{
					//ָ���ⱨ��
					CheckInterval = "33:33:33";//����ʱ���ʽ��ʾ����������¼��ָ��
				}
				else
				{
					CheckInterval = TimeUtil::TimeToStr(StrUtil::Str2Int(Step));
				}
				//if(!(BUSINESSLAYOUTMGR::instance()->IsExistTaskByTaskid(GetDVBType(),strFreq)))
				//{
				//	CreateQualityTask(strFreq,CheckInterval,strTdevid);//��Ƶ����Ϊ����ID,ʱ���ǵ�ǰʱ���һ��
				//}
			}
			//********************���¼��ָ��**********************
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
			//********************ɾ��¼��ָ��**********************
			//if((IndexSet == "1" || ((IndexSet == "2")&&(Step.size() > 0)))&&(strFreq.size() > 0))
			if (strFreq.size() > 0)
			{
				if(!(BUSINESSLAYOUTMGR::instance()->IsExistRecTaskByFreq(GetDVBType(),strFreq)))
				{
					BUSINESSLAYOUTMGR::instance()->DelTask(strFreq);//��Ƶ����Ϊ����ID,ʱ���ǵ�ǰʱ���һ��
				}
			}
			//********************ɾ��¼��ָ��**********************
		}
	}

#ifdef ZONG_JU_ZHAO_BIAO
	//�������¼����������ʧ�ܣ���ô��Ϊ�����������ʧ��.
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
		SendXML(TranslateUpXML::TranslateRecord(this,vecRecSetInfo));//���͵�ǰ��
	else
#endif
		SendXML(TranslateUpXML::TranslateRecord(this,vecTask));//���͵�ǰ��
	
	SetFinised();
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]����¼������ֹͣ !\n",DeviceID));
}

std::string RecordSetTask::GetObjectName()
{
	return std::string("RecordSetTask");
}
string RecordSetTask::GetTaskName()
{
	return "����¼������";
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
	std::string xml ="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\" ?> ";//����xmlͷ
	xml+="<Msg><QualityTaskSet TaskID=\"";
	xml+=freq;
	xml+="\" TunerIndex=\"";
	xml+=TunerIndex;
	xml+="\" Action=\"Set\"><TaskParm><QualityQueryTask Freq=\"";
	xml+=freq;
	xml+="\" STD=\"\" SymbolRate=\"\"/></TaskParm><QualityParam><QualityIndex Type=\"0\" Desc=\"ALL\"/></QualityParam><TimeParam><TimeIndex/></TimeParam></QualityTaskSet></Msg>";
	XmlParser parser(xml.c_str());
	pXMLNODE root=parser.GetRootNode();
	parser.SetAttrNode( "Version",string("3.0"),root );//�汾
	string strDateTime = TimeUtil::GetCurDateTime();
	string  MsgID = OSFunction::GetXmlMsgID(strDateTime);
	parser.SetAttrNode( "MsgID",MsgID,root );//��Ϣid����
	parser.SetAttrNode( "DateTime",strDateTime/*TimeUtil::GetCurDateTime()*/,root );//��ǰʱ��
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
	parser.SetAttrNode("DVBType",TypeDvb,root);               //����Msg�ڵ��DVBType����ֵ
	parser.SetAttrNode("TaskType",string("QualityTask"),root);    //����Msg�ڵ��TaskType����ֵ
	parser.SetAttrNode( "SrcCode",PROPMANAGER::instance()->GetDefSrcCode(GetDVBType()),root );//������ʶ����ͨ���ӿڻ��
	parser.SetAttrNode( "DstCode",PROPMANAGER::instance()->GetDefDstCode(GetDVBType()),root );//Ŀ�������ʶ
	parser.SetAttrNode( "Priority",string("1"),root );//�ظ�����Ϣid
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
		QualityTask* qualitytask = new QualityTask(strVecTask[i]);//�µ�ָ������
		//���������
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
