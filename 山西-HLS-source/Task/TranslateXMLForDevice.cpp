#include "TranslateXMLForDevice.h"
#include "../Foundation/XmlParser.h"
#include "../Foundation/AppLog.h"
#include "../BusinessProcess/ChannelInfoMgr.h"
#include "../Foundation/TypeDef.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/OSFunction.h"
#include "../DeviceAccess/ATVTcpDeviceAccess.h"
#include "../Foundation/PropManager.h"


#ifdef ZONG_JU_ZHAO_BIAO
#include "../Foundation/ParamSetManager.h"
#endif

TranslateXMLForDevice::TranslateXMLForDevice(void)
{
}

TranslateXMLForDevice::~TranslateXMLForDevice(void)
{
}

std::string TranslateXMLForDevice::GetXMLHeader( std::string Type)
{
	std::string retXml;//����ֵ
	char* xml="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser parser(xml);
	pXMLNODE rootNode=parser.GetRootNode();//���ڵ�msg
	parser.SetAttrNode("DVBType",Type,rootNode);//����msg�ڵ��DVBType����ֵ
	parser.SaveToString(retXml);//���淵��xml
	return retXml;
}

std::string TranslateXMLForDevice::TranslateStreamRealTimeXML(std::string  strStdXML )
{
	std::string retXML;//���ص�xml
	std::string Freq,OrgNetID, TsID,ServiceID,VideoPID,AudioPID,DataType,ProName,SymbolRate,QAM,PmtPid,PcrPid,isosd;
	std::string Width,Height,Fps,Bps,ThreeDSet;
	std::string TunerDeviceID,CoderDeviceID,Version,IsTranAudio;
	std::string DownType,TaskType = "TsQuery";//��Ϣ����

	XmlParser parser(strStdXML.c_str());
	pXMLNODE rootNode=parser.GetRootNode();//���ڵ�msg

	parser.GetAttrNode(rootNode,"Version",Version);
	parser.GetAttrNode(rootNode,"DVBType",DownType);//���msg�ڵ��DownType����ֵ

	enumDVBType dvbtype = OSFunction::GetEnumDVBType(DownType);

	pXMLNODE queryTaskNode=parser.GetNodeFromPath("Msg/StreamRealtimeQuery/RealtimeQueryTask");//RealtimeQueryTask�ڵ�
	//�����Ǵӱ�׼strStdXML�л����Ӧ������ֵ
	parser.GetAttrNode(queryTaskNode,"Freq",Freq);//���Ƶ������ֵ
	parser.GetAttrNode(queryTaskNode,"OrgNetID",OrgNetID);//���orgNetID����ֵ
	parser.GetAttrNode(queryTaskNode,"TsID",TsID);//���TsID����ֵ
	parser.GetAttrNode(queryTaskNode,"ServiceID",ServiceID);//���ServiceID����ֵ
	parser.GetAttrNode(queryTaskNode,"VideoPID",VideoPID);//�����Ƶpid����ֵ
	parser.GetAttrNode(queryTaskNode,"AudioPID",AudioPID);//�����Ƶpid����ֵ
	parser.GetAttrNode(queryTaskNode,"DataType",DataType);//���������������ֵ
	parser.GetAttrNode(queryTaskNode,"Width",Width);//��ÿ������ֵ
	parser.GetAttrNode(queryTaskNode,"Height",Height);//��ø߶�����ֵ
	parser.GetAttrNode(queryTaskNode,"OSDEnable",isosd);//��ø߶�����ֵ
	parser.GetAttrNode(queryTaskNode,"Fps",Fps);//���FPs����ֵ
	parser.GetAttrNode(queryTaskNode,"Bps",Bps);//���Bps����ֵ
	parser.GetAttrNode(queryTaskNode,"ThreeDSet",ThreeDSet);
	parser.GetAttrNode(queryTaskNode,"CDeviceID",CoderDeviceID);//���FPs����ֵ
	parser.GetAttrNode(queryTaskNode,"TDeviceID",TunerDeviceID);//���Bps����ֵ
	parser.GetAttrNode(queryTaskNode,"Audio",IsTranAudio);//���Bps����ֵ

#ifdef ZONG_JU_ZHAO_BIAO
	std::string strCodingFormat;
	parser.GetAttrNode(queryTaskNode,"CodingFormat",strCodingFormat);
#endif
	

	if (Freq != strHDMIFreq)
	{
		//add by gxd 2010-10-13
		CHANNELINFOMGR::instance()->GetProName(dvbtype,Freq,ServiceID,ProName);
		CHANNELINFOMGR::instance()->GetSymbolRateQAMByFreq(dvbtype,Freq,SymbolRate,QAM);
		if (ProName.empty())
		{
			CHANNELINFOMGR::instance()->GetProName(dvbtype,Freq,ServiceID,ProName);
			if (ProName.empty())
			{
				ProName = "δ֪Ƶ��";
			}
		}
	}
	else
	{
		ProName = "������";
	}


	std::string headerXML=GetXMLHeader(DownType);
	
	XmlParser retParser;
	retParser.Set_xml(headerXML);
	std::string HDTV="0";
	if(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")
	{
		if (StrUtil::Str2Int(Width)>720||StrUtil::Str2Int(Height)>576)
		{
			HDTV="1";
		}
	}
	else
	{
		if (StrUtil::Str2Int(Width)>640||StrUtil::Str2Int(Height)>480)
		{
			HDTV="1";
		}
	}

	OSDInfo osd;
	eDVBType etype=OSFunction::GetEnumDVBType(DownType);
	PROPMANAGER::instance()->GetOSDInfo(etype,HDTV,osd);
	VideoParam param;
	if(HDTV=="1")
		PROPMANAGER::instance()->GetHDRealStreamVideoParam(dvbtype,param);
	else if(HDTV=="0")
		PROPMANAGER::instance()->GetSDRealStreamVideoParam(dvbtype,param);

	string audiobps="";
	PROPMANAGER::instance()->GetAudioBps(dvbtype,audiobps);

	if(Width.length()<1)
		Width = param.width;
	if(Height.length()<1)
		Height = param.height;
	if(Bps.length()<1)
		Bps = StrUtil::Int2Str(StrUtil::Str2Int(param.bps)*1000);

	int LBps = StrUtil::Str2Int(Bps);
	if(dvbtype==RADIO||dvbtype==AM)
	{
		Bps = StrUtil::Int2Str(LBps/1000); 
	}
	else if (dvbtype == CTTB)
	{
		Bps = StrUtil::Int2Str(LBps/1000);
		int ibps = StrUtil::Str2Int(Bps);
		if (HDTV == "0")
		{
			if (ibps < 500)
			{
				Bps = "500";
			}
			else if (ibps > 1000)
			{
				Bps = "1000";
			}
			else 
			{
				Bps = param.bps;
			}
		}
		else if (HDTV == "1")
		{
			if (ibps < 1000)
			{
				Bps = "1000";
			}
			else if (ibps > 2000)
			{
				Bps = "2000";
			}
			else
			{
				Bps = param.bps;
			}
		}
	}
	else
	{
		Bps = StrUtil::Int2Str(LBps/1000-StrUtil::Str2Int(audiobps));
		if(StrUtil::Str2Int(Bps)<150&&HDTV=="0")
		{	
			Bps = "150";
		}
#ifndef ZONG_JU_ZHAO_BIAO
		else if(StrUtil::Str2Int(Bps)<500&&HDTV=="1")
		{
			Bps = "500";
		}
#endif
		else if(StrUtil::Str2Int(Bps)>2000&&HDTV=="0")
		{
			Bps = "2000";
		}
		else if(StrUtil::Str2Int(Bps)>4000&&HDTV=="1")
		{
			Bps = "4000";
		}
	}
	
	pXMLNODE tsQueryNode=parser.CreateNodePtr(retParser.GetRootNode(),"TSQuery");//���ص�xml����TsQuery�ڵ�
	pXMLNODE tsNode=parser.CreateNodePtr(tsQueryNode,"TS");
	//������Ϊ���ص�Ӳ��ͨ����xml��������ֵ
	retParser.SetAttrNode("Version",Version,tsNode);
	retParser.SetAttrNode("Freq",Freq,tsNode);//���÷���xml��TsQuery�ڵ��Freq����ֵ
	retParser.SetAttrNode("OrgNetID",OrgNetID,tsNode);//���÷���xml��TsQuery�ڵ��OrgNetID����ֵ
	retParser.SetAttrNode("TsID",TsID,tsNode);//���÷���xml��TsQuery�ڵ��TsID����ֵ
	retParser.SetAttrNode("SymbolRate",SymbolRate,tsNode);
	retParser.SetAttrNode("QAM",QAM,tsNode);
	retParser.SetAttrNode("ServiceID",ServiceID,tsNode);//���÷���xml��TsQuery�ڵ��ServiceID����ֵ

	//add by gxd 2010-10-13
	retParser.SetAttrNode("ServiceName",ProName,tsNode);
	CHANNELINFOMGR::instance()->GetPmtPidPcrPid(dvbtype,Freq,ServiceID,PmtPid,PcrPid);
	retParser.SetAttrNode("PmtPID",PmtPid,tsNode);
	retParser.SetAttrNode("PcrPID",PcrPid,tsNode);
	retParser.SetAttrNode("VideoPID",VideoPID,tsNode);//���÷���xml��TsQuery�ڵ��VideoPID����ֵ
	retParser.SetAttrNode("AudioPID",AudioPID,tsNode);//���÷���xml��TsQuery�ڵ��AudioPID����ֵ
	retParser.SetAttrNode("IsChangeFreq",string("1"),tsNode);
	retParser.SetAttrNode("HDTV",HDTV,tsNode);
	retParser.SetAttrNode("DataType",DataType,tsNode);//���÷���xml��TsQuery�ڵ��DataType����ֵ
	retParser.SetAttrNode("Width",Width,tsNode);//���÷���xml��TsQuery�ڵ��Width����ֵ
	retParser.SetAttrNode("Height",Height,tsNode);//���÷���xml��TsQuery�ڵ��Height����ֵ
	retParser.SetAttrNode("Fps",Fps,tsNode);//���÷���xml��TsQuery�ڵ��Fps����ֵ
	retParser.SetAttrNode("OSDEnable",isosd,tsNode);//���÷���xml��TsQuery�ڵ��Fps����ֵ
	retParser.SetAttrNode("Bps",Bps,tsNode);//���÷���xml��TsQuery�ڵ��Bps����ֵ
	if(dvbtype==THREED)
		retParser.SetAttrNode("ThreeDSet",ThreeDSet,tsNode);

#ifdef ZONG_JU_ZHAO_BIAO
	retParser.SetAttrNode("CodingFormat",strCodingFormat,tsNode);//���÷���xml��TsQuery�ڵ��Bps����ֵ
#endif

	retParser.SetAttrNode("SystemClock",(long)time(0),tsNode);
	retParser.SetAttrNode("OSDFontSize",osd.FontSize,tsNode);
	retParser.SetAttrNode("OSDInfo",osd.Info,tsNode);
	retParser.SetAttrNode("OSDInfoX",osd.InfoX,tsNode);
	retParser.SetAttrNode("OSDInfoY",osd.InfoY,tsNode);
	retParser.SetAttrNode("OSDTimeType",osd.TimeType,tsNode);
	retParser.SetAttrNode("OSDTimeX",osd.TimeX,tsNode);
	retParser.SetAttrNode("OSDTimeY",osd.TimeY,tsNode);
	retParser.SetAttrNode("OSDProgramX",osd.ProgramX,tsNode);
	retParser.SetAttrNode("OSDProgramY",osd.ProgramY,tsNode);
	retParser.SetAttrNode("CDeviceID",CoderDeviceID,tsNode);//���FPs����ֵ
	retParser.SetAttrNode("TDeviceID",TunerDeviceID,tsNode);//���Bps����ֵ
	retParser.SetAttrNode("Audio",IsTranAudio,tsNode);//���Bps����ֵ
	
	retParser.SaveToString(retXML);
	return retXML;
}

std::string TranslateXMLForDevice::TranslateQualityRealTimeQuery( std::string strStdXML )
{
	std::string STD,Freq,SymbolRate,Type,Desc,Value,QAM,strDeviceID;

	std::string DownType;//��Ϣ����

	XmlParser parser;
	parser.Set_xml(strStdXML);
	pXMLNODE rootNode=parser.GetRootNode();//���ڵ�msg

	parser.GetAttrNode(rootNode,"DVBType",DownType);//���msg�ڵ��DownType����ֵ
	
	enumDVBType dvbtype = OSFunction::GetEnumDVBType(DownType);

	pXMLNODE qualityQueryTaskNode=parser.GetNodeFromPath("Msg/QualityRealtimeQuery/QualityQueryTask");//��׼xml��QualityQueryTask�ڵ�
	parser.GetAttrNode(qualityQueryTaskNode,"STD",STD);//��ñ�׼xml��QualityQueryTask�ڵ��STD����
	parser.GetAttrNode(qualityQueryTaskNode,"Freq",Freq);//��ñ�׼xml��QualityQueryTask�ڵ��Freq����
	parser.GetAttrNode(qualityQueryTaskNode,"DeviceID",strDeviceID);//��ñ�׼xml��QualityQueryTask�ڵ��Freq����

	CHANNELINFOMGR::instance()->GetSymbolRateQAMByFreq(dvbtype,Freq,SymbolRate,QAM);
	//parser.GetAttrNode(qualityQueryTaskNode,"SymbolRate",SymbolRate);//��ñ�׼xml��QualityQueryTask�ڵ��SymbolRate����
	
	XmlParser retParser;
	retParser.Set_xml(GetXMLHeader(DownType));
	/*char* xml="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser retParser(xml);*/
	pXMLNODE DevRootNode=retParser.GetRootNode();//���ڵ�msg
	
	pXMLNODE retQualityQueryNode=retParser.CreateNodePtr(retParser.GetRootNode(),"QualityQuery");//����xml����QualityQuery�ڵ�

	retParser.SetAttrNode("STD",STD,retQualityQueryNode);//Ϊ���ص�xml��QualityQuery�ڵ�����STD����
	retParser.SetAttrNode("Freq",Freq,retQualityQueryNode);//Ϊ���ص�xml��QualityQuery�ڵ�����Freq����
	retParser.SetAttrNode("SymbolRate",SymbolRate,retQualityQueryNode);//Ϊ���ص�xml��QualityQuery�ڵ�����SymbolRate����
	retParser.SetAttrNode("QAM",QAM,retQualityQueryNode);
	retParser.SetAttrNode("DeviceID",strDeviceID,retQualityQueryNode);

	pXMLNODE ParamNode=retParser.CreateNodePtr(retParser.GetRootNode(),"QualityParam");
	pXMLNODE qualityParamNode=parser.GetNodeFromPath("Msg/QualityRealtimeQuery/QualityParam");//��׼xml��QualityParam�ڵ�
	pXMLNODELIST qualityParamNodeList=parser.GetNodeList(qualityParamNode);//��׼xml��QualityParam�ڵ���ӽڵ��б�
	for (int i=0;i!=qualityParamNodeList->Size();++i)
	{
		pXMLNODE retQualityIndexNode=retParser.CreateNodePtr(ParamNode,"QualityIndex");//Ϊ���ص�xml����QualityIndex�ڵ�

		pXMLNODE qualityIndexNode=parser.GetNextNode(qualityParamNodeList);

		parser.GetAttrNode(qualityIndexNode,"Type",Type);//��ñ�׼xml��qualityIndex�ڵ��Type����
		parser.GetAttrNode(qualityIndexNode,"Desc",Desc);//��ñ�׼xml��qualityIndex�ڵ��Desc����
		parser.GetAttrNode(qualityIndexNode,"Value",Value);//��ñ�׼xml��qualityIndex�ڵ��Value����

		retParser.SetAttrNode("Type",Type,retQualityIndexNode);//Ϊ���ص�xml��QualityIndex�ڵ�����Type����
		retParser.SetAttrNode("Desc",Desc,retQualityIndexNode);//Ϊ���ص�xml��QualityIndex�ڵ�����Desc����
		retParser.SetAttrNode("Value",Value,retQualityIndexNode);//Ϊ���ص�xml��QualityIndex�ڵ�����Value����
	}

	std::string retXML;
	retParser.SaveToString(retXML);
	return retXML;
}
std::string TranslateXMLForDevice::TranslateRecordTask(std::string strStdXML)
{
	if (strStdXML.size() == 0)
		return "";

	char * tsFetch = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg><TSQuery></Msg>";
   
	XmlParser fetchParse( tsFetch );
	XmlParser stdParse(strStdXML.c_str());

	string strHardXML;   //ת����ȷʱ���ص�XML
	string errorXML;     //ת����������ʱ���ص�XML

	string Freq,OrgNetID,TsID,ServiceID,VideoPID,AudioPID,DataType,Width,Height,Fps,Bps,ProName,SymbolRate,QAM,PmtPid,PcrPid,ThreeDSet;
	string sOsd;
	std::string TunerDeviceID,CoderDeviceID,Version;

	std::string DownType,TaskType = "TsQuery";//��Ϣ����

	try
	{
		pXMLNODE fetchRootNode = fetchParse.GetNodeFromPath( "Msg"); //���͵��豸��XML�ĸ��ڵ�msg
		pXMLNODE stdRootNode = stdParse.GetNodeFromPath( "Msg" );    //��׼¼��XML�ĸ��ڵ�msg
		stdParse.GetAttrNode(stdRootNode,"Version",Version);
		stdParse.GetAttrNode( stdRootNode,"DVBType",DownType ); //���msg�ڵ��DownType����ֵ
		
		fetchParse.SetAttrNode( "DVBType",DownType,fetchRootNode ); //���÷��͵��豸��XML��msg�ڵ��е�DownType����
		
		pXMLNODE RecordNode;
		pXMLNODE tsQueryyNode = fetchParse.GetNodeFromPath( "Msg/TSQuery" ); // //��ȡ���͵��豸��XML��TsQuery�ڵ�
		pXMLNODE tsFetchNode=fetchParse.CreateNodePtr(tsQueryyNode,"TS");
		if(strStdXML.find("AutoRecord")!=string::npos)
			RecordNode = stdParse.GetNodeFromPath( "Msg/AutoRecord/Record" );
		else if(strStdXML.find("TaskRecord")!=string::npos)
			RecordNode = stdParse.GetNodeFromPath( "Msg/TaskRecord/Record" );
		//��ȡ¼���ѯָ��
		stdParse.GetAttrNode( RecordNode,"CDeviceID",CoderDeviceID );            //��ȡ��׼¼��XML��RecordSetTask�ڵ��Freq����
		stdParse.GetAttrNode( RecordNode,"TDeviceID",TunerDeviceID );            //��ȡ��׼¼��XML��RecordSetTask�ڵ��Freq����
		stdParse.GetAttrNode( RecordNode,"Freq",Freq );            //��ȡ��׼¼��XML��RecordSetTask�ڵ��Freq����
		stdParse.GetAttrNode( RecordNode,"OrgNetID",OrgNetID );	   //��ȡ��׼¼��XML��RecordSetTask�ڵ��OrgNetID����
		stdParse.GetAttrNode( RecordNode,"TsID",TsID );			   //��ȡ��׼¼��XML��RecordSetTask�ڵ��TsID����
		stdParse.GetAttrNode( RecordNode,"ServiceID",ServiceID );  //��ȡ��׼¼��XML��RecordSetTask�ڵ��ServiceID����
		stdParse.GetAttrNode( RecordNode,"VideoPID",VideoPID );	   //��ȡ��׼¼��XML��RecordSetTask�ڵ��VideoPID����
		stdParse.GetAttrNode( RecordNode,"AudioPID",AudioPID );	   //��ȡ��׼¼��XML��RecordSetTask�ڵ��AudioPID����
		stdParse.GetAttrNode( RecordNode,"DataType",DataType );	   //��ȡ��׼¼��XML��RecordSetTask�ڵ��DataType����
		stdParse.GetAttrNode( RecordNode,"Width",Width );		   //��ȡ��׼¼��XML��RecordSetTask�ڵ��Width����
		stdParse.GetAttrNode( RecordNode,"Height",Height );		   //��ȡ��׼¼��XML��RecordSetTask�ڵ��Height����
		stdParse.GetAttrNode( RecordNode,"Fps",Fps );			   //��ȡ��׼¼��XML��RecordSetTask�ڵ��Fps����
		stdParse.GetAttrNode( RecordNode,"Bps",Bps );			   //��ȡ��׼¼��XML��RecordSetTask�ڵ��Bps����
		stdParse.GetAttrNode(RecordNode, "OSDEnable", sOsd);  
		stdParse.GetAttrNode( RecordNode,"ThreeDSet",ThreeDSet);
		
#ifdef ZONG_JU_ZHAO_BIAO
		std::string strCodingFormat;
		stdParse.GetAttrNode( RecordNode,"CodingFormat",strCodingFormat);
#endif
		
		enumDVBType dvbtype = OSFunction::GetEnumDVBType(DownType);

		std::string HDTV="0";
		if(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")
		{
			if (StrUtil::Str2Int(Width)>720||StrUtil::Str2Int(Height)>576)
			{
				HDTV="1";
			}
		}
		else
		{
			if (StrUtil::Str2Int(Width)>640||StrUtil::Str2Int(Height)>480)
			{
				HDTV="1";
			}
		}
		if (Freq != strHDMIFreq)
		{
			//add by gxd 2010-10-13
			CHANNELINFOMGR::instance()->GetProName(dvbtype,Freq,ServiceID,ProName);
			CHANNELINFOMGR::instance()->GetSymbolRateQAMByFreq(dvbtype,Freq,SymbolRate,QAM);
			if (ProName.empty())
			{
				ProName = "δ֪Ƶ��";
			}
		}
		else
		{
			ProName = "������";
		}

		OSDInfo osd;
		eDVBType etype=OSFunction::GetEnumDVBType(DownType);
		PROPMANAGER::instance()->GetOSDInfo(etype,HDTV,osd);
		if(Freq.find("259000")!=-1&&HDTV=="1")//����ֱ��ʷ�960x528���⴦��
		{
			osd.InfoX = StrUtil::Int2Str(550);
			osd.TimeX = StrUtil::Int2Str(450);
		}

		VideoParam param;
		if(HDTV=="1")
			PROPMANAGER::instance()->GetHDRecordVideoParam(dvbtype,param);
		else if(HDTV=="0")
			PROPMANAGER::instance()->GetSDRecordVideoParam(dvbtype,param);

		string audiobps="";
		PROPMANAGER::instance()->GetAudioBps(dvbtype,audiobps);

		if(Width.length()<1)
			Width = param.width;
		if(Height.length()<1)
			Height = param.height;
		if(Bps.length()<1)
			Bps = StrUtil::Int2Str(StrUtil::Str2Int(param.bps)*1000);
		int LBps = StrUtil::Str2Int(Bps);
		if(dvbtype==RADIO||dvbtype==AM)
		{
			Bps = StrUtil::Int2Str(LBps/1000);
		}
		else
		{
			Bps = StrUtil::Int2Str(LBps/1000-StrUtil::Str2Int(audiobps));
			if(StrUtil::Str2Int(Bps)<150&&HDTV=="0")
			{	
				Bps = "150";
			}
#ifndef ZONG_JU_ZHAO_BIAO
			else if(StrUtil::Str2Int(Bps)<500&&HDTV=="1")
			{
				Bps = "500";
			}
#endif
			else if(StrUtil::Str2Int(Bps)>2000&&HDTV=="0")
			{
				Bps = "2000";
			}
			else if(StrUtil::Str2Int(Bps)>4000&&HDTV=="1")
			{
				Bps = "4000";
			}
		}

		//����Ts����ѯԪ��ָ��
		fetchParse.SetAttrNode( "Version",Version,tsFetchNode );              //����TsQuery�ڵ��Freq����
		fetchParse.SetAttrNode( "TDeviceID",TunerDeviceID,tsFetchNode );	    //����TsQuery�ڵ��OrgNetID����
		fetchParse.SetAttrNode( "CDeviceID",CoderDeviceID,tsFetchNode);			    //����TsQuery�ڵ��TsID����
		fetchParse.SetAttrNode( "Freq",Freq,tsFetchNode );              //����TsQuery�ڵ��Freq����
		fetchParse.SetAttrNode( "OrgNetID",OrgNetID,tsFetchNode );	    //����TsQuery�ڵ��OrgNetID����
		fetchParse.SetAttrNode( "TsID",TsID,tsFetchNode);			    //����TsQuery�ڵ��TsID����
		fetchParse.SetAttrNode("SymbolRate",SymbolRate,tsFetchNode);   //����TsQuery�ڵ��SymbolRate����
		fetchParse.SetAttrNode("QAM",QAM,tsFetchNode);                 //����TsQuery�ڵ��QAM����
		fetchParse.SetAttrNode( "ServiceID",ServiceID,tsFetchNode );    //����TsQuery�ڵ��ServiceID����
		
		//add by gxd 2010-10-13
		fetchParse.SetAttrNode("ServiceName",ProName,tsFetchNode);
		if (Freq != strHDMIFreq)
		{
			CHANNELINFOMGR::instance()->GetPmtPidPcrPid(dvbtype,Freq,ServiceID,PmtPid,PcrPid);
		}
		fetchParse.SetAttrNode("PmtPID",PmtPid,tsFetchNode);
		fetchParse.SetAttrNode("PcrPID",PcrPid,tsFetchNode);

		fetchParse.SetAttrNode( "VideoPID",VideoPID,tsFetchNode );	    //����TsQuery�ڵ��VideoPID����
		fetchParse.SetAttrNode( "AudioPID",AudioPID,tsFetchNode );	    //����TsQuery�ڵ��AudioPID����
		fetchParse.SetAttrNode( "DataType",DataType,tsFetchNode );	    //����TsQuery�ڵ��DataType����
		fetchParse.SetAttrNode( "Width",Width,tsFetchNode );		    //����TsQuery�ڵ��Width����
		fetchParse.SetAttrNode( "Height",Height,tsFetchNode );		    //����TsQuery�ڵ��Height����
		fetchParse.SetAttrNode( "Fps",Fps,tsFetchNode );			    //����TsQuery�ڵ��Fps����
		fetchParse.SetAttrNode( "Bps",Bps,tsFetchNode );			    //����TsQuery�ڵ��Bps����
		fetchParse.SetAttrNode("OSDEnable", sOsd, tsFetchNode);  
		if(dvbtype == THREED)
			fetchParse.SetAttrNode("ThreeDSet",ThreeDSet,tsFetchNode);

#ifdef ZONG_JU_ZHAO_BIAO
		fetchParse.SetAttrNode("CodingFormat",strCodingFormat,tsFetchNode);
#endif
		fetchParse.SetAttrNode( "HDTV",HDTV,tsFetchNode);
		fetchParse.SetAttrNode( "IsChangeFreq",string("0"),tsFetchNode);
		fetchParse.SetAttrNode("SystemClock",(long)time(0),tsFetchNode);
		

		fetchParse.SetAttrNode("OSDFontSize",osd.FontSize,tsFetchNode);
		fetchParse.SetAttrNode("OSDInfo",osd.Info,tsFetchNode);
		fetchParse.SetAttrNode("OSDInfoX",osd.InfoX,tsFetchNode);
		fetchParse.SetAttrNode("OSDInfoY",osd.InfoY,tsFetchNode);
		fetchParse.SetAttrNode("OSDTimeType",osd.TimeType,tsFetchNode);
		fetchParse.SetAttrNode("OSDTimeX",osd.TimeX,tsFetchNode);
		fetchParse.SetAttrNode("OSDTimeY",osd.TimeY,tsFetchNode);
		fetchParse.SetAttrNode("OSDProgramX",osd.ProgramX,tsFetchNode);
		fetchParse.SetAttrNode("OSDProgramY",osd.ProgramY,tsFetchNode);
	}
	catch( ...)
	{ 
		string error = "����ת����Ӳ��XML�ĵ�����ʧ��";
		APPLOG::instance()->WriteLog( OTHER,LOG_EVENT_ERROR,error,LOG_OUTPUT_BOTH );
		return  errorXML;
	}
	fetchParse.Get_xml( strHardXML );
	return strHardXML;
}

std::vector<std::string> TranslateXMLForDevice::TranslateRoundStreamXML( std::string strStdXML )
{
	string DownType,Version;//��Ϣ����
	vector< string > vecRtnXML;
	XmlParser parser;
	parser.Set_xml(strStdXML);
	pXMLNODE rootNode=parser.GetRootNode();//���ڵ�msg

	parser.GetAttrNode(rootNode,"DVBType",DownType);//���msg�ڵ��DownType����ֵ
	parser.GetAttrNode(rootNode,"Version",Version);

	pXMLNODE streamRoundQueryNode=parser.GetNodeFromPath("Msg/StreamRoundQuery");
	pXMLNODELIST streamRoundQueryNodeList=parser.GetNodeList(streamRoundQueryNode);

#ifdef ZONG_JU_ZHAO_BIAO
	std::string strCodingFormat, strWindowNum;
	parser.GetAttrNode( streamRoundQueryNode,"CodingFormat",strCodingFormat);
	parser.GetAttrNode( streamRoundQueryNode,"WindowNumber",strWindowNum);
#endif

	for (int i=0;i!=streamRoundQueryNodeList->Size();++i)
	{
		XmlParser rtnParser;
		string Freq,OrgNetID,TsID,ServiceID,VideoPID,AudioPID,DataType,ProName,SymbolRate,QAM,PmtPid,PcrPid,isosd;
		string Width,Height,Fps,Bps,ThreeDSet;

		rtnParser.Set_xml(GetXMLHeader(DownType));
		pXMLNODE ChannelNode=parser.GetNextNode(streamRoundQueryNodeList);
		string NodeName = parser.GetNodeName(ChannelNode);
		if(NodeName == "RoundChannel")
		{
			parser.GetAttrNode( ChannelNode,"Freq",Freq );
			parser.GetAttrNode( ChannelNode,"OrgNetID",OrgNetID );
			parser.GetAttrNode( ChannelNode,"TsID",TsID );
			parser.GetAttrNode( ChannelNode,"ServiceID",ServiceID );
			parser.GetAttrNode( ChannelNode,"VideoPID",VideoPID );
			parser.GetAttrNode( ChannelNode,"AudioPID",AudioPID );
			parser.GetAttrNode( ChannelNode,"DataType",DataType );
			parser.GetAttrNode( ChannelNode,"Width",Width );
			parser.GetAttrNode( ChannelNode,"Height",Height );
			parser.GetAttrNode( ChannelNode,"Fps",Fps );
			parser.GetAttrNode( ChannelNode,"Bps",Bps );
			parser.GetAttrNode( ChannelNode,"OSDEnable",isosd);
			parser.GetAttrNode( ChannelNode,"ThreeDSet",ThreeDSet);

			enumDVBType dvbtype = OSFunction::GetEnumDVBType(DownType);

			std::string HDTV="0";
			if(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")
			{
				if (StrUtil::Str2Int(Width)>720||StrUtil::Str2Int(Height)>576)
				{
					HDTV="1";
				}
			}
			else
			{
				if (StrUtil::Str2Int(Width)>640||StrUtil::Str2Int(Height)>480)
				{
					HDTV="1";
				}
			}

			OSDInfo osd;
			eDVBType etype=OSFunction::GetEnumDVBType(DownType);
			PROPMANAGER::instance()->GetOSDInfo(etype,HDTV,osd);

			//add by gxd 2010-10-13
			CHANNELINFOMGR::instance()->GetProName(dvbtype,Freq,ServiceID,ProName);
			CHANNELINFOMGR::instance()->GetSymbolRateQAMByFreq(dvbtype,Freq,SymbolRate,QAM);
			if (ProName.empty())
			{
				ProName = "δ֪Ƶ��";
			}

			VideoParam param;
			if(HDTV=="1")
				PROPMANAGER::instance()->GetHDRealStreamVideoParam(dvbtype,param);
			else if(HDTV=="0")
				PROPMANAGER::instance()->GetSDRealStreamVideoParam(dvbtype,param);

			string audiobps="";
			PROPMANAGER::instance()->GetAudioBps(dvbtype,audiobps);

			if(Width.length()<1)
				Width = param.width;
			if(Height.length()<1)
				Height = param.height;
			if(Bps.length()<1)
				Bps = StrUtil::Int2Str(StrUtil::Str2Int(param.bps)*1000);

			int LBps = StrUtil::Str2Int(Bps);

			Bps = StrUtil::Int2Str(LBps/1000-StrUtil::Str2Int(audiobps));

			if(StrUtil::Str2Int(Bps)<150&&HDTV=="0")
			{	
				Bps = "150";
			}
	#ifndef ZONG_JU_ZHAO_BIAO
			else if(StrUtil::Str2Int(Bps)<500&&HDTV=="1")
			{
				Bps = "500";
			}
	#endif
			else if(StrUtil::Str2Int(Bps)>2000&&HDTV=="0")
			{
				Bps = "2000";
			}
			else if(StrUtil::Str2Int(Bps)>4000&&HDTV=="1")
			{
				Bps = "4000";
			}
			
			pXMLNODE tsFetchNode=rtnParser.CreateNodePtr("Msg","TSQuery");
			pXMLNODE tsNode=rtnParser.CreateNodePtr(tsFetchNode,"TS");
			rtnParser.SetAttrNode( "Freq",Freq,tsNode );
			rtnParser.SetAttrNode( "OrgNetID",OrgNetID,tsNode );
			rtnParser.SetAttrNode( "TsID",TsID,tsNode);
			rtnParser.SetAttrNode("SymbolRate",SymbolRate,tsNode);
			rtnParser.SetAttrNode("QAM",QAM,tsNode);
			rtnParser.SetAttrNode( "ServiceID",ServiceID,tsNode );
			rtnParser.SetAttrNode("Version",Version,tsNode);

			//add by gxd 2010-10-13
			rtnParser.SetAttrNode("ServiceName",ProName,tsNode);

			CHANNELINFOMGR::instance()->GetPmtPidPcrPid(dvbtype,Freq,ServiceID,PmtPid,PcrPid);
			rtnParser.SetAttrNode("PmtPID",PmtPid,tsNode);
			rtnParser.SetAttrNode("PcrPID",PcrPid,tsNode);
			rtnParser.SetAttrNode( "VideoPID",VideoPID,tsNode );
			rtnParser.SetAttrNode( "AudioPID",AudioPID,tsNode );
			rtnParser.SetAttrNode( "DataType",DataType,tsNode );
			rtnParser.SetAttrNode( "Width",Width,tsNode );
			rtnParser.SetAttrNode( "Height",Height,tsNode );
			rtnParser.SetAttrNode( "OSDEnable",isosd,tsNode );
			rtnParser.SetAttrNode( "Fps",Fps,tsNode );
			rtnParser.SetAttrNode( "Bps",Bps,tsNode );
			if(dvbtype == THREED)
				rtnParser.SetAttrNode("ThreeDSet",ThreeDSet,tsNode);

	#ifdef ZONG_JU_ZHAO_BIAO
			rtnParser.SetAttrNode( "CodingFormat",strCodingFormat,tsNode);
			rtnParser.SetAttrNode( "WindowNumber",strWindowNum,tsNode);
	#endif
			rtnParser.SetAttrNode( "HDTV",HDTV,tsNode);
			rtnParser.SetAttrNode( "IsChangeFreq",string("1"),tsNode);
			rtnParser.SetAttrNode("SystemClock",(long)time(0),tsNode);
			
			
			rtnParser.SetAttrNode("OSDFontSize",osd.FontSize,tsNode);
			rtnParser.SetAttrNode("OSDInfo",osd.Info,tsNode);
			rtnParser.SetAttrNode("OSDInfoX",osd.InfoX,tsNode);
			rtnParser.SetAttrNode("OSDInfoY",osd.InfoY,tsNode);
			rtnParser.SetAttrNode("OSDTimeType",osd.TimeType,tsNode);
			rtnParser.SetAttrNode("OSDTimeX",osd.TimeX,tsNode);
			rtnParser.SetAttrNode("OSDTimeY",osd.TimeY,tsNode);
			rtnParser.SetAttrNode("OSDProgramX",osd.ProgramX,tsNode);
			rtnParser.SetAttrNode("OSDProgramY",osd.ProgramY,tsNode);
			string xml;
			rtnParser.Get_xml(xml);
			string streamtype;
			CHANNELINFOMGR::instance()->GetVideoStreamType(dvbtype,Freq,ServiceID,VideoPID,AudioPID,streamtype);
			if(streamtype != "27")//�ֲ��Ͷ໭�治����H264
			{
				vecRtnXML.push_back(xml);
			}
		}
	}

	return vecRtnXML;	
}

std::vector<std::string> TranslateXMLForDevice::TranslateRoundRecordXML(std::string strStdXML)
{
	string downType;
	string Freq,OrgNetID,TsID,ServiceID,VideoPID,AudioPID,DataType,ProName,SymbolRate,QAM,PmtPid,PcrPid;
	string Width,Height,Fps,Bps;
	string DownType;//��Ϣ����
	vector< string > vecRtnXML;
	XmlParser parser;
	parser.Set_xml(strStdXML);
	pXMLNODE rootNode=parser.GetRootNode();//���ڵ�msg

	parser.GetAttrNode(rootNode,"DVBType",DownType);//���msg�ڵ��DownType����ֵ

	pXMLNODE streamRoundQueryNode=parser.GetNodeFromPath("Msg/RoundRecord");
	pXMLNODELIST streamRoundQueryNodeList=parser.GetNodeList(streamRoundQueryNode);

	for (int i=0;i!=streamRoundQueryNodeList->Size();++i)
	{
		XmlParser rtnParser;
		rtnParser.Set_xml(GetXMLHeader(DownType));
		pXMLNODE ChannelNode=parser.GetNextNode(streamRoundQueryNodeList);
		parser.GetAttrNode( ChannelNode,"Freq",Freq );
		parser.GetAttrNode( ChannelNode,"OrgNetID",OrgNetID );
		parser.GetAttrNode( ChannelNode,"TsID",TsID );
		parser.GetAttrNode( ChannelNode,"ServiceID",ServiceID );
		parser.GetAttrNode( ChannelNode,"VideoPID",VideoPID );
		parser.GetAttrNode( ChannelNode,"AudioPID",AudioPID );
		parser.GetAttrNode( ChannelNode,"DataType",DataType );
		parser.GetAttrNode( ChannelNode,"Width",Width );
		parser.GetAttrNode( ChannelNode,"Height",Height );
		parser.GetAttrNode( ChannelNode,"Fps",Fps );
		parser.GetAttrNode( ChannelNode,"Bps",Bps );


		enumDVBType dvbtype = OSFunction::GetEnumDVBType(DownType);
		std::string HDTV="0";
		if(PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")
		{
			if (StrUtil::Str2Int(Width)>720||StrUtil::Str2Int(Height)>576)
			{
				HDTV="1";
			}
		}
		else
		{
			if (StrUtil::Str2Int(Width)>640||StrUtil::Str2Int(Height)>480)
			{
				HDTV="1";
			}
		}

		//add by gxd 2010-10-13
		CHANNELINFOMGR::instance()->GetProName(dvbtype,Freq,ServiceID,ProName);
		CHANNELINFOMGR::instance()->GetSymbolRateQAMByFreq(dvbtype,Freq,SymbolRate,QAM);
		if (ProName.empty())
		{
			ProName = "δ֪Ƶ��";
		}

		OSDInfo osd;
		eDVBType etype=OSFunction::GetEnumDVBType(DownType);
		PROPMANAGER::instance()->GetOSDInfo(etype,HDTV,osd);

		VideoParam param;
		if(HDTV=="1")
			PROPMANAGER::instance()->GetHDRecordVideoParam(dvbtype,param);
		else if(HDTV=="0")
			PROPMANAGER::instance()->GetSDRecordVideoParam(dvbtype,param);

		string audiobps="";
		PROPMANAGER::instance()->GetAudioBps(dvbtype,audiobps);

		if(Width.length()<1)
			Width = param.width;
		if(Height.length()<1)
			Height = param.height;
		if(Bps.length()<1)
			Bps = StrUtil::Int2Str(StrUtil::Str2Int(param.bps)*1000);
		int LBps = StrUtil::Str2Int(Bps);

		Bps = StrUtil::Int2Str(LBps/1000-StrUtil::Str2Int(audiobps));

		if(StrUtil::Str2Int(Bps)<150&&HDTV=="0")
		{	
			Bps = "150";
		}
		else if(StrUtil::Str2Int(Bps)<500&&HDTV=="1")
		{
			Bps = "500";
		}
		else if(StrUtil::Str2Int(Bps)>2000&&HDTV=="0")
		{
			Bps = "2000";
		}
		else if(StrUtil::Str2Int(Bps)>4000&&HDTV=="1")
		{
			Bps = "4000";
		}

		pXMLNODE tsFetchNode=rtnParser.CreateNodePtr("Msg","TSQuery");
		pXMLNODE tsNode=rtnParser.CreateNodePtr(tsFetchNode,"TS");
		rtnParser.SetAttrNode( "Freq",Freq,tsNode );
		rtnParser.SetAttrNode( "OrgNetID",OrgNetID,tsNode );
		rtnParser.SetAttrNode( "TsID",TsID,tsNode);
		rtnParser.SetAttrNode("SymbolRate",SymbolRate,tsNode);
		rtnParser.SetAttrNode("QAM",QAM,tsNode);
		rtnParser.SetAttrNode( "ServiceID",ServiceID,tsNode );

		//add by gxd 2010-10-13
		rtnParser.SetAttrNode("ServiceName",ProName,tsNode);

		CHANNELINFOMGR::instance()->GetPmtPidPcrPid(dvbtype,Freq,ServiceID,PmtPid,PcrPid);
		rtnParser.SetAttrNode("PmtPID",PmtPid,tsNode);
		rtnParser.SetAttrNode("PcrPID",PcrPid,tsNode);
		rtnParser.SetAttrNode( "VideoPID",VideoPID,tsNode );
		rtnParser.SetAttrNode( "AudioPID",AudioPID,tsNode );
		rtnParser.SetAttrNode( "DataType",DataType,tsNode );
		rtnParser.SetAttrNode( "Width",Width,tsNode );
		rtnParser.SetAttrNode( "Height",Height,tsNode );
		rtnParser.SetAttrNode( "Fps",Fps,tsNode );
		rtnParser.SetAttrNode( "Bps",Bps,tsNode );
		rtnParser.SetAttrNode( "HDTV",HDTV,tsNode);
		rtnParser.SetAttrNode( "IsChangeFreq",string("1"),tsNode);
		rtnParser.SetAttrNode("SystemClock",(long)time(0),tsNode);

		rtnParser.SetAttrNode("OSDFontSize",osd.FontSize,tsNode);
		rtnParser.SetAttrNode("OSDInfo",osd.Info,tsNode);
		rtnParser.SetAttrNode("OSDInfoX",osd.InfoX,tsNode);
		rtnParser.SetAttrNode("OSDInfoY",osd.InfoY,tsNode);
		rtnParser.SetAttrNode("OSDTimeType",osd.TimeType,tsNode);
		rtnParser.SetAttrNode("OSDTimeX",osd.TimeX,tsNode);
		rtnParser.SetAttrNode("OSDTimeY",osd.TimeY,tsNode);
		rtnParser.SetAttrNode("OSDProgramX",osd.ProgramX,tsNode);
		rtnParser.SetAttrNode("OSDProgramY",osd.ProgramY,tsNode);
		string xml;
		rtnParser.Get_xml(xml);
		vecRtnXML.push_back(xml);
	}
	return vecRtnXML;
}
//����ָ���׼XMLת��Ϊ��Ӳ��ͨѶ��XML
 std::vector<std::string> TranslateXMLForDevice::TranslateQualityTask(std::string strStdXML)
 {
	 XmlParser parser;
	 parser.Set_xml(strStdXML);
	 vector<std::string> retVec;
	  std::string DownType;
	 pXMLNODE stdRootNode=parser.GetRootNode();//���ڵ�msg
	 parser.GetAttrNode(stdRootNode,"DVBType",DownType);
	 enumDVBType dvbtype = OSFunction::GetEnumDVBType(DownType);

	 pXMLNODE qualityTaskNode = parser.GetNodeFromPath("Msg/QualityTask/TaskParm");
	 pXMLNODELIST NodeList = parser.GetNodeList(qualityTaskNode);
	 for(int i=0;i<NodeList->Size();i++)
	 {
		 char * cheader= "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
		 XmlParser tmpparser;
		 tmpparser.Set_xml(cheader);
		
		 std::string strxml;
		 pXMLNODE rootNode = tmpparser.GetRootNode();

		 tmpparser.SetAttrNode("DVBType",DownType,rootNode);
		 pXMLNODE NextNode=parser.GetNextNode(NodeList);
		 std::string STD,freq,SymbolRate,QAM;
		 parser.GetAttrNode(NextNode,"STD",STD);
		 parser.GetAttrNode(NextNode,"Freq",freq);
		 //parser.GetAttrNode(NextNode,"SymbolRate",SymbolRate);
		 CHANNELINFOMGR::instance()->GetSymbolRateQAMByFreq(dvbtype,freq,SymbolRate,QAM);

		 pXMLNODE QueryNode= tmpparser.CreateNodePtr(rootNode,"QualityQuery");
		 tmpparser.SetAttrNode("STD",STD,QueryNode);
		 tmpparser.SetAttrNode("Freq",freq,QueryNode);
		 tmpparser.SetAttrNode("SymbolRate",SymbolRate,QueryNode);
		 tmpparser.SetAttrNode("QAM",QAM,QueryNode);
		 pXMLNODE ParamNode = tmpparser.CreateNodePtr(rootNode,"QualityParam");

		 pXMLNODE qualityParamNode = parser.GetNodeFromPath("Msg/QualityTask/QualityParam");
		 pXMLNODELIST ParamList = parser.GetNodeList(qualityParamNode);

		 for(int k=0;k<ParamList->Size();k++)
		 {
			 std::string Type,Desc,Value;
			 pXMLNODE IndexNode = parser.GetNextNode(ParamList);
			 parser.GetAttrNode(IndexNode,"Type",Type);
			 parser.GetAttrNode(IndexNode,"Desc",Desc);

			 pXMLNODE tmpIndexNode = tmpparser.CreateNodePtr(ParamNode,"QualityIndex");
			 tmpparser.SetAttrNode("Type",Type,tmpIndexNode);
			 tmpparser.SetAttrNode("Desc",Desc,tmpIndexNode);
			 tmpparser.SetAttrNode("Value",Value,tmpIndexNode);

		 }

		 tmpparser.SaveToString(strxml);
		 retVec.push_back(strxml);

	 }
	 return retVec;
 }

 std::string TranslateXMLForDevice::TranslateSpectrumScan(std::string strStdXML)
 {
	 float mStartFreq=0.0f;               //Ƶ��ɨ��Ŀ�ʼƵ��
	 float mEndFreq=0.0f;                 //Ƶ��ɨ��Ľ���Ƶ��
	 float mStepFreq=0.0f;                //Ƶ��ɨ��Ĳ���
	 float mNowStartFreq=0.0f;            //��ǰƵ�׶εĿ�ʼƵ��
	 float mNowEndFreq=0.0f;              //��ǰƵ�׶εĽ���Ƶ��
	 float mNowFreq=0.0f;                 //��ǰƵ�׶��ڵĵ�ǰ��Ƶ��
	 int mIntevalLen;                //ÿ�εĳ���
	 string mTaskType;

	 string DvbType,STD,SymbolRate;
	 string templen("0");
	 XmlParser parser(strStdXML.c_str());
	 pXMLNODE rootNode=parser.GetRootNode();
	 parser.GetAttrNode(rootNode,"DVBType",DvbType);
	 pXMLNODE SpectrumNode=parser.GetNodeFirstChild(rootNode);
	 parser.GetAttrNode(SpectrumNode,"StartFreq",mStartFreq);
	 parser.GetAttrNode(SpectrumNode,"EndFreq",mEndFreq);
	 parser.GetAttrNode(SpectrumNode,"StepFreq",mStepFreq);
	 parser.GetAttrNode(SpectrumNode,"Steps",templen);
	 parser.GetAttrNode(SpectrumNode,"TaskType",mTaskType);
	 parser.GetAttrNode(SpectrumNode,"STD",STD);
	 parser.GetAttrNode(SpectrumNode,"SymbolRate",SymbolRate);

	 if(mStepFreq<=0)
	 {
		 mTaskType = "1";
		 mStepFreq = 8;
	 }
	 mIntevalLen = (int)StrUtil::Str2Int(templen);
	 if (mEndFreq < mStartFreq)
	 {
		 float temp = mEndFreq;
		 mEndFreq = mStartFreq;
		 mStartFreq = temp;
	 }
	 mNowStartFreq = mStartFreq;
	 mNowEndFreq = mNowStartFreq + mIntevalLen*mStepFreq;
	 if(mNowEndFreq > mEndFreq)
	 {
		 mNowEndFreq = mEndFreq;
	 }
	 mNowFreq = mNowStartFreq;

	 vector<SpectrumInfo> SpectrumVec;
	 if(DvbType=="ATV" || DvbType=="CTV")
	 {
		 while((mNowStartFreq <= mNowEndFreq)&&(mNowStartFreq <=mEndFreq))
		 {
			 mNowFreq = mNowStartFreq;
			 if ((!::stricmp(mTaskType.c_str(),"1")))
			 {
				 mNowFreq = AtvTcpDeviceAccess::GetOpenTVStandardFreq(mNowFreq);
			 }

			 SpectrumInfo Stemp;
			 Stemp.freq = mNowFreq;
			 Stemp.level = 0;
			 SpectrumVec.push_back(Stemp);

			 if (mNowFreq == 464.25)
			 {
				 mNowStartFreq =mNowFreq + mStepFreq -1;	//��ֹ©��471.25
			 }
			 else
			 {
				 mNowStartFreq =mNowFreq + mStepFreq;
			 }

			 if(mNowStartFreq > mEndFreq)
			 {
				 break;
			 }
			 if(mNowStartFreq <= mEndFreq)
			 {	
				 mNowEndFreq = mNowStartFreq + mIntevalLen*mStepFreq;
				 if(mNowEndFreq >= mEndFreq)
				 {
					 mNowEndFreq = mEndFreq;
				 }
			 }
		 }
	 }
	 else if(DvbType=="RADIO")
	 {
		 int i=0;
		 while((mNowStartFreq <= mNowEndFreq)&&(mNowStartFreq <= mEndFreq))
		 {
			 SpectrumInfo Stemp;
			 if(i==0)
			 {
				 mNowStartFreq= mNowFreq + 3.66*0.511;//Ƶ������
			 }
			 else
			 {
				 mNowStartFreq= mNowFreq + 3.66*1.023;
			 }
				
			 i++;

			 Stemp.freq = mNowStartFreq;
			 Stemp.level = 0;
			 SpectrumVec.push_back(Stemp);
			 if(mNowStartFreq > mEndFreq)
				 break;
			 mNowFreq=mNowStartFreq;

			 mNowEndFreq = mNowStartFreq + mIntevalLen*mStepFreq;
		 }
	 }
	 else if(DvbType=="AM")
	 {
		 mNowFreq=mStartFreq + 10;
		 while(mNowFreq<=mEndFreq)
		 {
			 SpectrumInfo Stemp;
			 Stemp.freq = mNowFreq;
			 Stemp.level = 0;
			 SpectrumVec.push_back(Stemp);
			 mNowFreq += (mStepFreq*20);
		 }
	 }
	 XmlParser retParser;
	 retParser.Set_xml(GetXMLHeader(DvbType));

	 pXMLNODE DevRootNode=retParser.GetRootNode();
	 pXMLNODE QuaryNode=retParser.CreateNodePtr(DevRootNode,"SpectrumQuery");

	 retParser.SetAttrNode("STD",STD,QuaryNode);
	 retParser.SetAttrNode("SymbolRate",SymbolRate,QuaryNode);
	 retParser.SetAttrNode("StartFreq",mStartFreq,QuaryNode);
	 retParser.SetAttrNode("EndFreq",mEndFreq,QuaryNode);
	 retParser.SetAttrNode("StepFreq",mStepFreq,QuaryNode);

	 pXMLNODE ParamNode=retParser.CreateNodePtr(DevRootNode,"SpectrumParam");

	 int count =SpectrumVec.size();
	 for(int i=0;i<count;i++)
	 {
		 pXMLNODE IndexNode=retParser.CreateNodePtr(ParamNode,"SpectrumIndex");
		 retParser.SetAttrNode("Freq",SpectrumVec[i].freq,IndexNode);
		 retParser.SetAttrNode("Value",SpectrumVec[i].level,IndexNode);
	 }


	 std::string retXML;
	 retParser.SaveToString(retXML);
	 return retXML;
 }

 std::string TranslateXMLForDevice::TranslateSpectrumTask(std::string strStdXML)
 {
	 float mStartFreq=0.0f;               //Ƶ��ɨ��Ŀ�ʼƵ��
	 float mEndFreq=0.0f;                 //Ƶ��ɨ��Ľ���Ƶ��
	 float mStepFreq=0.0f;                //Ƶ��ɨ��Ĳ���
	 float mNowStartFreq=0.0f;            //��ǰƵ�׶εĿ�ʼƵ��
	 float mNowEndFreq=0.0f;              //��ǰƵ�׶εĽ���Ƶ��
	 float mNowFreq=0.0f;                 //��ǰƵ�׶��ڵĵ�ǰ��Ƶ��
	 int mIntevalLen;                //ÿ�εĳ���
	 string mTaskType;

	 string DvbType,STD,SymbolRate;
	 string templen("0");
	 XmlParser parser(strStdXML.c_str());
	 pXMLNODE rootNode=parser.GetRootNode();
	 parser.GetAttrNode(rootNode,"DVBType",DvbType);
	 pXMLNODE SpectrumNode=parser.GetNodeFromPath("Msg/SpectrumTask/TaskParam/SpectrumQueryIndex");
	 parser.GetAttrNode(SpectrumNode,"StartFreq",mStartFreq);
	 parser.GetAttrNode(SpectrumNode,"EndFreq",mEndFreq);
	 parser.GetAttrNode(SpectrumNode,"StepFreq",mStepFreq);
	 parser.GetAttrNode(SpectrumNode,"Steps",templen);
	 parser.GetAttrNode(SpectrumNode,"TaskType",mTaskType);
	 //parser.GetAttrNode(SpectrumNode,"STD",STD);
	 //parser.GetAttrNode(SpectrumNode,"SymbolRate",SymbolRate);

	 if(mStepFreq<=0)
	 {
		 mTaskType = "1";
		 mStepFreq = 8;
	 }
	 mIntevalLen = 1;//(int)StrUtil::Str2Int(templen);
	 if (mEndFreq < mStartFreq)
	 {
		 float temp = mEndFreq;
		 mEndFreq = mStartFreq;
		 mStartFreq = temp;
	 }
	 mNowStartFreq = mStartFreq;
	 mNowEndFreq = mNowStartFreq + mIntevalLen*mStepFreq;
	 if(mNowEndFreq > mEndFreq)
	 {
		 mNowEndFreq = mEndFreq;
	 }
	 mNowFreq = mNowStartFreq;

	 vector<SpectrumInfo> SpectrumVec;
	 if(DvbType=="ATV" || DvbType=="CTV")
	 {
		 while((mNowStartFreq <= mNowEndFreq)&&(mNowStartFreq <=mEndFreq))
		 {
			 mNowFreq = mNowStartFreq;
			 if ((!::stricmp(mTaskType.c_str(),"1")))
			 {
				 mNowFreq = AtvTcpDeviceAccess::GetOpenTVStandardFreq(mNowFreq);
			 }

			 SpectrumInfo Stemp;
			 Stemp.freq = mNowFreq;
			 Stemp.level = 0;
			 SpectrumVec.push_back(Stemp);
			 mNowStartFreq =mNowFreq + mStepFreq;
			 if(mNowStartFreq > mEndFreq)
			 {
				 break;
			 }
			 if(mNowStartFreq <= mEndFreq)
			 {
				 mNowEndFreq = mNowStartFreq + mIntevalLen*mStepFreq;
				 if(mNowEndFreq >= mEndFreq)
				 {
					 mNowEndFreq = mEndFreq;
				 }
			 }
		 }
	 }
	 else if(DvbType=="RADIO")
	 {
		 int i=0;
		 while((mNowStartFreq <= mNowEndFreq)&&(mNowStartFreq <= mEndFreq))
		 {
			 SpectrumInfo Stemp;
			 if(i==0)
			 {
				 mNowStartFreq= mNowFreq + 3.66*0.511;//Ƶ������
			 }
			 else
			 {
				 mNowStartFreq= mNowFreq + 3.66*1.023;
			 }

			 i++;

			 Stemp.freq = mNowStartFreq;
			 Stemp.level = 0;
			 SpectrumVec.push_back(Stemp);
			 if(mNowStartFreq > mEndFreq)
				 break;
			 mNowFreq=mNowStartFreq;

			 mNowEndFreq = mNowStartFreq + mIntevalLen*mStepFreq;
		 }
	 }
	 else if(DvbType=="AM")
	 {
		 int i=0;
		 while((mNowStartFreq <= mNowEndFreq)&&(mNowStartFreq <= mEndFreq))
		 {
			 SpectrumInfo Stemp;
			 if(i==0)
			 {
				 mNowStartFreq= mNowFreq + 10/*3.66*0.511*/;//Ƶ������
			 }
			 else
			 {
				 mNowStartFreq= mNowFreq + 20/*3.66*1.023*/;
			 }

			 i++;

			 Stemp.freq = mNowStartFreq;
			 Stemp.level = 0;
			 SpectrumVec.push_back(Stemp);
			 if(mNowStartFreq > mEndFreq)
				 break;
			 mNowFreq=mNowStartFreq;

			 mNowEndFreq = mNowStartFreq + mIntevalLen*mStepFreq;
		 }
	 }
	 XmlParser retParser;
	 retParser.Set_xml(GetXMLHeader(DvbType));

	 pXMLNODE DevRootNode=retParser.GetRootNode();
	 pXMLNODE QuaryNode=retParser.CreateNodePtr(DevRootNode,"SpectrumQuery");

	 retParser.SetAttrNode("STD",STD,QuaryNode);
	 retParser.SetAttrNode("SymbolRate",SymbolRate,QuaryNode);
	 retParser.SetAttrNode("StartFreq",mStartFreq,QuaryNode);
	 retParser.SetAttrNode("EndFreq",mEndFreq,QuaryNode);
	 retParser.SetAttrNode("StepFreq",mStepFreq,QuaryNode);


	 pXMLNODE ParamNode=retParser.CreateNodePtr(DevRootNode,"SpectrumParam");

	 int count =SpectrumVec.size();
	 for(int i=0;i<count;i++)
	 {
		 pXMLNODE IndexNode=retParser.CreateNodePtr(ParamNode,"SpectrumIndex");
		 retParser.SetAttrNode("Freq",SpectrumVec[i].freq,IndexNode);
		 retParser.SetAttrNode("Value",SpectrumVec[i].level,IndexNode);
	 }

	 std::string retXML;
	 retParser.SaveToString(retXML);
	 return retXML;
 }

 std::vector<std::string> TranslateXMLForDevice::TranslateRoundRoundXML(std::string strStdXML)
 {
	 vector< string > vecRtnXML;
	 return vecRtnXML;
 }