
#include "./AMTcpDeviceAccess.h"
#include "../Foundation/XmlParser.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/OSFunction.h"
#include "../BusinessProcess/ChannelInfoMgr.h"

#include <algorithm>
using namespace std;


AmTcpDeviceAccess::AmTcpDeviceAccess(int deviceid,std::string strIP,int nPort) : TCPDeviceAccess(deviceid, strIP, nPort)
{

}
AmTcpDeviceAccess::~AmTcpDeviceAccess()	
{
	stream.close_reader();
	stream.close_writer();
	stream.dump();
	stream.close();
}
bool AmTcpDeviceAccess::GetQualityRetXML(const std::string& strCmdMsg,std::string& strRetMsg)
{
	XmlParser parser;
	parser.Set_xml(strCmdMsg);
	pXMLNODE root=parser.GetRootNode();
	string dvbtype;
	parser.GetAttrNode(root,"DVBType",dvbtype);
	pXMLNODE childnode=parser.GetNodeFirstChild(root);
	string nodename=parser.GetNodeName(childnode);

	vector<QualityDesc> vecDesc;
	char* source="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser Retparser(source);
	float freq,f_value=0.0f;
	string StrFreq;
	pXMLNODE QuaryNode = parser.GetNodeFirstChild(root);
	parser.GetAttrNode(QuaryNode,"Freq",StrFreq);
	pXMLNODE SrcparamNode = parser.GetNodeFromPath("Msg/QualityParam");
	pXMLNODELIST paramList = parser.GetNodeList(SrcparamNode);
	freq =StrUtil::Str2Float(StrFreq);
	RadioQuaRetMessage_Obj rqr;
	bool bRadioQua=false;
	int ff[7]={0,0,0,0,0,0,0};
	int count = paramList->Size();
	for(int k=0;k<count;k++)
	{
		std::string Type,Desc,Value;
		QualityDesc TempDesc;
		pXMLNODE childNode = parser.GetNextNode(paramList);
		parser.GetAttrNode(childNode,"Type",Type);
		parser.GetAttrNode(childNode,"Desc",Desc);
		parser.GetAttrNode(childNode,"Value",Value);
		int i_type=StrUtil::Str2Int(Type);

		if(dvbtype=="AM")
		{
			if(!bRadioQua)
			{
				if(GetQuality(freq,rqr))
					bRadioQua=true;
			}	
			if(bRadioQua)
			{
				switch(i_type)
				{
				case 1:
					f_value=rqr.level_int;
					if(f_value<6)
						f_value=0.0f;
					break;
				case 2:
					f_value = rqr.dev_int;;
					break;
				case 3:
					f_value=rqr.dF;
					break;
				case 4:
					f_value=6000;
					break;
				case 5:
					f_value=8000;
					break;
				case 6:
					f_value=7000;
					break;
				}
			}
			else
			{
				f_value =0.0f;
			}
		}
		else
		{
			return false;
		}

		ff[k]=(int)f_value;
		TempDesc.Desc=Desc;
		TempDesc.Type=Type;
		TempDesc.Value=StrUtil::Float2Str(f_value);
		vecDesc.push_back(TempDesc);
	}
	pXMLNODE retrootNode=Retparser.GetRootNode();
	Retparser.SetAttrNode("DVBType",dvbtype,retrootNode);

	pXMLNODE retNode=Retparser.CreateNodePtr(retrootNode,"Return");
	Retparser.SetAttrNode("Type",string("QualityQuery"),retNode);
	Retparser.SetAttrNode("Value",string("0"),retNode);
	Retparser.SetAttrNode("Desc",string("�ɹ�"),retNode);
	Retparser.SetAttrNode("Comment",string(""),retNode);

	pXMLNODE reportNode=Retparser.CreateNodePtr(retrootNode,"QualityQueryReport");

	Retparser.SetAttrNode("STD",string(""),reportNode);
	Retparser.SetAttrNode("Freq",StrUtil::Float2Str(freq),reportNode);
	Retparser.SetAttrNode("SymbolRate",string(""),reportNode);

	pXMLNODE paramNode=Retparser.CreateNodePtr(reportNode,"QualityParam");
	for(int i=0;i<count;i++)
	{
		pXMLNODE indexNode=Retparser.CreateNodePtr(paramNode,"QualityIndex");
		Retparser.SetAttrNode("Type",vecDesc[i].Type,indexNode);
		Retparser.SetAttrNode("Desc",vecDesc[i].Desc,indexNode);
		Retparser.SetAttrNode("Value",vecDesc[i].Value,indexNode);
	}

	Retparser.SaveToString(strRetMsg);
	return true;
}

bool AmTcpDeviceAccess::GetSpectrumRetXML(const std::string& strCmdMsg,std::string& strRetMsg)
{
	XmlParser parser;
	parser.Set_xml(strCmdMsg);
	pXMLNODE root=parser.GetRootNode();
	string dvbtype;
	parser.GetAttrNode(root,"DVBType",dvbtype);
	pXMLNODE childnode=parser.GetNodeFirstChild(root);
	string nodename=parser.GetNodeName(childnode);

	vector<SpectrumInfo> SpectrumVec;
	std::string StartFreq,EndFreq,StepFreq;
	pXMLNODE QueryNode = parser.GetNodeFromPath("Msg/SpectrumQuery");
	parser.GetAttrNode(QueryNode,"EndFreq",EndFreq);
	parser.GetAttrNode(QueryNode,"StartFreq",StartFreq);
	parser.GetAttrNode(QueryNode,"StepFreq",StepFreq);
	pXMLNODE ParamNode=parser.GetNodeFromPath("Msg/SpectrumParam");
	pXMLNODELIST NodeList =parser.GetNodeList(ParamNode);
	int ListCount =parser.GetChildCount(ParamNode);
	for(int i=0;i<ListCount;i++)
	{
		OSFunction::Sleep(0,50);
		SpectrumInfo Temp;
		string freq,level;
		pXMLNODE IndexNode=parser.GetNextNode(NodeList);
		parser.GetAttrNode(IndexNode,"Freq",freq);

		if(dvbtype =="AM")
		{
			float ffreq = (float)StrUtil::Str2Float(freq);

			RadioSpecRetMessage_Obj rsr;
			if(GetAMSpec(ffreq,rsr))
			{
				if(rsr.freq>53100)
				{
					for(int tt=-10,i=0; tt<10; ++tt,++i)	//
					{
						float tempfreq = rsr.freq + tt*100;
						if ( tempfreq > ( StrUtil::Str2Float(EndFreq)*100 ) )
							break;
						Temp.freq = tempfreq*10;

						float templevel = (float)rsr.spec[i];
						if (templevel < 0.0f)
							templevel = 1.0f;
						Temp.level = templevel;

						SpectrumVec.push_back(Temp);

					}
				}
			}	
		}
		else
		{
			return false;
		}
	}

	char* source="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";
	XmlParser Retparser(source);

	pXMLNODE retrootNode=Retparser.GetRootNode();
	Retparser.SetAttrNode("DVBType",dvbtype,retrootNode);

	pXMLNODE retNode=Retparser.CreateNodePtr(retrootNode,"Return");
	Retparser.SetAttrNode("Type",string("SpectrumQuery"),retNode);
	Retparser.SetAttrNode("Value",string("0"),retNode);
	Retparser.SetAttrNode("Desc",string("�ɹ�"),retNode);
	Retparser.SetAttrNode("Comment",string(""),retNode);

	pXMLNODE reportNode=Retparser.CreateNodePtr(retrootNode,"SpectrumQueryReport");

	Retparser.SetAttrNode("STD",string(""),reportNode);
	Retparser.SetAttrNode("SymbolRate",string(""),reportNode);

	pXMLNODE paramNode=Retparser.CreateNodePtr(reportNode,"SpectrumParam");

	for(int k=0;k<SpectrumVec.size();k++)
	{
		pXMLNODE indexNode=Retparser.CreateNodePtr(paramNode,"SpectrumIndex");
		Retparser.SetAttrNode("Freq",StrUtil::Float2Str(SpectrumVec[k].freq),indexNode);
		Retparser.SetAttrNode("Value",StrUtil::Float2Str(SpectrumVec[k].level),indexNode);
	}

	Retparser.SaveToString(strRetMsg);
	return true;
}

bool AmTcpDeviceAccess::GetChannelScanRetXML(const std::string& strCmdMsg,std::string& strRetMsg)
{
	string retAMxml="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?>\
					<Msg DVBType=\"AM\" >\
					<Return Type=\"\" Value=\"0\" Desc=\"�ɹ�\" Comment=\"\"/> \
					<ChannelScan></ChannelScan></Msg>";
	XmlParser parser;
	parser.Set_xml(strCmdMsg);
	pXMLNODE root=parser.GetRootNode();
	string dvbtype;
	parser.GetAttrNode(root,"DVBType",dvbtype);
	pXMLNODE childnode=parser.GetNodeFirstChild(root);
	string nodename=parser.GetNodeName(childnode);
	
	if(dvbtype=="AM")
	{
		XmlParser retpaser(retAMxml.c_str());
		pXMLNODE channscannode=parser.GetNodeFirstChild(childnode);
		string strstartfreq,strendfreq;
		parser.GetAttrNode(channscannode,"StartFreq",strstartfreq);
		parser.GetAttrNode(channscannode,"EndFreq",strendfreq);
		float ResultFreq=0.0;
		int centerfreqs[150]={0};
		RadioSpecRetMessage_Obj SpecRetObj[150];
		int len=0,lenth=0;
		int startfreq=StrUtil::Str2Float(strstartfreq)*1000,endfreq=StrUtil::Str2Float(strendfreq)*1000;
		CaculateCenterFreq(startfreq,endfreq,centerfreqs,len);

		vector<float> vecAMSpec;
		for(int i=0;i<len;i++)
		{
			if(SpecScan(centerfreqs[i]/1000,SpecRetObj[i]))
			{
				for (int specNum=0;specNum<20;++specNum)
				{
					vecAMSpec.push_back( (float)SpecRetObj[i].spec[specNum] );
				}

			}
			else
			{
				continue;
			}
		}

		vector<int> vecChanFreq;		
		FindChannels(startfreq, endfreq, vecAMSpec, vecChanFreq);

		pXMLNODE retchannode=retpaser.GetNodeFromPath("Msg/ChannelScan");
		for(int k=0;k<vecChanFreq.size();++k)
		{
			float ResultFreq=((float)(vecChanFreq[k]) / 1000);
			string tmpResult=StrUtil::Float2Str1(ResultFreq);

			pXMLNODE freqnode=retpaser.CreateNodePtr(retchannode,"Channel");
			retpaser.SetAttrNode("Freq",tmpResult,freqnode);
		}
		retpaser.SaveToString(strRetMsg);	
		return true;
	}
	else
	{
		return false;
	}
}

bool AmTcpDeviceAccess::GetTsQueryRetXML(const std::string& strCmdMsg,std::string& strRetMsg)
{
	string retsuccxml="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?>\
					  <Msg DVBType=\"\" >\
					  <Return Type=\"TSQuery\" Value=\"0\" Desc=\"�ɹ�\" Comment=\"\"/> \</Msg>";
	string retfailxml="<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?>\
					  <Msg DVBType=\"\" >\
					  <Return Type=\"TSQuery\" Value=\"1\" Desc=\"ʧ��\" Comment=\"\"/> \</Msg>";

	XmlParser parser;
	parser.Set_xml(strCmdMsg);
	pXMLNODE root=parser.GetRootNode();
	string dvbtype;
	parser.GetAttrNode(root,"DVBType",dvbtype);
	pXMLNODE childnode=parser.GetNodeFirstChild(root);
	string nodename=parser.GetNodeName(childnode);

	pXMLNODE tsnode=parser.GetNodeFromPath("Msg/TSQuery/TS");
	string freq,bps;
	parser.GetAttrNode(tsnode,"Bps",bps);
	parser.GetAttrNode(tsnode,"Freq",freq);

	MediaConfig_Obj media_obj;
	TunerConfig_Obj tuner_obj;

	PROPMANAGER::instance()->GetDeviceTunerID(DeviceId,media_obj.chan);		
	PROPMANAGER::instance()->GetDeviceTunerID(DeviceId,tuner_obj.chan);

	if (dvbtype != string("AM"))
	{
		strRetMsg=retfailxml;
		return false;
	}

	tuner_obj.freq=int(StrUtil::Str2Float(freq));
	media_obj.audio_idx=GetAudioIndex(StrUtil::Str2Int(bps));

	if (TurnFreq(tuner_obj) && SetAudioRate(media_obj))
	{
		string name;
		CHANNELINFOMGR::instance()->GetProNameByFreq(OSFunction::GetEnumDVBType(dvbtype),freq,name);
		SubtitleConfig_Obj osdinfo;

		if ((name.empty() && name.length()< 39))
		{
			memcpy(osdinfo.subtitle0,"δ֪Ƶ��",8);
		}
		else
		{
			memcpy(osdinfo.subtitle0,name.c_str(),name.length());
		}
		OSDInfo osd;
		eDVBType etype=OSFunction::GetEnumDVBType(dvbtype);
		PROPMANAGER::instance()->GetOSDInfo(etype,"0",osd);	//ģ���޸���
		if(osd.Info.length()<1)
			osd.Info="��ʯ.����";
		memcpy(osdinfo.subtitle1,osd.Info.c_str(),40);
		osdinfo.size=StrUtil::Str2Int(osd.FontSize);
		osdinfo.subtitle0_x=StrUtil::Str2Int(osd.ProgramX);
		osdinfo.subtitle0_y=StrUtil::Str2Int(osd.ProgramY);
		osdinfo.subtitle1_x=StrUtil::Str2Int(osd.InfoX);
		osdinfo.subtitle1_y=StrUtil::Str2Int(osd.InfoY);
		osdinfo.time_x=StrUtil::Str2Int(osd.TimeX);
		osdinfo.time_y=StrUtil::Str2Int(osd.TimeY);
		osdinfo.mode=StrUtil::Str2Int(osd.TimeType);
		SetOSD(tuner_obj.chan,osdinfo);
		SetSysTime();
		strRetMsg=retsuccxml;
		return true;
	}
	else
	{
		strRetMsg=retfailxml;
		return false;
	}
	return true;
}

int AmTcpDeviceAccess::SendCmdForSpecScan(MSG_TYPE msg_type,void* info, RadioSpecRetMessage_Handle pRetObj,int infolen)
{
	if ( ConnectToServer() == false)
		return -1;

	int pkgLen = 0;
	char sendbuf[200] = {0}; 
	char RetMsg[2000] = {0};

	//��Ϣͷ
	PkgHeader_Obj   ph;

	ph.header   = 0x53;
	ph.msg_type = msg_type;

	memcpy(sendbuf, &ph, sizeof(PkgHeader_Obj));
	pkgLen	+= sizeof(PkgHeader_Obj);

	//��Ϣ����
	memcpy(sendbuf+pkgLen, info, infolen);
	pkgLen	+= infolen;
	//������Ϣ
	ACE_Time_Value SendTimeOut(10);
	ACE_Time_Value RecvTimeOut (60);

	string sysmsg;

	if(stream.send((char*)sendbuf, pkgLen,&SendTimeOut) <= 0)		//int lenth = send(m_sSocket,(char*)sendbuf, len, 0)
	{
		sysmsg = string("ͨ��[") + StrUtil::Int2Str(mChannelID) + string("]����ָ��ʧ��");
		//SYSMSGSENDER::instance()->SendMsg(sysmsg,AM,VS_MSG_SYSALARM);

		stream.close_reader();
		stream.close_writer();
		stream.dump();
		stream.close();
		return -2;
	}
	sysmsg = string("ͨ��[") + StrUtil::Int2Str(mChannelID) + string("]����ָ��ɹ�");
	//SYSMSGSENDER::instance()->SendMsg(sysmsg,AM);

	//���շ�������
	int lenth=0;
	if((lenth=stream.recv((char *)&RetMsg,sizeof(RadioSpecRetMessage_Obj),&RecvTimeOut) <= 0))
	{
		sysmsg = string("ͨ��[") + strIPAddress +string(":") + StrUtil::Int2Str(mChannelID) + string("]���ݽ���ʧ��");
		//SYSMSGSENDER::instance()->SendMsg(sysmsg,AM,VS_MSG_SYSALARM);

		stream.close_reader();
		stream.close_writer();
		stream.dump();
		stream.close();
		OSFunction::Sleep(0,500);
		return -3;
	}
	sysmsg = string("ͨ��[") + StrUtil::Int2Str(mChannelID) + string("]���ݽ��ճɹ�");
	//SYSMSGSENDER::instance()->SendMsg(sysmsg,AM);

	OSFunction::Sleep(0,100);

	RadioSpecRetMessage_Handle rm = (RadioSpecRetMessage_Handle)(RetMsg);
	
	stream.close_reader();
	stream.close_writer();
	stream.dump();
	stream.close();
	if(rm->ph.header!=0x54 || rm->ph.msg_type!=(msg_type+0x100) || rm->status != 1)
	{
		sysmsg = string("ͨ��[") + StrUtil::Int2Str(mChannelID) + string("]�������ݴ���");
		//SYSMSGSENDER::instance()->SendMsg(sysmsg,AM,VS_MSG_SYSALARM);
		return -4;
	}
	pRetObj->ph=rm->ph;
	pRetObj->freq=rm->freq;
	pRetObj->status=rm->status;
	pRetObj->level_int=rm->level_int;
	memcpy(pRetObj->spec,rm->spec,1024*sizeof(char));

	sysmsg = string("ͨ��[") + StrUtil::Int2Str(mChannelID) + string("]Ƶ��ɨ��ɹ�");
	//SYSMSGSENDER::instance()->SendMsg(sysmsg,AM);

	return 1;
}

int AmTcpDeviceAccess::SendCmdForQuality(MSG_TYPE msg_type,void* info, RadioQuaRetMessage_Handle pRetObj,int infolen)
{
	if ( ConnectToServer() == false)
		return -1;

	{//�Ȳ��n������û�в�������������ʷ���ݣ�û�����ֱ��������һ��
		int n =10;//10���ڶ�����ʷ���� ��Ҫ����Ӳ�����Խ��е���
		std::vector<RadioQuaRetMessage_Obj>::iterator pRQR;
		for(pRQR=m_vecRQRhistory.begin();pRQR!=m_vecRQRhistory.end();pRQR++)
		{//�ж�Ƶ����ͬ�����ָ���б�����ʷ��¼
			if(((*pRQR).freq - pRetObj->freq)<1 && ((*pRQR).freq - pRetObj->freq)>-1)
			{
				if((pRQR->getTime - time(0))>n)
					break;
				else
				{
					*pRetObj = *pRQR;
					/*pRetObj->ph=pRQR->ph;
					pRetObj->dev_int=pRQR->dev_int;
					pRetObj->dF=pRQR->dF;
					pRetObj->freq=pRQR->freq;
					pRetObj->level_int=pRQR->level_int;
					pRetObj->status=pRQR->status;
					pRetObj->getTime=pRQR->getTime;*/
					return 1;
				}
			}
		}
	}
	int pkgLen = 0;
	unsigned char sendbuf[200] = {0}; 
	unsigned char RetMsg[5000] = {0};
	//��Ϣͷ
	PkgHeader_Obj   ph;

	ph.header   = 0x53;
	ph.msg_type = msg_type;

	memcpy(sendbuf, &ph, sizeof(PkgHeader_Obj));
	pkgLen	+= sizeof(PkgHeader_Obj);

	//��Ϣ����
	memcpy(sendbuf+pkgLen, info, infolen);
	pkgLen	+= infolen;

	ACE_Time_Value sendtimeout(8);
	ACE_Time_Value rectimeout(20);

	string sysmsg;
	//������Ϣ
	if(stream.send((char*)sendbuf, pkgLen,&sendtimeout) <= 0)		
	{
		sysmsg = string("ͨ��[") + StrUtil::Int2Str(mChannelID) + string("]����ָ��ʧ��");
		//SYSMSGSENDER::instance()->SendMsg(sysmsg,AM,VS_MSG_SYSALARM);

		stream.close_reader();
		stream.close_writer();
		stream.dump();
		stream.close();
		return -2;
	}
	sysmsg = string("ͨ��[") + StrUtil::Int2Str(mChannelID) + string("]����ָ��ɹ�");
	//SYSMSGSENDER::instance()->SendMsg(sysmsg,AM);

	//���շ�������
	int lenth=0;
	if((lenth=stream.recv(( char *)&RetMsg,sizeof(RetMsg),&rectimeout)) <= 0)
	{
		sysmsg = string("ͨ��[") + strIPAddress +string(":") + StrUtil::Int2Str(mChannelID) + string("]���ݽ���ʧ��");
		//SYSMSGSENDER::instance()->SendMsg(sysmsg,AM,VS_MSG_SYSALARM);

		stream.close_reader();
		stream.close_writer();
		stream.dump();
		stream.close();
		OSFunction::Sleep(0,500);
		return -3;
	}

	sysmsg = string("ͨ��[") + StrUtil::Int2Str(mChannelID) + string("]���ݽ��ճɹ�");
	//SYSMSGSENDER::instance()->SendMsg(sysmsg,AM);

	RadioQuaRetMessage_Handle rm = (RadioQuaRetMessage_Handle)(RetMsg);
	
	stream.close_reader();
	stream.close_writer();
	stream.dump();
	stream.close();
	if(rm->ph.header!=0x54 || rm->ph.msg_type!=(msg_type+0x100) || rm->status != 1)
	{
		sysmsg = string("ͨ��[") + StrUtil::Int2Str(mChannelID) + string("]�������ݴ���");
		//SYSMSGSENDER::instance()->SendMsg(sysmsg,AM,VS_MSG_SYSALARM);

		return -4;
	}
	pRetObj->ph=rm->ph;
	pRetObj->dev_int=rm->dev_int;
	pRetObj->dF=rm->dF;
	pRetObj->freq=rm->freq;
	pRetObj->level_int=rm->level_int;
	pRetObj->status=rm->status;
	pRetObj->getTime = time(0);
	{
		std::vector<RadioQuaRetMessage_Obj>::iterator pRQR;
		bool isUpdate = false;
		for(pRQR=m_vecRQRhistory.begin();pRQR!=m_vecRQRhistory.end();pRQR++)
		{
			if(((*pRQR).freq - pRetObj->freq)<1 && ((*pRQR).freq - pRetObj->freq)>-1)
			{//�ж�Ƶ����ͬ�����ָ���б�����ʷ��¼
				*pRQR = *pRetObj;
				isUpdate = true;
				break;
			}
		}
		if(!isUpdate)
		{//���vec�������ݸ������϶�Ϊ�����ݼ�¼,�����ݲ���vec��
			m_vecRQRhistory.push_back(*pRetObj);
		}
	}
	sysmsg = string("ͨ��[") + StrUtil::Int2Str(mChannelID) + string("]ָ������ɹ�");
	//SYSMSGSENDER::instance()->SendMsg(sysmsg,AM);

	return 1;
}

bool AmTcpDeviceAccess::SpecScan(int freq,RadioSpecRetMessage_Obj& RadioSpec)
{
	Radiopayload_Obj freqinfo;

	freqinfo.flag=0x01;
	freqinfo.freq=freq;
	freqinfo.gain=5;
	int ret=SendCmdForSpecScan(MSG_GET_RADIO,(void *)&freqinfo,&RadioSpec,sizeof(Radiopayload_Obj));
	if(ret>0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void AmTcpDeviceAccess::CaculateCenterFreq(int startfreq,int endfreq,int *centerfreqs,int &len)
{
	int I=(endfreq-startfreq) /(20*1000);

	int middlefreq=startfreq;
	int k=0;
	centerfreqs[0]=startfreq + 10*1000;
	for(k=1;k<=I;k++)
	{
		centerfreqs[k]=(centerfreqs[k-1]+20*1000);
	}

	len = k;
}

void AmTcpDeviceAccess::FindChannels(int startfreq, int endfreq, const vector<float>& vecSpecValue, vector<int>& vecChanFreq)
{
	//ÿ10KHzȡһ����ƽ����Ƶ��
	const int FIND_CHANNEL_STEP =  10;

	//ȡ10����ƽֵ����
	const int TOP_CHANNEL_NUM = 10;

	//20KHz�ڣ�ȥһ��Ƶ�㵱��Ƶ��ɨ����
	const int FREQ_STEP = 20000;

	try
	{
		/** ÿʮ��Ƶ��ȡһ����ƽ��ߵ�
		*/
		std::multimap<float, int, greater<float> > mapLevelFreq;
		float fMaxlevel = -1000.0f;
		int iMaxFreq = -1;
		for (int specNum=0; specNum<vecSpecValue.size(); specNum++)
		{
			if (vecSpecValue[specNum] > fMaxlevel)
			{
				fMaxlevel = vecSpecValue[specNum];
				iMaxFreq = startfreq + specNum*1000;
			}
			if (specNum % 10 == 0 && specNum != 0)
			{
				mapLevelFreq.insert( make_pair(fMaxlevel, iMaxFreq) );		//map�Զ����յ�ƽ����
				fMaxlevel = -1000.0f;
			}

		}
		if (fMaxlevel != -1000.0f)	//���һ�����ֵ����mapLevelFreq��
			mapLevelFreq.insert( make_pair(fMaxlevel, iMaxFreq) );

		//���ȡ������Ƶ�����С��TOP_CHANNEL_NUM������Ƶ�����
		int iChanCount = mapLevelFreq.size();
		if (iChanCount > TOP_CHANNEL_NUM)
			iChanCount = TOP_CHANNEL_NUM;

		/**	������iChanCount��Ƶ�����tempMap��
		*/	
		std::multimap<float, int, greater<float> >::iterator beginIter = mapLevelFreq.begin();
		std::multimap<float, int, greater<float> >::iterator endIter = beginIter;
		//����iter��λ��
		for (int i=0; i<iChanCount; i++)
		{
			if (endIter == mapLevelFreq.end())
				break;

			endIter ++ ;
		}
		std::multimap<float, int, greater<float> > tempMap(beginIter, endIter);

		/**	���mapLevelFreq�л�����tempMap�е�ƽֵ��ͬ�ģ���ȡ��������tempMap��
		*/
		std::multimap<float, int, greater<float> >::iterator getlevelIter = endIter;
		if(beginIter!=endIter)
		{
			getlevelIter--;
			float tempLastLevel = getlevelIter->first;
			while (endIter != mapLevelFreq.end())
			{
				if ((*endIter).first != tempLastLevel)
					break;
				tempMap.insert( *endIter++ );
			}
		}

		/** ÿ��FREQ_STEP������ƽ�͵�Ƶ��
		*/
		std::multimap<float, int, greater<float> >::iterator ChanIter = tempMap.begin();
		for (; ChanIter!=tempMap.end(); ChanIter++)
		{
			std::multimap<float, int, greater<float> >::iterator inerIter = ChanIter;
			inerIter++;

			while ( inerIter != tempMap.end() )
			{
				//Ƶ����20KHz�ڣ���ɾ����ƽֵС��
				if ( abs((ChanIter->second) - (inerIter->second))<= FREQ_STEP )
				{
					tempMap.erase(inerIter ++);
				}
				else
					inerIter ++;
			}
		}

		/**	��������뷵������vecChanFreq��
		*/
		std::multimap<float, int, greater<float> >::iterator inputIter = tempMap.begin();
		for (;inputIter!=tempMap.end();inputIter++)
		{
			vecChanFreq.push_back( (*inputIter).second );
		}

		//��С��������
		sort( vecChanFreq.begin(), vecChanFreq.end() );
	}
	catch (...)
	{
		string sysmsg = string("Ƶ��ɨ���쳣");
		//SYSMSGSENDER::instance()->SendMsg(sysmsg,AM,VS_MSG_SYSALARM);
	}

	return;
}


bool AmTcpDeviceAccess::GetAMSpec(float fFreq, RadioSpecRetMessage_Obj &rqr)
{
	if(fFreq<2610&&fFreq>=531)
	{
		int nfreq = (int)fFreq;//����float to int �������0.2
		if(SpecScan(nfreq,rqr))
		{
			return true;
		}
		else
		{
			string sysmsg = string("�㲥Ƶ��[") + strIPAddress +string(":") + StrUtil::Int2Str(nfreq) + string("]����ʧ��");
			//SYSMSGSENDER::instance()->SendMsg(sysmsg,AM,VS_MSG_SYSALARM);
		}
	}
	return false;
}


bool AmTcpDeviceAccess::GetQuality(float fFreq, RadioQuaRetMessage_Obj &rqr)
{	
	int nfreq = (int)(fFreq);//����float to int �������0.2

	Radiopayload_Obj freqinfo;
	freqinfo.flag = 0x02;
	freqinfo.freq = nfreq;
	freqinfo.gain = 5;
	int ret = SendCmdForQuality(MSG_GET_RADIO,(void *)&freqinfo,&rqr,sizeof(Radiopayload_Obj));
	if(ret>0)
	{
		return true;
	}
	else
	{
		string sysmsg = string("�㲥Ƶ��[") + strIPAddress +string(":")  + StrUtil::Int2Str(nfreq) + string("]����ʧ��");
		//SYSMSGSENDER::instance()->SendMsg(sysmsg,AM,VS_MSG_SYSALARM);
		return false;
	}
	return false;
}


// bool AmTcpDeviceAccess::QueryQuality(int freq,RadioQuaRetMessage_Obj& RadioQua)
// {
// 	Radiopayload_Obj freqinfo;
// 	freqinfo.flag=0x02;
// 	freqinfo.freq=freq;
// 	freqinfo.gain=5;
// 	int ret=SendCmdForQuality(MSG_GET_RADIO,(void *)&freqinfo,&RadioQua,sizeof(Radiopayload_Obj));
// 	if(ret>0)
// 	{
// 		return true;
// 	}
// 	else
// 	{
// 		return false;
// 	}
// 
// }


bool AmTcpDeviceAccess::TurnFreq( TunerConfig_Obj& tunerinfo)
{
	//tunerinfo.freq += 100;		//��Ӧ�°忨��Ƶƫ
	bool rtn = SendCmdToServer(MSG_SET_AMTUNER,(void*)&tunerinfo,sizeof(TunerConfig_Obj));

	string sysmsg;
	if (rtn == false)
	{
		sysmsg = string("ͨ��[") + strIPAddress + string(":") +StrUtil::Int2Str(mChannelID) + string("]��Ƶ��Ƶʧ��[") + \
			StrUtil::Int2Str(tunerinfo.chan) + string(":") + StrUtil::Int2Str(tunerinfo.freq) + string("]") ;
		//SYSMSGSENDER::instance()->SendMsg(sysmsg,AM,VS_MSG_SYSALARM);
	}
	else
	{
		sysmsg = string("ͨ��[")  + strIPAddress + GetDeviceIP() + string(":")+ StrUtil::Int2Str(mChannelID) + string("]��Ƶ��Ƶ�ɹ�[") + \
			StrUtil::Int2Str(tunerinfo.chan) + string(":") + StrUtil::Int2Str(tunerinfo.freq) + string("]") ;
		//SYSMSGSENDER::instance()->SendMsg(sysmsg,AM);
	}

	return rtn;
}


bool AmTcpDeviceAccess::SetAudioRate(const MediaConfig_Obj& rateinfo)
{
	bool rtn = SendCmdToServer(MSG_SET_AUDIOBITRATE,(void*)&rateinfo,sizeof(MediaConfig_Obj));

	string sysmsg;
	if (rtn == false)
	{
		sysmsg = string("ͨ��[") + StrUtil::Int2Str(mChannelID) + string("]������Ƶ����ʧ��[") + \
			StrUtil::Int2Str(rateinfo.chan) + string(":") + StrUtil::Int2Str(rateinfo.video_bitrate)+ \
			string(":") + StrUtil::Int2Str(rateinfo.audio_idx) + string("]") ;

		//SYSMSGSENDER::instance()->SendMsg(sysmsg,AM,VS_MSG_SYSALARM);
	}
	else
	{
		sysmsg = string("ͨ��[") + StrUtil::Int2Str(mChannelID) + string("]������Ƶ���ʳɹ�[") + \
			StrUtil::Int2Str(rateinfo.chan) + string(":") + StrUtil::Int2Str(rateinfo.video_bitrate)+ \
			string(":") + StrUtil::Int2Str(rateinfo.audio_idx) + string("]") ;

		//SYSMSGSENDER::instance()->SendMsg(sysmsg,AM);
	}

	return rtn;
}