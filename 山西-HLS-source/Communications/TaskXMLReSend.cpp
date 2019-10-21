#include "TaskXMLReSend.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/AppLog.h"
#include "../Foundation/XmlParser.h"
#include "../Foundation/PropManager.h"
#include "XMLSend.h"
#include "ace/OS.h"

TaskXMLReSend::TaskXMLReSend()
{
	bFlag = false;
}

TaskXMLReSend::~TaskXMLReSend()
{

}

int TaskXMLReSend::Init()
{
	//��ʼ��
	bFlag = true;
	return 0;
}

int TaskXMLReSend::open(void*)
{
	//�����߳�
	this->activate();
	return 0;
}

int TaskXMLReSend::svc()
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ����XMLѭ�������߳̿�ʼִ�� !\n"));

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
				if (-1 == ProcessMessage(mb))
				{
					//����ʧ�����¼��뵽���Ͷ����У�
					putq(mb);
					continue;
				}
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
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ����XMLѭ�������߳�ִֹͣ�� !\n"));

	return 0;
}

int TaskXMLReSend::Stop()
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

int TaskXMLReSend::ProcessMessage(ACE_Message_Block* mb)
{
	std::string strXML ="";

	int xmllen = mb->length() + 100;	
	char* buf = new char[xmllen];
	
	if (buf == NULL)
	{
		std::string msg = string("TaskXMLReSend:�ڴ�ռ䲻�㣡");
		//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);
		return -1;
	}
	memset(buf,0,xmllen);
	memcpy(buf,mb->rd_ptr(),mb->length());//xml�������ڴ�
	strXML = buf;
	std::string strUrl = GetURL(strXML);//��÷��͵�Ŀ��url

	if ((strXML.empty()) || (strUrl.empty()))
	{
		delete[] buf;
		return 0;
	}

	//���ͻظ�xml
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

std::string TaskXMLReSend::GetURL(std::string& upXML)
{
	std::string dstCode,dstUrl,strRet;

	XmlParser parser(upXML.c_str());
	pXMLNODE rootNode = parser.GetNodeFromPath( "Msg" );
	//��ȡ��Ԫ�ص� "DstCODE" ����ֵ 
	parser.GetAttrNode(rootNode,"DstURL",dstUrl);
	parser.GetAttrNode( rootNode,"DstCode",dstCode);
	//DstURL���Բ�Ϊ�գ�����URL��ΪDstURL���������DstCode�������ļ��л��URL
	if(dstUrl.length()>0)
	{
		strRet=dstUrl;
	}
	else if (!(dstCode.empty()))
	{
		strRet = PROPMANAGER::instance()->GetUrl(dstCode);//����Ŀ������url
	}
	//ȥ��upxml��msg�ڵ��DstURL����
	size_t pos=upXML.find("DstURL=");

	size_t pos1=upXML.find("\"",pos);
	size_t pos2=upXML.find("\"",pos1+1);
	upXML.erase(pos,pos2-pos+1);

	return strRet;
}