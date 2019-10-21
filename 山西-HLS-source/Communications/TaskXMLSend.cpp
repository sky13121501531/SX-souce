///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����TaskTaskXMLSend.cpp
// �����ߣ�jiangcheng
// ����ʱ�䣺2009-05-20
// ��������������ִ�н�������з�����
///////////////////////////////////////////////////////////////////////////////////////////
#include "TaskXMLSend.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/AppLog.h"
#include "../Foundation/XmlParser.h"
#include "../Foundation/PropManager.h"
#include "CommunicationMgr.h"
#include "ace/OS.h"
#include "XMLSend.h"

TaskXMLSend::TaskXMLSend()
{
	bFlag = false;
}

TaskXMLSend::~TaskXMLSend()
{

}

int TaskXMLSend::Init()
{
	//��ʼ��
	bFlag = true;

	return 0;
}

int TaskXMLSend::open(void*)
{
	//�����߳�
	this->activate();
	return 0;
}

int TaskXMLSend::svc()
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ����XML�����߳̿�ʼִ�� !\n"));

	ACE_Message_Block *mb = 0;

	while (bFlag)
	{
		OSFunction::Sleep(0,50);
		try 
		{
			ACE_Time_Value OutTime(ACE_OS::gettimeofday()+ACE_Time_Value(2));
			if (getq(mb,&OutTime) != -1 && mb != NULL)
			{
				if (mb->msg_type() == ACE_Message_Block::MB_BREAK)
				{
					mb->release();
					break;
				}
				//if (-1 == ProcessMessage(mb))
				//{
				//	std::string msg = string("�ظ�XML������ѭ�����Ͷ��У�");
			
				//	COMMUNICATIONMGR::instance()->AccessTaskXMLReSend()->putq(mb);//xml����ѭ�����Ͷ���
				//	continue;
				//}
				ProcessMessage(mb);
				mb->release();
			}
		}
		catch (...)
		{
			std::string msg = string("�����������XML�ļ��쳣��");
			//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);
		}
	}

	bFlag = false;
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ����XML�����߳�ִֹͣ�� !\n"));

	return 0;
}

int TaskXMLSend::Stop()
{
	bFlag = false;
	ACE_Message_Block* pMsgBreak;
	ACE_NEW_NORETURN(pMsgBreak, ACE_Message_Block (0, ACE_Message_Block::MB_BREAK) );
	//��ն��У�ֹͣ���ͻظ�xml
	msg_queue()->flush();
	msg_queue()->enqueue_head(pMsgBreak);
	this->wait();

	return 0;
}

int TaskXMLSend::ProcessMessage(ACE_Message_Block* mb)
{
	std::string strXML ="";

	int xmllen = mb->length() + 100;
	char* buf = new char[xmllen];

	if (buf == NULL)
	{
		std::string msg = string("TaskXMLSend:�ڴ�ռ䲻�㣡");
		//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);
		return -1;
	}

	memset(buf,0,xmllen);
	memcpy(buf,mb->rd_ptr(),mb->length());//xml�������ڴ�
	strXML = buf;
	if (strXML.empty())
	{
		std::string msg = string("������󷵻�XMLΪ�գ�");
		//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);
		
		delete[] buf;
		return 0;
	}
	
	std::string strUrl = GetURL(strXML);//��÷��͵�Ŀ��url
	std::cout<<"XMLSend Url: "<<strUrl<<std::endl;

	if (strUrl.empty())
	{
		std::string msg = string("������󷵻�XML���ϱ���ַ��");
		//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);
		
		delete[] buf;
		return 0;
	}

	if(false == XMLSend::SendXML(strUrl,strXML))
	{
		std::string msg = string("�����������XML�ļ�ʧ�ܣ�") + "URL:" + strUrl;
		//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);
		
		delete[] buf;
		return -1;
	}

	delete[] buf;
	return 0;
}

std::string TaskXMLSend::GetURL(std::string& upXML)
{
	std::string dstCode,dstUrl,strRet,Type;

	XmlParser parser(upXML.c_str());
	pXMLNODE rootNode = parser.GetNodeFromPath( "Msg" );
	//��ȡ��Ԫ�ص� "DstCODE" ����ֵ 
	parser.GetAttrNode(rootNode,"DstURL",dstUrl);
	parser.GetAttrNode( rootNode,"DstCode",dstCode);
	parser.GetAttrNode(rootNode,"Type",Type);
	
	//DstURL���Բ�Ϊ�գ�����URL��ΪDstURL���������DstCode�������ļ��л��URL
	//add by licf ע�ͣ���ΪҪ���ûظ�CAS SMS��URL��������ݻظ��������ͽ��������֣����������ļ��н�CAS SMS��srccode�ֱ�����
	//ΪCAS SMS�������Ϳ��Ը������񷵻����ʹ������ļ��л���ϱ�CAS SMS�ĵ�ַ
	if(dstUrl.length()>0)
	{
		strRet=dstUrl;
	}
	else if (!(dstCode.empty()))
	{
		if (Type=="CASUp")
		{
#ifdef ZONG_JU_ZHAO_BIAO
			strRet = PROPMANAGER::instance()->GetUrlByType( "CAS" );
#else
			strRet = PROPMANAGER::instance()->GetUrl("CAS");//����
#endif
			
		}
		else if (Type=="SMSUp")
		{
#ifdef ZONG_JU_ZHAO_BIAO
			strRet = PROPMANAGER::instance()->GetUrlByType( "SMS" );
#else
			strRet = PROPMANAGER::instance()->GetUrl("SMS");//����Ŀ������url
#endif
		}
		else
		{
		    strRet = PROPMANAGER::instance()->GetUrl(dstCode);//����Ŀ������url
		}
	}
	//ȥ��upxml��msg�ڵ��DstURL����
	size_t pos=upXML.find("DstURL=");
	if(pos!=std::string::npos)
	{
		size_t pos1=upXML.find("\"",pos);
		size_t pos2=upXML.find("\"",pos1+1);
		upXML.erase(pos,pos2-pos+1);
	}
	return strRet;
}
