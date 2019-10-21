
#include "AlarmParamSetTask.h"
#include "TranslateDownXML.h"
#include "TranslateUpXML.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/XmlParser.h"
#include <vector>
#include <iostream>
#include "ace/Task.h"
#include "../DBAccess/DBManager.h"
#include "../BusinessProcess/AlarmParamInfoMgr.h"
#include "../Foundation/TimeUtil.h"
#include "./TranslateDownXML.h"
#include "../Foundation/AlarmPropManager.h"
using namespace std;

AlarmParamSetTask::AlarmParamSetTask() : DeviceIndependentTask()
{

}

AlarmParamSetTask::AlarmParamSetTask(std::string strXML) : DeviceIndependentTask(strXML)
{
}

AlarmParamSetTask::~AlarmParamSetTask()
{

}
void AlarmParamSetTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]����������������ִ�� !\n",DeviceID));
	bRun = true;
	SetRunning();
	XmlParser parser;
	parser.Set_xml(strStandardXML);
	pXMLNODE paramSetNode=parser.GetNodeFromPath("Msg/AlarmParamSet");
	std::string STD,Freq,SymbolRate;
	parser.GetAttrNode(paramSetNode,string("STD"),STD);//���STD����
	parser.GetAttrNode(paramSetNode,string("Freq"),Freq); //���Freq����
	parser.GetAttrNode(paramSetNode,string("SymbolRate"),SymbolRate); //���SymbolRate����
	parser.GetAttrNode(paramSetNode,string("ReturnTaskName"),m_strObjectName); //���SymbolRate����
	pXMLNODELIST paramSetNodeList=parser.GetNodeList(paramSetNode);//���������ڵ��б�
	vector<sAlarmParam> vevAlarmParam;
	for (int i=0;i!=paramSetNodeList->Size();++i)
	{
		sAlarmParam tempAlarmParam;
		tempAlarmParam.STD=STD;
		tempAlarmParam.Freq=Freq;
		tempAlarmParam.SymbolRate=SymbolRate;
		tempAlarmParam.DVBType = GetDVBType();//�����������
		pXMLNODE childNode=paramSetNodeList->GetNextNode();//�ӽڵ�
		std::string TypeID;
		parser.GetAttrNode(childNode,string("Type"),TypeID);//���Type����
		tempAlarmParam.TypeID=TypeID;
		int alarmType=atoi(TypeID.c_str());

		if(alarmType<20)
		{
			tempAlarmParam.AlarmType=ALARM_FREQ;
		}
		else if(alarmType<50)
		{
			tempAlarmParam.AlarmType=ALARM_TR101_290;
		}
		else
		{
			tempAlarmParam.AlarmType=ALARM_PROGRAM;
		}
		if(tempAlarmParam.Freq.length()<1)
		{
			parser.GetAttrNode(childNode,string("Freq"),tempAlarmParam.Freq);
		}
		parser.GetAttrNode(childNode,string("Desc"),tempAlarmParam.TypeDesc);//���Desc����	
		parser.GetAttrNode(childNode,string("ChannelID"),tempAlarmParam.ChannelID);
		parser.GetAttrNode(childNode,string("Duration"),tempAlarmParam.Duration);//�������
		parser.GetAttrNode(childNode,string("TimeInterval"),tempAlarmParam.TimeInterval);//ʱ����
		
		parser.GetAttrNode(childNode,string("Num"),tempAlarmParam.Num);//����
		parser.GetAttrNode(childNode,string("DownThreshold"),tempAlarmParam.DownThreshold);//����
		parser.GetAttrNode(childNode,string("UpThreshold"),tempAlarmParam.UpThreshold);//����
		
		parser.GetAttrNode(childNode,string("Switch"),tempAlarmParam.Switch);
		
		if (DVBType == THREED)
		{
			if (tempAlarmParam.Switch!="" )
			{
				m_strObjectName = "AlarmProgramSwitchSet";
			}
			else 
			{
				m_strObjectName = "AlarmProgramThresholdSet";
			}
		}
		
		vevAlarmParam.push_back(tempAlarmParam);

		if((DVBType==ATV&&TypeID=="10")||(DVBType==RADIO&&TypeID=="23")||(DVBType==AM&&TypeID=="23")||(DVBType==CTV&&TypeID=="10"))
		{
			string AlarmThreshold;
			parser.GetAttrNode(childNode,string("AlarmThreshold"),AlarmThreshold);
			ALARMPROPMANAGER::instance()->SetAlarmThreShold(DVBType,tempAlarmParam.Freq,AlarmThreshold);
		}
    }
	
	//���±���������Ϣ
	if(( ALARMPARAMINFOMGR::instance()->UpdateAlarmParam(vevAlarmParam)&&ALARMPARAMINFOMGR::instance()->UpdateAllAlarmParam(vevAlarmParam))==false )
		RetValue = RUN_FAILED;
	else
		RetValue = RUN_SUCCESS;
	SendXML(TranslateUpXML::TranslateAlarmParamSet(this));//���ͻظ�xml
	SetFinised();
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]����������������ֹͣ !\n",DeviceID));
}
string AlarmParamSetTask::GetTaskName()
{
	return "����������������";
}
std::string AlarmParamSetTask::GetObjectName()
{
	return m_strObjectName/*std::string("AlarmParamSetTask")*/;
}

