


#include "./ChanEncryptQueryTask.h"
#include "./TranslateUpXML.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/OSFunction.h"
#include "../BusinessProcess/ChanEncryptCheckMgr.h"
#include "../Foundation/ParamSetManager.h"
#include "./EncryptCheckTask.h"
#include "../BusinessProcess/BusinessLayoutMgr.h"



ChanEncryptQueryTask::ChanEncryptQueryTask() : DeviceIndependentTask()
{

}


ChanEncryptQueryTask::ChanEncryptQueryTask(std::string strXML) : DeviceIndependentTask(strXML)
{
	
}


ChanEncryptQueryTask::~ChanEncryptQueryTask()
{

}


void ChanEncryptQueryTask::Run( void )
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]С����Ŀ��Ϣ��Ȩ��ѯ����ִ�� !\n",DeviceID));
	bRun = true;

	SetRunning(); 
	//
	CHANENCRYPTCHECKMGR::instance()->ClearEncryptInfo();
	//ת������xml
	std::vector<std::string> vecEncryptXml;
	TranslateEncryptXml( vecEncryptXml );

	RetValue = RUN_SUCCESS;
	for (int n=0; n<vecEncryptXml.size(); n++)
	{
		EncryptCheckTask* pTask = new EncryptCheckTask( vecEncryptXml[n] );
		if (pTask != NULL)
		{
			bool bRet = BUSINESSLAYOUTMGR::instance()->AddTask( pTask );
			if (!bRet)
			{
				ACE_DEBUG ((LM_DEBUG,"(%T| %t) �����Ȩ�������ʧ�� !\n"));
				RetValue = RUN_FAILED;	//�����һ������û����ӳɹ�������ΪС����Ȩ��ѯ����ʧ��
				break;
			}
		}
	}

	//�ȷ��ͷ�����Ϣ��ȷ����ҳ��ϵͳ�������̳ɹ�
	SendXML( TranslateUpXML::TranslateChanEncryptQuery(this) );//����xml

	if (RetValue == RUN_FAILED)
	{
		SetFinised(); 

		ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]С����Ŀ��Ϣ��Ȩ��ѯ����ֹͣ !\n",DeviceID));
		return;
	}

	//�ȴ�С����Ȩ������
	std::string strStartTime = TimeUtil::GetCurDateTime();
	while (true)
	{
		std::string strCurTime = TimeUtil::GetCurDateTime();
		if (TimeUtil::DiffMinute(strCurTime,strStartTime) >= 120)  //FRee_fanrong_20131224 ԭ��ȡ��90
		{
			ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]С����Ŀ��Ϣ��Ȩ��ѯ����ʱ�˳� !\n",DeviceID));
			break;
		}
		//����Ѿ������������С������Ȩ
		if ( CHANENCRYPTCHECKMGR::instance()->GetMapSize() == vecEncryptXml.size() )
		{
			break;
		}

		OSFunction::Sleep( 10 );	//10s ���һ��
	}

	//����С����Ȩ�ļ����
	SendXML( TranslateUpXML::TranslateEncryptResult(this) );//����xml

	SetFinised(); 

	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ͨ��[%d]С����Ŀ��Ϣ��Ȩ��ѯ����ֹͣ !\n",DeviceID));
	return;
}


std::string ChanEncryptQueryTask::GetObjectName()
{
	return std::string("ChanEncryptQueryTask");
}


std::string ChanEncryptQueryTask::GetTaskName()
{
	return std::string("С����Ŀ��Ϣ��Ȩ��ѯ����");
}

bool ChanEncryptQueryTask::TranslateEncryptXml( std::vector<std::string>& vecTaskXml )
{
	//¼���׼��XML�ṹ
	char * source = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\"?><Msg></Msg>";

	XmlParser parser( strStandardXML.c_str());
	XmlParser tempParser( source );

	string downType,version,msgid,protocol, dateTime, priority,srccode,dstcode,srcurl;//ͷ��Ϣ
	//��ȡ��Ԫ��
	pXMLNODE rootNode= parser.GetNodeFromPath( "Msg" );
	parser.GetAttrNode( rootNode,"DVBType",downType );
	parser.GetAttrNode( rootNode,"MsgID",msgid );
	parser.GetAttrNode( rootNode,"Version",version );
	parser.GetAttrNode( rootNode,"Protocol",protocol );
	parser.GetAttrNode( rootNode,"SrcCode",srccode );
	parser.GetAttrNode( rootNode,"DstCode",dstcode );
	parser.GetAttrNode( rootNode,"DateTime",dateTime );
	parser.GetAttrNode( rootNode,"Priority",priority );
	parser.GetAttrNode( rootNode,"SrcURL",srcurl );

	pXMLNODE tempRootNode = tempParser.GetNodeFromPath( "Msg" );
	tempParser.SetAttrNode( "Version",version,tempRootNode );
	tempParser.SetAttrNode( "MsgID",msgid,tempRootNode );
	tempParser.SetAttrNode( "DVBType",downType,tempRootNode );
	tempParser.SetAttrNode( "TaskType",string("EncryptCheckTask"),tempRootNode );
	tempParser.SetAttrNode( "Protocol",protocol,tempRootNode );
	tempParser.SetAttrNode( "DateTime",dateTime,tempRootNode );
	tempParser.SetAttrNode( "SrcCode",srccode,tempRootNode );
	tempParser.SetAttrNode( "DstCode",dstcode,tempRootNode );
	tempParser.SetAttrNode( "SrcURL",srcurl,tempRootNode );
	tempParser.SetAttrNode( "Priority",std::string("9999"),tempRootNode );	//�������������ȼ��������

	pXMLNODE tempQueryNode = tempParser.CreateNodePtr(tempRootNode, "EncryptCheckTask");
	
	int nListenPort = 10086;
	pXMLNODE queryNode = parser.GetNodeFromPath("Msg/ICInfoChannelEncryptQuery");
	pXMLNODELIST infoNodeList = parser.GetNodeList( queryNode );
	int nInfoCount = parser.GetChildCount( queryNode );
	for (int nIndex=0; nIndex<nInfoCount; nIndex++)
	{
		pXMLNODE infoNode = parser.GetNextNode( infoNodeList );

		std::string strCardNo;
		parser.GetAttrNode(infoNode, "CardNo", strCardNo);

		//��ȡ��ӦС���������Ϣ
		CACardInfo cardinfo;
		if(PROPMANAGER::instance()->GetCurWorkVersion() != "5.0")
		{
			PARAMSETMANAGER::instance()->GetCACardInfoByCardNo(strCardNo, cardinfo);
		}
		else
		{
			if(!(PROPMANAGER::instance()->GetCACardInfoByCardNo(strCardNo, cardinfo)))
			{
				cout<<"С����Ȩ��ѯ���Ų�����:"<<strCardNo<<endl;
			}
		}

		pXMLNODE tempInfoNode = tempParser.CreateNodePtr(tempQueryNode, "ICInfo");
		tempParser.SetAttrNode("CardNo", strCardNo, tempInfoNode);
		tempParser.SetAttrNode("Position", cardinfo.position, tempInfoNode);
		tempParser.SetAttrNode("Desc", cardinfo.desc, tempInfoNode);
		tempParser.SetAttrNode("Ip", cardinfo.ip, tempInfoNode);
		tempParser.SetAttrNode("ListenPort", nListenPort++, tempInfoNode);
		//
		int devnum = PROPMANAGER::instance()->GetCoderDevNum();
		devnum += PROPMANAGER::instance()->GetQuaDevInfoNum()*2;
		tempParser.SetAttrNode("DeviceId", StrUtil::Int2Str(devnum), tempInfoNode);//ʹ�ù����̵߳����һ���߳�

		std::string strTempXml;
		tempParser.SaveToString( strTempXml );
		vecTaskXml.push_back( strTempXml );
		tempParser.DeleteNode(tempQueryNode, tempInfoNode);
	}

	return true;
}