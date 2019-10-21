
#include "RealTimeQualityServer.h"
#include "CommunicationMgr.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/TypeDef.h"
#include "../DBAccess/DBManager.h"
#include "../Foundation/XmlParser.h"
#include "../BusinessProcess/BusinessLayoutMgr.h"
#include "ace/OS.h"
#include <string>
#include <sstream>
#include <iostream>


using namespace std;

RealTimeQualityServer::RealTimeQualityServer()
{
	bFlag = false;
}

RealTimeQualityServer::~RealTimeQualityServer()
{

}

int RealTimeQualityServer::Open(ACE_Addr &addr)
{
	if (peer_acceptor.open(addr) == -1)//�����˿�
	{
		cout<<"RealTimeQualityServer error"<<endl;
		return -1;
	}
	return 0;
}

int RealTimeQualityServer::handle_input(ACE_HANDLE handle)
{
	ACE_SOCK_Stream client;
	ACE_INET_Addr clientAddr;//����ͻ������ӵ�ַ��Ϣ

	if (this->peer_acceptor.accept(client,&clientAddr) == -1)//���տͻ�������
	{
		ACE_DEBUG((LM_ERROR,"(%T| %t)Error in connection\n"));
	}
	else
	{
		const int PKGSIZE = 1024;
		int value =PKGSIZE*10;	
		//����socket���շ��ͻ����С
		client.set_option(SOL_SOCKET,SO_SNDBUF,(char*)&value,sizeof(value));
		client.set_option(SOL_SOCKET,SO_RCVBUF,(char*)&value,sizeof(value));

		string TaskXml = "";

		if(false == RecvTaskXml(client,TaskXml))//tcp��ʽ����ָ������
		{
			string msg = string("ָ���û������������û����͵��������");
			//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);
			return 0;
		}
		//SYSMSGSENDER::instance()->SendMsg(TaskXml);

		XmlParser parser(TaskXml.c_str());
		pXMLNODE Node= parser.GetNodeFirstChild(parser.GetRootNode());
		if(parser.GetNodeName(Node)=="ClientInfo")
		{
			BUSINESSLAYOUTMGR::instance()->AddUser(client,TaskXml);
		}
		else
		{
			string  MsgID ;
			DBMANAGER::instance()->GetMsgID(MsgID);
			int msgid=StrUtil::Str2Int(MsgID);
			DBMANAGER::instance()->UpdateMsgID(StrUtil::Int2Str(++msgid));

			parser.SetAttrNode("MsgID",MsgID,parser.GetRootNode());
			parser.SaveToString(TaskXml);

			ACE_Message_Block *mbXML = new ACE_Message_Block(TaskXml.length());
			memcpy(mbXML->wr_ptr(),TaskXml.c_str(),TaskXml.length());
			mbXML->wr_ptr(TaskXml.length());

			if (mbXML != NULL)
			{
				ACE_Time_Value OutTime(ACE_OS::time(0)+1);
				COMMUNICATIONMGR::instance()->AccessOrderReceive()->putq(mbXML,&OutTime);
			}

			//����ͻ���������Ϣ
			//sVedioUserInfo tempuserinfo;
			//tempuserinfo.client = client;
			//tempuserinfo.MsgID = MsgID;
			//tempuserinfo.DeviceID = StrUtil::Int2Str(0);
			//tempuserinfo.IP = clientAddr.get_host_addr();
			//tempuserinfo.Port = StrUtil::Int2Str(clientAddr.get_port_number());

			//REALTIMEQUALITYSENDER::instance()->AddClient(tempuserinfo);//���ָ�������û�
			//
			//string msg = string("�û�(") + string(clientAddr.get_host_addr()) + string(":") \
			//			+ StrUtil::Int2Str(clientAddr.get_port_number()) + string(")����ָ�������");
			//SYSMSGSENDER::instance()->SendMsg(msg);
		}
		
	}

	return 0;
}

int RealTimeQualityServer::handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask)
{
	close_mask = ACE_Event_Handler::ALL_EVENTS_MASK | ACE_Event_Handler::DONT_CALL;
	//�ӷ�Ӧ�����Ƴ�ָ�������
	COMMUNICATIONMGR::instance()->AccessMyReactor()->GetReactor()->remove_handler(this,close_mask);
	this->peer_acceptor.close();

	delete this;
	return 0;
}

ACE_HANDLE RealTimeQualityServer::get_handle(void) const
{
	return this->peer_acceptor.get_handle();
}

int RealTimeQualityServer::Stop()
{
	ACE_Reactor_Mask close_mask = ACE_Event_Handler::ALL_EVENTS_MASK | ACE_Event_Handler::DONT_CALL;
	COMMUNICATIONMGR::instance()->AccessMyReactor()->GetReactor()->remove_handler(this,close_mask);
	this->peer_acceptor.close();

	return 0;
}

bool RealTimeQualityServer::RecvTaskXml(ACE_SOCK_Stream& newclient,std::string& taskxml)
{
	int countTimes = 10;//���տͻ��˷������ݵĴ���
	char RecvBuf[4096] = {0};
	int RecvCount = -1;
	try
	{
		while(--countTimes)
		{
			ACE_Time_Value TimeOut(1);
			ssize_t RecvCount = newclient.recv(RecvBuf,4096,&TimeOut);

			if(RecvCount < 4 || RecvCount == SOCKET_ERROR)
			{
				OSFunction::Sleep(0,50);
				continue;
			}
			char* p = RecvBuf;
			//�ҵ�xml�ַ����Ŀ�ʼ֮��
			for (int i=0;i<RecvCount;++i)
			{
				if (*p == '<')
					break;
				p++;
			}

			taskxml = p;
			return true;
		}
	}
	catch(...)
	{
		;
	}
	return false;
}