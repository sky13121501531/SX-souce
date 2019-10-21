///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����TranslateUpXMLForCTTB.cpp
// �����ߣ�jiangcheng
// ����ʱ�䣺2009-06-04
// ������������ԭ������ִ�н����XMLת��ΪӦ��ϵͳ�����XML
///////////////////////////////////////////////////////////////////////////////////////////
#pragma warning(disable:4996)
#include "TranslateUpXMLForCTTB.h"
#include "../Foundation/XmlParser.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/AppLog.h"
#include "../Foundation/PropManager.h"
#include "../Task//StreamRealtimeQueryTask.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include "../Foundation/PropManager.h"
#include "../DBAccess/DBManager.h"
#include "../BusinessProcess/QualityCompensationMgr.h"
#include "../Foundation/OSFunction.h"
#include <fstream>
//����ʵʱ��ƵXML
std::string TranslateUpXMLForCTTB::TranslateStreamRealtimeQuery(const XMLTask* task )
{
	std::string retXml=GetXmlHeader(task,"StreamRealtimeQuery");//��׼�ķ���ͷxml

	std::string index, url;
	if (task->GetRetValue()==RUN_SUCCESS)
	{
		index="0";
		string protocol;

		//wz_110309(�޸Ľӿ�)
		PROPMANAGER::instance()->GetStreamProtocol(CTTB,protocol);
		if (protocol=="rtsp")
		{
			url="rtsp://" + PROPMANAGER::instance()->GetRtspVideoIp() + ":" + PROPMANAGER::instance()->GetRtspVideoPort() +"/"+ StrUtil::Int2Str(task->GetDeviceID());//��Ƶ���ŵ�ַ
		}
		else
		{
			url=protocol+"://" + PROPMANAGER::instance()->GetHttpVideoIp() + ":" + PROPMANAGER::instance()->GetHttpVideoPort() +"/"+ StrUtil::Int2Str(task->GetDeviceID());//��Ƶ���ŵ�ַ
		}
		
	}
	XmlParser retParser;
	retParser.Set_xml(retXml);

	pXMLNODE rootNode=retParser.GetRootNode();
	pXMLNODE streamNode=retParser.CreateNodePtr(rootNode,"Stream");//����Stream�ڵ�
	pXMLNODE mediaStreamNode=retParser.CreateNodePtr(streamNode,"MediaStream");//����Stream�ڵ���ӽڵ�MediaStream�ڵ�

	retParser.SetAttrNode("index",index,mediaStreamNode);//����MediaStream�ڵ��index����
	retParser.SetAttrNode("URL",url,mediaStreamNode);//����MediaStream�ڵ��URL����
	retParser.SaveToString(retXml);
	return retXml;
}
//�����ֲ���ѯXML
std::string TranslateUpXMLForCTTB::TranslateStreamRoundQuery(const XMLTask* task,std::string deviceID,bool multi)
{
	std::string retXml;//���ص�xml
	std::string retType="StreamRoundQuery";//Ӳ������xml��return�ڵ�����ͺ�ֵ

	std::string headerxml=GetXmlHeader(task,retType);//��׼�ķ���ͷxml

	std::string index, url;
	if(task->GetRetValue()==5)
	{	
		XmlParser headParser;
		headParser.Set_xml(headerxml);
		pXMLNODE retNode = headParser.GetNodeFromPath("Msg/Return");
		headParser.SetAttrNode( "Type",string("StreamRoundQuery"),retNode );
		headParser.SaveToString(retXml);
		return retXml;
	}
	if (task->GetRetValue()==0)
	{
		index="0";
		
		string protocol;

		//wz_110309(�޸Ľӿ�)
		PROPMANAGER::instance()->GetStreamProtocol(CTTB,protocol);
		if (protocol=="rtsp")
		{
			url="rtsp://" + PROPMANAGER::instance()->GetRtspVideoIp() + ":" + \
				PROPMANAGER::instance()->GetRtspVideoPort() +"/roundstream" ;//��Ƶ���ŵ�ַ
		}
		else
		{
			//wz_0217
			int virtualid = 0;
			PROPMANAGER::instance()->GetVirDeviceId(CTTB, virtualid);
			url=protocol+"://" + PROPMANAGER::instance()->GetHttpVideoIp() + ":" + \
				PROPMANAGER::instance()->GetHttpVideoPort() +"/"+ StrUtil::Int2Str(virtualid);	//��Ƶ���ŵ�ַ
			//wz_0217
		}

	}
	XmlParser retParser;
	retParser.Set_xml(headerxml);

	pXMLNODE rtnNode=retParser.GetNodeFromPath("Msg/Return");//Return�ڵ�

	pXMLNODE rootNode=retParser.GetRootNode();
	pXMLNODE streamNode=retParser.CreateNodePtr(rootNode,"Stream");//����Stream�ڵ�
	pXMLNODE mediaStreamNode=retParser.CreateNodePtr(streamNode,"MediaStream");//����Stream�ڵ���ӽڵ�MediaStream�ڵ�

	retParser.SetAttrNode("Type",retType,rtnNode);//����Return�ڵ��Type����ֵ
	retParser.SetAttrNode("index",index,mediaStreamNode);//����MediaStream�ڵ��index����
	retParser.SetAttrNode("URL",url,mediaStreamNode);//����MediaStream�ڵ��URL����
	retParser.SaveToString(retXml);

	return retXml;
}
//����Ƶ��ɨ��XML
std::string TranslateUpXMLForCTTB::TranslateChannelScan(const XMLTask* task,const std::string& strRtnXML)
{
	std::string retXml=GetXmlHeader(task,"ChannelScanQuery");//��׼�ķ���ͷxml

	XmlParser parser;
	parser.Set_xml(retXml);

	pXMLNODE channelScanReportNode=parser.CreateNodePtr(parser.GetRootNode(),"ChannelScanReport");//Ϊ���ص�xml����ChannelScanReport�ڵ�

	//��ʼ������
	string   STD, Freq, QAM, HeaderMode, CarrierNumber, EncodeEfficiency, InterwovenMode;
	string   DoublePilot, PnPhase, OrgNetID, TsID;
	string   Program, ServiceID, VideoPID, AudioPID, Encryption; 

	XmlParser retParser;
	retParser.Set_xml(strRtnXML);

	pXMLNODE rtnTypeNode=retParser.GetNodeFromPath("Msg/Return");
	std::string valu="";
	retParser.GetAttrNode(rtnTypeNode,"Value",valu);//����Ӳ������xml�е�valueֵ���Դ��ж�Ӳ��ִ�гɹ����
	if (valu=="0")
	{
		pXMLNODE rtnChannelScanReportNode=retParser.GetNodeFromPath("Msg/ChannelScanReport");//Ӳ������xml��ChannelScanReport�ڵ�
		pXMLNODELIST nodeList=retParser.GetNodeList(rtnChannelScanReportNode);//Ӳ������xml��ChannelScanReport�ڵ���ӽڵ��б�
		for (int j=0;j!=nodeList->Size();++j)
		{
			pXMLNODE channelScanNode=parser.CreateNodePtr(channelScanReportNode,"ChannelScan");//Ϊ���ص�xml����ChannelScan�ڵ�
			pXMLNODE childNode=retParser.GetNextNode(nodeList);//Ӳ������xml��ChannelScan�ڵ���ӽڵ��б�

			retParser.GetAttrNode(childNode,"STD",STD);//���STD����ֵ
			retParser.GetAttrNode(childNode,"Freq",Freq);//���Freq����ֵ
			retParser.GetAttrNode(childNode,"QAM",QAM);//���QAM������ֵ
			retParser.GetAttrNode(childNode,"HeaderMode",HeaderMode);//���HeaderMode����ֵ
			retParser.GetAttrNode(childNode,"CarrierNumber",CarrierNumber);//���CarrierNumber����ֵ
			retParser.GetAttrNode(childNode,"EncodeEfficiency",EncodeEfficiency);//���EncodeEfficiency����ֵ
			retParser.GetAttrNode(childNode,"InterwovenMode",InterwovenMode);//���InterwovenMode����ֵ
			retParser.GetAttrNode(childNode,"DoublePilot",DoublePilot);//���DoublePilot����ֵ
			retParser.GetAttrNode(childNode,"PnPhase",PnPhase);//���PnPhase����ֵ
			retParser.GetAttrNode(childNode,"OrgNetID",OrgNetID);//���OrgNetID����ֵ
			retParser.GetAttrNode(childNode,"TsID",TsID);//���TsID����ֵ

			parser.SetAttrNode("STD",STD,channelScanNode);//���÷���xml��ChannelScan�ڵ��STD����ֵ
			parser.SetAttrNode("Freq",Freq,channelScanNode);//���÷���xml��ChannelScan�ڵ��Freq����ֵ
			parser.SetAttrNode("QAM",QAM,channelScanNode);//���÷���xml��ChannelScan�ڵ��QAM����ֵ
			parser.SetAttrNode("HeaderMode",HeaderMode,channelScanNode);//���÷���xml��ChannelScan�ڵ��HeaderMode����ֵ
			parser.SetAttrNode("CarrierNumber",CarrierNumber,channelScanNode);//���÷���xml��ChannelScan�ڵ��CarrierNumber����ֵ
			parser.SetAttrNode("EncodeEfficiency",EncodeEfficiency,channelScanNode);//���÷���xml��ChannelScan�ڵ��EncodeEfficiency����ֵ
			parser.SetAttrNode("InterwovenMode",InterwovenMode,channelScanNode);//���÷���xml��ChannelScan�ڵ��InterwovenMode����ֵ
			parser.SetAttrNode("DoublePilot",DoublePilot,channelScanNode);//���÷���xml��ChannelScan�ڵ��DoublePilot����ֵ
			parser.SetAttrNode("PnPhase",PnPhase,channelScanNode);//���÷���xml��ChannelScan�ڵ��PnPhase����ֵ
			parser.SetAttrNode("OrgNetID",OrgNetID,channelScanNode);//���÷���xml��ChannelScan�ڵ��OrgNetID����ֵ
			parser.SetAttrNode("TsID",TsID,channelScanNode);//���÷���xml��ChannelScan�ڵ��TsID����ֵ

			pXMLNODELIST childNodeList=retParser.GetNodeList(childNode);//Ӳ������xml��ChannelScan�ڵ���ӽڵ��б�
			for (int k=0;k!=childNodeList->Size();++k)
			{
				pXMLNODE channelNode=parser.CreateNodePtr(channelScanNode,"Channel");//Ϊ���ص�xml����Channel�ڵ�

				pXMLNODE subChildNode=retParser.GetNextNode(childNodeList);//Ӳ������xml��ChannelScan�ڵ���ӽڵ�channel
				retParser.GetAttrNode(subChildNode,"Program",Program);//���Program����ֵ
				retParser.GetAttrNode(subChildNode,"ServiceID",ServiceID);//���ServiceID����ֵ
				retParser.GetAttrNode(subChildNode,"VideoPID",VideoPID);//���VideoPID����ֵ
				retParser.GetAttrNode(subChildNode,"AudioPID",AudioPID);//���AudioPID����ֵ
				retParser.GetAttrNode(subChildNode,"Encryption",Encryption);//���Encryption����ֵ

				parser.SetAttrNode("Program",Program,channelNode);//���÷���xml��Channel�ڵ��Program����ֵ
				parser.SetAttrNode("ServiceID",ServiceID,channelNode);//���÷���xml��Channel�ڵ��ServiceID����ֵ
				parser.SetAttrNode("VideoPID",VideoPID,channelNode);//���÷���xml��Channel�ڵ��VideoPID����ֵ
				parser.SetAttrNode("AudioPID",AudioPID,channelNode);//���÷���xml��Channel�ڵ��AudioPID����ֵ
				parser.SetAttrNode("Encryption",Encryption,channelNode);//���÷���xml��Channel�ڵ��Encryption����ֵ
			}
		}
	}
	parser.SaveToString(retXml);
	return retXml;
}
//����Ƶ������xml
std::string TranslateUpXMLForCTTB::TranslateChannelSet( const XMLTask* task )
{
	std::string retXml=GetXmlHeader(task,"ChannelSetReport");//��׼�ķ���ͷxml
	return retXml;
}

//������Ƶ�û���ѯXML
std::string TranslateUpXMLForCTTB::TranslateClientInfoQuery(const XMLTask* task,std::vector<sVedioUserInfo>& vecClient)
{
	std::string retXml=GetXmlHeader(task,"ClientInfoQuery");
	XmlParser parser;
	parser.Set_xml(retXml);

	pXMLNODE rootNode=parser.GetRootNode();
	pXMLNODE newNode=parser.CreateNodePtr(rootNode,"ClientInfoQueryReport");

	//���ݿͻ��˵���Ϣ����xml
	if(vecClient.size()>0)
	{
		std::string serverip="http://"+PROPMANAGER::instance()->GetHttpVideoIp()+"/";
		//����ClientInfoQueryReport�ڵ�
		pXMLNODE ClientNode = parser.GetNodeFromPath("Msg/ClientInfoQueryReport");
		size_t count=0;
		while (count<vecClient.size())
		{
			sVedioUserInfo user=vecClient[count];
			pXMLNODE clientinfonode=parser.CreateNodePtr(ClientNode,"ClientInfo");
			parser.SetAttrNode(std::string("URL"),serverip+user.DeviceID,clientinfonode);
			for (;count < vecClient.size();++count)
			{
				if (user.DeviceID==vecClient[count].DeviceID)
				{
					//��д���ӿͻ���Ϣ
					pXMLNODE childnode=parser.CreateNodePtr(clientinfonode,"Client");
					parser.SetAttrNode(std::string("IP"),vecClient[count].IP,childnode);
					parser.SetAttrNode(std::string("UserName"),vecClient[count].Name,childnode);
					parser.SetAttrNode(std::string("Priority"),vecClient[count].Priority,childnode);
					parser.SetAttrNode(std::string("CenterCode"),string("900000X20"),childnode);
				}
				else
					break;
			}
		}
	}
	parser.SaveToString(retXml);

	return retXml;

}
//���ر��ѯXML
std::string TranslateUpXMLForCTTB::TranslateTableQuery(const XMLTask *task, std::string strFileName)
{
	std::string rtnxml = GetXmlHeader(task,"TableQuery");

	XmlParser parser;
	parser.Set_xml(rtnxml);

	pXMLNODE queryReport = parser.CreateNodePtr( parser.GetRootNode(),"TableQueryReport" );
	pXMLNODE tableQuery = parser.CreateNodePtr( queryReport,"TableQuery" );

	std::string strURL;

	if (task->GetRetValue() == RUN_SUCCESS)
	{
		//modified by jidushan 11.03.31 : ����·����Ϊͨ�������ļ���ȡ
		strURL= "http://" + PROPMANAGER::instance()->GetHttpServerIP() + ":";
		strURL += PROPMANAGER::instance()->GetHttpServerPort();
		std::string sharepath;
		PROPMANAGER::instance()->GetTableSharePath(task->GetDVBType(), sharepath);
		strURL += sharepath + strFileName;
	}

	parser.SetAttrNode( "URL",strURL,tableQuery ); //����URL����
	parser.SetAttrNode( "DateTime",TimeUtil::GetCurDateTime(),tableQuery ); //����DateTime����

	parser.SaveToString(rtnxml); //���浽�ַ���
	return rtnxml;

}
//����¼������XML
std::string TranslateUpXMLForCTTB::TranslateRecord(const XMLTask* task,const std::vector<XMLTask*>& resultVec)
{
	std::string retXML;

	if(task->GetTaskID() == "0")
	{
		//retXML=GetXmlHeader(task,"SetAutoRecordChannel");
		retXML=GetXmlHeader(task,"SetAutoRecordChannel");
		XmlParser parser;
		parser.Set_xml(retXML);
		pXMLNODE ReportNode=parser.CreateNodePtr("Msg","SetAutoRecordChannelReport");
		pXMLNODE ChanelNode=parser.CreateNodePtr(ReportNode,"Channel");
		parser.SetAttrNode("EquCode",task->GetDstCode(),ChanelNode);
		parser.SetAttrNode("Action",string("Set"),ChanelNode);
		string Desc;
		eTaskRetStatus retValue=task->GetRetValue();
		switch(retValue)
		{
		case RUN_SUCCESS:
			Desc="�ɹ�";
			break;
		case RUN_FAILED:
			Desc="ʧ��";
			break;
		case NOFILE_EXIST:
			Desc="�ļ�������";
			break;
		case CHANNELID_UNAVAIABLE:
			Desc="Ƶ���Ƿ�";
			break;
		case DEVICEID_UNAVAIABLE:
			Desc="ͨ���Ƿ�";
			break;
		case NODEVICEIDMATCH:
			Desc="û��ͨ��ƥ�������";
			break;
		case PREFERENTIALTASK_USE:
			Desc="�����ȼ�����ռ��ͨ��";
			break;
		case DATABASEERROR:
			Desc="�������ݿ����";
			break;
		case DEVICEERROR:
			Desc="����Ӳ������";
			break;
		case RECORD_DOWN_OUTOFRAND:
			Desc="¼��ʱ�䳬����";
			break;
		default:
			Desc="ʧ��";
			break;
		}
		for(int i=0;i<resultVec.size();i++)
		{
			string devicexml=resultVec[i]->GetDeviceXml();
			XmlParser tempparser;
			string OrgNetID,TsID,ServiceID;
			tempparser.Set_xml(devicexml);
			pXMLNODE tempNode=tempparser.GetNodeFromPath("Msg/TSQuery/TS");
			tempparser.GetAttrNode(tempNode,"OrgNetID",OrgNetID);
			tempparser.GetAttrNode(tempNode,"TsID",TsID);
			tempparser.GetAttrNode(tempNode,"ServiceID",ServiceID);
			pXMLNODE ChNode=parser.CreateNodePtr(ChanelNode,"ChCode");
			int logindex=0;
			PROPMANAGER::instance()->GetDeviceLogIndex(resultVec[i]->GetDeviceID(),logindex);
			parser.SetAttrNode("Index",logindex,ChNode);
			parser.SetAttrNode("DeviceID",logindex,ChNode);
			parser.SetAttrNode("OrgNetID",OrgNetID,ChNode);
			parser.SetAttrNode("TsID",TsID,ChNode);
			parser.SetAttrNode("ServiceID",ServiceID,ChNode);
			parser.SetAttrNode("Width",string(""),ChNode);
			parser.SetAttrNode("Height",string(""),ChNode);
			parser.SetAttrNode("Fps",string(""),ChNode);
			parser.SetAttrNode("Bps",string(""),ChNode);
			parser.SetAttrNode("Value",retValue,ChNode);
			parser.SetAttrNode("Desc",Desc,ChNode);
			parser.SetAttrNode("Comment",string(""),ChNode);		
		}
		parser.SaveToString(retXML);
	}
	else
	{
		retXML=GetXmlHeader(task,"TaskRecordSet");
	}
	return retXML;
}

//����ָ����������xml
std::string TranslateUpXMLForCTTB::TranslateQualitySet( const XMLTask* task )
{
	std::string retXml=GetXmlHeader(task,"QualityReportTaskSet");//��׼�ķ���ͷxml
	return retXml;
}
//����ָ���ѯXML
std::string TranslateUpXMLForCTTB::TranslateQualityQuery( const XMLTask* task,std::map< string,vector<eQualityInfo> > FreqQualityInfo)
{
	string rtnHead =  GetXmlHeader(task,"QualityQuery");

	//����������Ķ���
	XmlParser parser;
	parser.Set_xml(rtnHead);

	//���÷���XML��Ԫ������
	size_t count=0;//ָ���������Ŀ������1000�����
	std::map< string,vector<eQualityInfo> >::iterator ptr=FreqQualityInfo.begin();
	for (;ptr!=FreqQualityInfo.end();++ptr)
	{
		count+=(*ptr).second.size();
	}
	if (count>1000)
	{
		string strURL,sharepath;
		sharepath=PROPMANAGER::instance()->GetTempSharePath();
		strURL= "http://" + PROPMANAGER::instance()->GetHttpServerIP() + ":" ;
		strURL += PROPMANAGER::instance()->GetHttpServerPort() + sharepath+"Quality_";
		time_t curtime;
		int type=0;
		float valu=0.0;
		string strtime=TimeUtil::DateTimeToString(time(0));
		string dstZipPath=PROPMANAGER::instance()->GetTempFileLoac()+"Quality_"+strtime+".zip";
		string srcFilePath=PROPMANAGER::instance()->GetTempFileLoac()+"Quality_"+strtime+".txt";
		strURL +=strtime+".zip";
		ofstream ofile(srcFilePath.c_str());
		std::map< string,vector<eQualityInfo> >::iterator itr=FreqQualityInfo.begin();
		for (;itr!=FreqQualityInfo.end();++itr)
		{
			for(vector<eQualityInfo>::iterator vecitr=(*itr).second.begin();vecitr!=(*itr).second.end();vecitr++)
			{
				std::string channelcode;
				CHANNELINFOMGR::instance()->GetChannelCodeByFreq(CTTB,vecitr->freq,channelcode);
				curtime=TimeUtil::StrToDateTime(vecitr->checktime);
				type =StrUtil::Str2Int(vecitr->type);
				valu = StrUtil::Str2Float(vecitr->valu)/1000;
				ofile<<channelcode<<" "<<curtime<<" "<<type<<" "<<valu<<endl;
			}
		}
		ofile.close();
		OSFunction::CompressFile(dstZipPath,srcFilePath);

		pXMLNODE filenode=parser.CreateNodePtr(parser.GetRootNode(),"QualityReportFile");
		parser.SetAttrNode(string("URL"),strURL,filenode); //����ѹ�����ݴ�ŵ�ַ
		parser.SetAttrNode(string("Size"),(int)count,filenode); //ѹ�������ļ���С
	}
	else
	{
		for (std::map< string,vector<eQualityInfo> >::iterator ptr=FreqQualityInfo.begin();ptr!=FreqQualityInfo.end();++ptr)
		{
			pXMLNODE reportnode=parser.CreateNodePtr(parser.GetRootNode(),"QualityReport");

			std::map< string,vector<eQualityInfo> >  TimeQualityInfo;
			//��checktime����������ͬchecktime��eQualityInfo����ͬһvector������
			for (size_t i = 0;i<(*ptr).second.size();i++)
			{
				TimeQualityInfo[(*ptr).second.at(i).checktime].push_back((*ptr).second.at(i));
			}
			//���ڲ�ͬ��checktime����Quality�ڵ㼰QualityIndex�ڵ��б�
			for (std::map< string,vector<eQualityInfo> >::iterator in_ptr=TimeQualityInfo.begin();in_ptr!=TimeQualityInfo.end();++in_ptr)
			{
				try
				{
					pXMLNODE qualitynode=parser.CreateNodePtr(reportnode,"Quality");
					parser.SetAttrNode(string("STD"),(*in_ptr).second[0].STD,qualitynode); //����Quality�ڵ��STD����
					parser.SetAttrNode(string("Freq"),(*ptr).first,qualitynode);  //����Quality�ڵ��Freq����
					parser.SetAttrNode(string("CheckTime"),(*in_ptr).first,qualitynode); //����Quality�ڵ��CheckTime����
					//��ͬSTD�� Freq�� CheckTime��ָ�����б�
					for (vector<eQualityInfo>::iterator pptr=(*in_ptr).second.begin();pptr!=(*in_ptr).second.end();++pptr)
					{

						pXMLNODE qualityindexnode=parser.CreateNodePtr(qualitynode,"QualityIndex");
						parser.SetAttrNode(string("Type"),(*pptr).type,qualityindexnode); //����QualityIndex�ڵ��Type����
						parser.SetAttrNode(string("Desc"),(*pptr).desc,qualityindexnode); //����QualityIndex�ڵ��Desc����
						parser.SetAttrNode(string("Value"),(*pptr).valu,qualityindexnode); //����QualityIndex�ڵ��Value����
					}
				}
				catch(...)
				{
					string error = "����QualityReport�ӽڵ�����ʧ��";
					APPLOG::instance()->WriteLog( OTHER,LOG_EVENT_ERROR,error,LOG_OUTPUT_BOTH );
					return "";
				}
			}
		}
	}
	string rtnXML;
	parser.SaveToString(rtnXML);
	return  rtnXML;
}


//���ر�����������XML
std::string TranslateUpXMLForCTTB::TranslateAlarmParamSet( const XMLTask* task )
{
	std::string retXml=GetXmlHeader(task,"AlarmParamSet");//��׼�ķ���ͷxml
	return retXml;
}


//����ͨ��״̬��ѯXML
std::string TranslateUpXMLForCTTB::TranslateDeviceStatusQuery(const XMLTask* task,std::vector<sDeviceStatus> & VecDeviceStatus)
{
	std::string retXml=GetXmlHeader(task,"DeviceStatusQuery");
	XmlParser parser;
	parser.Set_xml(retXml);
	pXMLNODE rootNode=parser.GetRootNode();
	pXMLNODE deviceNode=parser.CreateNodePtr(rootNode,"ReturnInfo");

	pXMLNODE statusnode=parser.GetNodeFromPath("Msg/ReturnInfo");
	for (size_t i=0;i!=VecDeviceStatus.size();++i)
	{
		pXMLNODE nodeinfo=parser.CreateNodePtr(statusnode,"GetDeviceIndexStatus");//����GetDeviceIndexStatus�ڵ�
		//����GetDeviceIndexStatus�ڵ������
		parser.SetAttrNode("DeviceID",VecDeviceStatus[i].DevicID,nodeinfo);
		parser.SetAttrNode("Status",VecDeviceStatus[i].Statuc,nodeinfo);
		parser.SetAttrNode("Desc",VecDeviceStatus[i].Desc,nodeinfo);

	}
	parser.SaveToString(retXml);
	return retXml;
}

//��������ͼ����XML
std::string TranslateUpXMLForCTTB::TranslateRunPlanSet(const XMLTask* task)
{
	std::string retXml=GetXmlHeader(task,"RunPlanScan");//��׼�ķ���ͷxml
	return retXml;
}

//�����Զ�¼������Ƶ��ѯXML
std::string TranslateUpXMLForCTTB::TranslateAutoRecordStream( const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime )
{
	std::string retXml=GetXmlHeader(task,"AutoRecordQuery");//��׼�ķ���ͷxml

	XmlParser parser;
	parser.Set_xml(retXml);

	pXMLNODE autoRecordReportNode=parser.CreateNodePtr(parser.GetRootNode(),"AutoRecordReport");
	pXMLNODE autoRecordNode=parser.CreateNodePtr(autoRecordReportNode,"AutoRecord");

	parser.SetAttrNode("URL",URL,autoRecordNode);
	parser.SetAttrNode("StartDateTime",startDateTime,autoRecordNode);
	parser.SetAttrNode("EndDateTime",endDateTime,autoRecordNode);

	parser.SaveToString(retXml);
	return retXml;
}

//�����Զ�¼���Ŀ�ļ�����XML
std::string TranslateUpXMLForCTTB::TranslateAutoRecordFile( const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime)
{
	std::string retXml=GetXmlHeader(task,"AutoRecordFileQuery");//��׼�ķ���ͷxml

	XmlParser parser;
	parser.Set_xml(retXml);

	pXMLNODE autoRecordFileReportNode=parser.CreateNodePtr(parser.GetRootNode(),"AutoRecordFileReport");
	pXMLNODE autoRecordNode=parser.CreateNodePtr(autoRecordFileReportNode,"AutoRecord");
	if (URL.empty())
	{
		parser.SetAttrNode("URL",string(""),autoRecordNode);
		parser.SetAttrNode("StartDateTime",string(""),autoRecordNode);
		parser.SetAttrNode("EndDateTime",string(""),autoRecordNode);
	}
	else
	{
		
		parser.SetAttrNode("URL",URL,autoRecordNode);    //����AutoRecord�ڵ��URL����	
		parser.SetAttrNode("StartDateTime",startDateTime,autoRecordNode);
		parser.SetAttrNode("EndDateTime",endDateTime,autoRecordNode);
	}

	parser.SaveToString(retXml);
	return retXml;
}

//��������¼������Ƶ��ѯXML
std::string TranslateUpXMLForCTTB::TranslateTaskRecordStream( const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime )
{
	std::string retXml=GetXmlHeader(task,"TaskRecordQuery");//��׼�ķ���ͷxml

	XmlParser parser;
	parser.Set_xml(retXml);

	pXMLNODE taskRecordReportNode=parser.CreateNodePtr(parser.GetRootNode(),"TaskRecordReport");
	pXMLNODE taskRecordNode=parser.CreateNodePtr(taskRecordReportNode,"TaskRecord");

	parser.SetAttrNode("URL",URL,taskRecordNode);
	parser.SetAttrNode("StartDateTime",startDateTime,taskRecordNode);
	parser.SetAttrNode("EndDateTime",endDateTime,taskRecordNode);

	parser.SaveToString(retXml);
	return retXml;
}

//��������¼���Ŀ�ļ�����XML
std::string TranslateUpXMLForCTTB::TranslateTaskRecordFile( const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime )
{
	std::string retXml=GetXmlHeader(task,"TaskRecordFileQuery");//��׼�ķ���ͷxml

	XmlParser parser;
	parser.Set_xml(retXml);

	pXMLNODE autoRecordFileReportNode=parser.CreateNodePtr(parser.GetRootNode(),"TaskRecordFileReport");
	pXMLNODE autoRecordNode=parser.CreateNodePtr(autoRecordFileReportNode,"TaskRecord");
	if (URL.empty())
	{
		parser.SetAttrNode("URL",string(""),autoRecordNode);
		parser.SetAttrNode("StartDateTime",string(""),autoRecordNode);
		parser.SetAttrNode("EndDateTime",string(""),autoRecordNode);
	}
	else
	{
		parser.SetAttrNode("URL",URL,autoRecordNode);    //����AutoRecord�ڵ��URL����	
		parser.SetAttrNode("StartDateTime",startDateTime,autoRecordNode);
		parser.SetAttrNode("EndDateTime",endDateTime,autoRecordNode);
	}
	parser.SaveToString(retXml);
	return retXml;
}

//���췵��XMLͷ��Ϣ
std::string TranslateUpXMLForCTTB::GetXmlHeader(const XMLTask* task ,std::string retType)
{
	char * cheader= "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><Return/></Msg>";

// 	string  MsgID ;
// 	DBMANAGER::instance()->GetMsgID(MsgID);
// 	int msgid=StrUtil::Str2Int(MsgID);
// 	DBMANAGER::instance()->UpdateMsgID(StrUtil::Int2Str(++msgid));
	string   Desc ="",Comment = "";
	eTaskRetStatus retValue=task->GetRetValue();//�����ִ�н��

	switch(retValue)
	{
	case RUN_SUCCESS:
		Desc="�ɹ�";
		break;
	case RUN_FAILED:
		Desc="ʧ��";
		break;
	case NOFILE_EXIST:
		Desc="�ļ�������";
		break;
	case CHANNELID_UNAVAIABLE:
		Desc="Ƶ���Ƿ�";
		break;
	case DEVICEID_UNAVAIABLE:
		Desc="ͨ���Ƿ�";
		break;
	case NODEVICEIDMATCH:
		Desc="û��ͨ��ƥ�������";
		break;
	case PREFERENTIALTASK_USE:
		Desc="�����ȼ�����ռ��ͨ��";
		break;
	case DATABASEERROR:
		Desc="�������ݿ����";
		break;
	case DEVICEERROR:
		Desc="����Ӳ������";
		break;
	case RECORD_DOWN_OUTOFRAND:
		Desc="¼��ʱ�䳬����";
		break;
	default:
		Desc="ʧ��";
		break;
	}

	std::string msgType;//��Ϣ����
	if (task->GetDVBType()== CTTB)
		msgType="DVBTHUp";
	else
		msgType="UnKnow";

	std::string retXml="";
	XmlParser headParser(cheader);
	pXMLNODE headRootNode = headParser.GetNodeFromPath( "Msg" );//���ڵ�msg

	headParser.SetAttrNode( "Version",task->GetVersion(),headRootNode );//�汾
	string strDateTime = TimeUtil::GetCurDateTime();
	string  MsgID = OSFunction::GetXmlMsgID(strDateTime);
	headParser.SetAttrNode( "MsgID",MsgID,headRootNode );//��Ϣid����
	headParser.SetAttrNode( "Type",msgType,headRootNode );//��Ϣ����
	headParser.SetAttrNode( "DateTime",strDateTime/*TimeUtil::GetCurDateTime()*/,headRootNode );//��ǰʱ��
	headParser.SetAttrNode( "SrcCode",task->GetDstCode(),headRootNode );//������ʶ����ͨ���ӿڻ��
	headParser.SetAttrNode( "DstCode",task->GetSrcCode(),headRootNode );//Ŀ�������ʶ
	headParser.SetAttrNode( "DstURL",task->GetSrcURL(),headRootNode);//Ŀ��URL
	headParser.SetAttrNode( "ReplyID",task->GetMsgID(),headRootNode );//�ظ�����Ϣid

	pXMLNODE retNode = headParser.GetNodeFromPath("Msg/Return");
	headParser.SetAttrNode( "Type",retType,retNode );

	headParser.SetAttrNode( "Value",retValue,retNode );//return�ڵ��value����
	headParser.SetAttrNode( "Desc",Desc,retNode );//return�ڵ��Desc����
	headParser.SetAttrNode( "Comment",Comment,retNode );//return�ڵ��Comment����
	headParser.SaveToString( retXml );

	return retXml;
}
//����EPG��ѯXML
std::string TranslateUpXMLForCTTB::TranslateEPGQuery(const XMLTask* task,std::string URL)
{
	std::string retXML = GetXmlHeader(task,"EPGQuery");
	XmlParser parser;
	parser.Set_xml(retXML);
	pXMLNODE returnInfoNode = parser.CreateNodePtr(parser.GetRootNode(),"ReturnInfo");   //����ReturnInfo�ڵ� 
	pXMLNODE epgQueryNode = parser.CreateNodePtr(returnInfoNode,"EPGQuery");             //����EPGQuery�ڵ�    
	parser.SetAttrNode("ScanTime",TimeUtil::GetCurDateTime(),epgQueryNode);              //����EPG��ѯʱ��
	pXMLNODE epgNode = parser.CreateNodePtr(epgQueryNode,"EPGInfo");                     //����EPGInfo�ڵ�

	std::string strURL;

	if (task->GetRetValue() == RUN_SUCCESS)
	{
		//modified by jidushan 11.03.31 : ����·����Ϊͨ�������ļ���ȡ
		strURL= "http://" + PROPMANAGER::instance()->GetHttpServerIP() + ":" ;
		strURL += PROPMANAGER::instance()->GetHttpServerPort();
		std::string sharepath;
		PROPMANAGER::instance()->GetTableSharePath(task->GetDVBType(), sharepath);
		strURL += sharepath + URL;
	}

	//epg ���⴦��  jidushan 110406
	pXMLNODE returnNode = parser.GetNodeFromPath("Msg/Return");
	parser.SetAttrNode("Redirect", strURL, returnNode);

	parser.SetAttrNode("URL",strURL,epgNode);       //����EPG·����Ϣ                                
	std::string rtnXML;
	parser.SaveToString(rtnXML);
	return rtnXML;
}
std::string TranslateUpXMLForCTTB::TranslateQualityRealtimeQuery(const XMLTask* task,std::string retxml)
{
	std::string rtnHead = GetXmlHeader(task,"QualityRealtimeQuery");//��׼�ķ���ͷxml
	XmlParser parser;
	parser.Set_xml(rtnHead);   
	XmlParser devParser;
	devParser.Set_xml(retxml);
	string freq,type,desc,val;
	pXMLNODE rootNode = parser.GetRootNode();
	pXMLNODE devRootNode= devParser.GetRootNode();

	pXMLNODE IndexNode = parser.CreateNodePtr(rootNode,"ReturnInfo");
	pXMLNODE GetListNode = parser.CreateNodePtr(IndexNode,"QualityRealtimeQuery");
	pXMLNODE ReportNode = devParser.GetNodeFromPath("Msg/QualityQueryReport");
	devParser.GetAttrNode(ReportNode,"Freq",freq);
	parser.SetAttrNode("Index",task->GetDeviceID(),GetListNode);
	parser.SetAttrNode("Freq",freq,GetListNode);
	pXMLNODE paramsNode = devParser.GetNodeFromPath("Msg/QualityQueryReport/QualityParam");

	pXMLNODELIST paramList = devParser.GetNodeList(paramsNode);

	int count=devParser.GetChildCount(paramsNode);
	for(int k=0;k<count;k++)
	{
		pXMLNODE getnode=NULL;
		pXMLNODE paramnode=NULL;

		paramnode = devParser.GetNextNode(paramList);
		if(devParser.GetNodeName(paramnode)=="QualityIndex")
		{
			string add;
			getnode=parser.CreateNodePtr(GetListNode,"GetIndex");
			devParser.GetAttrNode(paramnode,"Type",type);
			devParser.GetAttrNode(paramnode,"Desc",desc);
			devParser.GetAttrNode(paramnode,"Value",val);
			if (type == "9")
			{
				parser.SetAttrNode("Type",type,getnode);
				parser.SetAttrNode("Desc",desc,getnode);

				pXMLNODELIST paramStarList = devParser.GetNodeList(paramnode);

				int startnum=devParser.GetChildCount(paramnode);

				for(int startindex=0;startindex<startnum;++startindex)
				{
					pXMLNODE startgetnode = devParser.GetNextNode(paramStarList);

					string Ivalue,Qvalue;
					pXMLNODE startnode=parser.CreateNodePtr(getnode,"Value");
					devParser.GetAttrNode(startgetnode,"I",Ivalue);
					devParser.GetAttrNode(startgetnode,"Q",Qvalue);

					parser.SetAttrNode("I",Ivalue,startnode);
					parser.SetAttrNode("Q",Qvalue,startnode);
				}
				continue;
			}
			QUALITYCOMPENSATIONMGR::instance()->GetQualityCompensation(task->GetDVBType(),StrUtil::Int2Str(task->GetDeviceID()),type,"","",add);
			float val_int = StrUtil::Str2Float(val)+StrUtil::Str2Float(add);
			parser.SetAttrNode("Type",type,getnode);
			parser.SetAttrNode("Desc",desc,getnode);
			parser.SetAttrNode("Value",StrUtil::Float2Str(val_int),getnode);
		}

	}

	string rtnXML;
	parser.SaveToString(rtnXML);
	return  rtnXML;
}

std::string TranslateUpXMLForCTTB::TranslateAutoAnalysisTimeSet(const XMLTask* task)
{
	std::string retXml=GetXmlHeader(task,"AutoAnalysisTimeSet");//��׼�ķ���ͷxml
	return retXml;
}
//����ָ���ѯXML
std::string TranslateUpXMLForCTTB::TranslateEnvironmentQuery(const XMLTask* task,EnvMapInfo& EnvMap,std::vector<sCheckParam>& paramVec)
{
	std::string retXml = GetXmlHeader(task,"GetIndexESet");
	XmlParser parser;
	parser.Set_xml(retXml);
	pXMLNODE returnInfoNode = parser.CreateNodePtr(parser.GetRootNode(),"ReturnInfo");   //����ReturnInfo�ڵ�
	pXMLNODE getIndexESetNode=parser.CreateNodePtr(returnInfoNode,"GetIndexESet");       //����GetIndexESet�ڵ�
	EnvMapInfo::iterator ptr = EnvMap.begin();
	for (;ptr!=EnvMap.end();ptr++)
	{
		pXMLNODE Node=parser.CreateNodePtr(getIndexESetNode,"GetIndexE");                     
		parser.SetAttrNode("Type",(*ptr).first.Type,Node);        
		parser.SetAttrNode("Desc",(*ptr).first.Desc,Node);        
		parser.SetAttrNode("Value",(*ptr).second,Node);    
		sCheckParam param;
		param.DVBType = task->GetDVBType();
		param.AlarmType = ALARM_ENVIRONMENT;
		param.TypeID = (*ptr).first.Type;
		param.TypeDesc = (*ptr).first.Desc;
		param.STD = "";
		param.Freq = task->GetTaskFreq();
		param.SymbolRate = "";
		param.ChannelID = task->GetChannelID();
		param.TypedValue = (*ptr).second;

		paramVec.push_back(param);

	}
	parser.SaveToString(retXml);
	return retXml;
}
//MHP��ѯXML
std::string TranslateUpXMLForCTTB::TranslateMHPQuery(const XMLTask *task, std::string ftp, std::string userName, std::string passWord)
{
	std::string retXml=GetXmlHeader(task,"MHPQuery");//��׼�ķ���ͷxml

	XmlParser parser;
	parser.Set_xml(retXml);

	pXMLNODE returnInfoNode = parser.CreateNodePtr(parser.GetRootNode(),"ReturnInfo");   //����ReturnInfo�ڵ�
	pXMLNODE MHPQueryNode=parser.CreateNodePtr(returnInfoNode,"MHPQuery");               //����MHPQuery�ڵ�
	pXMLNODE MHPNode=parser.CreateNodePtr(MHPQueryNode,"MHP");                           //����MHP�ڵ�

	parser.SetAttrNode("ScanTime",TimeUtil::GetCurDateTime(),MHPQueryNode);        //����MHPQuery�ڵ��Ftp����
	parser.SetAttrNode("Ftp",ftp,MHPNode);        //����MHP�ڵ��Ftp����
	parser.SetAttrNode("UserName",userName,MHPNode);        //����MHP�ڵ��UserName����
	parser.SetAttrNode("Pass",passWord,MHPNode);        //����MHP�ڵ��Pass����
	parser.SaveToString(retXml);
	return retXml;

}
//ָ�겹������XML
std::string TranslateUpXMLForCTTB::TranslateQualityCompensationSet(const XMLTask *task)
{
	std::string retXml=GetXmlHeader(task,"IndexCompensationSet");//��׼�ķ���ͷxml  
	return retXml;
}
//�ֶ�¼�������ϱ�
std::string TranslateUpXMLForCTTB::TranslateManualRecordReport( const XMLTask *task,const std::string& strStandXML,std::string URL )
{
	const char * cheader= "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><Return/></Msg>";
	XmlParser parser,temParser(cheader);
	parser.Set_xml(strStandXML);
	pXMLNODE orgRootNode=parser.GetRootNode();
	pXMLNODE stdRootNode=temParser.GetRootNode();
	string version,msgid,type,protocol,datetime,srccode,dstcode,srcurl,priority;
	parser.GetAttrNode(orgRootNode,"Version",version);
	parser.GetAttrNode(orgRootNode,"MsgID",msgid);
	parser.GetAttrNode(orgRootNode,"DVBType",type);
	parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
	parser.GetAttrNode(orgRootNode,"DstCode",dstcode);

	if(type=="CTTB")
	{
		type="DVBTHUp";
	}
	else
	{
		return false;
	}
	temParser.SetAttrNode("Version",version,stdRootNode);
	temParser.SetAttrNode("Type",type,stdRootNode);
	temParser.SetAttrNode("DateTime",TimeUtil::GetCurDateTime(),stdRootNode);
	temParser.SetAttrNode("SrcCode",dstcode,stdRootNode);
	temParser.SetAttrNode("DstCode",srccode,stdRootNode);
	temParser.SetAttrNode("DstURL",task->GetSrcURL(),stdRootNode);
	temParser.SetAttrNode("ReplyID",std::string("-1"),stdRootNode);

	pXMLNODE retNode=temParser.GetNodeFromPath("Msg/Return");
	temParser.SetAttrNode("Type",std::string("ManualRecordQuery"),retNode);
	temParser.SetAttrNode("Value",std::string("0"),retNode);
	temParser.SetAttrNode("Desc",std::string("�ɹ�"),retNode);

	std::string Index,Time,Path;
	pXMLNODE recordNode=parser.GetNodeFromPath("Msg/ManualRecordSet/ManualRecord");
	parser.GetAttrNode(recordNode,"DeviceID",Index);
	parser.GetAttrNode(recordNode,"Time",Time);
	parser.GetAttrNode(recordNode,"Path",Path);
	pXMLNODE node=temParser.CreateNodePtr(temParser.CreateNodePtr("Msg","ReturnInfo"),"ManualRecordQuery");
	pXMLNODE retRecordNode=temParser.CreateNodePtr(node,"ManualRecord");
	temParser.SetAttrNode("Index",Index,retRecordNode);
	temParser.SetAttrNode("Time",Time,retRecordNode);
	temParser.SetAttrNode("Path",Path,retRecordNode);
	temParser.SetAttrNode("URL",URL,retRecordNode);

	std::string retXML;
	temParser.SaveToString(retXML);
	return retXML;
}

std::string TranslateUpXMLForCTTB::TranslateManualRecordSet( const XMLTask* task )
{
	std::string retXml=GetXmlHeader(task,"ManualRecordQuery");//��׼�ķ���ͷxml  
	return retXml;
}