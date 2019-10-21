///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����BusinessIterfaceMgr.h
// �����ߣ�jiangcheng
// ����ʱ�䣺2009-05-21
// ����������ҵ��ӿڲ�ӿڹ�����
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ace/Synch.h"
#include "ace/Singleton.h"
#include <string>

class DownXMLProcess;
class BusinessIterfaceMgr
{
public:
	BusinessIterfaceMgr();
	~BusinessIterfaceMgr();
public:
	int Start();
	int Stop();	
	int UplaodXML(const std::string& strXML);
private:
	void CreateInstance();
	void DestoryInstance();
private:
	DownXMLProcess* pDownXMLProcess;
};

typedef ACE_Singleton<BusinessIterfaceMgr,ACE_Mutex>  BUSINESSITERFACEMGR;