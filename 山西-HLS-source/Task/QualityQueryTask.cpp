
#include "QualityQueryTask.h"
#include "TranslateDownXML.h"
#include "ace/Log_Msg.h"
#include "../Foundation/TimeUtil.h"
#include "../DBAccess/QualityDbManager.h"
#include "../Foundation/StrUtil.h"
#include "TranslateUpXML.h"
#include "../Foundation/AppLog.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
QualityQueryTask::QualityQueryTask() : DeviceIndependentTask()
{

}

QualityQueryTask::QualityQueryTask(std::string strXML) : DeviceIndependentTask(strXML)
{
}

QualityQueryTask::~QualityQueryTask()
{
}

void QualityQueryTask::Run()
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]ָ���ѯ����ִ�� !\n",DeviceID));
	SetRunning();
	XmlParser psr;
	psr.Set_xml(strStandardXML);
    RetValue = RUN_SUCCESS;
	std::string taskid,freq,startdatetime,enddatetime,reporttime,channelcode;
	std::string STD;
	std::map< string,vector<eQualityInfo> > FreqQualityInfo;

	std::string qualitypath="Msg/QualityQuery";
	pXMLNODE node = NULL;
	pXMLNODELIST childlist;
	try
	{
		node=psr.GetNodeFromPath((char*)qualitypath.c_str());
		childlist=psr.GetNodeList(node);//��ȡQualityQuery���ӽڵ��б�
	}
	catch(...)
	{
		RetValue = RUN_FAILED;
		string error = "��ȡQualityQuery�ڵ�ʧ��";
		APPLOG::instance()->WriteLog( OTHER,LOG_EVENT_ERROR,error,LOG_OUTPUT_BOTH );
		SendXML(TranslateUpXML::TranslateQualityQuery(this,FreqQualityInfo));
		SetFinised();
		ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]ָ���ѯ����ֹͣ !\n",DeviceID));
		return;
	}
	for (int i=0;i!=childlist->Size();++i)
	{
		pXMLNODE childnode=psr.GetNextNode(childlist);
		try
		{
			psr.GetAttrNode(childnode,string("TaskID"),taskid);                //��ȡQuality�ڵ��TaskID����
			psr.GetAttrNode(childnode,string("Freq"),freq);                    //��ȡQuality�ڵ��Freq����
			psr.GetAttrNode(childnode,string("STD"),STD);                      //��ȡQuality�ڵ��STD����
			psr.GetAttrNode(childnode,string("StartDateTime"),startdatetime);  //��ȡQuality�ڵ��StartDateTime����
			psr.GetAttrNode(childnode,string("EndDateTime"),enddatetime);      //��ȡQuality�ڵ��EndDateTime����
			psr.GetAttrNode(childnode,string("ReportTime"),reporttime);        //��ȡQuality�ڵ��ReportTime����
			psr.GetAttrNode(childnode,"ChannelCode",channelcode);
			if(channelcode!="")
				CHANNELINFOMGR::instance()->GetFreqByChannelCode(DVBType,channelcode,freq);
		}
		catch(...)
		{
			string error = "��ȡQuality�ڵ�����ʧ��";
			APPLOG::instance()->WriteLog( OTHER,LOG_EVENT_ERROR,error,LOG_OUTPUT_BOTH );
			continue;
		}
		vector<eQualityInfo> vecQualityInfo;
		//��ѯָ����������Ϣ
		if( QUALITYDBMANAGER::instance()->QueryQualityInfo(DVBType,freq,taskid,vecQualityInfo,startdatetime,enddatetime) ==false )
		{
			RetValue = DATABASEERROR;
			continue;
		}
	    if (vecQualityInfo.empty())
	    {
            eQualityInfo quality;
			quality.checktime ="";
			quality.STD = STD;
			quality.desc ="";
			quality.type ="";
			quality.valu = "";
			vecQualityInfo.push_back(quality);
	    }
		else
		{
			for (size_t j=0;j!=vecQualityInfo.size();++j)
			{
				vecQualityInfo[j].STD=STD;
			}
		}

		//���°����·�all����������⴦���Ѹ���taskid�������ָ����Ϣ������Ƶ����࣬���뵽map��
		if (channelcode=="All" || channelcode=="all")	
		{
			vector<eQualityInfo>::iterator quaInfoIter = vecQualityInfo.begin();
			for (; quaInfoIter!=vecQualityInfo.end(); quaInfoIter++)
			{
				string strTempFreq = quaInfoIter->freq;
				if (FreqQualityInfo.find(strTempFreq) != FreqQualityInfo.end())	//map�������Ƶ���ˣ���ֱ�Ӳ��뵽��Ӧ��vector��
				{
					FreqQualityInfo[strTempFreq].push_back( *quaInfoIter );
				}
				else	//map��û�����Ƶ��
				{
					std::vector<eQualityInfo> vecTempQuaInfo;
					vecTempQuaInfo.push_back( *quaInfoIter );
					FreqQualityInfo.insert( make_pair(strTempFreq, vecTempQuaInfo) );
				}
			}	
		}
		else
			FreqQualityInfo.insert(make_pair(freq,vecQualityInfo)); 
		if (reporttime!="")
		{
			//�����ϱ�����
		}
	}
	//��������͵�ǰ��ϵͳ
	SendXML(TranslateUpXML::TranslateQualityQuery(this,FreqQualityInfo));
	SetFinised();
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]ָ���ѯ����ֹͣ !\n",DeviceID));
}

std::string QualityQueryTask::GetTaskName()
{
	return "ָ���ѯ����";
}

std::string QualityQueryTask::GetObjectName()
{
	return std::string("QualityQueryTask");
}