///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����XMLReceive.h
// �����ߣ�jiangcheng
// ����ʱ�䣺2009-05-20
// ��������������Ӧ��ϵͳ�´�XML��ʽ������
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "DownOrderReceive.h"
#include <string>

class XMLReceive : public DownOrderReceive
{
public:
	XMLReceive();
	~XMLReceive();
public:
	int svc();
private:
	void ProcessReceiveXML();
private:
	long lastMsgID;
};