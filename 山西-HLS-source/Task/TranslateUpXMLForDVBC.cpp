///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����TranslateUpXMLForDVBC.cpp
// �����ߣ�jiangcheng
// ����ʱ�䣺2009-06-04
// ������������ԭ������ִ�н����XMLת��ΪӦ��ϵͳ�����XML
///////////////////////////////////////////////////////////////////////////////////////////
#pragma warning(disable:4996)
#include "TranslateUpXMLForDVBC.h"
#include "../Foundation/XmlParser.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/AppLog.h"
#include "../Foundation/PropManager.h"
#include "../Task//StreamRealtimeQueryTask.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TimeUtil.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include "../BusinessProcess/BusinessLayoutMgr.h"
#include "../Foundation/PropManager.h"
#include "../DBAccess/DBManager.h"
#include "../BusinessProcess/QualityCompensationMgr.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/Config.h"
#include "../DeviceAccess/HTTPDeviceAccess.h"
#include "../FileSysAccess/DiskOperation.h"
#include "../DeviceMgr/OS_Environment.h"
#include "XmlValidator.h"
#include <fstream>
#include "../BusinessProcess/RecordUploadProcess.h"

float g_fMerVal[100][10];
int g_iMerCurIndex[100];
string g_strBer;
std::vector<sEncryptInfo> g_sEncryptInfo;
extern standardBoardCheckInfo g_standardBoardCheckInfo[ChassisMaxNum][SlotMaxNum];
#ifdef ZONG_JU_ZHAO_BIAO
#include "../BusinessProcess/ChanEncryptCheckMgr.h"
#include "../Foundation/ParamSetManager.h"
#endif
//����ʵʱ��ƵXML
std::string TranslateUpXMLForDVBC::TranslateStreamRealtimeQuery(const XMLTask* task )
{
	std::string retXml=GetXmlHeader(task,"RealStreamQuery");//��׼�ķ���ͷxml

	std::string index, url;
	if (task->GetRetValue()==RUN_SUCCESS)
	{
		index=StrUtil::Int2Str(task->GetDeviceID());
		string protocol;

		//wz_110309(�޸Ľӿ�)
		PROPMANAGER::instance()->GetStreamProtocol(DVBC,protocol);
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
	if(task->GetVersion() != "3.0")
	{
		retParser.SetAttrNode("Index",index,mediaStreamNode);//����MediaStream�ڵ��index����
	}
	retParser.SetAttrNode("URL",url,mediaStreamNode);//����MediaStream�ڵ��URL����
	retParser.SaveToString(retXml);
	return retXml;
}
//�����ֲ���ѯXML
std::string TranslateUpXMLForDVBC::TranslateStreamRoundQuery(const XMLTask* task,std::string deviceID,bool multi)
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
	if ((task->GetRetValue()==RUN_SUCCESS) || (task->GetRetValue()==SET_SUCCESS))
	{
		index=StrUtil::Int2Str(task->GetDeviceID());
		string protocol;

		//wz_110309(�޸Ľӿ�)
		PROPMANAGER::instance()->GetStreamProtocol(DVBC,protocol);
		if (protocol=="rtsp")
		{
			url="rtsp://" + PROPMANAGER::instance()->GetRtspVideoIp() + ":" + \
				PROPMANAGER::instance()->GetRtspVideoPort() +"/roundstream" ;//��Ƶ���ŵ�ַ
		}
		else
		{
			if(PROPMANAGER::instance()->GetUdpVideoServerUsed())
			{
				url="udp://@" + PROPMANAGER::instance()->GetUdpVideoServerClientIP() + ":5670";	//��Ƶ���ŵ�ַ
			}
			else
			{
				if(PROPMANAGER::instance()->GetCurWorkVersion() == "3.9")
				{
					url=protocol+"://" + PROPMANAGER::instance()->GetHttpVideoIp() + ":" + \
						PROPMANAGER::instance()->GetHttpVideoPort() +"/"+ StrUtil::Int2Str(task->GetDeviceID());	//��Ƶ���ŵ�ַ
				}
				else
				{
					//wz_0217
					int virtualid = 0;
					PROPMANAGER::instance()->GetVirDeviceId(DVBC, virtualid);
					if(multi)
					{
						url=protocol+"://" + PROPMANAGER::instance()->GetHttpVideoIp() + ":" + \
							PROPMANAGER::instance()->GetHttpVideoPort() +"/"+ deviceID;	//��Ƶ���ŵ�ַ
					}
					else
					{
						url=protocol+"://" + PROPMANAGER::instance()->GetHttpVideoIp() + ":" + \
							PROPMANAGER::instance()->GetHttpVideoPort() +"/"+ StrUtil::Int2Str(virtualid);	//��Ƶ���ŵ�ַ
					}
				}
			}
		}
	}
	XmlParser retParser;
	retParser.Set_xml(headerxml);

	pXMLNODE rootNode=retParser.GetRootNode();
	pXMLNODE streamNode=retParser.CreateNodePtr(rootNode,"ReturnInfo");//����ReturnInfo�ڵ�
	if(task->GetVersion() == "3.0")
	{
		pXMLNODE roundStreamNode=retParser.CreateNodePtr(streamNode,"RoundStreamURL");
		retParser.SetAttrNode("URL",url,roundStreamNode);//����MediaStream�ڵ��URL����
		retParser.SaveToString(retXml);
	}
	else
	{
		pXMLNODE mediaStreamNode=retParser.CreateNodePtr(streamNode,"StreamRoundInfoQuery");//����ReturnInfo�ڵ���ӽڵ�StreamRoundInfoQuery�ڵ�
		pXMLNODE roundStreamNode=retParser.CreateNodePtr(mediaStreamNode,"RoundStream");
		retParser.SetAttrNode("URL",url,roundStreamNode);//����MediaStream�ڵ��URL����
		retParser.SaveToString(retXml);
	}

	return retXml;
}
//����Ƶ��ɨ��XML
std::string TranslateUpXMLForDVBC::TranslateChannelScan(const XMLTask* task,const std::string& strRtnXML)
{
	std::string retXml=GetXmlHeader(task,"ChannelScanQuery");//��׼�ķ���ͷxml

	XmlParser retParser;
	std::string value, ScanType,ScanTime;
	if(strRtnXML.size()>0)
	{
		retParser.Set_xml(strRtnXML);
		pXMLNODE rtnTypeNode=retParser.GetNodeFromPath("Msg/Return");
		retParser.GetAttrNode(rtnTypeNode,"Value",value);//����Ӳ������xml�е�valueֵ���Դ��ж�Ӳ��ִ�гɹ����
		retParser.GetAttrNode(rtnTypeNode,"ScanTime",ScanTime);//ScanTime ΪɨƵ����ʱ��ʱ��
	}
	XmlParser parser;
	parser.Set_xml(retXml);
	pXMLNODE returnInfoNode = parser.CreateNodePtr(parser.GetRootNode(),"ReturnInfo");
	pXMLNODE channelScanQueryNode=parser.CreateNodePtr(returnInfoNode,"ChannelScanQuery");//Ϊ���ص�xml����ChannelScanReport�ڵ�
	parser.SetAttrNode("ScanTime",ScanTime,channelScanQueryNode);
	//��ʼ������
	string   STD, Freq, QAM, HeaderMode, CarrierNumber, EncodeEfficiency, InterwovenMode,SymbolRate;
	string   DoublePilot, PnPhase, OrgNetID, TsID;
	string   Program, ProgramID,ServiceID, VideoPID, AudioPID, Encryption, Encrypt, ISHDTV, ISNIT, IS3DTV; 

	///////////////////////////////ScanType////////////////////////////////
	string strTaskStandardXml  = task->GetStandardXml();
	XmlParser xml(strTaskStandardXml.c_str());
	pXMLNODE queryNode = xml.GetNodeFromPath("Msg/ChannelScanQuery/ChannelScan");
	xml.GetAttrNode(queryNode,"ScanType",ScanType);           
	ISNIT = (ScanType == "1") ? "1" : "0";
	///////////////////////////////////////////////////////////////
	IS3DTV = "0";//3D��Ŀ��ʶ


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
			if(OrgNetID.size()<1)
			{
				OrgNetID = "0";
			}
			if(TsID.size()<1)
			{
				TsID = "0";
			}
			parser.SetAttrNode("Freq",Freq,channelScanNode);//���÷���xml��ChannelScan�ڵ��Freq����ֵ
			parser.SetAttrNode("OrgNetID",OrgNetID,channelScanNode);//���÷���xml��ChannelScan�ڵ��OrgNetID����ֵ
			parser.SetAttrNode("TsID",TsID,channelScanNode);//���÷���xml��ChannelScan�ڵ��TsID����ֵ

			//add by gxd 2010-10-15
			parser.SetAttrNode("QAM",QAM,channelScanNode);
			parser.SetAttrNode("SymbolRate",SymbolRate,channelScanNode);

			pXMLNODELIST childNodeList=retParser.GetNodeList(childNode);//Ӳ������xml��ChannelScan�ڵ���ӽڵ��б�
			for (int k=0;k!=childNodeList->Size();++k)
			{
				pXMLNODE subChildNode=retParser.GetNextNode(childNodeList);//Ӳ������xml��ChannelScan�ڵ���ӽڵ�channel
				retParser.GetAttrNode(subChildNode,"Program",Program);//���Program����ֵ
				if(Program.find("unknow")==-1)//20131225 unknowƵ����Ȩ���ÿ���
				{
					retParser.GetAttrNode(subChildNode,"ProgramID",ProgramID); //���ProgramID����ֵ
					retParser.GetAttrNode(subChildNode,"ServiceID",ServiceID);//���ServiceID����ֵ
					retParser.GetAttrNode(subChildNode,"VideoPID",VideoPID);//���VideoPID����ֵ
					retParser.GetAttrNode(subChildNode,"AudioPID",AudioPID);//���AudioPID����ֵ
					retParser.GetAttrNode(subChildNode,"Encrypt", Encrypt);
					retParser.GetAttrNode(subChildNode,"Encryption", Encryption);
					retParser.GetAttrNode(subChildNode,"HDTV",ISHDTV);
					if((VideoPID.size()==0)&&(AudioPID.size() > 0)&&(AudioPID != "0"))
					{
						VideoPID = "0";
					}
					std::string strServiceType;
					retParser.GetAttrNode(subChildNode, "ServiceType", strServiceType);
					if((AudioPID.size() > 0)&&(AudioPID != "0"))
					{
						pXMLNODE channelNode;
						if (task->GetVersion() != "3.0")
						{
							channelNode=parser.CreateNodePtr(channelScanNode,"Channel");//Ϊ���ص�xml����Channel�ڵ�
							parser.SetAttrNode("Program",Program,channelNode);//���÷���xml��Channel�ڵ��Program����ֵ
						}
						else
						{
							channelNode=parser.CreateNodePtr(channelScanNode,"ChannelInfo");//Ϊ���ص�xml����Channel�ڵ�
							parser.SetAttrNode("ServiceName",Program,channelNode);//���÷���xml��Channel�ڵ��Program����ֵ
						}
						if(Program.find("����") != -1)
						{
							strServiceType = "0xF9";
						}
						else if((Program.find("�㲥") != -1)||(VideoPID == "5000"))//VideoPID=5000 �������ֹ㲥
						{
							strServiceType = "0x02";
						}
						else if(((VideoPID.size()<=0)||(VideoPID=="0"))&&(AudioPID.size()>0))//���ؽ�Ŀ�㲥
						{
							strServiceType = "0x02";
						}
						else if(Program.find("3D") != -1)
						{
							strServiceType = "0xE4";
						}
						else if((Program.find("���") != -1)||(Program.find("Ƭ��") != -1))
						{
							strServiceType = "0xC5";
						}
						else
						{
							strServiceType = "0x01";
						}
						parser.SetAttrNode("ServiceType", strServiceType, channelNode);
						parser.SetAttrNode("ServiceID",ServiceID,channelNode);//���÷���xml��Channel�ڵ��ServiceID����ֵ
						if (task->GetVersion() != "3.0")
						{
							if(VideoPID=="0")
							{
								parser.SetAttrNode("VideoPID",string(""),channelNode);//���÷���xml��Channel�ڵ��VideoPID����ֵ
							}
							else
							{
								parser.SetAttrNode("VideoPID",VideoPID,channelNode);//���÷���xml��Channel�ڵ��VideoPID����ֵ
							}
							parser.SetAttrNode("AudioPID",AudioPID,channelNode);//���÷���xml��Channel�ڵ��AudioPID 	
							parser.SetAttrNode("ProgramID",ProgramID,channelNode); //���÷���xml��Channel�ڵ��ProgramID����ֵ //FRee_fanrong_20131218  ¼���ѯ����ProgramID��Ƶ��ɨ������һ�£���Ϊһ��
							//
							if((VideoPID=="0")||(VideoPID.size() < 0)||(strServiceType == "0x02"))
							{
								parser.SetAttrNode("Encrypt", std::string("0"), channelNode);
							}
							else
							{
								if(Encrypt == "0")//ChannelScanQuery.xml����,Encrypt="0"��Ŀδ��Ȩ,Encrypt="1"��Ŀ��Ȩ��ICInfoChannelEncryptQuery_LastReport.xmlһ��
								{
									parser.SetAttrNode("Encrypt", string("1"),channelNode);//V2.6����ǰ�ӿ��ϱ�1Ϊ��Ŀδ��Ȩ
								}
								else
								{
									parser.SetAttrNode("Encrypt", string("0"),channelNode);//V2.6����ǰ�ӿ��ϱ�0Ϊ��Ŀ��Ȩ
								}
								if(g_sEncryptInfo.size()>0)
								{
									for(int i=0;i<g_sEncryptInfo.size();i++)
									{
										if((g_sEncryptInfo[i].freq == Freq)/*&&(g_sEncryptInfo[i].programid == ProgramID)*/ //����3.0�ӿ�ʹ�� 2017-11-21
											&&(g_sEncryptInfo[i].serviceid == ServiceID))
										{
											parser.SetAttrNode("Encrypt", g_sEncryptInfo[i].Encrypt,channelNode);
											break;
										}
									}
								}
							}
						}
						//
						if (task->GetVersion() != "3.0")
						{
							parser.SetAttrNode("ISNIT", ISNIT, channelNode);
							parser.SetAttrNode("HDTV",ISHDTV,channelNode);
							if (strServiceType == "0xE4")
								IS3DTV = "1";
							else
								IS3DTV = "0";
							parser.SetAttrNode("TV3D", IS3DTV, channelNode);
						}
						else
						{
							parser.SetAttrNode("IsNIT", ISNIT, channelNode);
							if (strServiceType == "0xE4")
							{
								IS3DTV = "2";
							}
							else if(ISHDTV == "1")
							{
								IS3DTV = "1";
							}
							else
							{
								IS3DTV = "0";
							}
							parser.SetAttrNode("TVType", IS3DTV, channelNode);
						}

						pXMLNODELIST StreamNodeList=retParser.GetNodeList(subChildNode);
						for(int n=0;n!=StreamNodeList->Size();n++)
						{
							string StreamType,PID;
							pXMLNODE subStreamNode=retParser.GetNextNode(StreamNodeList);
							retParser.GetAttrNode(subStreamNode,"StreamType",StreamType);
							retParser.GetAttrNode(subStreamNode,"PID",PID);
							if(PID != "0"&&PID.size()>0)
							{
								if (task->GetVersion() != "3.0")
								{
									pXMLNODE StreamNode=parser.CreateNodePtr(channelNode,"elementryPID");
									parser.SetAttrNode("StreamType",StreamType,StreamNode);
									parser.SetAttrNode("PID",PID,StreamNode);
								}
								else
								{
									pXMLNODE StreamNode=parser.CreateNodePtr(channelNode,"ElementryPID");
									parser.SetAttrNode("StreamType",StreamType,StreamNode);
									parser.SetAttrNode("PID",PID,StreamNode);
								}
							}
						}
					}
				}

			}
		}
	}
	parser.SaveToString(retXml);

	std::string strValidXml;
	if(retXml.find("RADIOUp") != -1 || retXml.find("AMUp") != -1 )
	{
		strValidXml = retXml;
	}
	else
	{
		strValidXml = XmlValidator::ValidChanScanXml( retXml );
	}
	//����&��;
	while(true)
	{
		if(strValidXml.find("&")!=-1||strValidXml.find(";")!=-1)
		{
			int posdz = strValidXml.find("&");
			if(posdz != -1)
			{
				strValidXml.replace(posdz,1,"A");
			}
			int posfh = strValidXml.find(";");
			if(posfh != -1)
			{
				strValidXml.replace(posfh,1,"B");
			}
		}
		else
		{
			break;
		}
	}
	XmlParser validParser( strValidXml.c_str() );
	validParser.SaveAsFile("C:/vscttb/ChanScanLastReport.xml");
	retXml = strValidXml;
	return retXml;
}
//����Ƶ������xml
std::string TranslateUpXMLForDVBC::TranslateChannelSet( const XMLTask* task )
{
	std::string retXml=GetXmlHeader(task,"ChannelSetReport");//��׼�ķ���ͷxml
	return retXml;
}

//������Ƶ�û���ѯXML
std::string TranslateUpXMLForDVBC::TranslateClientInfoQuery(const XMLTask* task,std::vector<sVedioUserInfo>& vecClient)
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
std::string TranslateUpXMLForDVBC::TranslateTableQuery(const XMLTask *task, std::string strFileName)
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
std::string TranslateUpXMLForDVBC::TranslateRecord(const XMLTask* task,const std::vector<XMLTask*>& resultVec)
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
	if (PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")
	{
		retXML=GetXmlHeader(task,"SetAutoRecordChannel");
		XmlParser parser(retXML.c_str());
		pXMLNODE returnNode = parser.GetNodeFromPath("Msg");
		pXMLNODE returninfoNode = parser.CreateNodePtr(returnNode, "ReturnInfo");
		pXMLNODE setNode = parser.CreateNodePtr(returninfoNode, "SetAutoRecordChannel");
		parser.SaveToString( retXML );
	}
	return retXML;
}

std::string TranslateUpXMLForDVBC::TranslateRecord( const XMLTask* task,const std::vector<sRecordSetInfo>& recSetInfoVec )
{
	std::string retXML;

	if(task->GetTaskID() == "0")
	{
		bool bnodevid = false;
		bool bokdevid = false;
		retXML=GetXmlHeader(task,"SetAutoRecordChannel");
		XmlParser parser(retXML.c_str());
		pXMLNODE returnNode = parser.GetNodeFromPath("Msg/Return");
		if(task->GetVersion()=="3.0")
		{
			pXMLNODE MsgNode = parser.GetNodeFromPath("Msg");
			pXMLNODE returninfoNode = parser.CreateNodePtr(MsgNode, "ReturnInfo");
			pXMLNODE setNode = parser.CreateNodePtr(returninfoNode, "SetAutoRecordChannel");
			std::vector<sRecordSetInfo>::const_iterator setIter = recSetInfoVec.begin();
			while ( setIter!=recSetInfoVec.end() )
			{
				std::string strFreq, strServiceId, strVideoPid, strAudioPid, strSymbolRate, strQam;
				XmlParser tempParser( (setIter->strTaskXml).c_str() );
				pXMLNODE recordNode = tempParser.GetNodeFromPath("Msg/AutoRecord/Record");
				tempParser.GetAttrNode(recordNode, "Freq", strFreq);
				pXMLNODE chanNode = parser.CreateNodePtr(setNode, "FreqInfo");
				parser.SetAttrNode("Freq", strFreq, chanNode);
				//
				pXMLNODE chcodeNode = parser.CreateNodePtr(chanNode, "ChannelInfo");
				tempParser.GetAttrNode(recordNode, "ServiceID", strServiceId);
				tempParser.GetAttrNode(recordNode, "VideoPID", strVideoPid);
				tempParser.GetAttrNode(recordNode, "AudioPID", strAudioPid);
				tempParser.GetAttrNode(recordNode, "SymbolRate", strSymbolRate);
				tempParser.GetAttrNode(recordNode, "QAM", strQam);
				
				if( strFreq == strHDMIFreq)
				{
					parser.SetAttrNode("ServiceID", std::string(""), chcodeNode);
					parser.SetAttrNode("Status", std::string(""), chcodeNode);
				}
				else
				{
					parser.SetAttrNode("ServiceID", strServiceId, chcodeNode);
					parser.SetAttrNode("Status", std::string(""), chcodeNode);
				}

				if(setIter->nDeviceId==-1)
				{
					bnodevid = true;
					parser.SetAttrNode("Status", std::string("1"), chcodeNode);
					parser.SetAttrNode("Comment", std::string("û��ͨ����Դ !"), chcodeNode);
				}
				else	//���óɹ�������¼���URL
				{
					bokdevid = true;
					std::string strStreamProtocol;
					PROPMANAGER::instance()->GetStreamProtocol(task->GetDVBType(), strStreamProtocol);
					std::string strUrl;
					if (strStreamProtocol == "rtsp")
					{
						strUrl = strStreamProtocol + "://" + PROPMANAGER::instance()->GetRtspVideoIp() + ":" + \
							PROPMANAGER::instance()->GetRtspVideoPort()+"/"+StrUtil::Int2Str(setIter->nDeviceId);
					}
					else
					{
						strUrl = strStreamProtocol + "://" + PROPMANAGER::instance()->GetHttpVideoIp() + ":" + \
							PROPMANAGER::instance()->GetHttpVideoPort()+"/"+StrUtil::Int2Str(setIter->nDeviceId);
					}
					parser.SetAttrNode("Status", std::string("0"), chcodeNode);
					parser.SetAttrNode("Url", strUrl, chcodeNode);
				}
				//
				pXMLNODE TranschanNode = parser.CreateNodePtr(chcodeNode, "TranscodeInfo");
				vector<CoderDev> CoderDevInfo;
				PROPMANAGER::instance()->GetCoderVecFromFreqSerID(strFreq,strServiceId,CoderDevInfo);
				bool bfirst = true;
				for(int i=0;i<CoderDevInfo.size();i++)
				{
					if((CoderDevInfo[i].s_maincoderdeviceid == StrUtil::Int2Str(setIter->nDeviceId))||(CoderDevInfo[i].s_coderdeviceid == StrUtil::Int2Str(setIter->nDeviceId)))
					{
						pXMLNODE TranschanNodeInfoNode = parser.CreateNodePtr(TranschanNode, "Info");
						parser.SetAttrNode("ChassisID", CoderDevInfo[i].s_monitorindex, TranschanNodeInfoNode);
						parser.SetAttrNode("Position", CoderDevInfo[i].s_cardindex, TranschanNodeInfoNode);
						parser.SetAttrNode("ModuleID", CoderDevInfo[i].s_modindex, TranschanNodeInfoNode);
						parser.SetAttrNode("Transcode", CoderDevInfo[i].s_modcoderindex, TranschanNodeInfoNode);
						if((bfirst)&&(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0"))
						{
							std::string strStreamProtocol;
							PROPMANAGER::instance()->GetStreamProtocol(task->GetDVBType(), strStreamProtocol);
							std::string strUrl = strStreamProtocol + "://" + PROPMANAGER::instance()->GetHttpVideoIp() + ":" + \
							PROPMANAGER::instance()->GetHttpVideoPort()+"/"+CoderDevInfo[i].s_monitorindex+"_"+ \
							CoderDevInfo[i].s_cardindex+"_"+CoderDevInfo[i].s_modindex+"_"+CoderDevInfo[i].s_modcoderindex;
							parser.SetAttrNode("Url", strUrl, chcodeNode);
							bfirst = false;
						}
					}
				}
				//
				setIter ++;
			}
		}
		else
		{
			pXMLNODE setNode = parser.CreateNodePtr(returnNode, "SetAutoRecordChannel");
			pXMLNODE chanNode = parser.CreateNodePtr(setNode, "Channel");
			parser.SetAttrNode("Action", std::string("Set"), chanNode);
			std::vector<sRecordSetInfo>::const_iterator setIter = recSetInfoVec.begin();
			while ( setIter!=recSetInfoVec.end() )
			{
				pXMLNODE chcodeNode = parser.CreateNodePtr(chanNode, "ChCode");
				if(setIter->nDeviceId!=-1)
				{
					parser.SetAttrNode("SetAction", std::string("0"), chcodeNode);
				}
				else
				{
					parser.SetAttrNode("SetAction", std::string("1"), chcodeNode);
				}

				XmlParser tempParser( (setIter->strTaskXml).c_str() );
				pXMLNODE recordNode = tempParser.GetNodeFromPath("Msg/AutoRecord/Record");
				std::string strFreq, strServiceId, strVideoPid, strAudioPid, strSymbolRate, strQam;
				tempParser.GetAttrNode(recordNode, "Freq", strFreq);
				tempParser.GetAttrNode(recordNode, "ServiceID", strServiceId);
				tempParser.GetAttrNode(recordNode, "VideoPID", strVideoPid);
				tempParser.GetAttrNode(recordNode, "AudioPID", strAudioPid);
				tempParser.GetAttrNode(recordNode, "SymbolRate", strSymbolRate);
				tempParser.GetAttrNode(recordNode, "QAM", strQam);
				
				if( strFreq == strHDMIFreq)
				{
					parser.SetAttrNode("Freq", strFreq, chcodeNode);
					parser.SetAttrNode("SymbolRate", std::string(""), chcodeNode);
					parser.SetAttrNode("QAM", std::string(""), chcodeNode);
					parser.SetAttrNode("ServiceID", std::string(""), chcodeNode);
				}
				else
				{
					parser.SetAttrNode("Index", setIter->nDeviceId, chcodeNode);
					parser.SetAttrNode("Freq", strFreq, chcodeNode);
					parser.SetAttrNode("SymbolRate", strSymbolRate, chcodeNode);
					parser.SetAttrNode("QAM", strQam, chcodeNode);
					parser.SetAttrNode("ServiceID", strServiceId, chcodeNode);
					parser.SetAttrNode("VideoPID", strVideoPid, chcodeNode);
					parser.SetAttrNode("AudioPID", strAudioPid, chcodeNode);
				}

				//if (!setIter->bSetAction)	//����ʧ�ܣ��ͷ���ʧ��ԭ��
				if(setIter->nDeviceId==-1)
				{
					bnodevid = true;
					parser.SetAttrNode("Comment", std::string("û��ͨ����Դ !"), chcodeNode);
				}
				else	//���óɹ�������¼���URL
				{
					bokdevid = true;
					std::string strStreamProtocol;
					PROPMANAGER::instance()->GetStreamProtocol(task->GetDVBType(), strStreamProtocol);
					std::string strUrl;
					if (strStreamProtocol == "rtsp")
					{
						strUrl = strStreamProtocol + "://" + PROPMANAGER::instance()->GetRtspVideoIp() + ":" + \
							PROPMANAGER::instance()->GetRtspVideoPort()+"/"+StrUtil::Int2Str(setIter->nDeviceId);
					}
					else
					{
						strUrl = strStreamProtocol + "://" + PROPMANAGER::instance()->GetHttpVideoIp() + ":" + \
							PROPMANAGER::instance()->GetHttpVideoPort()+"/"+StrUtil::Int2Str(setIter->nDeviceId);
					}

					parser.SetAttrNode("Url", strUrl, chcodeNode);
				}

				setIter ++;
			}
		}
		if(bnodevid&&bokdevid)
		{
			parser.SetAttrNode("Value", std::string("1"), returnNode);
			parser.SetAttrNode("Desc", std::string("����¼��ɹ�����"), returnNode);
		}
		else if(bnodevid)
		{
			parser.SetAttrNode("Value", std::string("1"), returnNode);
			parser.SetAttrNode("Desc", std::string("ȫ��¼������ʧ��"), returnNode);
		}
		else if(bokdevid)
		{
			parser.SetAttrNode("Value", std::string("0"), returnNode);
			parser.SetAttrNode("Desc", std::string("ȫ��¼�����óɹ�"), returnNode);
		}

		parser.SaveToString(retXML);

		if(task->GetDVBType() == CTTB)
		{
			parser.SaveAsFile("c:\\Recordlist\\CTTB\\RealRecordSet.xml");//xml���浽�ļ�
		}
		else if(task->GetDVBType() == DVBC)
		{
			parser.SaveAsFile("c:\\RecordList\\DVBC\\RealRecordSet.xml");//xml���浽�ļ�
		}
		else if(task->GetDVBType() == DVBS)
		{
			parser.SaveAsFile("c:\\RecordList\\DVBS\\RealRecordSet.xml");//xml���浽�ļ�
		}
		else if(task->GetDVBType() == RADIO)
		{
			parser.SaveAsFile("c:\\RecordList\\RADIO\\RealRecordSet.xml");//xml���浽�ļ�
		}
		else if(task->GetDVBType() == AM)
		{
			parser.SaveAsFile("c:\\RecordList\\AM\\RealRecordSet.xml");//xml���浽�ļ�
		}
	}
	else
	{
		retXML=GetXmlHeader(task,"ProvisionalRecordTaskSet");
	}

	return retXML;
}

//����¼�����ò�ѯXML
std::string TranslateUpXMLForDVBC::TranslateRecordSetQuery(const XMLTask* task,const std::vector<string>& recSetQueryInfoVec)
{
	string retXML = GetXmlHeader(task,"AutoRecordChannelQuery");
	string orgFreq;
	XmlParser retParser(retXML.c_str());
	pXMLNODE rootNode = retParser.GetRootNode();
	pXMLNODE returnNode = retParser.CreateNodePtr(rootNode, "ReturnInfo");
	pXMLNODE reportNode = retParser.CreateNodePtr(returnNode, "AutoRecordChannelQuery");
	pXMLNODE pFreqInfoNode;
	std::vector<string>::const_iterator recSetQueryIter=recSetQueryInfoVec.begin();
	for (;recSetQueryIter != recSetQueryInfoVec.end();recSetQueryIter++)
	{
		XmlParser parser;
		parser.Set_xml((*recSetQueryIter).c_str());
		pXMLNODE pRecQueryNode = parser.GetNodeFromPath("Msg/AutoRecord/Record");

		string Freq,IndexSet,ServiceID,ServiceType,CodingFormat,Width,Height,Fps,VideoRate,OSDEnable,Encoder,Url,TDeviceID,CDeviceID;
		parser.GetAttrNode(pRecQueryNode,"Freq",Freq);
		parser.GetAttrNode(pRecQueryNode,"ServiceID",ServiceID);
		parser.GetAttrNode(pRecQueryNode,"IndexSet",IndexSet);
		parser.GetAttrNode(pRecQueryNode,"ServiceType",ServiceType);
		parser.GetAttrNode(pRecQueryNode,"CodingFormat",CodingFormat);
		parser.GetAttrNode(pRecQueryNode,"Width",Width);
		parser.GetAttrNode(pRecQueryNode,"Height",Height);
		parser.GetAttrNode(pRecQueryNode,"Fps",Fps);
		parser.GetAttrNode(pRecQueryNode,"Bps",VideoRate);
		parser.GetAttrNode(pRecQueryNode,"OSDEnable",OSDEnable);
		parser.GetAttrNode(pRecQueryNode,"Encoder",Encoder);
		parser.GetAttrNode(pRecQueryNode,"TDeviceID",TDeviceID);
		parser.GetAttrNode(pRecQueryNode,"CDeviceID",CDeviceID);

		if (orgFreq != Freq)
		{
			pFreqInfoNode = retParser.CreateNodePtr(reportNode, "FreqInfo");
			retParser.SetAttrNode("Freq",Freq,pFreqInfoNode);
			retParser.SetAttrNode("IndexSet",IndexSet,pFreqInfoNode);
			orgFreq = Freq;
		}

		pXMLNODE pChannelInfoNode = retParser.CreateNodePtr(pFreqInfoNode, "ChannelInfo");
		retParser.SetAttrNode("ServiceID",ServiceID,pChannelInfoNode);
		retParser.SetAttrNode("ServiceType",ServiceType,pChannelInfoNode);
		retParser.SetAttrNode("CodingFormat",CodingFormat,pChannelInfoNode);
		retParser.SetAttrNode("Width",Width,pChannelInfoNode);
		retParser.SetAttrNode("Height",Height,pChannelInfoNode);
		retParser.SetAttrNode("Fps",Fps,pChannelInfoNode);
		retParser.SetAttrNode("Fps",Fps,pChannelInfoNode);
		retParser.SetAttrNode("VideoRate",VideoRate,pChannelInfoNode);
		retParser.SetAttrNode("OSDEnable",OSDEnable,pChannelInfoNode);
		retParser.SetAttrNode("Encoder",Encoder,pChannelInfoNode);

		std::string strStreamProtocol;
		PROPMANAGER::instance()->GetStreamProtocol(task->GetDVBType(), strStreamProtocol);
		std::string strUrl;
		if (strStreamProtocol == "rtsp")
		{
			Url = strStreamProtocol + "://" + PROPMANAGER::instance()->GetRtspVideoIp() + ":" + \
				PROPMANAGER::instance()->GetRtspVideoPort()+"/"+ CDeviceID;
		}
		else
		{
			Url = strStreamProtocol + "://" + PROPMANAGER::instance()->GetHttpVideoIp() + ":" + \
				PROPMANAGER::instance()->GetHttpVideoPort()+"/"+ CDeviceID;
		}
		retParser.SetAttrNode("Url",Url,pChannelInfoNode);

		TunerDev tdev;
		PROPMANAGER::instance()->GetTunerInfo(atoi(TDeviceID.c_str()),task->GetDVBType(),tdev);
		pXMLNODE pDemodulateInfoNode = retParser.CreateNodePtr(pChannelInfoNode, "DemodulateInfo");
		retParser.SetAttrNode("ChassisID",tdev.s_monitorindex,pDemodulateInfoNode);
		retParser.SetAttrNode("Position",tdev.s_cardindex,pDemodulateInfoNode);
		retParser.SetAttrNode("Demodulate",tdev.s_chanindex,pDemodulateInfoNode);

		vector<CoderDev> CoderDevInfo;
		PROPMANAGER::instance()->GetCoderVecFromFreqSerID(Freq,ServiceID,CoderDevInfo);

		pXMLNODE pTranscodeInfoNode = retParser.CreateNodePtr(pChannelInfoNode, "TranscodeInfo");
		vector<CoderDev>::const_iterator coderDevIter = CoderDevInfo.begin();
		for(;coderDevIter !=CoderDevInfo.end();coderDevIter++)
		{
			pXMLNODE pInfoNode = retParser.CreateNodePtr(pTranscodeInfoNode, "Info");
			retParser.SetAttrNode("ChassisID",(*coderDevIter).s_monitorindex,pInfoNode);
			retParser.SetAttrNode("Position",(*coderDevIter).s_cardindex,pInfoNode);
			retParser.SetAttrNode("ModuleID",(*coderDevIter).s_modindex,pInfoNode);
			retParser.SetAttrNode("Transcode",(*coderDevIter).s_modcoderindex,pInfoNode);
		}
	}

	retParser.SaveToString( retXML );
	return retXML;
}

//���؂���¼������XML
std::string TranslateUpXMLForDVBC::TranslateRoundRecord(const XMLTask* task,const std::vector<XMLTask*>& resultVec)
{
	std::string retXML = GetXmlHeader(task,"RoundRecordSet");
	return retXML;
}

//����ָ����������xml
std::string TranslateUpXMLForDVBC::TranslateQualitySet( const XMLTask* task )
{
	std::string retXml=GetXmlHeader(task,"QualityReportTaskSet");//��׼�ķ���ͷxml
	return retXml;
}
//����ָ���ѯXML
std::string TranslateUpXMLForDVBC::TranslateQualityQuery( const XMLTask* task,std::map< string,vector<eQualityInfo> > FreqQualityInfo)
{
	string TaskXml = task->GetStandardXml();
	XmlParser xml;
	xml.Set_xml(TaskXml);
	pXMLNODE pNode = xml.GetNodeFromPath("Msg/QualityQuery/Quality");
	string sIndex;
	xml.GetAttrNode(pNode, "Index", sIndex);
	string rtnHead;
	XmlParser parser;
	if (sIndex.size() > 0)
	{
		rtnHead =  GetXmlHeader(task,"GetIndexSet");
		parser.Set_xml(rtnHead);
		pXMLNODE rootNode = parser.GetRootNode();
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
					CHANNELINFOMGR::instance()->GetChannelCodeByFreq(DVBC,vecitr->freq,channelcode);
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
				pXMLNODE childNode = parser.CreateNodePtr(rootNode, "ReturnInfo");

				std::map< string,vector<eQualityInfo> >  TimeQualityInfo;
				//��checktime����������ͬchecktime��eQualityInfo����ͬһvector������
				for (size_t i = 0;i<(*ptr).second.size();i++)
				{
					TimeQualityInfo[(*ptr).second.at(i).checktime].push_back((*ptr).second.at(i));
				}
				//���ڲ�ͬ��checktime����GetIndexSet�ڵ㼰GetIndex�ڵ��б�
				for (std::map< string,vector<eQualityInfo> >::iterator in_ptr=TimeQualityInfo.begin();in_ptr!=TimeQualityInfo.end();++in_ptr)
				{
					try
					{
						pXMLNODE reportnode=parser.CreateNodePtr(childNode,"GetIndexSet");

						parser.SetAttrNode(string("Index"),sIndex,reportnode); //����GetIndexSet�ڵ��Index����
						parser.SetAttrNode(string("Freq"),(*ptr).first,reportnode);  //����GetIndexSet�ڵ��Freq����
						parser.SetAttrNode(string("Time"),(*in_ptr).first,reportnode); //����GetIndexSet�ڵ��Time����
						for (vector<eQualityInfo>::iterator pptr=(*in_ptr).second.begin();pptr!=(*in_ptr).second.end();++pptr)
						{
							pXMLNODE qualitynode=parser.CreateNodePtr(reportnode,"GetIndex");
							parser.SetAttrNode(string("Type"),(*pptr).type,qualitynode); //����GetIndex�ڵ��Type����
							parser.SetAttrNode(string("Desc"),(*pptr).desc,qualitynode); //����GetIndex�ڵ��Desc����
							parser.SetAttrNode(string("Value"),(*pptr).valu,qualitynode); //����GetIndex�ڵ��Value����
						}
					}
					catch(...)
					{
						string error = "���� ��ʷָ��鿴 �ӽڵ�����ʧ��";
						APPLOG::instance()->WriteLog( OTHER,LOG_EVENT_ERROR,error,LOG_OUTPUT_BOTH );
						return "";
					}
				}
			}
		}
	}
	else
	{
		if(task->GetVersion() == "3.0")
		{
			rtnHead =  GetXmlHeader(task,"IndexQuery");
		}
		else
		{
			rtnHead =  GetXmlHeader(task,"QualityQuery");
		}
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
					CHANNELINFOMGR::instance()->GetChannelCodeByFreq(DVBC,vecitr->freq,channelcode);
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
			if(task->GetVersion() == "3.0")
			{
				pXMLNODE reportnode=parser.CreateNodePtr(parser.GetRootNode(),"ReturnInfo");
				for (std::map< string,vector<eQualityInfo> >::iterator ptr=FreqQualityInfo.begin();ptr!=FreqQualityInfo.end();++ptr)
				{
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
							pXMLNODE qualitynode=parser.CreateNodePtr(reportnode,"IndexQuery");
							parser.SetAttrNode(string("Freq"),(*ptr).first,qualitynode);  //����Quality�ڵ��Freq����
							parser.SetAttrNode(string("Time"),(*in_ptr).first,qualitynode); //����Quality�ڵ��CheckTime����
							//��ͬSTD�� Freq�� CheckTime��ָ�����б�
							for (vector<eQualityInfo>::iterator pptr=(*in_ptr).second.begin();pptr!=(*in_ptr).second.end();++pptr)
							{

								pXMLNODE qualityindexnode=parser.CreateNodePtr(qualitynode,"Index");
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
		}
	}
	string rtnXML;
	parser.SaveToString(rtnXML);
	return  rtnXML;
}

//ʵʱָ���ѯ
std::string TranslateUpXMLForDVBC::TranslateQualityRealtimeQuery(const XMLTask* task,std::string retxml)
{
	string rtnHead;
	if(task->GetVersion() == "3.0")
	{
		rtnHead =  GetXmlHeader(task,"IndexQuery");
	}
	else
	{
		rtnHead =  GetXmlHeader(task,"GetIndexSet");
	}
	eDVBType dtype = task->GetDVBType();
	//����������Ķ���
	XmlParser parser;
	parser.Set_xml(rtnHead);
	XmlParser devParser;
	devParser.Set_xml(retxml);
	string freq,type,desc,val;
	pXMLNODE rootNode = parser.GetRootNode();
	pXMLNODE devRootNode= devParser.GetRootNode();

	pXMLNODE IndexNode = parser.CreateNodePtr(rootNode,"ReturnInfo");
	pXMLNODE GetListNode;
	if(task->GetVersion() == "3.0")
	{
		GetListNode = parser.CreateNodePtr(IndexNode,"IndexQuery");
	}
	else
	{
		GetListNode = parser.CreateNodePtr(IndexNode,"GetIndexSet");
	}
	pXMLNODE ReportNode = devParser.GetNodeFromPath("Msg/QualityQueryReport");
	devParser.GetAttrNode(ReportNode,"Freq",freq);
	if(task->GetVersion() != "3.0")
	{
		parser.SetAttrNode("Index",task->GetDeviceID(),GetListNode);
	}
	parser.SetAttrNode("Freq",freq,GetListNode);
	parser.SetAttrNode("Time",TimeUtil::GetCurDateTime(),GetListNode);
	pXMLNODE paramsNode = devParser.GetNodeFromPath("Msg/QualityQueryReport/QualityParam");

	pXMLNODELIST paramList = devParser.GetNodeList(paramsNode);

	int count=devParser.GetChildCount(paramsNode);

	float level = 0;
	for(int k=0;k<count;k++)
	{
		pXMLNODE getnode=NULL;
		pXMLNODE paramnode=NULL;

		paramnode = devParser.GetNextNode(paramList);
		if(devParser.GetNodeName(paramnode)=="QualityIndex")
		{
			string add;
			if(task->GetVersion() == "3.0")
			{
				getnode=parser.CreateNodePtr(GetListNode,"Index");
			}
			else
			{
				getnode=parser.CreateNodePtr(GetListNode,"GetIndex");
			}
			devParser.GetAttrNode(paramnode,"Type",type);
			devParser.GetAttrNode(paramnode,"Desc",desc);
			devParser.GetAttrNode(paramnode,"Value",val);
			if (type=="1")
			{
				level=StrUtil::Str2Int(val);
				//
			}
			else if (type == "2")
			{
				if(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")
				{
					int per = 0;
					string tval = "0.0";
					float fValue = StrUtil::Str2Int(val)*0.0000000001;
					if(fValue<1)
					{
						for(int i=0;i<10;i++)
						{
							float temp = float(fValue)*10.0f;
							per = per-1;
							fValue = temp;
							if(temp>=1)
							{
								tval = StrUtil::Float2Str1(fValue);
								break;
							}
						}
					}
					else
					{
						tval = StrUtil::Float2Str1(fValue);
					}
					QUALITYCOMPENSATIONMGR::instance()->GetQualityCompensation(task->GetDVBType(),"0",type,freq,StrUtil::Int2Str(level),add);//�°�����ָ��ͨ��ʹ��0,���ݿ�У׼����Ҳ��0
					if(add.size()>0)
					{
						per = per + StrUtil::Str2Int(add);
					}
					if(per < -8)
					{
						val = "0.0E-8";
					}
					else
					{
						val = tval+string("E")+StrUtil::Int2Str(per);
					}
				}
				else
				{
					int ps = val.find("_");
					string num;
					string sum;
					if(ps != -1)
					{
						num = val.substr(0,ps);
						sum = val.substr(ps+1,val.size()-ps-1);
					}
					if(num=="0")
					{
						if((sum=="0")&&(g_strBer.size()>0))
						{
							val = g_strBer;
						}
						else
						{
							val = "0.0E-8";
						}
					}
					else
					{
						//
						QUALITYCOMPENSATIONMGR::instance()->GetQualityCompensation(task->GetDVBType(),"0",type,freq,StrUtil::Int2Str(level),add);//�°�����ָ��ͨ��ʹ��0,���ݿ�У׼����Ҳ��0
						float ftemnumValue = StrUtil::Str2Float(num);
						float fValue = 0.00000001;
						if(add.size()>0)
						{
							ftemnumValue = ftemnumValue+StrUtil::Str2Int(add);
						}
						if(ftemnumValue>0&&sum.size()>0)
						{
							fValue = ftemnumValue/StrUtil::Str2Float(sum);
						}
						else if(num.size()>0&&sum.size()>0)//��ֹ��������ɸ����Ĵ���
						{
							fValue = 0.1f/StrUtil::Str2Float(sum);
						}
						int per = 0;
						string tval = "1.0";
						for(int i=0;i<8;i++)
						{
							float temp = fValue*10;
							per = per-1;
							fValue = temp;
							if(temp>1)
							{
								tval = StrUtil::Float2Str1(fValue);
								if(tval.size()<=0)
								{
									tval = "1.0";	
								}
								break;
							}
						}
						if(per <= -8)
						{
							tval = "0.0";
						}
						val = tval+string("E")+StrUtil::Int2Str(per);
						g_strBer = val;
					}
				}
			}
			if (type == "2")
			{
				parser.SetAttrNode("Type",type,getnode);
				parser.SetAttrNode("Desc",desc,getnode);
				parser.SetAttrNode("Value",val,getnode);
			}
			else
			{
				QUALITYCOMPENSATIONMGR::instance()->GetQualityCompensation(task->GetDVBType(),"0",type,freq,StrUtil::Int2Str(level),add);//�°�����ָ��ͨ��ʹ��0,���ݿ�У׼����Ҳ��0
				float val_int = StrUtil::Str2Float(val)+StrUtil::Str2Float(add);
				parser.SetAttrNode("Type",type,getnode);
				parser.SetAttrNode("Desc",desc,getnode);
				parser.SetAttrNode("Value",StrUtil::Float2Str(val_int),getnode);
			}
		}
	}

	string rtnXML;
	parser.SaveToString(rtnXML);
	return  rtnXML;
}
//���ر�����������XML
std::string TranslateUpXMLForDVBC::TranslateAlarmParamSet( const XMLTask* task )
{
	//std::string retXml=GetXmlHeader(task,"AlarmParamSet");//��׼�ķ���ͷxml
	std::string strTaskType = (const_cast<XMLTask*>(task))->GetObjectName();
	std::string retXml=GetXmlHeader(task, strTaskType);//��׼�ķ���ͷxml
	return retXml;
}

//���ر���������ѯXML
std::string TranslateUpXMLForDVBC::TranslateAlarmParamQuery(const XMLTask* task, const AlarmParamVec& vecAlarmParam)
{
	std::string strTaskType = (const_cast<XMLTask*>(task))->GetObjectName();
	std::string retXml=GetXmlHeader(task, strTaskType);//��׼�ķ���ͷxml

	XmlParser retParser(retXml.c_str());
	pXMLNODE rootNode = retParser.GetRootNode();
	pXMLNODE returnNode = retParser.CreateNodePtr(rootNode, "ReturnInfo");
	pXMLNODE reportNode = retParser.CreateNodePtr(returnNode, (char*)strTaskType.c_str());

	AlarmParamVec ::const_iterator alarmParamIter= vecAlarmParam.begin();
	string alarmParamFreq,alarmParamChannelID;
	pXMLNODE alarmFreqInfoNode;
	pXMLNODE alarmServiceIdInfoNode;
	for (;alarmParamIter!=vecAlarmParam.end();alarmParamIter++)
	{
		if (strTaskType == "AlarmRFInfoQuery")
		{
			if((*alarmParamIter).Freq.size()>0)
			{
				if ((*alarmParamIter).Freq != alarmParamFreq)
				{
					alarmFreqInfoNode = retParser.CreateNodePtr(reportNode, "AlarmRFInfo");
					retParser.SetAttrNode("Freq",(*alarmParamIter).Freq,alarmFreqInfoNode);
					alarmParamFreq = (*alarmParamIter).Freq;
				}
				pXMLNODE alarmParamSwitchNode = retParser.CreateNodePtr(alarmFreqInfoNode, "AlarmRFSwitch");
				retParser.SetAttrNode("Type",(*alarmParamIter).TypeID,alarmParamSwitchNode);
				retParser.SetAttrNode("Desc",(*alarmParamIter).TypeDesc,alarmParamSwitchNode);
				retParser.SetAttrNode("Switch",(*alarmParamIter).Switch,alarmParamSwitchNode);
				if ((*alarmParamIter).TypeID != "1")
				{
					pXMLNODE alarmParamThresholdNode = retParser.CreateNodePtr(alarmParamSwitchNode, "AlarmRFThreshold");
					retParser.SetAttrNode("DownThreshold",(*alarmParamIter).DownThreshold,alarmParamThresholdNode);
					retParser.SetAttrNode("UpThreshold",(*alarmParamIter).UpThreshold,alarmParamThresholdNode);
				}
			}
		}
		else if (strTaskType == "AlarmStreamInfoQuery")
		{
			if((*alarmParamIter).Freq.size()>0)
			{
				if ((*alarmParamIter).Freq != alarmParamFreq)
				{
					alarmFreqInfoNode = retParser.CreateNodePtr(reportNode, "AlarmStreamInfo");
					retParser.SetAttrNode("Freq",(*alarmParamIter).Freq,alarmFreqInfoNode);
					alarmParamFreq = (*alarmParamIter).Freq;
				}
				pXMLNODE alarmParamSwitchNode = retParser.CreateNodePtr(alarmFreqInfoNode, "AlarmStreamSwitch");
				retParser.SetAttrNode("Type",(*alarmParamIter).TypeID,alarmParamSwitchNode);
				retParser.SetAttrNode("Desc",(*alarmParamIter).TypeDesc,alarmParamSwitchNode);
				retParser.SetAttrNode("Switch",(*alarmParamIter).Switch,alarmParamSwitchNode);
				pXMLNODE alarmParamThresholdNode = retParser.CreateNodePtr(alarmParamSwitchNode, "AlarmStreamThreshold");
				retParser.SetAttrNode("Num",(*alarmParamIter).Num,alarmParamThresholdNode);
				retParser.SetAttrNode("TimeInterval",(*alarmParamIter).TimeInterval,alarmParamThresholdNode);
			}
		}
		else if (strTaskType == "AlarmChannelInfoQuery")
		{
			if((*alarmParamIter).Freq.size()>0)
			{
				if ((*alarmParamIter).Freq != alarmParamFreq)
				{
					alarmFreqInfoNode = retParser.CreateNodePtr(reportNode, "FreqInfo");
					retParser.SetAttrNode("Freq",(*alarmParamIter).Freq,alarmFreqInfoNode);
					alarmParamFreq = (*alarmParamIter).Freq;
				}
				string serviceId;
				CHANNELINFOMGR::instance()->GetServIdByChanId(task->GetDVBType(),serviceId,(*alarmParamIter).ChannelID);
				if((*alarmParamIter).ChannelID.size()>0)
				{
					if((*alarmParamIter).ChannelID != alarmParamChannelID)
					{
						alarmServiceIdInfoNode = retParser.CreateNodePtr(alarmFreqInfoNode, "ChannelInfo");
						retParser.SetAttrNode("ServiceID",serviceId,alarmServiceIdInfoNode);
						alarmParamChannelID = (*alarmParamIter).ChannelID;
					}
					pXMLNODE alarmAlarmChannelNode = retParser.CreateNodePtr(alarmServiceIdInfoNode, "AlarmChannel");
					retParser.SetAttrNode("Type",(*alarmParamIter).TypeID,alarmAlarmChannelNode);
					retParser.SetAttrNode("Desc",(*alarmParamIter).TypeDesc,alarmAlarmChannelNode);
					retParser.SetAttrNode("Switch",(*alarmParamIter).Switch,alarmAlarmChannelNode);
					retParser.SetAttrNode("Duration",(*alarmParamIter).Duration,alarmAlarmChannelNode);
				}
			}
		}
	}

	retParser.SaveToString(retXml);
	return retXml;
}

std::string TranslateUpXMLForDVBC::TranslateAlarmReport( const XMLTask* task )
{
	std::string retXml=GetXmlHeader(task,"AlarmTypeSet");//��׼�ķ���ͷxml
	return retXml;
}

//wz_110107
std::string TranslateUpXMLForDVBC::TranslateCleanAlarmStateTask(const XMLTask* task)
{
	std::string retXml=GetXmlHeader(task,"CleanAlarmState");//��׼�ķ���ͷxml
	return retXml;
}


//wz_110107
std::string TranslateUpXMLForDVBC::TranslateMatrixQuery(const XMLTask* task)
{
	std::string retXml=GetXmlHeader(task,"MatrixQuery");//��׼�ķ���ͷxml
	return retXml;
}

//wz_110107
std::string TranslateUpXMLForDVBC::TranslateChangeQAMQuery(const XMLTask* task)
{
	std::string retXml=GetXmlHeader(task,"ChangeQAMQuery");//��׼�ķ���ͷxml
	return retXml;
}

//wz_110107
std::string TranslateUpXMLForDVBC::TranslateLoopAlarmInfo(const XMLTask* task)
{
	std::string retXml=GetXmlHeader(task,"LoopAlaInf");//��׼�ķ���ͷxml
	return retXml;
}

//wz_110107
std::string TranslateUpXMLForDVBC::TranslateRecordParamSet(const XMLTask* task)
{
	std::string retXml=GetXmlHeader(task,"RecordParamSet");//��׼�ķ���ͷxml
	return retXml;
}

//����ͨ��״̬��ѯXML
std::string TranslateUpXMLForDVBC::TranslateDeviceStatusQuery(const XMLTask* task,std::vector<sDeviceStatus> & VecDeviceStatus)
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
std::string TranslateUpXMLForDVBC::TranslateRunPlanSet(const XMLTask* task)
{
	std::string retXml;
	if (task->GetVersion() == "3.0")
		retXml=GetXmlHeader(task,"RunningChartSet");//��׼�ķ���ͷxml
	else
		retXml=GetXmlHeader(task,"AlarmTimeSet");
	return retXml;
}
//��������ͼ��ѯXML
std::string TranslateUpXMLForDVBC::TranslateRunPlanCheck(const XMLTask* task, std::string equCode, const RunPlanParamMap& mapRunplanParam)
{
	std::string retXml=GetXmlHeader(task,"RunningChartQuery");		//��׼�ķ���ͷxml

	XmlParser retParser( retXml.c_str() );
	pXMLNODE rootNode = retParser.GetRootNode();
	pXMLNODE returnNode = retParser.CreateNodePtr(rootNode, "ReturnInfo");
    pXMLNODE reportNode = retParser.CreateNodePtr(returnNode, "RunningChartQuery");

	if (mapRunplanParam.size() > 0)
	{
		RunPlanParamMap::const_iterator cRunplanIter = mapRunplanParam.begin();
		RunPlanParamVec::const_iterator RunplanVecIter = (*cRunplanIter).second.begin();
		string channelId;
		pXMLNODE channelNode;
		for (; RunplanVecIter!=(*cRunplanIter).second.end(); RunplanVecIter++)
		{
			if ((*RunplanVecIter).ChannelID != channelId)
			{
				channelId = (*RunplanVecIter).ChannelID;
				channelNode = retParser.CreateNodePtr(reportNode, "ChannelInfo");
				string freq,serviceId;
				CHANNELINFOMGR::instance()->GetFreqByChanId(task->GetDVBType(),freq, (*RunplanVecIter).ChannelID);
				CHANNELINFOMGR::instance()->GetServIdByChanId(task->GetDVBType(),serviceId, (*RunplanVecIter).ChannelID);
				retParser.SetAttrNode("Freq", freq, channelNode);
				retParser.SetAttrNode("ServiceID", serviceId, channelNode);
			}
			if ( (*RunplanVecIter).Month!="" && (*RunplanVecIter).DayOfMonth!="" && (*RunplanVecIter).StartTime!="" && (*RunplanVecIter).EndTime!="" )
			{	
				pXMLNODE cycleNode = retParser.CreateNodePtr(channelNode, "MonthInfo");
				retParser.SetAttrNode("Month", (*RunplanVecIter).Month, cycleNode);
				retParser.SetAttrNode("Day", (*RunplanVecIter).DayOfMonth, cycleNode);
				retParser.SetAttrNode("StartTime", (*RunplanVecIter).StartTime, cycleNode);
				retParser.SetAttrNode("EndTime", (*RunplanVecIter).EndTime, cycleNode);
				retParser.SetAttrNode("Type", (*RunplanVecIter).Type, cycleNode);
				retParser.SetAttrNode("ValidityBegin", (*RunplanVecIter).ValidStartDateTime, cycleNode);
				retParser.SetAttrNode("ValidityEnd", (*RunplanVecIter).ValidEndDateTime, cycleNode);
			}
			else if ( (*RunplanVecIter).DayOfWeek!="" && (*RunplanVecIter).StartTime!="" && (*RunplanVecIter).EndTime!="" )
			{	
				pXMLNODE weekNode = retParser.CreateNodePtr(channelNode, "WeekInfo");
				retParser.SetAttrNode("Week", (*RunplanVecIter).DayOfWeek, weekNode);
				retParser.SetAttrNode("StartTime", (*RunplanVecIter).StartTime, weekNode);
				retParser.SetAttrNode("EndTime", (*RunplanVecIter).EndTime, weekNode);
				retParser.SetAttrNode("Type", (*RunplanVecIter).Type, weekNode);
				retParser.SetAttrNode("ValidityBegin", (*RunplanVecIter).ValidStartDateTime, weekNode);
				retParser.SetAttrNode("ValidityEnd", (*RunplanVecIter).ValidEndDateTime, weekNode);
			}
			else
			{
				pXMLNODE singleNode = retParser.CreateNodePtr(channelNode, "DayInfo");
				retParser.SetAttrNode("StartDateTime", (*RunplanVecIter).StartDateTime, singleNode);
				retParser.SetAttrNode("EndDateTime", (*RunplanVecIter).EndDateTime, singleNode);
				retParser.SetAttrNode("Type", (*RunplanVecIter).Type, singleNode);
			}
		}
	}
	retParser.SaveToString( retXml );
	return retXml;
}
//�����Զ�¼������Ƶ��ѯXML
std::string TranslateUpXMLForDVBC::TranslateAutoRecordStream( const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime )
{
	if(task->GetVersion() == "3.0")
	{
		std::string retXml=GetXmlHeader(task,"AutoRecordQuery");//��׼�ķ���ͷxml
		XmlParser parser;
		parser.Set_xml(retXml);
		pXMLNODE returnInfoNode = parser.CreateNodePtr(parser.GetRootNode(),"ReturnInfo");           //����ReturnInfo�ڵ�
		pXMLNODE autoRecordReportNode=parser.CreateNodePtr(returnInfoNode,"AutoRecordURL"); //����NVRVideoHistoryInquiry�ڵ�
		parser.SetAttrNode("URL",URL,autoRecordReportNode);  //����NVRVideoHistory�ڵ��URL����
		parser.SaveToString(retXml);
		return retXml;
	}
	else
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
}

//�����Զ�¼���Ŀ�ļ�����XML
std::string TranslateUpXMLForDVBC::TranslateAutoRecordFile( const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime,std::vector<sRecordInfo> vecRecLostTime)
{
	if(task->GetVersion() == "3.0")
	{
		std::string retXml=GetXmlHeader(task,"AutoRecordDownload");//��׼�ķ���ͷxml
		XmlParser parser;
		parser.Set_xml(retXml);
		pXMLNODE returnInfoNode = parser.CreateNodePtr(parser.GetRootNode(),"ReturnInfo");           //����ReturnInfo�ڵ�
		pXMLNODE autoRecordDownNode=parser.CreateNodePtr(returnInfoNode,"AutoRecordDownload");   //����NVRVideoHistoryDownInquiry�ڵ�
		pXMLNODE autoRecordReportNode=parser.CreateNodePtr(autoRecordDownNode,"DownloadURL"); //����NVRVideoHistoryInquiry�ڵ�
		parser.SetAttrNode("URL",URL,autoRecordReportNode);  //����NVRVideoHistory�ڵ��URL����
		//
		pXMLNODE getNodechild = parser.CreateNodePtr(autoRecordDownNode, "AutoRecordIntegraity");
		if(vecRecLostTime.size() == 0)
		{
			parser.SetAttrNode("Integraity", string("0"), getNodechild);
		}
		else
		{
			parser.SetAttrNode("Integraity", string("1"), getNodechild);
		}
		parser.SetAttrNode("LostNum", StrUtil::Int2Str(vecRecLostTime.size()), getNodechild);
		parser.SetAttrNode("LostLength", StrUtil::Int2Str(vecRecLostTime.size()), getNodechild);
		int i=0;//***
		std::vector<sRecordInfo>::const_iterator iter = vecRecLostTime.begin();
		long totallosttime = 0;
		while ( iter != vecRecLostTime.end() )
		{
			int losttime = TimeUtil::StrToDateTime((*iter).endtime)-TimeUtil::StrToDateTime((*iter).starttime);
			if(losttime > 5*60)//***
			{//***
				pXMLNODE infoNode = parser.CreateNodePtr(getNodechild, "LostVideoInfo");
				parser.SetAttrNode("LostStartTime", (*iter).starttime, infoNode);
				parser.SetAttrNode("LostEndTime", (*iter).endtime, infoNode);
				i++;//***
				totallosttime+=losttime;
			}//***
			iter ++;
		}
		if(i < 1)//***
		{//***
			parser.SetAttrNode("Integraity", string("0"), getNodechild);//***
		}//***
		else//***
		{//***
			parser.SetAttrNode("Integraity", string("1"), getNodechild);//***
		}//***
		parser.SetAttrNode("LostNum", StrUtil::Int2Str(i)/*vecRecLostTime.size()*/, getNodechild);//***
		parser.SetAttrNode("LostLength", StrUtil::Long2Str(totallosttime), getNodechild);
		//
		parser.SaveToString(retXml);
		return retXml;
	}
	else
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
}

//��������¼������Ƶ��ѯXML
std::string TranslateUpXMLForDVBC::TranslateTaskRecordStream( const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime )
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
std::string TranslateUpXMLForDVBC::TranslateTaskRecordFile( const XMLTask* task,std::string URL,std::string startDateTime,std::string endDateTime)
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
std::string TranslateUpXMLForDVBC::TranslateMHPQuery(const XMLTask *task, std::string ftp, std::string userName, std::string passWord)
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
std::string TranslateUpXMLForDVBC::TranslateQualityCompensationSet(const XMLTask *task)
{
	std::string retXml=GetXmlHeader(task,"IndexCompensationSet");//��׼�ķ���ͷxml  
	return retXml;
}
//����ҵ�����ʱ������XML
std::string TranslateUpXMLForDVBC::TranslateAutoAnalysisTimeSet(const XMLTask* task)
{
	std::string retXml=GetXmlHeader(task,"AutoAnalysisTimeSet");//��׼�ķ���ͷxml
	return retXml;
}
//����ҵ�����ʱ�����ò�ѯXML
std::string TranslateUpXMLForDVBC::TranslateAutoAnalysisTimeQuery(const XMLTask* task)
{
	std::string retXml=GetXmlHeader(task,"AutoAnalysisTimeQuery");//��׼�ķ���ͷxml
	string StartTime,Type,ScanType,DevID;
	DBMANAGER::instance()->GetAutoAnalysisInfo(StartTime,Type,ScanType,DevID);
	StartTime.erase(StartTime.find_last_not_of(" ") + 1);
	Type.erase(Type.find_last_not_of(" ") + 1);
	ScanType.erase(ScanType.find_last_not_of(" ") + 1);
	DevID.erase(DevID.find_last_not_of(" ") + 1);

	XmlParser retParser(retXml.c_str());
	pXMLNODE rootNode = retParser.GetRootNode();
	pXMLNODE returnNode = retParser.CreateNodePtr(rootNode, "ReturnInfo");
	pXMLNODE reportNode = retParser.CreateNodePtr(returnNode, "AutoAnalysisTimeQuery");

	pXMLNODE pAutoAnalysisTimeNode = retParser.CreateNodePtr(reportNode, "AutoAnalysisTime");
	retParser.SetAttrNode("StartTime",StartTime,pAutoAnalysisTimeNode);
	retParser.SetAttrNode("Type",Type,pAutoAnalysisTimeNode);
	retParser.SetAttrNode("ScanType",ScanType,pAutoAnalysisTimeNode);

	pXMLNODE pDemodulateInfoNode = retParser.CreateNodePtr(reportNode, "DemodulateInfo");
	if (DevID.size() <= 0)
		DevID = "-1";
	TunerDev tdev;
	PROPMANAGER::instance()->GetTunerInfo(atoi(DevID.c_str()),task->GetDVBType(),tdev);
	retParser.SetAttrNode("ChassisID",tdev.s_monitorindex,pDemodulateInfoNode);
	retParser.SetAttrNode("Position",tdev.s_cardindex,pDemodulateInfoNode);
	retParser.SetAttrNode("Demodulate",tdev.s_chanindex,pDemodulateInfoNode);
	retParser.SaveToString(retXml);
	return retXml;
}
//����ָ���ѯXML
std::string TranslateUpXMLForDVBC::TranslateEnvironmentQuery(const XMLTask* task,EnvMapInfo& EnvMap,std::vector<sCheckParam>& paramVec)
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
std::string TranslateUpXMLForDVBC::TranslateManualRecordSet( const XMLTask* task )
{
	std::string retXml=GetXmlHeader(task,"ManualRecordQuery");//��׼�ķ���ͷxml
	return retXml;
}
//���췵��XMLͷ��Ϣ
std::string TranslateUpXMLForDVBC::GetXmlHeader(const XMLTask* task ,std::string retType)
{
	char * cheader= "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><Return/></Msg>";

#ifdef ZONG_JU_ZHAO_BIAO
	string  MsgID ;
	DBMANAGER::instance()->GetMsgID(MsgID);
	int msgid=StrUtil::Str2Int(MsgID);
	DBMANAGER::instance()->UpdateMsgID(StrUtil::Int2Str(++msgid));
#endif

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
	case RECORD_DOWN_OUTOFRAND:
		Desc="¼��ʱ�䳬����";
		break;
	case ADRESS_ERROR:
		Desc="�ϴ�����·������";
		break;
	case USERNAME_ERROR:
		Desc="�û��������벻��ȷ";
		break;
	case CHANNELID_DEVICEID_NO:
		Desc="��ĿƵ����ͨ����ƥ��";
		break;
	case NO_RECORD:
		Desc="ʱ�����¼���ļ�";
		break;
	case CHANLID_CHANGE:
		Desc="���ϴ���ǰƵ��¼���ļ�";
		break;
	case TIMEFORMAT_ERROR:
		Desc="�ϴ��������ʱ�����";
		break;
	default:
		Desc="ʧ��";
		break;
	}

	std::string msgType;//��Ϣ����
	if (retType == "GetAgentInfo")
	{
		msgType="MONUp";
	}
	else if (task->GetDVBType()== DVBC)
	{
		msgType="DVBCUp";
	}
	else if (task->GetDVBType()== CTTB)
	{
		msgType="DTMBUp";
	}
	else if (task->GetDVBType()== DVBS)
	{
		msgType="DVBSUp";
	}
	else if (task->GetDVBType()== RADIO)
	{
		msgType="RADIOUp";
	}
	else if (task->GetDVBType()== AM)
	{
		msgType="AMUp";
	}
	else
		return "";

	std::string retXml="";
	XmlParser headParser(cheader);
	pXMLNODE headRootNode = headParser.GetNodeFromPath( "Msg" );//���ڵ�msg

	headParser.SetAttrNode( "Version",task->GetVersion(),headRootNode );//�汾
	string strDateTime = TimeUtil::GetCurDateTime();

#ifdef ZONG_JU_ZHAO_BIAO	//�б�ʱmsgId�����replyId��ͬ
	if (retType=="ProvisionalRecordTaskSet"||retType=="NVRVideoHistoryInquiry"||retType=="NVRTaskRecordInquiry"
		|| retType=="ChannelScanQuery" || retType=="EPGQuery" || retType=="AutoAnalysisTimeQuery"
		|| retType=="ICInfoChannelEncryptQuery" || retType=="NVRTaskRecordDownInquiry" || retType=="NVRVideoHistoryDownInquiry")
	{
		headParser.SetAttrNode( "MsgID",task->GetMsgID(),headRootNode );//��Ϣid����
	}
	else
		headParser.SetAttrNode( "MsgID",MsgID,headRootNode );//��Ϣid����
#else
	string  MsgID = OSFunction::GetXmlMsgID(strDateTime);
	headParser.SetAttrNode( "MsgID",MsgID,headRootNode );//��Ϣid����
#endif

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
std::string TranslateUpXMLForDVBC::TranslateManualRecordReport( const XMLTask *task,const std::string& strStandXML,std::string URL )
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

	if(type=="DVBC")
	{
		type="MonUp";
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

	std::string Index,Time,Path,Lifecycle;
	pXMLNODE recordNode=parser.GetNodeFromPath("Msg/ManualRecordSet/ManualRecord");
	parser.GetAttrNode(recordNode,"DeviceID",Index);
	parser.GetAttrNode(recordNode,"Time",Time);
	parser.GetAttrNode(recordNode,"Path",Path);

#ifdef ZONG_JU_ZHAO_BIAO
	parser.GetAttrNode(recordNode,"Lifecycle",Lifecycle);
#endif

	pXMLNODE node=temParser.CreateNodePtr(temParser.CreateNodePtr("Msg","ReturnInfo"),"ManualRecordQuery");
	pXMLNODE retRecordNode=temParser.CreateNodePtr(node,"ManualRecord");
	temParser.SetAttrNode("Index",Index,retRecordNode);
	temParser.SetAttrNode("Time",Time,retRecordNode);
	temParser.SetAttrNode("Path",Path,retRecordNode);
#ifdef ZONG_JU_ZHAO_BIAO
	temParser.SetAttrNode("Lifecycle",Lifecycle,retRecordNode);
#endif
	temParser.SetAttrNode("URL",URL,retRecordNode);

	std::string retXML;
	temParser.SaveToString(retXML);
	return retXML;
}
std::string TranslateUpXMLForDVBC::TranslateEPGQuery(const XMLTask* task,std::string URL)
{
	std::string retXML = GetXmlHeader(task,"EPGScanQuery");
	XmlParser parser;
	parser.Set_xml(retXML);
	pXMLNODE retNode = parser.GetNodeFromPath("Msg/Return");
	pXMLNODE returnInfoNode = parser.CreateNodePtr(parser.GetRootNode(),"ReturnInfo");   //����ReturnInfo�ڵ� 
	pXMLNODE autoRecordDownNode=parser.CreateNodePtr(returnInfoNode,"ReturnInfo");   
	pXMLNODE autoRecordReportNode=parser.CreateNodePtr(autoRecordDownNode,"EPGGetURL"); 
	parser.SetAttrNode("URL",URL,autoRecordReportNode);  
	std::string rtnXML;
	parser.SaveToString(rtnXML);
	return rtnXML;
}

std::string TranslateUpXMLForDVBC::TranslateFreqAlarmQuery( const XMLTask* task,std::vector<sCheckParam> vecAlarmResult )
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

std::string TranslateUpXMLForDVBC::TranslateProgramAlarmQuery( const XMLTask* task,std::vector<sCheckParam> vecAlarmResult )
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

std::string TranslateUpXMLForDVBC::TranslateEnvAlarmQuery( const XMLTask* task,std::vector<sCheckParam> vecAlarmResult )
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

std::string TranslateUpXMLForDVBC::TranslateOSDSet( const XMLTask* task )
{
	std::string retXml=GetXmlHeader(task,"OSDSet");//��׼�ķ���ͷxml
	return retXml;
}

std::string TranslateUpXMLForDVBC::TranslateStreamRealtimeRate(const XMLTask* task)
{
	std::string retXML = GetXmlHeader(task,"NVRSteamRateSet");
	return retXML;
}
std::string TranslateUpXMLForDVBC::TranslateRecordCapabilityQuery(const XMLTask* task)
{
	std::string retXML = GetXmlHeader(task,"RecordCapabilityQuery");
	XmlParser parser;
	parser.Set_xml(retXML); 
	list<int> DeviceList;
	int num=0;
	//PROPMANAGER::instance()->GetAllDeviceList(DeviceList);	
	int devnum = PROPMANAGER::instance()->GetCoderDevNum();

	for (int devi = 0;devi < devnum;devnum++)
	{
		bool ret = PROPMANAGER::instance()->IsDeviceAvaiable(devi,"AutoRecord",DVBC);
		if(ret)
		{
			num++;
		}
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


std::string TranslateUpXMLForDVBC::TranslateHDDefAudioParamSet(const XMLTask* task)
{
	std::string retXML = GetXmlHeader(task,"HDDefAudioParamSet");
	return retXML;
}
std::string TranslateUpXMLForDVBC::TranslateAudioParamSet(const XMLTask* task)
{
	std::string retXML = GetXmlHeader(task,"AudioParamSet");
	return retXML;
}


std::string TranslateUpXMLForDVBC::TranslateAlarmQuery( const XMLTask* task,std::vector<sCheckParam> vecAlarmResult )
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


std::string TranslateUpXMLForDVBC::TranslateStopClient(const XMLTask* task)
{
	std::string retXml=GetXmlHeader(task,"VideoClientStop");//��׼�ķ���ͷxml
	return retXml;
}



#ifdef ZONG_JU_ZHAO_BIAO
std::string TranslateUpXMLForDVBC::TranslateRecordCapabilityQuery( const XMLTask* task, int newIndexCount, int pessCount, std::string strRecordType, const std::vector<RecChanInfo>& vecRecChanInfo )
{
	std::string retXML = GetXmlHeader(task,"RecordCapabilityQuery");
	XmlParser parser;
	parser.Set_xml(retXML); 

	pXMLNODE returnInfoNode = parser.CreateNodePtr(parser.GetRootNode(),"ReturnInfo");   //����ReturnInfo�ڵ� 
	pXMLNODE QueryNode = parser.CreateNodePtr(returnInfoNode,"RecordCapabilityQuery");
	parser.SetAttrNode("SetRecordType",strRecordType,QueryNode);
	parser.SetAttrNode("NewIndexCount",StrUtil::Int2Str(newIndexCount),QueryNode);
	parser.SetAttrNode("PessCount",StrUtil::Int2Str(pessCount),QueryNode);

	std::vector<RecChanInfo>::const_iterator recChanIter = vecRecChanInfo.begin();
	for (; recChanIter!=vecRecChanInfo.end(); recChanIter++)
	{
		pXMLNODE chanInfoNode = parser.CreateNodePtr(QueryNode, "ChannelInfo");
		parser.SetAttrNode("Freq", (*recChanIter).freq, chanInfoNode);
		parser.SetAttrNode("ServiceID", (*recChanIter).serviceId, chanInfoNode);
		parser.SetAttrNode("ProgramID", (*recChanIter).programId, chanInfoNode);
		if ( recChanIter->canRecord )
		{
			parser.SetAttrNode("IsRecord", string("0"), chanInfoNode);
		}
		else
		{
			parser.SetAttrNode("IsRecord", string("1"), chanInfoNode);

			if (strRecordType == "2")	//�Զ�¼����Ҫ�����Ѿ����õ��Զ�¼��
			{
				continue;
			}

			std::list<int> taskDevList;
			PROPMANAGER::instance()->GetTaskDeviceList("AutoRecord", task->GetDVBType(), taskDevList);
			if (taskDevList.empty())
			{
				ACE_DEBUG ((LM_DEBUG,"(%T| %t) ¼��·����ѯ��ȡ�Զ�¼��ͨ���б�ʧ��!\n"));
			}

			std::list<int> devList;
			std::list<int>::iterator devIter = taskDevList.begin();
			for (; devIter!=taskDevList.end(); devIter++)
			{
				sTaskInfo taskInfo;
				bool ret = BUSINESSLAYOUTMGR::instance()->QueryRunTaskInfo((*devIter), taskInfo);
				if (!ret || taskInfo.taskname!="AutoRecord")	//ֻ�����Զ�¼��Ƶ��(��Ȩ�������Ҳ����ռ��ͨ��)
				{
					continue;
				}
				
				int iCardIndex=0;
				std::string strChanId = taskInfo.channelid;
				std::string strServId, strFreq, strProgramId, strPcrId;
				CHANNELINFOMGR::instance()->GetServIdByChanId(task->GetDVBType(), strServId, strChanId);
				CHANNELINFOMGR::instance()->GetFreqByChanId(task->GetDVBType(), strFreq, strChanId);
				CHANNELINFOMGR::instance()->GetPmtPidPcrPid(task->GetDVBType(), strFreq, strServId, strProgramId, strPcrId);
				pXMLNODE videoChanInfoNode = parser.CreateNodePtr(chanInfoNode, "VideoChannelInfo");
				
				PROPMANAGER::instance()->GetDeviceCardIndex(taskInfo.deviceid,iCardIndex);
				//
				parser.SetAttrNode("Index", iCardIndex, videoChanInfoNode); //FRee_fanrong_20131218 ¼���ѯ��ʱ�򣬰忨�ź�ͨ���ŵ�ͳһ�����⣿
				//
				parser.SetAttrNode("Freq", strFreq, videoChanInfoNode);
				parser.SetAttrNode("ServiceID", strServId, videoChanInfoNode);
				parser.SetAttrNode("ProgramID", strProgramId, videoChanInfoNode);
			}
		}
	}

	std::string rtnXML;
	parser.SaveToString(rtnXML);
	return rtnXML;
}

std::string TranslateUpXMLForDVBC::TranslateStopPlayingVideo( const XMLTask* task )
{
	std::string retXML = GetXmlHeader(task,"StopPlayingVideo");
	return retXML;
}

std::string TranslateUpXMLForDVBC::TranslateICInfoQuery( const XMLTask* task,std::vector<CACardInfo> cardinfo )
{
	std::string retXML = GetXmlHeader(task,"ICInfoQuery");
	XmlParser parser;
	parser.Set_xml(retXML);
	pXMLNODE returnInfoNode = parser.CreateNodePtr(parser.GetRootNode(),"ReturnInfo");   //����ReturnInfo�ڵ� 
	pXMLNODE QueryNode= parser.CreateNodePtr(returnInfoNode,"ICInfoQuery");
	int nCardCount = cardinfo.size();
	parser.SetAttrNode("Total", nCardCount, QueryNode);
	for(int i=0; i<nCardCount; i++)
	{
		pXMLNODE InfoNde=parser.CreateNodePtr(QueryNode,"ICInfo");
		parser.SetAttrNode("CardNO",cardinfo[i].cardno,InfoNde);
		parser.SetAttrNode("Position",i,InfoNde);
		parser.SetAttrNode("Desc",cardinfo[i].desc,InfoNde);
	}
	std::string rtnXML;
	parser.SaveToString(rtnXML);
	return rtnXML;
}

//�忨(ͨ��)״̬�鿴��������
std::string TranslateUpXMLForDVBC::TranslateNvrStatusQuery(const XMLTask* task, const std::map<int,sTaskInfo>& mapTaskInfo)
{
	std::string retXML = GetXmlHeader(task,"GetNvrStatus");

	XmlParser parser( retXML.c_str() );
	pXMLNODE rootNode = parser.GetRootNode();
	pXMLNODE rtnInfoNode = parser.CreateNodePtr(rootNode,"ReturnInfo");
	pXMLNODE totalNode = parser.CreateNodePtr(rtnInfoNode, "GetNvrIndexTotal");


	//int nTotal = mapTaskInfo.size();//FRee_fanrong_20131212
	int nTotal = PROPMANAGER::instance()->GetTunerDevNum();  //FRee_fanrong_20131212
	parser.SetAttrNode("Total", nTotal, totalNode);

	std::map<int,sTaskInfo>::const_iterator taskIter = mapTaskInfo.begin();

	int nCardIndexmark = 0;  //FRee_fanrong_20131212

	pXMLNODE statusNode = NULL;  //FRee_fanrong_20131212
	pXMLNODE oldTaskNode = NULL;  //FRee_fanrong_20131212

	std::string strOldDesc1 = "ʵʱ��Ƶ "; //FRee_fanrong_20131212
	std::string strOldDesc4 = "�Զ�¼�� "; //FRee_fanrong_20131212
	std::string strOldDesc5 = "����¼�� "; //FRee_fanrong_20131212
	map<eDVBType, map<int, vector<int>>>mapCardIndex2TcoderIndex;//<DVBType,<�������ͨ��, ����ת���ͨ��>>
	for(int n = 0; n < 11; n++)
	{
		int iTunerNum = PROPMANAGER::instance()->GetTunerDevNumByDVBType((eDVBType)n);
		if (iTunerNum <= 0)
			continue;
		map<int, vector<int>> mapCardIndex;
		for(int j = 0; j < iTunerNum; j++)
		{
			vector<int> vec;
			mapCardIndex.insert(make_pair(j, vec));
		}
		mapCardIndex2TcoderIndex.insert(make_pair((eDVBType)n, mapCardIndex));
	}

	while (taskIter != mapTaskInfo.end())
	{
		int nIndex = taskIter->first;	//ͨ����
		sTaskInfo taskInfo = (*taskIter).second;
		if ( taskInfo.taskname == "NoTask")	//û�����񣬾Ͳ�����д����������Ϣ
		{
			taskIter ++;
			continue;
		}
		int nCurCardIndex=0; //FRee_fanrong_20131212

		eDVBType dvbtype = DVBC;
		vector<int> vecTcoderIndex;
		PROPMANAGER::instance()->GetTunerDevFromCoderDev(nCurCardIndex,dvbtype,nIndex);
		map<int, vector<int>>CIndex2TIndex;//�������ͨ�� ��Ӧ ����ת���ͨ��
		map<eDVBType, map<int, vector<int>>>::iterator ite = mapCardIndex2TcoderIndex.find(dvbtype);
		if (ite != mapCardIndex2TcoderIndex.end())
		{
			CIndex2TIndex = (*ite).second;
			mapCardIndex2TcoderIndex.erase(ite);
			map<int, vector<int>>::iterator card_iter = CIndex2TIndex.find(nCurCardIndex);
			if (card_iter != CIndex2TIndex.end())
			{
				vecTcoderIndex = (*card_iter).second;	
				CIndex2TIndex.erase(card_iter);
			}
		}
		vecTcoderIndex.push_back(nIndex);
		CIndex2TIndex.insert(make_pair(nCurCardIndex, vecTcoderIndex));
		mapCardIndex2TcoderIndex.insert(make_pair(dvbtype, CIndex2TIndex));
		taskIter ++;
	}
	map<int, vector<int>> CardIndex2TcoderIndex ;
	int iTotalCardIndex = 0;
	for(int i = 0; i < 11; i++)
	{
		map<eDVBType, map<int, vector<int>>>::iterator ite = mapCardIndex2TcoderIndex.find((eDVBType)i);
		if (ite == mapCardIndex2TcoderIndex.end())
			continue;
		map<int, vector<int>> mapCardIndex = (*ite).second;
		map<int, vector<int>>::iterator pIter = mapCardIndex.begin();
		for(pIter; pIter != mapCardIndex.end(); pIter++)
		{
			vector<int> vecTcoderIndex = (*pIter).second;
			int iCardIndex = (*pIter).first;
			std::map<int, vector<int>>::iterator Iter = CardIndex2TcoderIndex.find(iCardIndex);
			if (Iter == CardIndex2TcoderIndex.end())
			{
				CardIndex2TcoderIndex.insert(make_pair(iCardIndex, vecTcoderIndex));
			}
			else
			{
				iTotalCardIndex = CardIndex2TcoderIndex.size();
				CardIndex2TcoderIndex.insert(make_pair(iTotalCardIndex, vecTcoderIndex));
			}
		}
	}
	map<int, vector<int>>::iterator cardIter = CardIndex2TcoderIndex.begin();
	int nCardIndex=0; 
	while(cardIter != CardIndex2TcoderIndex.end())
	{
		vector<int> vecTcoderIndex = (*cardIter).second;
		nCardIndex = (*cardIter).first;
		statusNode = parser.CreateNodePtr(totalNode, "GetNvrIndexStatus");
		parser.SetAttrNode("TaskState", std::string("1"), statusNode);
		parser.SetAttrNode("Index", nCardIndex, statusNode);
		parser.SetAttrNode("Status", std::string("0"), statusNode);
		oldTaskNode = parser.CreateNodePtr(statusNode, "OldTask");
		for(int i = 0; i < vecTcoderIndex.size(); i++)
		{
			int nIndex = vecTcoderIndex[i];
			std::map<int,sTaskInfo>::const_iterator Iter = mapTaskInfo.find(nIndex);
			if (Iter == mapTaskInfo.end())
				continue;
			sTaskInfo taskInfo = (*Iter).second;
	
			if (taskInfo.taskname == "AutoRecord")	//�Զ�¼��
			{
				std::string strChanId = taskInfo.channelid;
				std::string strProgName, strProgramId, strServId;
				CHANNELINFOMGR::instance()->GetProgNameByChanId(taskInfo.dvbtype, strChanId, strProgName);
				CHANNELINFOMGR::instance()->GetProgramIdByChanId(taskInfo.dvbtype, strChanId, strProgramId);
				CHANNELINFOMGR::instance()->GetServIdByChanId(taskInfo.dvbtype, strServId, strChanId);
				parser.SetAttrNode("Index", nCardIndex, oldTaskNode);
				parser.SetAttrNode("IndexType", std::string("4"), oldTaskNode);
				if (taskInfo.freq == strHDMIFreq)
				{
					strProgName = "������";
				}
				if (i == 0)  
				{	
					strOldDesc4 = "�Զ�¼�� ";
				}
				strOldDesc4 += (","+strProgName);
				
				parser.SetAttrNode("OldDesc", strOldDesc4, oldTaskNode);

				pXMLNODE videoNode = parser.CreateNodePtr(oldTaskNode, "AutoVideo");
				parser.SetAttrNode("Index", nCardIndex, videoNode);
				parser.SetAttrNode("VideoStatus", std::string("0"), videoNode);
				parser.SetAttrNode("Program", strProgName, videoNode);
				parser.SetAttrNode("ProgramID", strProgramId, videoNode);
				parser.SetAttrNode("Freq", taskInfo.freq, videoNode);
				parser.SetAttrNode("ServiceID", strServId, videoNode);
			}
			else if (taskInfo.taskname == "TaskRecord")
			{
				std::string strChanId = taskInfo.channelid;
				std::string strProgName, strProgramId, strServId;
				CHANNELINFOMGR::instance()->GetProgNameByChanId(taskInfo.dvbtype, strChanId, strProgName);
				parser.SetAttrNode("Index", nIndex, oldTaskNode);
				parser.SetAttrNode("IndexType", std::string("5"), oldTaskNode);
				if (taskInfo.freq == strHDMIFreq)
				{
					strProgName = "������";
				}
				if (i == 0)  
				{
					strOldDesc5 = "����¼�� ";
				}
				strOldDesc5 += (","+strProgName);
				parser.SetAttrNode("OldDesc", strOldDesc5, oldTaskNode);

				pXMLNODE videoNode = parser.CreateNodePtr(oldTaskNode, "AutoVideo");
				parser.SetAttrNode("Index", nCardIndex, videoNode);
				parser.SetAttrNode("VideoStatus", std::string("0"), videoNode);
				parser.SetAttrNode("TaskID", taskInfo.taskid, videoNode);
			}
			else if (taskInfo.taskname == "StreamRealtimeQueryTask")
			{
				std::string strChanId = taskInfo.channelid;
				std::string strProgName, strProgramId, strServId;
				CHANNELINFOMGR::instance()->GetProgNameByChanId(taskInfo.dvbtype, strChanId, strProgName);
				parser.SetAttrNode("Index", nCardIndex, oldTaskNode);
				parser.SetAttrNode("IndexType", std::string("1"), oldTaskNode);
				if (taskInfo.freq == strHDMIFreq)
				{
					strProgName = "������";
				}
				if (i == 0)  
				{
					strOldDesc1 = "ʵʱ��Ƶ ,";
				}
				strOldDesc1 += strProgName;
				
				parser.SetAttrNode("OldDesc", strOldDesc1, oldTaskNode);
			}
		}

		cardIter++;
	}
	parser.SaveToString( retXML );
	return retXML;
}

std::string TranslateUpXMLForDVBC::TranslateNvrStatusSet( const XMLTask* task )
{
	std::string retXML = GetXmlHeader(task,"NvrStatusSet");
	return retXML;
}
std::string TranslateUpXMLForDVBC::TranslateAgentInfoSet( const XMLTask* task )
{
	std::string retXML = GetXmlHeader(task,"AgentInfoSet");
	return retXML;
}

std::string TranslateUpXMLForDVBC::TranslateAgentInfoQuery( const XMLTask* task,const std::vector<sAgentSetInfo> &VecAgentSetInfo)
{
	std::string retXml=GetXmlHeader(task,"AgentInfoQuery");//��׼�ķ���ͷxml
	
	XmlParser retParser(retXml.c_str());
	pXMLNODE rootNode = retParser.GetRootNode();
	pXMLNODE returnNode = retParser.CreateNodePtr(rootNode, "ReturnInfo");
	pXMLNODE reportNode = retParser.CreateNodePtr(returnNode, "AgentInfoQuery");
	
	for(int i = 0; i < VecAgentSetInfo.size(); i++)
	{
		pXMLNODE pInfoNode = retParser.CreateNodePtr(reportNode, "Info");
		retParser.SetAttrNode("Type",VecAgentSetInfo[i].s_strType,pInfoNode);
		retParser.SetAttrNode("Desc",VecAgentSetInfo[i].s_strDesc,pInfoNode);
		retParser.SetAttrNode("Value",VecAgentSetInfo[i].s_strValue,pInfoNode);
	}

	retParser.SaveToString(retXml);
	return retXml;
}
std::string TranslateUpXMLForDVBC::TranslateRebootSet( const XMLTask* task, const std::string& strStandXML )
{
	if(task->GetVersion()=="3.0")
	{
		std::string retXML = GetXmlHeader(task,"RebootSet");
		XmlParser temParser(strStandXML.c_str());
		pXMLNODE setNode = temParser.GetNodeFromPath("Msg/RebootSet");
		std::string strType, strRebootTime;
		temParser.GetAttrNode(setNode, "Type", strType);
		XmlParser parser( retXML.c_str() );
		pXMLNODE rootNode = parser.GetRootNode();
		pXMLNODE rtnInfoNode = parser.CreateNodePtr(rootNode, "ReturnInfo");
		pXMLNODE getNode = parser.CreateNodePtr(rtnInfoNode, "Reboot");
		parser.SetAttrNode("ReStartTime",TimeUtil::GetCurDateTime(), getNode);
		parser.SetAttrNode("Type", strType, getNode);
		parser.SaveToString( retXML );
		return retXML;
	}
	else
	{
		std::string retXML = GetXmlHeader(task,"RebootSet");
		return retXML;
	}
}
std::string TranslateUpXMLForDVBC::TranslateRebootSetToOther( const XMLTask* task, const std::string& strStandXML )
{
	if(task->GetVersion()=="3.0")
	{
		char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
		XmlParser parser(strStandXML.c_str());
		XmlParser temParser( source );
		//��ȡ��Ԫ��
		pXMLNODE orgRootNode=parser.GetRootNode();
		pXMLNODE stdRootNode=temParser.GetRootNode();
		string version,msgid,downtype,datetime,srccode,dstcode,priority;
		string tasktype="RebootSet";
		parser.GetAttrNode(orgRootNode,"Version",version);
		parser.GetAttrNode(orgRootNode,"MsgID",msgid);
		parser.GetAttrNode(orgRootNode,"DateTime",datetime);
		parser.GetAttrNode(orgRootNode,"SrcCode",srccode);
		parser.GetAttrNode(orgRootNode,"DstCode",dstcode);
		parser.GetAttrNode(orgRootNode,"Priority",priority);

		temParser.SetAttrNode("Version",version,stdRootNode);
		temParser.SetAttrNode("MsgID",msgid,stdRootNode);
		temParser.SetAttrNode("DVBType",string("MonDown"),stdRootNode);
		temParser.SetAttrNode("DateTime",datetime,stdRootNode);
		temParser.SetAttrNode("SrcCode",srccode,stdRootNode);
		temParser.SetAttrNode("DstCode",dstcode,stdRootNode);
		temParser.SetAttrNode("SrcURL",string(""),stdRootNode);
		temParser.SetAttrNode("Priority",priority,stdRootNode);

		std::string retXML;
		pXMLNODE rtnInfoNode = temParser.CreateNodePtr(stdRootNode,"RebootSet");
		temParser.SetAttrNode("Type",string("0"), rtnInfoNode);
		temParser.SaveToString( retXML );
		return retXML;
	}
	else
	{
		std::string retXML = GetXmlHeader(task,"RebootSet");
		return retXML;
	}
}
std::string TranslateUpXMLForDVBC::TranslateRecVideoTime( const XMLTask* task, const VIDEOTIMEVEC& vecRecVideoTime )
{
	if(task->GetVersion()=="3.0")
	{
		std::string retXML = GetXmlHeader(task,"AutoRecordExpiryDate");
		XmlParser parser( retXML.c_str() );
		pXMLNODE rootNode = parser.GetRootNode();
		pXMLNODE rtnInfoNode = parser.CreateNodePtr(rootNode, "ReturnInfo");
		pXMLNODE getNode = parser.CreateNodePtr(rtnInfoNode, "AutoRecordExpiryDate");
		VIDEOTIMEVEC::const_iterator iter = vecRecVideoTime.begin();
		while ( iter != vecRecVideoTime.end() )
		{
			pXMLNODE infoNode = parser.CreateNodePtr(getNode, "ExpiryDateInfo");
			parser.SetAttrNode("StartDateTime", (*iter).strStartTime, infoNode);
			parser.SetAttrNode("EndDateTime", (*iter).strEndTime, infoNode);
			iter ++;
		}
		parser.SaveToString( retXML );
		return retXML;
	}
	else
	{
		std::string retXML = GetXmlHeader(task,"GetRecordVideoTime");
		XmlParser parser( retXML.c_str() );
		pXMLNODE rootNode = parser.GetRootNode();
		pXMLNODE rtnInfoNode = parser.CreateNodePtr(rootNode, "ReturnInfo");
		pXMLNODE getNode = parser.CreateNodePtr(rtnInfoNode, "GetRecordVideoTime");
		VIDEOTIMEVEC::const_iterator iter = vecRecVideoTime.begin();
		while ( iter != vecRecVideoTime.end() )
		{
			pXMLNODE infoNode = parser.CreateNodePtr(getNode, "RecordeVideoInfo");
			parser.SetAttrNode("ProgramID", (*iter).strProgramId, infoNode);
			parser.SetAttrNode("Freq", (*iter).strFreq, infoNode);
			parser.SetAttrNode("ServiceID", (*iter).strServiceId, infoNode);
			parser.SetAttrNode("StartDateTime", (*iter).strStartTime, infoNode);
			parser.SetAttrNode("EndDateTime", (*iter).strEndTime, infoNode);
			iter ++;
		}
		parser.SaveToString( retXML );
		return retXML;
	}
}
std::string TranslateUpXMLForDVBC::TranslateRecordFileLost(const XMLTask* task, const std::vector<sRecordInfo>& vecRecLostTime)
{
	if(task->GetVersion()=="3.0")
	{
		std::string retXML = GetXmlHeader(task,"AutoRecordIntegraity");
		XmlParser parser( retXML.c_str() );
		pXMLNODE rootNode = parser.GetRootNode();
		pXMLNODE rtnInfoNode = parser.CreateNodePtr(rootNode, "ReturnInfo");
		pXMLNODE getNodechild = parser.CreateNodePtr(rtnInfoNode, "IntegraityInfo");
		if(vecRecLostTime.size() == 0)
		{
			parser.SetAttrNode("Integraity", string("0"), getNodechild);
		}
		else
		{
			parser.SetAttrNode("Integraity", string("1"), getNodechild);
		}
		parser.SetAttrNode("LostNum", StrUtil::Int2Str(vecRecLostTime.size()), getNodechild);
		parser.SetAttrNode("LostLength", StrUtil::Int2Str(vecRecLostTime.size()), getNodechild);
		int i=0;//***
		std::vector<sRecordInfo>::const_iterator iter = vecRecLostTime.begin();
		long totallosttime = 0;
		while ( iter != vecRecLostTime.end() )
		{
			int losttime = TimeUtil::StrToDateTime((*iter).endtime)-TimeUtil::StrToDateTime((*iter).starttime);
			if(losttime > 5*60)//***
			{//***
				pXMLNODE infoNode = parser.CreateNodePtr(getNodechild, "LostVideoInfo");
				parser.SetAttrNode("LostStartTime", (*iter).starttime, infoNode);
				parser.SetAttrNode("LostEndTime", (*iter).endtime, infoNode);
				i++;//***
				totallosttime+=losttime;
			}//***
			iter ++;
		}
		parser.SetAttrNode("LostNum", StrUtil::Int2Str(i), getNodechild);//***
		parser.SetAttrNode("LostLength", StrUtil::Long2Str(totallosttime), getNodechild);
		parser.SaveToString( retXML );
		return retXML;
	}
	else
	{
		std::string retXML = GetXmlHeader(task,"NVRVideoHistoryLostInquiry");
		XmlParser parser( retXML.c_str() );
		pXMLNODE rootNode = parser.GetRootNode();
		pXMLNODE rtnInfoNode = parser.CreateNodePtr(rootNode, "ReturnInfo");
		pXMLNODE getNode = parser.CreateNodePtr(rtnInfoNode, "NVRVideoHistoryLostInquiry");
		pXMLNODE getNodechild = parser.CreateNodePtr(getNode, "NVRVideoHistoryLost");
		parser.SetAttrNode("LostNum", StrUtil::Int2Str(vecRecLostTime.size()), getNodechild);
		int i=0;//***
		std::vector<sRecordInfo>::const_iterator iter = vecRecLostTime.begin();
		while ( iter != vecRecLostTime.end() )
		{
			if(TimeUtil::StrToDateTime((*iter).endtime)-TimeUtil::StrToDateTime((*iter).starttime) > 5*60)//***
			{//***
				pXMLNODE infoNode = parser.CreateNodePtr(getNodechild, "LostVideoInfo");
				parser.SetAttrNode("ProgramID", task->GetTaskServiceID(), infoNode);
				parser.SetAttrNode("Freq", task->GetTaskFreq(), infoNode);
				parser.SetAttrNode("ServiceID", task->GetTaskServiceID(), infoNode);
				parser.SetAttrNode("LostStartTime", (*iter).starttime, infoNode);
				parser.SetAttrNode("LostEndTime", (*iter).endtime, infoNode);
				i++;//***
			}//***
			iter ++;
		}
		parser.SetAttrNode("LostNum", StrUtil::Int2Str(i), getNodechild);//***
		parser.SaveToString( retXML );
		return retXML;
	}
}

std::string TranslateUpXMLForDVBC::TranslateChanEncryptQuery(const XMLTask* task )
{
	std::string retXML = GetXmlHeader(task,"ICInfoChannelEncryptQuery");
	if(task->GetVersion() == "3.0")
	{
		retXML = GetXmlHeader(task,"ICCardEncryptQuery");
	}
	return retXML;
}

std::string TranslateUpXMLForDVBC::TranslateEncryptResult(const XMLTask* task)
{
	std::string retXML;
	std::string DstCode;
	if(task->GetVersion() == "3.0")
	{
		std::string UpStreamRoundUrl;
		UpStreamRoundUrl = PROPMANAGER::instance()->GetAgentInfoSet("14");
		if(UpStreamRoundUrl.size()==0)
		{
			PROPMANAGER::instance()->GetUpStreamRoundUrl(task->GetDVBType(),UpStreamRoundUrl);//ʹ���ֲ���ַ�ϱ�
		}
		//
		const char * cheader= "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
		XmlParser parser(cheader);
		pXMLNODE headRootNode = parser.GetNodeFromPath( "Msg" );//���ڵ�msg
		parser.SetAttrNode( "Version",task->GetVersion(),headRootNode );//�汾
		parser.SetAttrNode( "MsgID",task->GetMsgID(),headRootNode );//��Ϣid����
		parser.SetAttrNode( "Type",string("MonUp"),headRootNode );//��Ϣ����
		parser.SetAttrNode( "DateTime",TimeUtil::GetCurDateTime(),headRootNode );//��ǰʱ��
		parser.SetAttrNode( "SrcCode",task->GetDstCode(),headRootNode );//������ʶ����ͨ���ӿڻ��
		parser.SetAttrNode( "DstCode",task->GetSrcCode(),headRootNode );//Ŀ�������ʶ
		parser.SetAttrNode( "DstURL",UpStreamRoundUrl,headRootNode);//Ŀ��URL
		parser.SetAttrNode( "ReplyID",task->GetMsgID(),headRootNode );//�ظ�����Ϣid	
		//
		pXMLNODE ICCardEncrqueryNode = parser.CreateNodePtr(headRootNode, "ICCardEncryptQueryResult");
		STRCHANINFOMAP mapEncryptInfo;
		CHANENCRYPTCHECKMGR::instance()->GetEncryptInfo( mapEncryptInfo );

		STRCHANINFOMAP::iterator mapIter = mapEncryptInfo.begin();
		int inum=0;
		for (; mapIter!=mapEncryptInfo.end(); mapIter++)
		{
			std::string strTempCardNum = mapIter->first;
			CACardInfo cardinfo;
			PARAMSETMANAGER::instance()->GetCACardInfoByCardNo(strTempCardNum, cardinfo);
			pXMLNODE infoNode = parser.CreateNodePtr(ICCardEncrqueryNode, "ICCardInfo");
			parser.SetAttrNode("ICCardNO", cardinfo.cardno, infoNode);
			//
			std::vector<sChannelInfo>::iterator chanIter = (*mapIter).second.begin();
			for (; chanIter!=(*mapIter).second.end(); chanIter++)
			{
				pXMLNODE chanEncryptNode = parser.CreateNodePtr(infoNode, "Encrypt");

				parser.SetAttrNode("ServiceName", chanIter->ProgramName, chanEncryptNode);
				parser.SetAttrNode("Freq", chanIter->Freq, chanEncryptNode);
				parser.SetAttrNode("ServiceID", chanIter->ServiceID, chanEncryptNode);
				parser.SetAttrNode("Encrypt", chanIter->FreeCA, chanEncryptNode);
				parser.SetAttrNode("ExpiryDate", string(""), chanEncryptNode);
				if((chanIter->VideoPID=="0")||(chanIter->VideoPID.size() < 0)||(chanIter->VideoPID=="5000"))
				{
					parser.SetAttrNode("Encrypt", std::string("1"), chanEncryptNode);
				}
				else
				{
					//
					if(g_sEncryptInfo.size()>0)
					{
						for(int i=0;i<g_sEncryptInfo.size();i++)
						{
							if((g_sEncryptInfo[i].freq == chanIter->Freq)/*&&(g_sEncryptInfo[i].programid == chanIter->ServiceID)*///����3.0�ӿ�ʹ�� 2017-11-21
								&&(g_sEncryptInfo[i].serviceid == chanIter->ServiceID))
							{
								if(g_sEncryptInfo[i].Encrypt == "1")//С����ѯ��Ƶ��ɨ����෴
								{
									parser.SetAttrNode("Encrypt", std::string("0"), chanEncryptNode);
								}
								else
								{
									parser.SetAttrNode("Encrypt", std::string("1"), chanEncryptNode);
								}
								break;
							}
						}
					}
				}
			}
		}
		parser.SaveToString( retXML );
	}
	else
	{
		retXML = GetXmlHeader(task,"ICInfoChannelEncryptQuery");
		XmlParser parser( retXML.c_str() );
		pXMLNODE rootNode = parser.GetRootNode();
		parser.GetAttrNode(rootNode,"DstCode",DstCode);
		std::string UpStreamRoundUrl;
		PROPMANAGER::instance()->GetUpStreamRoundUrl(task->GetDVBType(),UpStreamRoundUrl);//ʹ���ֲ���ַ�ϱ�
		parser.SetAttrNode("DstURL",UpStreamRoundUrl, rootNode); //FRee_fanrong_20140103
		pXMLNODE rtnNode = parser.GetNodeFromPath("Msg/Return");
		pXMLNODE rtnInfoNode = parser.CreateNodePtr(rootNode, "ReturnInfo");
		pXMLNODE queryNode = parser.CreateNodePtr(rtnInfoNode, "ICInfoChannelEncryptQuery");
		//
		STRCHANINFOMAP mapEncryptInfo;
		CHANENCRYPTCHECKMGR::instance()->GetEncryptInfo( mapEncryptInfo );

		STRCHANINFOMAP::iterator mapIter = mapEncryptInfo.begin();
		int inum=0;
		for (; mapIter!=mapEncryptInfo.end(); mapIter++)
		{
			std::string strTempCardNum = mapIter->first;
			CACardInfo cardinfo;
			PARAMSETMANAGER::instance()->GetCACardInfoByCardNo(strTempCardNum, cardinfo);
			pXMLNODE infoNode = parser.CreateNodePtr(queryNode, "ICInfo");
			parser.SetAttrNode("CardNO", cardinfo.cardno, infoNode);
			string pos = cardinfo.position;
			while(true)
			{
				int pt = pos.find("_");
				if(pt != -1)
				{
					pos.replace(pt,1,"0");
				}
				else
				{
					break;
				}
			}
			parser.SetAttrNode("Position", inum, infoNode); //2.5�ӿ�����С��λ������PositionȡֵӦΪ���֣�����ȡ�忨�š�//FRee_fanrong_20131219
			inum++;
			std::vector<sChannelInfo>::iterator chanIter = (*mapIter).second.begin();
			for (; chanIter!=(*mapIter).second.end(); chanIter++)
			{
				pXMLNODE chanEncryptNode = parser.CreateNodePtr(infoNode, "ChannelEncrypt");

				parser.SetAttrNode("ChannelName", chanIter->ProgramName, chanEncryptNode);
				parser.SetAttrNode("Freq", chanIter->Freq, chanEncryptNode);
				parser.SetAttrNode("ServiceID", chanIter->ServiceID, chanEncryptNode);
				parser.SetAttrNode("ProgramID", chanIter->ServiceID, chanEncryptNode);
				parser.SetAttrNode("Encrypt", chanIter->FreeCA, chanEncryptNode);
				parser.SetAttrNode("DueTime", string(""), chanEncryptNode);
				if((chanIter->VideoPID=="0")||(chanIter->VideoPID.size() < 0)||(chanIter->VideoPID=="5000"))
				{
					parser.SetAttrNode("Encrypt", std::string("1"), chanEncryptNode);
				}
				else
				{
					//
					if(g_sEncryptInfo.size()>0)
					{
						for(int i=0;i<g_sEncryptInfo.size();i++)
						{
							if((g_sEncryptInfo[i].freq == chanIter->Freq)/*&&(g_sEncryptInfo[i].programid == chanIter->ServiceID)*///����3.0�ӿ�ʹ�� 2017-11-21
								&&(g_sEncryptInfo[i].serviceid == chanIter->ServiceID))
							{
								if(g_sEncryptInfo[i].Encrypt == "1")//С����ѯ��Ƶ��ɨ����෴
								{
									parser.SetAttrNode("Encrypt", std::string("0"), chanEncryptNode);
								}
								else
								{
									parser.SetAttrNode("Encrypt", std::string("1"), chanEncryptNode);
								}
								break;
							}
						}
					}
				}
			}
		}
		parser.SaveToString( retXML );
	}

#ifdef ZONG_JU_ZHAO_BIAO    //FRee_fanrong_20131224 
	if(DstCode == "888888G01")       
	{	
		XmlParser EncryptParser( retXML.c_str() );
		EncryptParser.SaveAsFile("C:/vscttb/ICInfoChannelEncryptQuery_LastReport.xml");
	}
#endif
	return retXML;
}


#endif

//���� ����ң���� ����� XML
std::string TranslateUpXMLForDVBC::TranslateInfraredRemoteControlQuery(const XMLTask* task)
{
	std::string retXml=GetXmlHeader(task,"STBVirtualController");//��׼�ķ���ͷxml

	return retXml;
}


std::string TranslateUpXMLForDVBC::TranslateOSDFormatSetQuery(const XMLTask* task)
{
	std::string retXml=GetXmlHeader(task,"OSDFormatSet");//��׼�ķ���ͷxml

	return retXml;
}

std::string TranslateUpXMLForDVBC::TranslateOSDFormatQuery(const XMLTask* task,const std::vector<OSDFormat> &VecOSDFormat)
{
	std::string retXml=GetXmlHeader(task,"OSDFormatQuery");//��׼�ķ���ͷxml
	XmlParser retParser(retXml.c_str());
	pXMLNODE rootNode = retParser.GetRootNode();
	pXMLNODE returnNode = retParser.CreateNodePtr(rootNode, "ReturnInfo");
	pXMLNODE pOSDFormatSetNode;
	string freq;

	for(int i = 0; i < VecOSDFormat.size(); i++)
	{
		if (VecOSDFormat[i].Freq != freq)
		{
			pOSDFormatSetNode = retParser.CreateNodePtr(returnNode, "OSDFormatSet");
			retParser.SetAttrNode("Freq",VecOSDFormat[i].Freq,pOSDFormatSetNode);
			freq = VecOSDFormat[i].Freq;
		}
		pXMLNODE pInfoNode = retParser.CreateNodePtr(pOSDFormatSetNode, "Info");
		retParser.SetAttrNode("ServiceID",VecOSDFormat[i].ServiceID,pInfoNode);
		retParser.SetAttrNode("FontSize",VecOSDFormat[i].FontSize,pInfoNode);
		retParser.SetAttrNode("Position",VecOSDFormat[i].Position,pInfoNode);
		retParser.SetAttrNode("AntiColor",VecOSDFormat[i].AntiColor,pInfoNode);
		retParser.SetAttrNode("Type",VecOSDFormat[i].Type,pInfoNode);
		retParser.SetAttrNode("Align",VecOSDFormat[i].Align,pInfoNode);
	}

	retParser.SaveToString(retXml);
	return retXml;
}
//
std::string TranslateUpXMLForDVBC::TranslateGetAgentInfoQuery(const XMLTask* task,string QueryLevel)
{
	std::string retXml=GetXmlHeader(task,"GetAgentInfo");//��׼�ķ���ͷxml
	XmlParser parser( retXml.c_str() );
	pXMLNODE rootNode = parser.GetRootNode();
	if(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")
	{
		string SrcUrl = PROPMANAGER::instance()->GetAgentInfoSet("18");
		parser.SetAttrNode("DstURL",SrcUrl,rootNode);
	}
	pXMLNODE retinfoNode = parser.CreateNodePtr(rootNode, "ReturnInfo");
	int MonitorNum = PROPMANAGER::instance()->GetMonitorDevNum();
	for(int i=0;i<MonitorNum;i++)
	{
		sDeviceInfo ChassInfo;
		PROPMANAGER::instance()->GetDevMonitorInfo(i,ChassInfo);
		if((QueryLevel=="1")||(QueryLevel=="2")||(QueryLevel=="3")||(QueryLevel=="4"))
		{
			pXMLNODE ChassisNode = parser.CreateNodePtr(retinfoNode, "Chassis");
			parser.SetAttrNode("ChassisID", ChassInfo.s_deviceindex, ChassisNode);
			parser.SetAttrNode("Manufacturer", string("��ʯ"), ChassisNode);
			parser.SetAttrNode("Status", string("2"), ChassisNode);
			//
			int curTunerPos = -1;
			int curCodererPos = -1;
			int curCodererModPos = -1;
			pXMLNODE cardNode;
			pXMLNODE ModuleNode;
			int TunerNum = PROPMANAGER::instance()->GetTunerDevNum();
			//��¼���ź�ͨ����
			int itunernum[11] = {0,0,0,0,0,0,0,0,0,0,0};//����ת�����
			//
			for(int i=0;i<TunerNum;i++)
			{
				TunerDev tunInfo;
				PROPMANAGER::instance()->GetTunerInfo(i,UNKNOWN,tunInfo);
				if(tunInfo.s_monitorindex == ChassInfo.s_deviceindex)
				{
					if((QueryLevel=="2")||(QueryLevel=="3")||(QueryLevel=="4"))
					{
						if(curTunerPos != StrUtil::Str2Int(tunInfo.s_cardindex))
						{
							curTunerPos = StrUtil::Str2Int(tunInfo.s_cardindex);
							cardNode = parser.CreateNodePtr(ChassisNode, "CardInfo");
							parser.SetAttrNode("Position", tunInfo.s_cardindex, cardNode);
							parser.SetAttrNode("CardType", PROPMANAGER::instance()->GetCardType(tunInfo.e_tunerdvbtype), cardNode);
							parser.SetAttrNode("Manufacturer", string("��ʯ"), cardNode);
							parser.SetAttrNode("SerialNum", tunInfo.s_serialnum, cardNode);
							parser.SetAttrNode("Status", string("2"), cardNode);
						}
						//
						if((QueryLevel=="3")||(QueryLevel=="4"))
						{
							pXMLNODE indexNode = parser.CreateNodePtr(cardNode, "DemodulateInfo");
							parser.SetAttrNode("Demodulate", tunInfo.s_chanindex, indexNode);
							itunernum[tunInfo.e_tunerdvbtype]++;//����ת�����
							parser.SetAttrNode("ICCardNO", tunInfo.s_ICcardNO, indexNode);
							parser.SetAttrNode("MaxDex", ChassInfo.s_devicecampronum, indexNode);
							parser.SetAttrNode("Desc", string(""), indexNode);
							//
							pXMLNODE detailNode = parser.CreateNodePtr(indexNode, "Detail");
							parser.SetAttrNode("Freq", tunInfo.s_freq, detailNode);
							parser.SetAttrNode("ServiceID", tunInfo.s_servidList, detailNode);
							if(tunInfo.s_freq.size()>0)
							{
								parser.SetAttrNode("Desc", tunInfo.s_desc, detailNode);
								parser.SetAttrNode("Status", string("0"), indexNode);
							}
							else
							{
								parser.SetAttrNode("Desc", string(""), detailNode);
								parser.SetAttrNode("Status", string("2"), indexNode);
							}
						}
					}
				}
			}
			//����ת�����********************
			int coderchan[24] = {10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10};
			int dvtotal = 0;
			int enddt = 0;
			for(int dt = 0;dt < 11;dt++)
			{
				if((dt != AM)&&(dt != RADIO)&&(itunernum[dt]>0))
				{
					dvtotal=dvtotal+itunernum[dt];
					enddt = dt;
				}
			}
			if(dvtotal == 0)
			{
				for(int ica = 0;ica < itunernum[AM];ica++)
				{
					coderchan[ica] = 14;
				}
				for(int icf = itunernum[AM];icf < 24;icf++)
				{
					coderchan[icf] = 13;
				}
			}
			else
			{
				for(int ica = 0;ica < itunernum[AM];ica++)
				{
					coderchan[ica] = 14;
				}
				for(int icf = itunernum[AM];icf < (itunernum[RADIO]+itunernum[AM]);icf++)
				{
					coderchan[icf] = 13;
				}
				int ic = itunernum[RADIO]+itunernum[AM];
				int eindex = (24-(itunernum[RADIO]+itunernum[AM]))/dvtotal;
				for(int ndt = 0;ndt < 11;ndt++)
				{
					if((ndt != RADIO)&&(itunernum[ndt]>0))
					{
						int sum = 0;
						for(;ic < 24;ic++)
						{
							if(ndt == CTTB)
							{
								coderchan[ic] = 11;
							}
							else if(ndt == DVBC)
							{
								coderchan[ic] = 10;
							}
							else if(ndt == DVBS)
							{
								coderchan[ic] = 12;
							}
							sum++;
							if((sum > eindex*itunernum[ndt])&&(enddt != ndt))
							{
								break;
							}
						}
					}
				}
			}
			int cindex = 0;
			//********************************
			int CoderNum = PROPMANAGER::instance()->GetCoderDevNum();
			for(int i=0;i<CoderNum;i++)
			{
				CoderDev codInfo;
				PROPMANAGER::instance()->GetCoderInfo(i,codInfo);
				if(codInfo.s_monitorindex == ChassInfo.s_deviceindex)
				{
					if((QueryLevel=="2")||(QueryLevel=="3")||(QueryLevel=="4"))
					{
						if(curCodererPos != StrUtil::Str2Int(codInfo.s_cardindex))
						{
							curCodererPos = StrUtil::Str2Int(codInfo.s_cardindex);
							cardNode = parser.CreateNodePtr(ChassisNode, "CardInfo");
							parser.SetAttrNode("Position", codInfo.s_cardindex, cardNode);
							parser.SetAttrNode("CardType", string("5"), cardNode);
							parser.SetAttrNode("Manufacturer", string("��ʯ"), cardNode);
							parser.SetAttrNode("SerialNum", codInfo.s_serialnum, cardNode);
							parser.SetAttrNode("SDNeeds", string("0,1,0,0"), cardNode);
							parser.SetAttrNode("HDNeeds", string("0,4,0,0"), cardNode);
							parser.SetAttrNode("Status", string("2"), cardNode);
						}
						//
						if((QueryLevel=="3")||(QueryLevel=="4"))
						{
							if(curCodererModPos != StrUtil::Str2Int(codInfo.s_modindex))
							{
								curCodererModPos = StrUtil::Str2Int(codInfo.s_modindex);
								ModuleNode = parser.CreateNodePtr(cardNode, "Module");
								parser.SetAttrNode("ModuleID", codInfo.s_modindex, ModuleNode);
								parser.SetAttrNode("Status", string("2"), ModuleNode);
							}
							pXMLNODE indexNode = parser.CreateNodePtr(ModuleNode, "TranscodeInfo");
							parser.SetAttrNode("Transcode", codInfo.s_modcoderindex, indexNode);
							parser.SetAttrNode("TranscodeType", StrUtil::Int2Str(coderchan[cindex]), indexNode);
							parser.SetAttrNode("Status", string("2"), indexNode);
							parser.SetAttrNode("Desc", string(""), indexNode);
							//
							pXMLNODE detailNode = parser.CreateNodePtr(indexNode, "Detail");
							parser.SetAttrNode("Freq", codInfo.s_freq, detailNode);
							parser.SetAttrNode("ServiceID", codInfo.s_spro, detailNode);
							string des = "���ڼ�Ⲣת��";
							string ProName;
							if(codInfo.s_freq.size()>0)
							{
								if((coderchan[cindex] == 10)&&(codInfo.s_spro.size()>0))
								{
									CHANNELINFOMGR::instance()->GetProName(DVBC,codInfo.s_freq,codInfo.s_spro,ProName);
									des+="��������";
									des+=ProName;
								}
								else if((coderchan[cindex] == 11)&&(codInfo.s_spro.size()>0))
								{
									CHANNELINFOMGR::instance()->GetProName(CTTB,codInfo.s_freq,codInfo.s_spro,ProName);
									des+="��������";
									des+=ProName;
								}
								else if((coderchan[cindex] == 12)&&(codInfo.s_spro.size()>0))
								{
									CHANNELINFOMGR::instance()->GetProName(DVBS,codInfo.s_freq,codInfo.s_spro,ProName);
									des+="����";
									des+=ProName;
								}
								else if(coderchan[cindex] == 13)
								{
									ProName = codInfo.s_freq;
									des+="��Ƶ";
									des+=ProName;
								}
								else if(coderchan[cindex] == 14)
								{
									ProName = codInfo.s_freq;
									des+="�в�";
									des+=ProName;
								}
							}
							if(ProName.size()>0)
							{
								parser.SetAttrNode("Desc", des, detailNode);
								parser.SetAttrNode("Status", string("0"), indexNode);
							}
							else
							{
								parser.SetAttrNode("Desc", string(""), detailNode);
								parser.SetAttrNode("Status", string("2"), indexNode);
							}
							cindex++;
						}
					}
				}
			}
		}
	}
	//
	parser.SaveToString( retXml );
	return retXml;
}
	
std::string  TranslateUpXMLForDVBC::TranslateCardDevStatusQuery(const XMLTask* task)
{
	std::string retXML = GetXmlHeader(task,"DevStautsQuery");
	XmlParser parser( retXML.c_str() );
	pXMLNODE rootNode = parser.GetRootNode();
	pXMLNODE rtnInfoNode = parser.CreateNodePtr(rootNode, "ReturnInfo");
	if(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")
	{
		sDeviceInfo ChassInfo;
		if(PROPMANAGER::instance()->GetDevMonitorInfo(0,ChassInfo))
		{
			string Temperature = StrUtil::Int2Str(40+rand()%4);
			string Voltage = StrUtil::Int2Str(215+rand()%4);
			string cpu = "2",mem = "40";
			pXMLNODE ChassisNode = parser.CreateNodePtr(rtnInfoNode, "Chassis");
			parser.SetAttrNode("ChassisID",ChassInfo.s_deviceindex, ChassisNode);
			parser.SetAttrNode("Voltage",Voltage, ChassisNode);
			parser.SetAttrNode("BandWidth",StrUtil::Float2Str(PROPMANAGER::instance()->GetChassisBandWidth(StrUtil::Str2Int(ChassInfo.s_deviceindex))), ChassisNode);
			int Chassindex = 0;
			if(StrUtil::Str2Int(ChassInfo.s_deviceindex) < ChassisMaxNum)
			{
				Chassindex = StrUtil::Str2Int(ChassInfo.s_deviceindex);
			}
			for(int slotn = 0;slotn < SlotMaxNum;slotn++)
			{
				pXMLNODE cardNode = parser.CreateNodePtr(ChassisNode, "CardInfo");
				parser.SetAttrNode("Positon", StrUtil::Int2Str(slotn), cardNode);
				if(g_standardBoardCheckInfo[Chassindex][slotn].ExistCard)
				{
					parser.SetAttrNode("Status", string("1"), cardNode);
					parser.SetAttrNode("CPU", cpu, cardNode);
					parser.SetAttrNode("Temperature", StrUtil::Int2Str(g_standardBoardCheckInfo[Chassindex][slotn].BoardInfo[4]), cardNode);
					if(g_standardBoardCheckInfo[Chassindex][slotn].BoardInfo[3] == 2)//�Լ���Ϣ 2-��⿨
					{
						Temperature = StrUtil::Int2Str(g_standardBoardCheckInfo[Chassindex][slotn].BoardInfo[4]);
					}
					parser.SetAttrNode("Memory", mem, cardNode);
				}
				else
				{
					parser.SetAttrNode("Status", string("0"), cardNode);
					parser.SetAttrNode("CPU", string(""), cardNode);
					parser.SetAttrNode("Temperature", string(""), cardNode);
					parser.SetAttrNode("Memory", string("��"), cardNode);
				}
			}
			parser.SetAttrNode("ChassisTemperature",Temperature, ChassisNode);
		}
	}
	else
	{
		int MonitorNum = PROPMANAGER::instance()->GetMonitorDevNum();
		int iCurIndex = 0;
		for(int i=0;i<MonitorNum;i++)
		{
			sDeviceInfo ChassInfo;
			PROPMANAGER::instance()->GetDevMonitorInfo(i,ChassInfo);
			string cpu = "2",mem = "40";
			//
			HTTPDeviceAccess* pDeviceAccess = new HTTPDeviceAccess(StrUtil::Str2Int(ChassInfo.s_deviceindex),ChassInfo.s_deviceip,StrUtil::Str2Int(ChassInfo.s_deviceport));//
			string cmd = "GET /device/info  HTTP/1.1  \r\n\r\n";
			string strRet;
			bool isok = pDeviceAccess->SendTaskMsg(cmd,strRet);
			if(!pDeviceAccess)
			{
				delete pDeviceAccess;
			}
			if(strRet.size() <= 0)
			{
				cout<<"TranslateCardDevStatusQuery����ʧ��:"<<ChassInfo.s_deviceip<<"--"<<ChassInfo.s_deviceport<<endl;
				cout<<cmd<<endl;
			}
			else
			{
				int pos = strRet.find("<html>");
				string strtem = strRet;
				if(pos >= 0)
				{
					strtem = strRet.substr(pos,strRet.size()-pos);
					XmlParser psr;
					psr.Set_xml(strtem);
					pXMLNODE node=psr.GetNodeFromPath("html/body");
					pXMLNODELIST nodeList = psr.GetNodeList(node);
					int count = nodeList->Size();
					pXMLNODE scanNode = NULL;
					for(int k=0;k<count;k++)
					{
						pXMLNODE childNode = psr.GetNextNode(nodeList);
						string name = psr.GetNodeName(childNode);
						if(name == "CPU")
						{
							psr.GetAttrNode(childNode,"num",cpu);
						}
						else if(name == "MEM")
						{
							psr.GetAttrNode(childNode,"num",mem);
						}
					}
				}
				else
				{
					cout<<"TranslateCardDevStatusQueryû��<html>���:"<<ChassInfo.s_deviceip<<"--"<<ChassInfo.s_deviceport<<endl;
				}
			}
			//
			pXMLNODE ChassisNode = parser.CreateNodePtr(rtnInfoNode, "Chassis");
			parser.SetAttrNode("ChassisID",ChassInfo.s_deviceindex, ChassisNode);
			if(iCurIndex < StrUtil::Str2Int(ChassInfo.s_deviceindex))
			{
				iCurIndex = StrUtil::Str2Int(ChassInfo.s_deviceindex);
			}
			parser.SetAttrNode("ChassisType", string("2"), ChassisNode);
			parser.SetAttrNode("CPU", cpu, ChassisNode);
			parser.SetAttrNode("Memory", mem, ChassisNode);
			parser.SetAttrNode("DiskFree", string(""), ChassisNode);
		}
		iCurIndex++;
		pXMLNODE ChassisNode = parser.CreateNodePtr(rtnInfoNode, "Chassis");
		parser.SetAttrNode("ChassisID",StrUtil::Int2Str(iCurIndex), ChassisNode);
		parser.SetAttrNode("ChassisType", string("1"), ChassisNode);
		parser.SetAttrNode("CPU", StrUtil::Long2Str(OS_Environment::CPURealtimeUsage()), ChassisNode);
		parser.SetAttrNode("Memory", StrUtil::Long2Str(OS_Environment::MemoryRealtimeUsage()), ChassisNode);
		parser.SetAttrNode("DiskFree", StrUtil::Long2Str(DISKOPERATION::instance()->GetTotalFreeSpace()), ChassisNode);
	}
	//
	parser.SaveToString( retXML );
	return retXML;
}
std::string  TranslateUpXMLForDVBC::TranslateSetCardStatusTask(const XMLTask* task)
{
	std::string retXML = GetXmlHeader(task,"SetCardStatus");

	return retXML;
}
std::string  TranslateUpXMLForDVBC::TranslateEquipmentHeartBeatQuery(const XMLTask* task)
{
	std::string retXML = GetXmlHeader(task,"EquipmentHeartBeatQuery");
	return retXML;
}
std::string  TranslateUpXMLForDVBC::TranslateCameraQuery(const XMLTask* task,const std::string& strStandXML)
{
	std::string retXML = GetXmlHeader(task,"CameraQuery");
	XmlParser parser( retXML.c_str() );
	pXMLNODE rootNode = parser.GetRootNode();
	pXMLNODE rtnInfoNode = parser.CreateNodePtr(rootNode, "ReturnInfo");
	//
	XmlParser temParser(strStandXML.c_str());
	pXMLNODE cameraNode = temParser.GetNodeFromPath("Msg/CameraQuery");
	pXMLNODELIST cameraList=parser.GetNodeList(cameraNode);
	for(int i=0;i<cameraList->Size();++i)
	{
		string strID;
		pXMLNODE camNode=parser.GetNextNode(cameraList);
		parser.GetAttrNode(camNode,"ID",strID);
		//
		pXMLNODE CameraCNode = parser.CreateNodePtr(rtnInfoNode, "Camera");
		parser.SetAttrNode("ID",strID, CameraCNode);
		parser.SetAttrNode("Desc", string(""), CameraCNode);
		parser.SetAttrNode("StreamUrl", string(""), CameraCNode);
	}
	parser.SaveToString( retXML );
	return retXML;
}
std::string TranslateUpXMLForDVBC::TranslateAutoRecordUploadTask(const XMLTask* task)
{
	std::string retXML = GetXmlHeader(task,"AutoRecordUploadTask");

	return retXML;
}
std::string TranslateUpXMLForDVBC::TranslateLimitedBandwidth(const XMLTask* task)
{
	std::string retXML = GetXmlHeader(task,"LimitedBandwidth");

	return retXML;
}

std::string TranslateUpXMLForDVBC::TranslateControlCardTimeQuery(const XMLTask* task)
{
	std::string retXML = GetXmlHeader(task,"ControlCardTimeQuery");
	XmlParser parser( retXML.c_str() );
	pXMLNODE rootNode = parser.GetRootNode();
	pXMLNODE rtnInfoNode = parser.CreateNodePtr(rootNode,"ReturnInfo");
	pXMLNODE cardTimeNode = parser.CreateNodePtr(rtnInfoNode,"ControlCardTime");
	parser.SetAttrNode("Time",TimeUtil::GetCurDateTime(), cardTimeNode);
	parser.SaveToString( retXML );
	return retXML;
}