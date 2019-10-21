///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����RecordQueryTask.cpp
// �����ߣ�jiangcheng
// ����ʱ�䣺2009-06-22
// �����������Զ�������¼���ѯ������������
///////////////////////////////////////////////////////////////////////////////////////////
#include "RecordQueryTask.h"
#include "TranslateDownXML.h"
#include "ace/Log_Msg.h"
#include "../Foundation/XmlParser.h"
#include "../Foundation/PropManager.h"
#include "TranslateUpXML.h"
#include "../DBAccess/DBManager.h"
#include "../BusinessProcess/BusinessLayoutMgr.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include "../Foundation/TimeUtil.h"
#include "../Foundation/OSFunction.h"
#include "../FileSysAccess/WPLFile.h"
#include "../task/QualityQueryTask.h"
#include <vector>
#include <string>
#include <fstream>
using namespace std;
#include "../Foundation/StrUtil.h"
string g_strQueryRecTaskFlag = "";
RecordQueryTask::RecordQueryTask() : DeviceIndependentTask()
{

}

RecordQueryTask::RecordQueryTask(std::string strXML) : DeviceIndependentTask(strXML)
{
	//����¼�����ͣ��Զ�¼���ļ��鿴������¼���ļ��鿴���Զ�¼������Ƶ����ѯ������¼������Ƶ����ѯ
	XmlParser parser;
	parser.Set_xml(strXML);
	pXMLNODE recordTypeNode=parser.GetNodeFirstChild(parser.GetRootNode());
	std::string strRecordType=parser.GetNodeName(recordTypeNode);//¼������

	if (strRecordType == "AutoRecordQuery")
	{
		RecordType = AUTORECORDQUERY;
	}
	else if (strRecordType == "AutoRecordFileQuery")
	{
		RecordType = AUTORECORDFILEQUERY;
	}
	else if (strRecordType == "TaskRecordQuery")
	{
		RecordType = TASKRECORDQUERY;
	}
	else if (strRecordType == "TaskRecordFileQuery")
	{
		RecordType = TASKRECORDFILEQUERY;
	}
	else if (strRecordType == "RecordFileLostQuery")
	{
		RecordType = RECORDFILELOSTQUERY;
	}
}

RecordQueryTask::~RecordQueryTask()
{

}

void RecordQueryTask::Run()
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]¼���ѯ����ִ�� !\n",DeviceID));
	SetRunning();

	XmlParser parser( strStandardXML.c_str());	
	//
	string strQueryRecTaskFlag = TimeUtil::DateTimeToStr(time(0));
	g_strQueryRecTaskFlag = strQueryRecTaskFlag;//���ں�һ�����ٲ����������ˣ�ǰһ�����ٲ�������û����˳�����
	//
	string RequireFreq,rate ;
	//��ȡ��Ҫ�Ĳ�ѯ����
	pXMLNODE childNode = parser.GetNodeFirstChild(parser.GetRootNode());
	pXMLNODE recordQueryTaskNode = parser.GetNodeFirstChild(childNode);
	parser.GetAttrNode( recordQueryTaskNode,"DeviceID",RequireDeviceID );//DeviceID����ֵ
	parser.GetAttrNode( recordQueryTaskNode,"OrgNetID",RequireOrgNetID );//OrgNetID����ֵ
	parser.GetAttrNode( recordQueryTaskNode,"TsID",RequireTsID );//TsID����ֵ
	parser.GetAttrNode( recordQueryTaskNode,"ServiceID",RequireServiceID );//ServiceID����ֵ
	parser.GetAttrNode( recordQueryTaskNode,"VideoPID",RequireVideoPID );//ServiceID����ֵ
	parser.GetAttrNode( recordQueryTaskNode,"Rate",rate );//ServiceID����ֵ
	if(rate.size()==0)
	{
		rate = "1";
	}
	parser.GetAttrNode( recordQueryTaskNode,"AudioPID",RequireAudioPID );//ServiceID����ֵ
	parser.GetAttrNode( recordQueryTaskNode,"TaskID",TaskID );//Taskid����ֵ
	parser.GetAttrNode(recordQueryTaskNode,"Code",RequireChannelCode);
	parser.GetAttrNode(recordQueryTaskNode,"Freq",RequireFreq);
	parser.GetAttrNode( recordQueryTaskNode,"StartDateTime",StartDateTime );//StartDateTime����ֵ
	parser.GetAttrNode( recordQueryTaskNode,"EndDateTime",EndDateTime );//EndDateTime����ֵ
	parser.GetAttrNode( recordQueryTaskNode, "IndexSet", sQualityIndexSet);
	string strChassisID,strPosition,strModuleID,strChanId;
	if(this->GetVersion() == "3.0")
	{
		parser.GetAttrNode(recordQueryTaskNode,"ChassisID",strChassisID);
		parser.GetAttrNode( recordQueryTaskNode,"Position",strPosition );//StartDateTime����ֵ
		parser.GetAttrNode( recordQueryTaskNode,"ModuleID",strModuleID );//EndDateTime����ֵ
		parser.GetAttrNode( recordQueryTaskNode, "Transcode", strChanId);
	}
   //
	//if (RequireFreq != strHDMIFreq)
	{
		//StartDateTime = TimeUtil::DateTimeToStr(TimeUtil::StrToDateTime(StartDateTime) + 4);//У׼���������µ�����ƫ��
		//EndDateTime = TimeUtil::DateTimeToStr(TimeUtil::StrToDateTime(EndDateTime) + 6);//У׼���������µ�����ƫ��
		StartDateTime = TimeUtil::DateTimeToStr(TimeUtil::StrToDateTime(StartDateTime)+2);//У׼���������µ�����ƫ��
		EndDateTime = TimeUtil::DateTimeToStr(TimeUtil::StrToDateTime(EndDateTime)+2);//У׼���������µ�����ƫ��
	}

	//
	//��Ƶ�鿴��ز���
	PROPMANAGER::instance()->GetStreamProtocol(DVBType,VideoStreamProtocol);
	PROPMANAGER::instance()->GetFileProtocol(DVBType,VideoFileProtocol);
	PROPMANAGER::instance()->GetFileUrlType(DVBType,VideoFileurltype);
	PROPMANAGER::instance()->GetFileOffset(DVBType,FileOffset);

	//�ļ�������ز���
	PROPMANAGER::instance()->GetRecDownProtocol(DVBType,DownFileProtocol);
	PROPMANAGER::instance()->GetRecDownUrlType(DVBType,DownFileurltype);
	PROPMANAGER::instance()->GetRecDownOffset(DVBType,DownOffset);
	//
	strServiceID = RequireServiceID;
	Freq = RequireFreq;
	//
	bool ret=true;
	RetValue=RUN_SUCCESS;

	if (RecordType==AUTORECORDQUERY && StartDateTime.length()<1 && EndDateTime.length()<1)			//�Զ�¼������Ƶ�鿴(ʵʱ)
	{
		if (RequireDeviceID.length()<1)		//ģ��ģ��ʹ��ChannelCode����¼��ʵʱ�鿴
		{

			int tempID = -1;

			//�°��������ֻ�·���freq��û�·�channelCode����ֱ��ͨ��freqȥ��ȡ�Զ�¼��ͨ����
			if (!((DVBType==ATV ||DVBType==RADIO||DVBType==CTV) && RequireFreq!="" && RequireChannelCode.length()<1))
			{
				CHANNELINFOMGR::instance()->GetFreqByChannelCode(DVBType,RequireChannelCode,RequireFreq);
			}
							
			BUSINESSLAYOUTMGR::instance()->GetAutoRecordDeviceIdByFreq(DVBType,RequireFreq,tempID);

			RequireDeviceID = StrUtil::Int2Str(tempID);	
		}
		//���ɷ���URL
		if (VideoStreamProtocol == "rtsp")
		{
			RequireURL = VideoStreamProtocol + "://" + PROPMANAGER::instance()->GetRtspVideoIp() + ":" + \
				PROPMANAGER::instance()->GetRtspVideoPort()+"/"+RequireDeviceID;
		}
		else
		{
			RequireURL = VideoStreamProtocol + "://" + PROPMANAGER::instance()->GetHttpVideoIp() + ":" + \
				PROPMANAGER::instance()->GetHttpVideoPort()+"/"+RequireDeviceID;
		}

		SendXML(TranslateUpXML::TranslateAutoRecordStream(this,RequireURL,string(""),string("")));	
	}
	else					//�Զ�/���� ¼������Ƶ�鿴/����
	{
		//�������ݿ�
		std::vector<sRecordInfo> vecRecordInfo;
		std::vector<sRecordInfo> vecLostRecordInfo;
		switch(RecordType)
		{
		case AUTORECORDQUERY:		//�Զ�¼������Ƶ�鿴
		case AUTORECORDFILEQUERY:	//�Զ�¼������Ƶ����
			{
				time_t T_StartDateTime = TimeUtil::StrToDateTime(StartDateTime);
				time_t T_EndDateTime =	TimeUtil::StrToDateTime(EndDateTime);
				
				long DiffTime = TimeUtil::DiffSecond(T_EndDateTime,T_StartDateTime);
				
				if( /*DiffTime  <= 14400 || */RecordType ==	AUTORECORDQUERY)
				{
					string RequireChannelID;

					//�°��������ֻ�·���freq��û�·�channelCode���Ͱ�freq����channelIdȥ�����ݿ�
					if ((DVBType==ATV || DVBType==RADIO || DVBType==CTV || DVBType==AM) && RequireFreq!="" && RequireChannelCode.length()<1)
					{
						RequireChannelID = RequireFreq;
					}
					else	//�����������ά��ԭ״
					{
						if (RequireFreq == strHDMIFreq)
						{//������¼��鿴 ChannelID = strHDMIFreq
							RequireChannelID = RequireFreq;
						}
						else
						{
							CHANNELINFOMGR::instance()->GetChannelID(DVBType,RequireOrgNetID,RequireTsID,RequireServiceID, \
								RequireVideoPID,RequireAudioPID,RequireChannelCode,RequireChannelID );//���ChannelCode
							if (sQualityIndexSet == "1" || sQualityIndexSet == "2")
							{
								CreateQualityQuery(RequireFreq, StartDateTime, EndDateTime);
							}
						}
					}

	#ifdef ZONG_JU_ZHAO_BIAO
					/** �ܾ��������ԣ����·�ͨ���ţ�ֻ��ͨ��channelId��ȡ¼��
					*   �ȸ����·���freq��serviceId��ȡchannelId
					*/
					if (RequireChannelID.length()<1)
					{
						CHANNELINFOMGR::instance()->GetChanIdByFreqServId(DVBType, RequireFreq, RequireServiceID, RequireChannelID);
					}
	#endif
					if((this->GetVersion() == "3.0")&&(RequireDeviceID.find("-1") != -1))
					{
						//vector<int> codlis;
						//PROPMANAGER::instance()->SeparateStrVec(RequireDeviceID,codlis);
						//for(int i=0;i<codlis.size();i++)
						//{
						//	ret = DBMANAGER::instance()->QueryRecordByChannelID(DVBType,"0",RequireChannelID,vecRecordInfo,StartDateTime,EndDateTime,StrUtil::Int2Str(codlis[i])); //��ѯ���ݿ�õ�¼���ļ��ĵ�ַ
						//}
						string DevPost = strChassisID+"_"+strPosition+"_"+strModuleID+"_"+strChanId;
						ret = DBMANAGER::instance()->QueryRecordByChannelID(DVBType,"0",RequireChannelID,vecRecordInfo,StartDateTime,EndDateTime,DevPost); //��ѯ���ݿ�õ�¼���ļ��ĵ�ַ
					}
					else
					{
						if (RequireChannelID.empty())
						{
							ret = DBMANAGER::instance()->QueryRecordByDeviceID(DVBType,"0",RequireDeviceID,vecRecordInfo,StartDateTime,EndDateTime);    //��ѯ���ݿ�õ�¼���ļ��ĵ�ַ
						}
						else
						{
							ret = DBMANAGER::instance()->QueryRecordByChannelID(DVBType,"0",RequireChannelID,vecRecordInfo,StartDateTime,EndDateTime); //��ѯ���ݿ�õ�¼���ļ��ĵ�ַ
						}
					}
					//
					if(this->GetVersion() == "3.0")//¼�������豨��ʧ
					{
						DBMANAGER::instance()->QueryLostRecordByChannelID(DVBType,TaskID,RequireChannelID,vecLostRecordInfo,StartDateTime,EndDateTime);
					}
					//
					break;
				}
				else 
				{
					RetValue = RECORD_DOWN_OUTOFRAND;
				}
			}
		case TASKRECORDQUERY:		//����¼������Ƶ�鿴
		case TASKRECORDFILEQUERY:	//����¼������Ƶ����
			{
				string RequireChannelID;
				//�°��������ֻ�·���freq��û�·�channelCode���Ͱ�freq����channelIdȥ�����ݿ�
				if ((DVBType==ATV ||DVBType==RADIO ||DVBType==CTV) && RequireFreq!="" && RequireChannelCode.length()<1)
				{
					RequireChannelID = RequireFreq;
				}
				else	//�����������ά��ԭ״
				{
					CHANNELINFOMGR::instance()->GetChannelID(DVBType,RequireOrgNetID,RequireTsID,RequireServiceID, \
						RequireVideoPID,RequireAudioPID,RequireChannelCode,RequireChannelID );//���ChannelCode
				}
				if(RequireChannelID.length()<1)
				{
					CHANNELINFOMGR::instance()->GetChanIdByFreqServId(DVBType, RequireFreq, RequireServiceID, RequireChannelID);
				}
				if(RequireChannelID.empty())
				{
					ret=DBMANAGER::instance()->QueryRecordByTaskid(DVBType,TaskID,vecRecordInfo,StartDateTime,EndDateTime);
				}
				else
				{
					ret = DBMANAGER::instance()->QueryRecordByChannelID(DVBType,TaskID,RequireChannelID,vecRecordInfo,StartDateTime,EndDateTime); 
				}
			}
		case RECORDFILELOSTQUERY:	
			{
				string RequireChannelID;
				CHANNELINFOMGR::instance()->GetChanIdByFreqServId(DVBType, RequireFreq, RequireServiceID, RequireChannelID);
				if((this->GetVersion() == "3.0")&&(RequireDeviceID.find("-1") != -1)&&(RequireDeviceID.find(";") != -1))//�����Բ�ѯ��ͨ����֧��
				{
					vector<int> codlis;
					PROPMANAGER::instance()->SeparateStrVec(RequireDeviceID,codlis);
					for(int i=0;i<codlis.size();i++)
					{
						ret = DBMANAGER::instance()->QueryLostRecordByChannelID(DVBType,TaskID,RequireChannelID,vecRecordInfo,StartDateTime,EndDateTime,StrUtil::Int2Str(codlis[i])); 
					}
				}
				else
				{
					DBMANAGER::instance()->QueryLostRecordByChannelID(DVBType,TaskID,RequireChannelID,vecRecordInfo,StartDateTime,EndDateTime); 
				}
			}
		default:
			break;
		}
		if(RecordType == RECORDFILELOSTQUERY)
		{
			for(int i = 0; i < vecRecordInfo.size(); i++)
			{
				sRecordInfo record = vecRecordInfo[i];
				//SendXML(CreateRecordLostReport(RequireFreq, RequireServiceID, record, true,RequireDeviceID));
				//SendXML(CreateRecordLostReport(RequireFreq, RequireServiceID, record, false,RequireDeviceID));
			}
			SendXML(TranslateUpXML::TranslateRecordFileLost(this, vecRecordInfo));
		}
		else
		{
			//��ѯ�������
			if (vecRecordInfo.size() == 0)		//δ�鵽����
			{
				if(RetValue != RECORD_DOWN_OUTOFRAND)
					RetValue = NOFILE_EXIST;
				
				SetRetValue(RetValue);
				
				switch(RecordType)
				{
				case AUTORECORDQUERY:			//�Զ�¼������Ƶ�鿴
					{
						SendXML(TranslateUpXML::TranslateAutoRecordStream(this,RequireURL,QueryStartDateTime,QueryEndDateTime));	
						break;
					}
				case AUTORECORDFILEQUERY:		//�Զ�¼������Ƶ����
					{
						SendXML(TranslateUpXML::TranslateAutoRecordFile(this,RequireURL,QueryStartDateTime,QueryEndDateTime,vecLostRecordInfo));	
						break;
					}
				case TASKRECORDQUERY:			//����¼������Ƶ�鿴
					{
						SendXML(TranslateUpXML::TranslateTaskRecordStream(this,RequireURL,QueryStartDateTime,QueryEndDateTime));
						break;
					}
				case TASKRECORDFILEQUERY:		//����¼������Ƶ����
					{
						SendXML(TranslateUpXML::TranslateTaskRecordFile(this,RequireURL,QueryStartDateTime,QueryEndDateTime));
						break;
					}
				default:
					break;
				}
			}
			else //��Ч���ݴ���
			{
				//��ȡ��ѯ����е�channelId(�°���Ҫ������¼���ѯʱ���ص�xml��channelcode��Ϊ��)
				if (ChannelID.length()<1)	//�޸Ļ����е�channelId
				{
					ChannelID = vecRecordInfo[0].channelID;
				}
				bool IsAudio = (DVBType==RADIO || DVBType==AM);		//����Ƶ�ļ���ʶ

				RecordFileSpliter(vecRecordInfo,IsAudio);	//����vecRecordInfo�е��ļ�ͷβ,�����Ч���ļ�����

				//�����ѯ��ʼ������ʱ��
				QueryStartDateTime = vecRecordInfo[0].starttime;
				QueryEndDateTime =vecRecordInfo[vecRecordInfo.size()-1].endtime;

				//������ʱ�ļ���
				string TempFileName = TimeUtil::DateTimeToString(time(0)) + GetMsgID();		//��ʱ�ļ���

				//����URL
				switch(RecordType)
				{
				case AUTORECORDQUERY:		//�Զ�¼������Ƶ�鿴
				case TASKRECORDQUERY:		//����¼������Ƶ�鿴
					{
						if (VideoFileurltype=="file")		//����ļ�ƴ��
						{
							TempFileName +=std::string(".ts");
							if (VideoStreamProtocol == "rtsp")
							{
								if(rate != "1")
								{
									string temfile = rate+TempFileName;
									RequireURL = VideoStreamProtocol + "://" + PROPMANAGER::instance()->GetRtspVideoIp() + ":" + \
										PROPMANAGER::instance()->GetRtspVideoPort() + PROPMANAGER::instance()->GetTempSharePath() + temfile;
								}
								else
								{
									RequireURL = VideoStreamProtocol + "://" + PROPMANAGER::instance()->GetRtspVideoIp() + ":" + \
										PROPMANAGER::instance()->GetRtspVideoPort() + PROPMANAGER::instance()->GetTempSharePath() + TempFileName;
								}
							}
							else
							{
								if(rate != "1")
								{
									string temfile = rate+TempFileName;
									RequireURL = VideoStreamProtocol + "://" + PROPMANAGER::instance()->GetHttpServerIP() + ":" + \
										PROPMANAGER::instance()->GetHttpServerPort() + PROPMANAGER::instance()->GetTempSharePath() + temfile;
								}
								else
								{
									RequireURL = VideoStreamProtocol + "://" + PROPMANAGER::instance()->GetHttpServerIP() + ":" + \
										PROPMANAGER::instance()->GetHttpServerPort() + PROPMANAGER::instance()->GetTempSharePath() + TempFileName;
								}
							}
						}
						else if (VideoFileurltype!="file")	//�ļ��б�
						{
							TempFileName += std::string(".") + VideoFileurltype;
							RequireURL = "http://" + PROPMANAGER::instance()->GetHttpServerIP() + ":" + \
								PROPMANAGER::instance()->GetHttpServerPort() + PROPMANAGER::instance()->GetTempSharePath() + TempFileName;
						}
						break;
					}
				case AUTORECORDFILEQUERY:	//�Զ�¼������Ƶ����
				case TASKRECORDFILEQUERY:	//����¼������Ƶ����
					{
						if (DownFileurltype=="file")	//����ļ�ƴ��
						{
							TempFileName +=std::string(".ts");
							if (DownFileProtocol == "rtsp")
							{
								RequireURL = DownFileProtocol + "://" + PROPMANAGER::instance()->GetRtspVideoIp() + ":" + \
									PROPMANAGER::instance()->GetRtspVideoPort() + PROPMANAGER::instance()->GetTempSharePath() + TempFileName;
							}
							else
							{
								RequireURL = DownFileProtocol + "://" + PROPMANAGER::instance()->GetHttpServerIP() + ":" + \
									PROPMANAGER::instance()->GetHttpServerPort() + PROPMANAGER::instance()->GetTempSharePath() + TempFileName;
							}
						}
						else if (DownFileurltype!="file")	//�ļ��б�
						{
							TempFileName += std::string(".") + DownFileurltype;
							RequireURL = "http://" + PROPMANAGER::instance()->GetHttpServerIP() + ":" + \
								PROPMANAGER::instance()->GetHttpServerPort() + PROPMANAGER::instance()->GetTempSharePath() + TempFileName;
						}				
						break;
					}
				default:
					break;
				}
			   std::vector<sRecordFileInfo> FileInfo;
			   bool bSendXml = true;
				//����������ʱ�ļ�
				switch(RecordType)
				{
				case AUTORECORDQUERY:		//�Զ�¼������Ƶ�鿴
				case TASKRECORDQUERY:		//����¼������Ƶ�鿴
					{
						if (VideoFileurltype == "file")			//����ļ�ƴ��
						{
							CreateDownFile(TempFileName,vecRecordInfo);
							//
							if(rate != "1")
							{
								string TempFileName1 = TempFileName;
								string path = "c:\\vscttb\\temp\\";
								TempFileName1 = rate+TempFileName;
								string command = string("bwsc_encoder.exe -i ");
								// bwsc_encoder.exe -i "cctv_hd_only(AD).ts" -preset fast -vcodec libx264 -b:v 1000k -an -speedup 2 -f mpegts -y test.ts
								command = command + path;
								command = command + TempFileName;
								command = command + string(" -preset fast -vcodec libx264 -b:v 700k -an -speedup ");
								command = command + rate;
								command = command + string(" -f mpegts -y ");
								command = command + path;
								command = command + TempFileName1;
								cout<<TempFileName1<<"======"<<TempFileName<<"###################"<<command<<endl;
								system(command.c_str());
								Sleep(3000);
							}
						}
						else if (VideoFileurltype != "file")	//�ļ��б�
						{
							if(rate != "1")
							{
								string path = "c:/vscttb/temp/";
								std::vector<sRecordInfo> vecMultiRateRecordInfo;
								for(int itsrec = 0;itsrec<vecRecordInfo.size();itsrec++)
								{
									sRecordInfo temRecInfo;
									temRecInfo = vecRecordInfo[itsrec];
									//
									size_t pos = temRecInfo.filename.find_last_of("/");
									if (pos ==std::string::npos)
									{
										continue;
									}
									std::string purename = temRecInfo.filename.substr(pos+1);
									//
									temRecInfo.filename = path+"mr"+rate+purename;
									vecMultiRateRecordInfo.push_back(temRecInfo);
								}
								//
								CreatePlayListFile(VideoFileurltype,TempFileName,vecMultiRateRecordInfo,VideoFileProtocol);
								//
								for(int itmrrec = 0;itmrrec<vecMultiRateRecordInfo.size();itmrrec++)
								{
									string sfile = vecRecordInfo[itmrrec].filename;
									string dfile = vecMultiRateRecordInfo[itmrrec].filename;
									//
									if(GetFileAttributes(dfile.c_str())!=-1)
									{
										DeleteFile(dfile.c_str());
									}
									//
									string command = string("bwsc_encoder.exe -i ");
									// bwsc_encoder.exe -i "cctv_hd_only(AD).ts" -preset fast -vcodec libx264 -b:v 1000k -an -speedup 2 -f mpegts -y test.ts
									command = command + sfile;
									command = command + string(" -preset fast -vcodec libx264 -b:v 700k -an -speedup ");
									command = command + rate;
									command = command + string(" -f mpegts -y ");
									command = command + dfile;
									cout<<dfile<<"======"<<sfile<<"###################"<<command<<endl;
									system(command.c_str());
									Sleep(5);
									//
									if(bSendXml)
									{
										//
										for(int copindex = itmrrec+1;copindex<vecMultiRateRecordInfo.size();copindex++)
										{
											CopyFile(dfile.c_str(),vecMultiRateRecordInfo[copindex].filename.c_str(),true);
										}
										//
										bSendXml = false;
										SendXML(TranslateUpXML::TranslateAutoRecordStream(this,RequireURL,QueryStartDateTime,QueryEndDateTime));	
									}
									//
									if(g_strQueryRecTaskFlag != strQueryRecTaskFlag)
									{
										cout<<"���ٲ��������滻�˳�......"<<endl;
										break;
									}
								}
							}
							else
							{
								CreatePlayListFile(VideoFileurltype,TempFileName,vecRecordInfo,VideoFileProtocol);
							}
						}
						break;
					}
				case AUTORECORDFILEQUERY:	//�Զ�¼������Ƶ����
				case TASKRECORDFILEQUERY:	//����¼������Ƶ����
					{
						if (DownFileurltype == "file")			//����ļ�ƴ��
						{
							if(vecRecordInfo.size()>3)
							{
								std::vector<sRecordInfo> vecTemRecordInfo;
								vecTemRecordInfo.push_back(vecRecordInfo[0]);
								vecRecordInfo.erase(vecRecordInfo.begin());
								CreateDownFile(TempFileName,vecTemRecordInfo);
								CreateDownFileEX(TempFileName,vecRecordInfo);

								SendXML(TranslateUpXML::TranslateAutoRecordFile(this,RequireURL,QueryStartDateTime,QueryEndDateTime,vecLostRecordInfo));	
								
								bSendXml = false;
							}
							else
							{
								CreateDownFile(TempFileName,vecRecordInfo);
							}
						}
						else if (DownFileurltype == "list")	//�б��ļ� ֻ����°���¼������
						{
							CreateListURL(vecRecordInfo,FileInfo,DownFileProtocol);
						}
						else  //m3u�ļ�����wpl�ļ�
						{
							CreatePlayListFile(DownFileurltype,TempFileName,vecRecordInfo,DownFileProtocol);
						}
						break;
					}
				default:
					break;
				}
				//���ͻظ�xml
				switch(RecordType)
				{
				case AUTORECORDQUERY:			//�Զ�¼������Ƶ�鿴
					{
						if(bSendXml)
						{
							SendXML(TranslateUpXML::TranslateAutoRecordStream(this,RequireURL,QueryStartDateTime,QueryEndDateTime));	
						}
						break;
					}
				case AUTORECORDFILEQUERY:		//�Զ�¼������Ƶ����
					{
						if(bSendXml)
						{
							SendXML(TranslateUpXML::TranslateAutoRecordFile(this,RequireURL,QueryStartDateTime,QueryEndDateTime,vecLostRecordInfo));
						}
						break;
					}
				case TASKRECORDQUERY:			//����¼������Ƶ�鿴
					{
						SendXML(TranslateUpXML::TranslateTaskRecordStream(this,RequireURL,QueryStartDateTime,QueryEndDateTime));
						break;
					}
				case TASKRECORDFILEQUERY:		//����¼������Ƶ����
					{
						if(DownFileurltype == "list")
						{
							SendXML(TranslateUpXML::TranslateTaskRecordFile(this,FileInfo));
						}
						else
						{
							SendXML(TranslateUpXML::TranslateTaskRecordFile(this,RequireURL,QueryStartDateTime,QueryEndDateTime));				
						}				
						break;
					}
				default:
					break;
				}

			}
		}
	}
	SetFinised();
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]¼���ѯ����ֹͣ !\n",DeviceID));
}

std::string RecordQueryTask::GetTaskName()
{
	return "¼���ѯ����";
}

std::string RecordQueryTask::GetObjectName()
{
	return std::string("RecordQueryTask");
}

bool RecordQueryTask::CreatePlayListFile(std::string listtype,std::string tempfilename,std::vector<sRecordInfo> vecFile,std::string Protocol)
{
	if (tempfilename.empty() || tempfilename.length()<1 || vecFile.size()==0)
		return false;

	std::vector<sRecordInfo>::iterator ptr = vecFile.begin();//�ļ�ͷ
	int iPos = 0;
	vector<std::string> vecLostFile;
	for(;iPos < vecFile.size()-1;)
	{
		int diffTime = TimeUtil::StrToDateTime(vecFile[iPos + 1].starttime) - TimeUtil::StrToDateTime(vecFile[iPos].endtime);
		/*if(diffTime<0)
		{
			vecFile.erase(ptr + iPos);
			continue;
		}*/
		if(diffTime!=0)
		{
			vecLostFile.push_back(vecFile[iPos].endtime);
		}
		iPos++;
	}
	string strTimestr = "";
	float MaxTime = 0.00;
	std::vector<std::string> filenamevec;
	for (;ptr!=vecFile.end();++ptr)
	{
		std::string tempfilename = (*ptr).filename;
		size_t pos = tempfilename.find_last_of("/");
		if (pos ==std::string::npos)
		{
			continue;
		}
		std::string dirpath = tempfilename.substr(0,pos+1);
		std::string purename = tempfilename.substr(pos+1);

		std::string sharepath;
		PROPMANAGER::instance()->GetSharePathByLoc(dirpath,sharepath);

		std::string sharefilename = sharepath + purename;
		std::string InnerRequireURL;

		if (Protocol == "rtsp")
		{
			InnerRequireURL = Protocol + "://" + PROPMANAGER::instance()->GetRtspVideoIp() + sharefilename;
		}
		else
		{
			int HttpServerPort = StrUtil::Str2Int(PROPMANAGER::instance()->GetHttpServerPort());//С��1ΪϵͳIIS����Ŀ¼û�ж˿ںŲ���apache����Ŀ¼
			if(HttpServerPort < 1)
			{
				InnerRequireURL = Protocol + "://" + PROPMANAGER::instance()->GetHttpServerIP() + sharefilename;
			}
			else
			{
				InnerRequireURL = Protocol + "://" + PROPMANAGER::instance()->GetHttpServerIP()+ ":" + PROPMANAGER::instance()->GetHttpServerPort() + sharefilename;
			}
		}
		strTimestr += sharefilename;
		strTimestr +=",";
		float tempmaxTime = (float)TimeUtil::DiffSecond(ptr->endtime,ptr->starttime);
		if(tempmaxTime>MaxTime)
		{
			MaxTime = tempmaxTime;
		}
		string strRecordTime = StrUtil::Float2Str(tempmaxTime);
		strTimestr += strRecordTime;
		strTimestr += ";";
		if(vecLostFile.size()>0)
		{
			int i = 0;
			while(i<vecLostFile.size())
			{
				if(ptr->endtime == vecLostFile[i])
				{
					strTimestr += "&,0;";
					break;
				}
				i++;
			}
		}
		filenamevec.push_back(InnerRequireURL);
	}
    
	string FilePath = PROPMANAGER::instance()->GetTempFileLoac() + tempfilename;	//��ʱ�ļ�·��
	ACE_DEBUG ((LM_DEBUG,"(%T| %t)��ѯ���񴴽��ļ�[%s]!\n",FilePath.c_str()));

	if(listtype == "m3u")
	{
		WPLFile::CreateVLCFile(FilePath,filenamevec);//����m3u�ļ�
	}
	else if (listtype == "wpl")
	{
		WPLFile::CreateWPLFile(FilePath,filenamevec);//����wpl�ļ�
	}
    else if(listtype == "m3u8")
    {
		/*string TimeCreat = "10";
		int timeLong = TimeUtil::DiffSecond(vecFile[0].endtime,vecFile[0].starttime);
		if(timeLong>=300)
		{
			TimeCreat = "60";
		}
		else if(timeLong>=200 && timeLong<300)
		{
			TimeCreat = "40";
		}
		else if(timeLong>=100 && timeLong<200)
		{
			TimeCreat = "20";
		}
		else if(timeLong<100)
		{
			TimeCreat = "10";
		}
		WPLFile::CreatVLCTsFile(FilePath,TimeCreat,vecFile[0].filename);
		int pos = vecFile[0].filename.find(".ts");*/
        strTimestr = string("MAX,") + StrUtil::Int2Str(MaxTime) + string(";") + strTimestr;
        if(WPLFile::CreateVLCFileM3U8(FilePath,filenamevec,strTimestr))
		{
			//DeleteFile(vecFile[0].filename.c_str());//ɾ���ļ�
		}
    }

	//��ʱ�ļ����
	DBMANAGER::instance()->AddTempFile(FilePath,TimeUtil::GetCurDateTime(),PROPMANAGER::instance()->GetTempFileExpire());

	return true;
}
bool RecordQueryTask::CreateDownFileEX(std::string tempfilename,std::vector<sRecordInfo> vecFile)
{
	if (tempfilename.empty()||vecFile.empty())
	{
		ACE_DEBUG ((LM_DEBUG,"(%T| %t) CreateDownFileEX��ѯ�����ļ������� !\n"));
		return false;
	}
	string newfilename=PROPMANAGER::instance()->GetTempFileLoac() + tempfilename;
	//
	FILE *newfile = fopen(newfilename.c_str(),"a+b");
	if(newfile  == NULL)
	{
		ACE_DEBUG ((LM_DEBUG,"(%T| %t)���ļ�ʧ��[%s]!\n",newfilename.c_str()));
		return false;
	}
	//
	const int BufLen=2000*188;
	unsigned char ch[BufLen];
	memset(ch,0,sizeof(ch));
	int index = vecFile.size();
	for (std::vector<sRecordInfo>::iterator ptr = vecFile.begin();ptr!= vecFile.end();++ptr)
	{
		FILE *file = NULL;
		if(index<=0)
		{
			break;
		}
		string tempfile=(*ptr).filename;
		file = fopen(tempfile.c_str(),"rb");
		if(file != NULL)
		{
			ACE_DEBUG ((LM_DEBUG,"(%T| %t)����׷���ļ�[%s]->[%s]!\n",tempfile.c_str(),newfilename.c_str()));
			//
			long fileAllSize = 0;
			long curpos = ftell(file);
			fseek(file,0,SEEK_END);
			fileAllSize = ftell(file);   //ȡ���ļ��Ĵ�С
			//
			fseek(file,curpos,SEEK_SET);
			unsigned long sizelen=0;
			while(sizelen+188<=fileAllSize)
			{
				memset(ch,0,sizeof(ch));	
				unsigned long len = fread(ch,1,BufLen,file);
				if(sizelen+len>fileAllSize)
				{
					fwrite(ch,1,fileAllSize-sizelen,newfile);
					break;
				}
				fwrite(ch,1,len,newfile);
				sizelen+=len;
			}
			fclose(file);   
			file = NULL;
		}
		//
		index--;
	}
	fclose(newfile);    
}

bool RecordQueryTask::CreateDownFile(std::string tempfilename,std::vector<sRecordInfo> vecFile)
{
	if (tempfilename.empty()||vecFile.empty())
	{
		ACE_DEBUG ((LM_DEBUG,"(%T| %t) ��ѯ�����ļ������� !\n"));
		return false;
	}
	string FileName=PROPMANAGER::instance()->GetTempFileLoac() + tempfilename;

	ACE_DEBUG ((LM_DEBUG,"(%T| %t)��ѯ���񴴽��ļ�[%s]��ʼ !\n",FileName.c_str()));

	size_t pos;
	while((pos=FileName.find("/"))!=string::npos)
	{
		FileName.replace(pos,1,"\\");
	}
	string cmdstr="copy /B ";
	//���������ļ�
	//int index = 6;
	int index = vecFile.size();
	for (std::vector<sRecordInfo>::iterator ptr = vecFile.begin();ptr!= vecFile.end();++ptr)
	{
		if(index<=0)
		{
			break;
		}
		string tempfile=(*ptr).filename;
		if(tempfile.length()<1)
			continue;
		while((pos=tempfile.find("/"))!=string::npos)
		{
			tempfile.replace(pos,1,"\\");
		}
		cmdstr+=tempfile+"+";
		index--;
	}
	pos = cmdstr.find_last_of('+');
	if (pos != std::string::npos)
	{
		cmdstr.replace(pos,1," ");
		cmdstr+=FileName;
		system(cmdstr.c_str());
	}
	//��ʱ�ļ����
	DBMANAGER::instance()->AddTempFile(FileName,TimeUtil::GetCurDateTime(),PROPMANAGER::instance()->GetTempFileExpire());

	ACE_DEBUG ((LM_DEBUG,"(%T| %t)��ѯ���񴴽��ļ�[%s]���� !\n",FileName.c_str()));
	return true;
}
bool RecordQueryTask::RecordFileSpliter(vector < sRecordInfo>& fileVector,bool IsAudio)
{
	if(StartDateTime.length()<1&&EndDateTime.length()<1)
		return true;
	if (fileVector.size()==1)
	{
		string newfilename;
		time_t taskstarttime=TimeUtil::StrToDateTime(StartDateTime);
		time_t taskendtime= TimeUtil::StrToDateTime(EndDateTime);
		time_t filestarttime=TimeUtil::StrToDateTime(fileVector[0].starttime);
		time_t fileendtime=TimeUtil::StrToDateTime(fileVector[0].endtime);
		//newfilename=WPLFile::TimeFileSplit(fileVector[0].filename,starttime,endtime);
		if (taskstarttime>filestarttime&&taskstarttime<fileendtime&&taskendtime>filestarttime&&taskendtime<fileendtime)//��ͷ��β���м�
		{
			newfilename=WPLFile::FileSplit(fileVector[0].filename,float(taskstarttime-filestarttime)/float(fileendtime-filestarttime),
				float(taskendtime-filestarttime)/float(fileendtime-filestarttime),false);
		}
		else if(taskstarttime>filestarttime&&taskstarttime<fileendtime&&taskendtime>fileendtime)//��ͷ��β
		{
			newfilename=WPLFile::FileSplit(fileVector[0].filename,float(taskstarttime-filestarttime)/float(fileendtime-filestarttime),true,false);
		}
		else if(taskendtime>filestarttime&&taskendtime<fileendtime&&taskstarttime<filestarttime)//��β��ͷ
		{
			newfilename=WPLFile::FileSplit(fileVector[0].filename,float(taskendtime-filestarttime)/float(fileendtime-filestarttime),false,false);
		}
		if(newfilename.size()>0)
		{
			fileVector[0].filename=newfilename;
		}
		fileVector[0].starttime=StartDateTime;
		fileVector[0].endtime=EndDateTime;
		
		if(newfilename.size()>0)
		{
			DBMANAGER::instance()->AddTempFile(newfilename,TimeUtil::GetCurDateTime(),PROPMANAGER::instance()->GetTempFileExpire());
		}
	}
	else
	{
		string newfilename;
		time_t taskstarttime=TimeUtil::StrToDateTime(StartDateTime);
		time_t taskendtime= TimeUtil::StrToDateTime(EndDateTime);
		time_t filestarttime=TimeUtil::StrToDateTime(fileVector[0].starttime);
		time_t fileendtime=TimeUtil::StrToDateTime(fileVector[0].endtime);
		//newfilename=WPLFile::TimeFileSplit(fileVector[0].filename,starttime,tempend,1);
		if(taskstarttime>filestarttime&&taskstarttime<fileendtime)//��ͷ��β
		{
			newfilename=WPLFile::FileSplit(fileVector[0].filename,float(taskstarttime-filestarttime)/float(fileendtime-filestarttime),true,false);
			if(newfilename.size()>0)
			{
				fileVector[0].filename=newfilename;
				fileVector[0].starttime=StartDateTime;
			}
			if(newfilename!="")
				DBMANAGER::instance()->AddTempFile(newfilename,TimeUtil::GetCurDateTime(),PROPMANAGER::instance()->GetTempFileExpire());
		}

		filestarttime=TimeUtil::StrToDateTime(fileVector[fileVector.size()-1].starttime);
		fileendtime=TimeUtil::StrToDateTime(fileVector[fileVector.size()-1].endtime);
       /* if(taskendtime>fileendtime)
        {
            taskendtime = fileendtime - 1;
        }*/
		if(taskendtime>filestarttime&&taskendtime<fileendtime)//��β��ͷ
		{
			newfilename=WPLFile::FileSplit(fileVector[fileVector.size()-1].filename,float(taskendtime-filestarttime)/float(fileendtime-filestarttime),false,false);
			if(newfilename.size()>0)
			{
				fileVector[fileVector.size()-1].filename=newfilename;
				fileVector[fileVector.size()-1].endtime=EndDateTime;
			}
			if(newfilename!="")
				DBMANAGER::instance()->AddTempFile(newfilename,TimeUtil::GetCurDateTime(),PROPMANAGER::instance()->GetTempFileExpire());
		}
	}
	return true;
}


bool RecordQueryTask::CreateListURL(std::vector<sRecordInfo> vecFile,std::vector<sRecordFileInfo> &FileInfo,std::string Protocol)
{
	if (vecFile.size()==0)
		return false;

	for (std::vector<sRecordInfo>::iterator ptr=vecFile.begin();ptr!=vecFile.end();++ptr)
	{
		sRecordFileInfo tempInfo;
		std::string tempfilename = (*ptr).filename;
		size_t pos = tempfilename.find_last_of("/");
		if (pos ==std::string::npos)
		{
			continue;
		}
		std::string dirpath = tempfilename.substr(0,pos+1);
		std::string purename = tempfilename.substr(pos+1);

		std::string sharepath;
		PROPMANAGER::instance()->GetSharePathByLoc(dirpath,sharepath);

		std::string sharefilename = sharepath + purename;
		std::string InnerRequireURL;

		if (Protocol == "rtsp")
		{
			InnerRequireURL = Protocol + "://" + PROPMANAGER::instance()->GetRtspVideoIp() + ":" + \
				PROPMANAGER::instance()->GetRtspVideoPort() + sharefilename;
		}
		else
		{
			InnerRequireURL = Protocol + "://" + PROPMANAGER::instance()->GetHttpServerIP() + ":" + \
				PROPMANAGER::instance()->GetHttpServerPort() + sharefilename;
		}
		tempInfo.url=InnerRequireURL;
		tempInfo.starttime=(*ptr).starttime;
		tempInfo.endtime = (*ptr).endtime;
		FileInfo.push_back(tempInfo);
	}



	return true;
}



bool RecordQueryTask::CreateQualityQuery(string freq, string sStartTime, string sEndTime)
{
	//***************************************
	/*<?xml version='1.0' encoding='GB2312'?>
	<Msg Version="2.6" MsgID="391501" Type="MonDown" DateTime="2016-07-07 15:54:18" SrcCode="110000G01" DstCode="300004D01" SrcURL="http://10.1.103.161:8080/dtss/callback.do?jsessionid=69FDA4F3E09939C10F7E8E2381D55A28" Priority="1">
	<GetIndexSet Index="1" Freq="714000" beginTime="2016-07-07 11:20:54" endTime="2016-07-07 12:00:14">
	<GetIndex Type="0" Desc="ALL"/>
	</GetIndexSet>
	</Msg>*/
	//***************************************
	bool ret = true;
	std::string xml ="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\" ?> \
					 <Msg></Msg>";//����xmlͷ
	XmlParser parser(xml.c_str());
	pXMLNODE root=parser.GetRootNode();
	parser.SetAttrNode( "Version",string("1"),root );//�汾
	string strDateTime = TimeUtil::GetCurDateTime();
	string  MsgID = OSFunction::GetXmlMsgID(strDateTime);
	parser.SetAttrNode( "MsgID", MsgID, root );//��Ϣid����
	parser.SetAttrNode("Type", string("MonDown"), root);               //����Msg�ڵ��DVBType����ֵ
	parser.SetAttrNode( "DateTime", strDateTime, root );//��ǰʱ��
	parser.SetAttrNode( "SrcCode", this->GetSrcCode(), root );//������ʶ����ͨ���ӿڻ��
	parser.SetAttrNode( "DstCode", this->GetDstCode(), root );//Ŀ�������ʶ
	parser.SetAttrNode("SrcURL", this->GetSrcURL(), root);
	parser.SetAttrNode( "Priority", string("1"), root );//�ظ�����Ϣid
	//
	pXMLNODE pNode = parser.CreateNodePtr(root, "GetIndexSet");
	parser.SetAttrNode( "Index", string("1"), pNode );
	parser.SetAttrNode( "Freq", freq , pNode );
	parser.SetAttrNode( "beginTime", sStartTime, pNode );
	parser.SetAttrNode( "endTime", sEndTime, pNode );
	pXMLNODE pChildNode = parser.CreateNodePtr(pNode, "GetIndex");
	parser.SetAttrNode( "Type", string("0"), pChildNode );
	parser.SetAttrNode( "Desc", string("ALL"), pChildNode );
	string xmltask;
	parser.SaveToString(xmltask);
	//
	string sdtXml;
	TranslateDownXML::TranslateTaskXML(xmltask,sdtXml);
	QualityQueryTask* pQualityQuery = new QualityQueryTask(sdtXml);//�µ�ָ������
	//���������
	if( BUSINESSLAYOUTMGR::instance()->AddTask(pQualityQuery) == true )
	{
	}
	
	return ret;
}

string RecordQueryTask::CreateRecordLostReport(string Freq, string ServiceID, sRecordInfo record, bool bStart,string RequireDeviceID)
{
	/*
	<? xmlversion="1.0"encoding="GB2312"standalone="yes" ?>
	<Msg Version="2.6"
	MsgID="2"
	Type="MonUp"
	DateTime="2002-08-17 15:30:00"
	SrcCode="110000D01"
	DstCode="110000G01"
	ReplyID="-1"
	>
	<AlarmNVRVideoLostReport Freq="658000" ServiceID="10" >
	<Alarm AlarmID ="12345" Value="0" Time="2002-08-17 15:30:00"/>
	</ AlarmNVRVideoLostReport>
	</Msg>
	*/
	std::string xml ="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\" ?> \
					 <Msg></Msg>";//����xmlͷ
	XmlParser parser(xml.c_str());
	pXMLNODE root=parser.GetRootNode();
	parser.SetAttrNode( "Version",string("3.0"),root );//�汾
	string strDateTime = TimeUtil::GetCurDateTime();
	string  MsgID = OSFunction::GetXmlMsgID(strDateTime);
	parser.SetAttrNode( "MsgID", MsgID, root );//��Ϣid����
	parser.SetAttrNode("Type", string("MonUp"), root);               //����Msg�ڵ��DVBType����ֵ
	parser.SetAttrNode( "DateTime", strDateTime, root );//��ǰʱ��
	parser.SetAttrNode( "SrcCode", this->GetSrcCode(), root );//������ʶ����ͨ���ӿڻ��
	parser.SetAttrNode( "DstCode", this->GetDstCode(), root );//Ŀ�������ʶ
	parser.SetAttrNode("DstURL", this->GetSrcURL(), root);
	parser.SetAttrNode( "ReplyID", string("-1"), root );//�ظ�����Ϣid
	//
	pXMLNODE pAutoRecordNode = parser.CreateNodePtr(root, "AutoRecordIntegraityReport");
	pXMLNODE pNode = parser.CreateNodePtr(pAutoRecordNode, "AutoRecordChannel");
	parser.SetAttrNode( "Freq", Freq , pNode);
	parser.SetAttrNode( "ServiceID", ServiceID, pNode);
	//
	vector<int> codlis;
	PROPMANAGER::instance()->SeparateStrVec(RequireDeviceID,codlis);
	pXMLNODE TranschanNode = parser.CreateNodePtr(pNode, "TranscodeInfo");
	CoderDev CoderInfo;
	for(int i=0;i<codlis.size();i++)
	{
		PROPMANAGER::instance()->GetCoderInfo(codlis[i],CoderInfo);
		pXMLNODE TranschanNodeInfoNode = parser.CreateNodePtr(TranschanNode, "Info");
		parser.SetAttrNode("ChassisID", CoderInfo.s_monitorindex, TranschanNodeInfoNode);
		parser.SetAttrNode("Position", CoderInfo.s_cardindex, TranschanNodeInfoNode);
		parser.SetAttrNode("ModuleID", CoderInfo.s_modindex, TranschanNodeInfoNode);
		parser.SetAttrNode("Transcode", CoderInfo.s_coderindex, TranschanNodeInfoNode);
	}
	//
	pXMLNODE pChildNode = parser.CreateNodePtr(pNode, "Alarm");
	long alarmid=0;
	PROPMANAGER::instance()->GetAlarmID(alarmid);
	parser.SetAttrNode( "AlarmID", StrUtil::Long2Str(alarmid) , pChildNode);
	if(bStart)
	{
		parser.SetAttrNode( "Value", "1", pChildNode);
		parser.SetAttrNode( "Time", record.starttime, pChildNode);
	}
	else
	{
		parser.SetAttrNode( "Value", string("2"), pChildNode);
		parser.SetAttrNode( "Time", record.endtime, pChildNode);
	}
	string strXML;
	parser.SaveToString(strXML);
	return strXML;
}

