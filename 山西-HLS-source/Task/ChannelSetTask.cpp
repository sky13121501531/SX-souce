
#include "ChannelSetTask.h"
#include "ace/Task.h"
#include "../Foundation/XmlParser.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/PropManager.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include "../DBAccess/DBManager.h"
#include "./TranslateDownXML.h"
#include "./TranslateUpXML.h"
#include <iostream>
using namespace std;

ChannelSetTask::ChannelSetTask() : DeviceIndependentTask()
{

}

ChannelSetTask::ChannelSetTask(std::string strXML) : DeviceIndependentTask(strXML)
{
}

ChannelSetTask::~ChannelSetTask()
{

}
void ChannelSetTask::Run(void)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]Ƶ����������ִ�� !\n",DeviceID));
	bRun = true;
	SetRunning();
	sChannelInfo channel;//Ƶ����Ϣ
	vector< sChannelInfo > vecChannel;//Ƶ��id
	XmlParser parser( strStandardXML.c_str());
	pXMLNODE channelSetNode = parser.GetNodeFromPath("Msg/ChannelSet");//ChannelSet�ڵ�
	pXMLNODELIST channelNodeList = parser.GetNodeList(channelSetNode);//ChannelSet�ڵ���ӽڵ��б�
	for (int i = 0;i!=channelNodeList->Size();i++)
	{
		pXMLNODE channelNode = parser.GetNextNode(channelNodeList);//channel�ڵ�
		
		if (DVBType==RADIO || DVBType==AM)
		{
			parser.GetAttrNode( channelNode,"Code",channel.ChannelCode );
			parser.GetAttrNode( channelNode,"Freq",channel.Freq );//
			parser.GetAttrNode( channelNode,"Freq",channel.ChannelID );//
		}
		else
		{
			parser.GetAttrNode( channelNode,"CODE",channel.ChannelCode );
			parser.GetAttrNode( channelNode,"VideoFreq",channel.Freq );//
			parser.GetAttrNode( channelNode,"VideoFreq",channel.ChannelID );//
		}
		
		parser.GetAttrNode( channelNode,"Name",channel.ProgramName);

		vecChannel.push_back( channel);//����Ƶ��id
	}
	//�������ݿ����ض�DVBTYPE��CHANNELID��Ϣ
	if( CHANNELINFOMGR::instance()->UpdateChannelInfo(GetDVBType(),vecChannel)==false )
		RetValue = RUN_FAILED;
	else
		RetValue = RUN_SUCCESS;

	SendXML(TranslateUpXML::TranslateChannelSet(this));//����xml
	SetFinised();//��������״̬Ϊ����
	bRun = false;
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]Ƶ����������ֹͣ !\n",DeviceID));
}
string ChannelSetTask::GetTaskName()
{
	return std::string("Ƶ����������");
}
std::string ChannelSetTask::GetObjectName()
{
	return std::string("ChannelSetTask");
}

