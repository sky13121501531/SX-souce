///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����HttpOpe.h
// �����ߣ�jiangcheng
// ����ʱ�䣺2009-06-05
// ������������Ӳ��Http����ͨ����
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "ace/SOCK_Connector.h"
#include "ace/INET_Addr.h"
#include <string>

class HttpOpe
{
public:
	HttpOpe(const char* hostName,int port);
	virtual ~HttpOpe();
public:
	bool PostHttpMessage(const std::string& strSubRoot,const std::string& strCmdMsg,std::string& strRetMsg);//post��ʽ����http��Ϣ
	bool GetRecvHandle(const std::string& strSubRoot,ACE_SOCK_Stream& socketHandle,bool IsRecvXmlHeader=true);//���socket
private:
	std::string PostHttpHeader(const std::string& strSubRoot,int len);//post http����ͷ
	std::string GetHttpHeader(const std::string& strSubRoot);//����http����ͷ
private:
	ACE_SOCK_Stream client_stream_;
	ACE_INET_Addr remote_addr_;
	ACE_SOCK_Connector connector_;
};