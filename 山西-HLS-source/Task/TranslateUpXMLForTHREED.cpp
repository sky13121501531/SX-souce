///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����TranslateUpXMLForTHREED.cpp
// �����ߣ�jiangcheng
// ����ʱ�䣺2009-06-04
// ������������ԭ������ִ�н����XMLת��ΪӦ��ϵͳ�����XML
///////////////////////////////////////////////////////////////////////////////////////////
#pragma warning(disable:4996)
#include "TranslateUpXMLForTHREED.h"
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
#include "../Foundation/BaseAnalyser.h"
#include "../Foundation/OSFunction.h"
#include <fstream>
//����ʵʱ��ƵXML
std::string TranslateUpXMLForTHREED::TranslateStreamRealtimeQuery(const XMLTask* task )
{
	std::string retXml=GetXmlHeader(task,"ChangeProgramQuery");//��׼�ķ���ͷxml

	std::string index, url;
	if (task->GetRetValue()==RUN_SUCCESS)
	{
		index=StrUtil::Int2Str(task->GetDeviceID());
		string protocol;

		//wz_110309(�޸Ľӿ�)
		PROPMANAGER::instance()->GetStreamProtocol(THREED,protocol);
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
	pXMLNODE streamNode=retParser.CreateNodePtr(rootNode,"ReturnInfo");//����ReturnInfo�ڵ�
	pXMLNODE mediaStreamNode=retParser.CreateNodePtr(streamNode,"RealStreamURL");//����ReturnInfo�ڵ���ӽڵ�RealStreamURL�ڵ�

	retParser.SetAttrNode("Index",index,mediaStreamNode);//����MediaStream�ڵ��index����
	retParser.SetAttrNode("URL",url,mediaStreamNode);//����MediaStream�ڵ��URL����
	retParser.SaveToString(retXml);
	return retXml;
}
//�����ֲ���ѯXML
std::string TranslateUpXMLForTHREED::TranslateStreamRoundQuery(const XMLTask* task,std::string deviceID,bool multi)
{
	std::string retXml;//���ص�xml
	std::string retType="StreamRoundInfoQuery";//Ӳ������xml��return�ڵ�����ͺ�ֵ

	std::string headerxml=GetXmlHeader(task,retType);//��׼�ķ���ͷxml

	std::string index, url;
	if(task->GetRetValue()==5)
	{	
		XmlParser headParser;
		headParser.Set_xml(headerxml);
		pXMLNODE retNode = headParser.GetNodeFromPath("Msg/Return");
		headParser.SetAttrNode( "Type",string("StreamRoundInfoQuery"),retNode );
		headParser.SaveToString(retXml);
		return retXml;
	}
	if (task->GetRetValue()==0)
	{
		index=StrUtil::Int2Str(task->GetDeviceID());
		string protocol;

		//wz_110309(�޸Ľӿ�)
		PROPMANAGER::instance()->GetStreamProtocol(THREED,protocol);
		if (protocol=="rtsp")
		{
			url="rtsp://" + PROPMANAGER::instance()->GetRtspVideoIp() + ":" + \
				PROPMANAGER::instance()->GetRtspVideoPort() +"/roundstream" ;//��Ƶ���ŵ�ַ
		}
		else
		{
			//wz_0217
			int virtualid = 0;
			PROPMANAGER::instance()->GetVirDeviceId(THREED, virtualid);
			url=protocol+"://" + PROPMANAGER::instance()->GetHttpVideoIp() + ":" + \
				PROPMANAGER::instance()->GetHttpVideoPort() +"/"+ StrUtil::Int2Str(virtualid);	//��Ƶ���ŵ�ַ
			//wz_0217
		}
	}
	XmlParser retParser;
	retParser.Set_xml(headerxml);

	pXMLNODE rootNode=retParser.GetRootNode();
	pXMLNODE streamNode=retParser.CreateNodePtr(rootNode,"ReturnInfo");//����ReturnInfo�ڵ�
	pXMLNODE mediaStreamNode=retParser.CreateNodePtr(streamNode,"StreamRoundInfoQuery");//����ReturnInfo�ڵ���ӽڵ�StreamRoundInfoQuery�ڵ�
	pXMLNODE roundStreamNode=retParser.CreateNodePtr(mediaStreamNode,"RoundStream");

	retParser.SetAttrNode("Index",index,roundStreamNode);//����MediaStream�ڵ��index����
	retParser.SetAttrNode("URL",url,roundStreamNode);//����MediaStream�ڵ��URL����
	retParser.SaveToString(retXml);

	return retXml;
}
//����Ƶ��ɨ��XML
std::string TranslateUpXMLForTHREED::TranslateChannelScan(const XMLTask* task,const std::string& strRtnXML)
{
	std::string retXml=GetXmlHeader(task,"ChannelScanQuery");//��׼�ķ���ͷxml

	XmlParser parser;
	parser.Set_xml(retXml);
	pXMLNODE returnInfoNode = parser.CreateNodePtr(parser.GetRootNode(),"ReturnInfo");
	pXMLNODE channelScanQueryNode=parser.CreateNodePtr(returnInfoNode,"ChannelScanQuery");//Ϊ���ص�xml����ChannelScanReport�ڵ�
	std::string scantime = TimeUtil::GetCurDateTime();
	parser.SetAttrNode("ScanTime",scantime,channelScanQueryNode);
	//��ʼ������
	string   STD, Freq, QAM, HeaderMode, CarrierNumber, EncodeEfficiency, InterwovenMode,SymbolRate;
	string   DoublePilot, PnPhase, OrgNetID, TsID;
	string   Program, ProgramID,ServiceID, VideoPID, AudioPID, Encryption, Encrypt,ISHDTV; 

	XmlParser retParser;
	retParser.Set_xml(strRtnXML);

	pXMLNODE rtnTypeNode=retParser.GetNodeFromPath("Msg/Return");
	std::string value;
	retParser.GetAttrNode(rtnTypeNode,"Value",value);//����Ӳ������xml�е�valueֵ���Դ��ж�Ӳ��ִ�гɹ����
	if (value=="0")
	{
		pXMLNODE rtnChannelScanReportNode=retParser.GetNodeFromPath("Msg/ChannelScanReport");//Ӳ������xml��ChannelScanReport�ڵ�
		pXMLNODELIST nodeList=retParser.GetNodeList(rtnChannelScanReportNode);//Ӳ������xml��ChannelScanReport�ڵ���ӽڵ��б�
		for (int j=0;j!=nodeList->Size();++j)
		{
			pXMLNODE channelScanNode=parser.CreateNodePtr(channelScanQueryNode,"ChannelScan");//Ϊ���ص�xml����ChannelScan�ڵ�
			pXMLNODE childNode=retParser.GetNextNode(nodeList);//Ӳ������xml��ChannelScan�ڵ���ӽڵ��б�

			retParser.GetAttrNode(childNode,"Freq",Freq);//���Freq����ֵ
			retParser.GetAttrNode(childNode,"OrgNetID",OrgNetID);//���OrgNetID����ֵ
			retParser.GetAttrNode(childNode,"TsID",TsID);//���TsID����ֵ
			retParser.GetAttrNode(childNode,"SymbolRate",SymbolRate);
			retParser.GetAttrNode(childNode,"QAM",QAM);

			parser.SetAttrNode("Freq",Freq,channelScanNode);//���÷���xml��ChannelScan�ڵ��Freq����ֵ
			parser.SetAttrNode("OrgNetID",OrgNetID,channelScanNode);//���÷���xml��ChannelScan�ڵ��OrgNetID����ֵ
			parser.SetAttrNode("TsID",TsID,channelScanNode);//���÷���xml��ChannelScan�ڵ��TsID����ֵ

			//add by gxd 2010-10-15
			parser.SetAttrNode("QAM",QAM,channelScanNode);
			parser.SetAttrNode("SymbolRate",SymbolRate,channelScanNode);

			pXMLNODELIST childNodeList=retParser.GetNodeList(childNode);//Ӳ������xml��ChannelScan�ڵ���ӽڵ��б�
			for (int k=0;k!=childNodeList->Size();++k)
			{
				pXMLNODE channelNode=parser.CreateNodePtr(channelScanNode,"Channel");//Ϊ���ص�xml����Channel�ڵ�

				pXMLNODE subChildNode=retParser.GetNextNode(childNodeList);//Ӳ������xml��ChannelScan�ڵ���ӽڵ�channel
				retParser.GetAttrNode(subChildNode,"Program",Program);//���Program����ֵ
				retParser.GetAttrNode(subChildNode,"ProgramID",ProgramID); //���ProgramID����ֵ
				retParser.GetAttrNode(subChildNode,"ServiceID",ServiceID);//���ServiceID����ֵ
				retParser.GetAttrNode(subChildNode,"VideoPID",VideoPID);//���VideoPID����ֵ
				retParser.GetAttrNode(subChildNode,"AudioPID",AudioPID);//���AudioPID����ֵ
				retParser.GetAttrNode(subChildNode,"Encrypt", Encrypt);
				retParser.GetAttrNode(subChildNode,"HDTV",ISHDTV);

				parser.SetAttrNode("Program",Program,channelNode);//���÷���xml��Channel�ڵ��Program����ֵ
				parser.SetAttrNode("ProgramID",ProgramID,channelNode); //���÷���xml��Channel�ڵ��ProgramID����ֵ
				parser.SetAttrNode("ServiceID",ServiceID,channelNode);//���÷���xml��Channel�ڵ��ServiceID����ֵ
				parser.SetAttrNode("VideoPID",VideoPID,channelNode);//���÷���xml��Channel�ڵ��VideoPID����ֵ
				parser.SetAttrNode("AudioPID",AudioPID,channelNode);//���÷���xml��Channel�ڵ��AudioPID 	
				parser.SetAttrNode("Encrypt", Encrypt,channelNode);
				parser.SetAttrNode("HDTV",ISHDTV,channelNode);

				pXMLNODELIST StreamNodeList=retParser.GetNodeList(subChildNode);
				for(int n=0;n!=StreamNodeList->Size();n++)
				{
					string StreamType,PID;
					pXMLNODE StreamNode=parser.CreateNodePtr(channelNode,"elementryPID");

					pXMLNODE subStreamNode=retParser.GetNextNode(StreamNodeList);
					retParser.GetAttrNode(subStreamNode,"StreamType",StreamType);
					retParser.GetAttrNode(subStreamNode,"PID",PID);

					parser.SetAttrNode("StreamType",StreamType,StreamNode);
					parser.SetAttrNode("PID",PID,StreamNode);
				}
			}
		}
	}
	parser.SaveToString(retXml);
	return retXml;
}
//����Ƶ������xml
std::string TranslateUpXMLForTHREED::TranslateChannelSet( const XMLTask* task )
{
	std::string retXml=GetXmlHeader(task,"ChannelSetReport");//��׼�ķ���ͷxml
	return retXml;
}

//������Ƶ�û���ѯXML
std::string TranslateUpXMLForTHREED::TranslateClientInfoQuery(const XMLTask* task,std::vector<sVedioUserInfo>& vecClient)
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
std::string TranslateUpXMLForTHREED::TranslateTableQuery(const XMLTask *task, std::string strFileName)
{
	std::string rtnxml = GetXmlHeader(task,"table");

	XmlParser parser;
	parser.Set_xml(rtnxml);

	pXMLNODE queryReport = parser.CreateNodePtr( parser.GetRootNode(),"ReturnInfo" );

	pXMLNODE RetNode=parser.GetNodeFromPath("Msg/Return");

	std::string strURL;

	if (task->GetRetValue() == RUN_SUCCESS)
	{
		//modified by jidushan 11.03.31 : ����·����Ϊͨ�������ļ���ȡ
		strURL = "http://" + PROPMANAGER::instance()->GetHttpServerIP() + ":";
		strURL += PROPMANAGER::instance()->GetHttpServerPort();
		std::string sharepath;
		PROPMANAGER::instance()->GetTableSharePath(task->GetDVBType(), sharepath);
		strURL += sharepath + strFileName;
	}

	parser.SetAttrNode( "Redirect",strURL,RetNode ); //����URL����

	parser.SaveToString(rtnxml); //���浽�ַ���
	return rtnxml;

}
//����¼������XML
std::string TranslateUpXMLForTHREED::TranslateRecord(const XMLTask* task,const std::vector<XMLTask*>& resultVec)
{
	std::string retXML;

	if(task->GetTaskID() == "0")
	{
		retXML=GetXmlHeader(task,"SetAutoRecordChannel");
	}
	else
	{
		retXML=GetXmlHeader(task,"ProvisionalRecordTaskSet");
	}
	return retXML;
}
//���؂���¼������XML
std::string TranslateUpXMLForTHREED::TranslateRoundRecord(const XMLTask* task,const std::vector<XMLTask*>& resultVec)
{
	std::string retXML = GetXmlHeader(task,"RoundRecordSet");
	return retXML;
}

//����ָ����������xml
std::string TranslateUpXMLForTHREED::TranslateQualitySet( const XMLTask* task )
{
	std::string retXml=GetXmlHeader(task,"QualityReportTaskSet");//��׼�ķ���ͷxml
	return retXml;
}
//����ָ���ѯXML
std::string TranslateUpXMLForTHREED::TranslateQualityQuery( const XMLTask* task,std::map< string,vector<eQualityInfo> > FreqQualityInfo)
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
		string strURL;
		strURL= "http://" + PROPMANAGER::instance()->GetHttpServerIP() + ":" ;
		strURL += PROPMANAGER::instance()->GetHttpServerPort() + PROPMANAGER::instance()->GetTempSharePath()+"Quality_";
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
				CHANNELINFOMGR::instance()->GetChannelCodeByFreq(THREED,vecitr->freq,channelcode);
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

//ʵʱָ���ѯ
std::string TranslateUpXMLForTHREED::TranslateQualityRealtimeQuery(const XMLTask* task,std::string retxml)
{
	string rtnHead =  GetXmlHeader(task,"GetIndexSet");

	//����������Ķ���
	XmlParser parser;
	parser.Set_xml(rtnHead);
	XmlParser devParser;
	devParser.Set_xml(retxml);
	string freq,type,desc,val;
	pXMLNODE rootNode = parser.GetRootNode();
	pXMLNODE devRootNode= devParser.GetRootNode();

	pXMLNODE IndexNode = parser.CreateNodePtr(rootNode,"ReturnInfo");
	pXMLNODE GetListNode = parser.CreateNodePtr(IndexNode,"GetIndexSet");
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
			float level;
			getnode=parser.CreateNodePtr(GetListNode,"GetIndex");
			devParser.GetAttrNode(paramnode,"Type",type);
			devParser.GetAttrNode(paramnode,"Desc",desc);
			devParser.GetAttrNode(paramnode,"Value",val);
			/*if (type=="1")
			{
				DEVICEFREQLEVEL::instance()->InitDevice();
				std::string tmpfreq=freq;
				if (tmpfreq.length()>3)
				{
					tmpfreq=tmpfreq.substr(0,3);
				}
				DEVICEFREQLEVEL::instance()->GetFreqLevel(StrUtil::Str2Float(tmpfreq),level);
				DEVICEFREQLEVEL::instance()->CloseDevice();
				val=StrUtil::Float2Str(level);
			}*/
			

			if (type == "2" && val.length() >=5)
			{
				val = "1.00";
			}
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
//���ر�����������XML
std::string TranslateUpXMLForTHREED::TranslateAlarmParamSet( const XMLTask* task )
{
	std::string strTaskType = (const_cast<XMLTask*>(task))->GetObjectName();
	std::string retXml=GetXmlHeader(task, strTaskType.c_str() );//��׼�ķ���ͷxml
	return retXml;
}

std::string TranslateUpXMLForTHREED::TranslateAlarmReport( const XMLTask* task )
{
	std::string retXml=GetXmlHeader(task,"AlarmTypeSet");//��׼�ķ���ͷxml
	return retXml;
}

//wz_110107
std::string TranslateUpXMLForTHREED::TranslateCleanAlarmStateTask(const XMLTask* task)
{
	std::string retXml=GetXmlHeader(task,"CleanAlarmState");//��׼�ķ���ͷxml
	return retXml;
}


//wz_110107
std::string TranslateUpXMLForTHREED::TranslateMatrixQuery(const XMLTask* task)
{
	std::string retXml=GetXmlHeader(task,"MatrixQuery");//��׼�ķ���ͷxml
	return retXml;
}

//wz_110107
std::string TranslateUpXMLForTHREED::TranslateChangeQAMQuery(const XMLTask* task)
{
	std::string retXml=GetXmlHeader(task,"ChangeQAMQuery");//��׼�ķ���ͷxml
	return retXml;
}

//wz_110107
std::string TranslateUpXMLForTHREED::TranslateLoopAlarmInfo(const XMLTask* task)
{
	std::string retXml=GetXmlHeader(task,"LoopAlaInf");//��׼�ķ���ͷxml
	return retXml;
}

//wz_110107
std::string TranslateUpXMLForTHREED::TranslateRecordParamSet(const XMLTask* task)
{
	std::string retXml=GetXmlHeader(task,"RecordParamSet");//��׼�ķ���ͷxml
	return retXml;
}

//����ͨ��״̬��ѯXML
std::string TranslateUpXMLForTHREED::TranslateDeviceStatusQuery(const XMLTask* task,std::vector<sDeviceStatus> & VecDeviceStatus)
{
	std::string retXml=GetXmlHeader(task,"GetNvrStatus");
	XmlParser parser;
	parser.Set_xml(retXml);
	pXMLNODE rootNode=parser.GetRootNode();
	pXMLNODE deviceNode=parser.CreateNodePtr(rootNode,"ReturnInfo");

	pXMLNODE statusnode=parser.GetNodeFromPath("Msg/ReturnInfo");
	for (size_t i=0;i!=VecDeviceStatus.size();++i)
	{
		pXMLNODE nodeinfo=parser.CreateNodePtr(statusnode,"GetNvrIndexStatus");//����GetNvrIndexStatus�ڵ�
		//����GetNvrIndexStatus�ڵ������
		std::string Freq,ServiceID;
		parser.SetAttrNode("Index",VecDeviceStatus[i].DevicID,nodeinfo);
		parser.SetAttrNode("Status",VecDeviceStatus[i].Statuc,nodeinfo);
		parser.SetAttrNode("Freq",Freq,nodeinfo);
		parser.SetAttrNode("ServiceID",ServiceID,nodeinfo);
		parser.SetAttrNode("Desc",VecDeviceStatus[i].Desc,nodeinfo);

	}
	parser.SaveToString(retXml);
	return retXml;
}

//��������ͼ����XML
std::string TranslateUpXMLForTHREED::TranslateRunPlanSet(const XMLTask* task)
{
	std::string retXml=GetXmlHeader(task,"AlarmTimeSet");//��׼�ķ���ͷxml
	return retXml;
}

//�����Զ�¼������Ƶ��ѯXML
std::string TranslateUpXMLForTHREED::TranslateAutoRecordStream( const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime )
{
	std::string retXml=GetXmlHeader(task,"NVRVideoHistoryInquiry");//��׼�ķ���ͷxml

	XmlParser parser;
	parser.Set_xml(retXml);

	pXMLNODE returnInfoNode = parser.CreateNodePtr(parser.GetRootNode(),"ReturnInfo");           //����ReturnInfo�ڵ�
	pXMLNODE autoRecordReportNode=parser.CreateNodePtr(returnInfoNode,"NVRVideoHistoryInquiry"); //����NVRVideoHistoryInquiry�ڵ�
	pXMLNODE autoRecordNode=parser.CreateNodePtr(autoRecordReportNode,"NVRVideoHistory");        //����NVRVideoHistory�ڵ�

	parser.SetAttrNode("URL",URL,autoRecordNode);  //����NVRVideoHistory�ڵ��URL����

	parser.SaveToString(retXml);
	return retXml;
}

//�����Զ�¼���Ŀ�ļ�����XML
std::string TranslateUpXMLForTHREED::TranslateAutoRecordFile( const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime)
{
	std::string retXml=GetXmlHeader(task,"NVRVideoHistoryDownInquiry");//��׼�ķ���ͷxml

	XmlParser parser;
	parser.Set_xml(retXml);

	pXMLNODE returnInfoNode = parser.CreateNodePtr(parser.GetRootNode(),"ReturnInfo");                     //����ReturnInfo�ڵ�
	pXMLNODE autoRecordFileReportNode=parser.CreateNodePtr(returnInfoNode,"NVRVideoHistoryDownInquiry");   //����NVRVideoHistoryDownInquiry�ڵ�

	pXMLNODE autoRecordNode=parser.CreateNodePtr(autoRecordFileReportNode,"NVRVideoHistoryDown");      //����NVRVideoHistoryDown�ڵ�

	parser.SetAttrNode("URL",URL,autoRecordNode);    //����NVRVideoHistoryDown�ڵ��URL����

	parser.SaveToString(retXml);
	return retXml;
}

//��������¼������Ƶ��ѯXML
std::string TranslateUpXMLForTHREED::TranslateTaskRecordStream( const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime )
{
	std::string retXml=GetXmlHeader(task,"NVRTaskRecordInquiry");//��׼�ķ���ͷxml

	XmlParser parser;
	parser.Set_xml(retXml);
	pXMLNODE rootnode=parser.GetRootNode();
	pXMLNODE rtnnode=parser.GetNodeFromPath("Msg/Return");
	if(URL.length()<1)
	{
		parser.SetAttrNode("Value",string("1"),rtnnode);
		parser.SetAttrNode("Desc",string("û�в鵽�������"),rtnnode);
		pXMLNODE errnode=parser.CreateNodePtr(rootnode,"ErrReport");
		pXMLNODE NVRTaskRecordInquiryNode=parser.CreateNodePtr(errnode,"NVRTaskRecordInquiry");
		pXMLNODE NVRTaskRecord =parser.CreateNodePtr(NVRTaskRecordInquiryNode,"NVRTaskRecord");
		parser.SetAttrNode("TaskID",task->GetTaskID(),NVRTaskRecord);
		parser.SetAttrNode("StartDateTime",startDateTime,NVRTaskRecord);
		parser.SetAttrNode("EndDateTime",endDateTime,NVRTaskRecord);
		parser.SetAttrNode("Comment",string("�ڲ�����"),NVRTaskRecord);

	}
	else
	{
		pXMLNODE returnInfoNode = parser.CreateNodePtr(rootnode,"ReturnInfo");  

		//����ReturnInfo�ڵ�
		pXMLNODE taskRecordReportNode=parser.CreateNodePtr(returnInfoNode,"NVRTaskRecordInquiry");  //����NVRTaskRecordInquiry�ڵ�
		pXMLNODE taskRecordNode=parser.CreateNodePtr(taskRecordReportNode,"NVRTaskRecord");         //����NVRTaskRecord�ڵ�

		parser.SetAttrNode("TaskID",task->GetTaskID(),taskRecordReportNode);    //����NVRTaskRecordInquiry�ڵ��TaskID����
		parser.SetAttrNode("Index",string("1"),taskRecordNode);                 //����NVRTaskRecord�ڵ��Index����
		parser.SetAttrNode("URL",URL,taskRecordNode);                          //����NVRTaskRecord�ڵ��URL����
		parser.SetAttrNode("StartDateTime",startDateTime,taskRecordNode);      //����NVRTaskRecord�ڵ��StartDateTime����
		parser.SetAttrNode("EndDateTime",endDateTime,taskRecordNode);          //����NVRTaskRecord�ڵ��EndDateTime����

	}

	parser.SaveToString(retXml);
	return retXml;
}

//��������¼���Ŀ�ļ�����XML
std::string TranslateUpXMLForTHREED::TranslateTaskRecordFile( const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime)
{
	std::string retXml=GetXmlHeader(task,"NVRTaskRecordDownInquiry");//��׼�ķ���ͷxml

	XmlParser parser;
	parser.Set_xml(retXml);

	pXMLNODE returnInfoNode = parser.CreateNodePtr(parser.GetRootNode(),"ReturnInfo");                   //����ReturnInfo�ڵ�
	pXMLNODE taskRecordFileReportNode=parser.CreateNodePtr(returnInfoNode,"NVRTaskRecordDownInquiry");   //����NVRTaskRecordDownInquiry�ڵ�

	pXMLNODE taskRecordNode=parser.CreateNodePtr(taskRecordFileReportNode,"NVRTaskRecordDown");   //����NVRTaskRecordDown�ڵ�
		
	parser.SetAttrNode("URL",URL,taskRecordNode);    //����NVRTaskRecordDown�ڵ��URL����

	parser.SaveToString(retXml);
	return retXml;
}
//MHP��ѯXML
std::string TranslateUpXMLForTHREED::TranslateMHPQuery(const XMLTask *task, std::string ftp, std::string userName, std::string passWord)
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
std::string TranslateUpXMLForTHREED::TranslateQualityCompensationSet(const XMLTask *task)
{
	std::string retXml=GetXmlHeader(task,"IndexCompensationSet");//��׼�ķ���ͷxml  
	return retXml;
}
//����ҵ�����ʱ������XML
std::string TranslateUpXMLForTHREED::TranslateAutoAnalysisTimeSet(const XMLTask* task)
{
	std::string retXml=GetXmlHeader(task,"AutoAnalysisTimeSet");//��׼�ķ���ͷxml
	return retXml;
}
//����ָ���ѯXML
std::string TranslateUpXMLForTHREED::TranslateEnvironmentQuery(const XMLTask* task,EnvMapInfo& EnvMap,std::vector<sCheckParam>& paramVec)
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
//�ֶ�¼������XML
std::string TranslateUpXMLForTHREED::TranslateManualRecordSet( const XMLTask* task )
{
	std::string retXml=GetXmlHeader(task,"ManualRecordQuery");//��׼�ķ���ͷxml
	return retXml;
}
//���췵��XMLͷ��Ϣ
std::string TranslateUpXMLForTHREED::GetXmlHeader(const XMLTask* task ,std::string retType)
{
	char * cheader= "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><Return/></Msg>";

// 	string  MsgID ;
// 	DBMANAGER::instance()->GetMsgID(MsgID);
// 	int msgid=StrUtil::Str2Int(MsgID);
// 	DBMANAGER::instance()->UpdateMsgID(StrUtil::Int2Str(++msgid));
	string Desc;
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
	default:
		Desc="ʧ��";
		break;
	}

	std::string msgType;//��Ϣ����
	if (task->GetDVBType()== THREED)
		msgType="3DUp";
	else
		return "";

	std::string retXml="";
	XmlParser headParser(cheader);
	pXMLNODE headRootNode = headParser.GetNodeFromPath( "Msg" );//���ڵ�msg

	headParser.SetAttrNode( "Version",task->GetVersion(),headRootNode );//�汾
	//if (retType=="ProvisionalRecordTaskSet"||retType=="NVRVideoHistoryInquiry"||retType=="NVRTaskRecordInquiry")
	//{
	//	headParser.SetAttrNode( "MsgID",task->GetMsgID(),headRootNode );//��Ϣid����
	//}
	//else
	//	headParser.SetAttrNode( "MsgID",MsgID,headRootNode );//��Ϣid����
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
	headParser.SaveToString( retXml );

	return retXml;
}
//�ֶ�¼�������ϱ�
std::string TranslateUpXMLForTHREED::TranslateManualRecordReport( const XMLTask *task,const std::string& strStandXML,std::string URL )
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

	if(type=="THREED")
	{
		type="3DUp";
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
	temParser.SetAttrNode("ReplyID",msgid,stdRootNode);

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
std::string TranslateUpXMLForTHREED::TranslateEPGQuery(const XMLTask* task,std::string URL)
{
	std::string retXML = GetXmlHeader(task,"EPGQuery");
	XmlParser parser;
	parser.Set_xml(retXML);
	pXMLNODE retNode = parser.GetNodeFromPath("Msg/Return");
	pXMLNODE returnInfoNode = parser.CreateNodePtr(parser.GetRootNode(),"ReturnInfo");   //����ReturnInfo�ڵ� 


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

	parser.SetAttrNode("Redirect",strURL,retNode);       //����EPG·����Ϣ                                
	std::string rtnXML;
	parser.SaveToString(rtnXML);
	return rtnXML;
}

std::string TranslateUpXMLForTHREED::TranslateFreqAlarmQuery( const XMLTask* task,std::vector<sCheckParam> vecAlarmResult )
{
	std::string retXML = GetXmlHeader(task,"AlarmSearchFSet");
	XmlParser parser;
	parser.Set_xml(retXML);
	pXMLNODE  returnInfoNode = parser.CreateNodePtr("Msg","ReturnInfo");
	pXMLNODE  alarmSetNode = parser.CreateNodePtr(returnInfoNode,"AlarmSearchFSet");
	std::string freq,index;
	std::string rtnXML;
	if (vecAlarmResult.empty())
	{
		parser.SetAttrNode(string("Freq"),string(""),alarmSetNode);
		parser.SetAttrNode(string("Index"),string("0"),alarmSetNode);
		parser.SaveToString(rtnXML);
		return rtnXML;
	}
	parser.SetAttrNode(string("Freq"),vecAlarmResult[0].Freq,alarmSetNode);
	parser.SetAttrNode(string("Index"),vecAlarmResult[0].DeviceID,alarmSetNode);

	std::vector<sCheckParam>::iterator ptr = vecAlarmResult.begin();
	for (;ptr!=vecAlarmResult.end();ptr++)
	{
		pXMLNODE alarmSearchNode = parser.CreateNodePtr(alarmSetNode,"AlarmSearchF");
		parser.SetAttrNode(string("Type"),(*ptr).TypeID,alarmSearchNode);
		parser.SetAttrNode(string("Desc"),(*ptr).TypeDesc,alarmSearchNode);
		parser.SetAttrNode(string("Value"),(*ptr).mode,alarmSearchNode);
		parser.SetAttrNode(string("Time"),(*ptr).StrCheckTime,alarmSearchNode);
	}

	parser.SaveToString(rtnXML);
	return rtnXML;
}

std::string TranslateUpXMLForTHREED::TranslateProgramAlarmQuery( const XMLTask* task,std::vector<sCheckParam> vecAlarmResult )
{
	std::string retXML = GetXmlHeader(task,"AlarmSearchPSet");
	XmlParser parser;
	parser.Set_xml(retXML);
	pXMLNODE  returnInfoNode = parser.CreateNodePtr("Msg","ReturnInfo");
	pXMLNODE  alarmSetNode = parser.CreateNodePtr(returnInfoNode,"AlarmSearchPSet");
	std::string freq,index;
	std::string rtnXML;
	if (vecAlarmResult.empty())
	{
		parser.SetAttrNode(string("Freq"),string(""),alarmSetNode);
		parser.SetAttrNode(string("Index"),string("0"),alarmSetNode);
		parser.SetAttrNode(string("ServiceID"),string(""),alarmSetNode);
		parser.SetAttrNode(string("VideoPID"),string(""),alarmSetNode);
		parser.SetAttrNode(string("AudioPID"),string(""),alarmSetNode);
		parser.SaveToString(rtnXML);
		return rtnXML;
	}
	parser.SetAttrNode(string("Freq"),vecAlarmResult[0].Freq,alarmSetNode);
	parser.SetAttrNode(string("Index"),vecAlarmResult[0].DeviceID,alarmSetNode);
	parser.SetAttrNode(string("ServiceID"),vecAlarmResult[0].ServiceID,alarmSetNode);
	parser.SetAttrNode(string("VideoPID"),vecAlarmResult[0].VideoID,alarmSetNode);
	parser.SetAttrNode(string("AudioPID"),vecAlarmResult[0].AudioID,alarmSetNode);

	std::vector<sCheckParam>::iterator ptr = vecAlarmResult.begin();
	for (;ptr!=vecAlarmResult.end();ptr++)
	{
		pXMLNODE alarmSearchNode = parser.CreateNodePtr(alarmSetNode,"AlarmSearchP");
		parser.SetAttrNode(string("Type"),(*ptr).TypeID,alarmSearchNode);
		parser.SetAttrNode(string("Desc"),(*ptr).TypeDesc,alarmSearchNode);
		parser.SetAttrNode(string("Value"),(*ptr).mode,alarmSearchNode);
		parser.SetAttrNode(string("Time"),(*ptr).StrCheckTime,alarmSearchNode);
	}

	parser.SaveToString(rtnXML);
	return rtnXML;
	return "";
}

std::string TranslateUpXMLForTHREED::TranslateEnvAlarmQuery( const XMLTask* task,std::vector<sCheckParam> vecAlarmResult )
{
	std::string retXML = GetXmlHeader(task,"AlarmSearchSet");
	XmlParser parser;
	parser.Set_xml(retXML);
	pXMLNODE retNode = parser.GetNodeFromPath("Msg/Return");
	pXMLNODE returnInfoNode = parser.CreateNodePtr(parser.GetRootNode(),"ReturnInfo");   //����ReturnInfo�ڵ� 
	pXMLNODE alarmNode = parser.CreateNodePtr(returnInfoNode,"AlarmSearchESet");

	for(size_t i=0;i<vecAlarmResult.size();i++)
	{
		pXMLNODE childNode = parser.CreateNodePtr(alarmNode,"AlarmSearchE");
		parser.SetAttrNode("Type",vecAlarmResult[i].TypeID,childNode);
		parser.SetAttrNode("Desc",vecAlarmResult[i].TypeDesc,childNode);
		parser.SetAttrNode("Value",vecAlarmResult[i].mode,childNode);
		parser.SetAttrNode("Time",vecAlarmResult[i].StrCheckTime,childNode);
	}                             
	std::string rtnXML;
	parser.SaveToString(rtnXML);
	return rtnXML;
}

std::string TranslateUpXMLForTHREED::TranslateOSDSet( const XMLTask* task )
{
	std::string retXml=GetXmlHeader(task,"OSDSet");//��׼�ķ���ͷxml
	return retXml;
}

std::string TranslateUpXMLForTHREED::TranslateStreamRealtimeRate(const XMLTask* task)
{
	std::string retXML = GetXmlHeader(task,"NVRSteamRateSet");
	return retXML;
}
std::string TranslateUpXMLForTHREED::TranslateRecordCapabilityQuery(const XMLTask* task)
{
	std::string retXML = GetXmlHeader(task,"RecordCapabilityQuery");
	XmlParser parser;
	parser.Set_xml(retXML); 
	list<int> DeviceList;
	int num=0;
	//PROPMANAGER::instance()->GetAllDeviceList(DeviceList);	
	int devnum = PROPMANAGER::instance()->GetCoderDevNum();
	for(int devi = 0;devi < devnum;devi++)
	{
		bool ret = PROPMANAGER::instance()->IsDeviceAvaiable(devi,"AutoRecord",THREED);
		if(ret)
			num++;

	}
	pXMLNODE returnInfoNode = parser.CreateNodePtr(parser.GetRootNode(),"ReturnInfo");   //����ReturnInfo�ڵ� 
	pXMLNODE QueryNode = parser.CreateNodePtr(returnInfoNode,"RecordCapabilityQuery");
	parser.SetAttrNode("Index",string("0"),QueryNode);
	parser.SetAttrNode("OptiCount",StrUtil::Int2Str(num),QueryNode);
	parser.SetAttrNode("PessCount",StrUtil::Int2Str(num),QueryNode);

	std::string rtnXML;
	parser.SaveToString(rtnXML);
	return rtnXML;
}

std::string TranslateUpXMLForTHREED::TranslateHDDefAudioParamSet(const XMLTask* task)
{
	std::string retXML = GetXmlHeader(task,"HDDefAudioParamSet");
	return retXML;
}
std::string TranslateUpXMLForTHREED::TranslateAudioParamSet(const XMLTask* task)
{
	std::string retXML = GetXmlHeader(task,"AudioParamSet");
	return retXML;
}


std::string TranslateUpXMLForTHREED::TranslateAlarmQuery( const XMLTask* task,std::vector<sCheckParam> vecAlarmResult )
{
	std::string retxml;
	eAlarmType type=ALARM_NONE;
	if(!vecAlarmResult.empty())
	{
		type=vecAlarmResult[0].AlarmType;
	}
	switch(type)
	{
	case ALARM_ENVIRONMENT:
		retxml=TranslateEnvAlarmQuery(task,vecAlarmResult);
		break;
	case ALARM_PROGRAM:
		retxml=TranslateProgramAlarmQuery(task,vecAlarmResult);
		break;
	case ALARM_FREQ:
	case ALARM_TR101_290:
		retxml=TranslateFreqAlarmQuery(task,vecAlarmResult);
		break;
	}
	return retxml;
}


std::string TranslateUpXMLForTHREED::TranslateStopClient(const XMLTask* task)
{
	std::string retXml=GetXmlHeader(task,"VideoClientStop");//��׼�ķ���ͷxml
	return retXml;
}