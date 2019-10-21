#ifndef _DBPARAMTYPEDEF_H_
#define _DBPARAMTYPEDEF_H_

///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����DBParamTypeDef.h
// �����ߣ�gaoxd
// ����ʱ�䣺2009-05-27
// �������������ݿ�����Զ������Ͷ��壬����ķ�ʽʵ��
///////////////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>

class DBParamChannel
{
public:
	DBParamChannel(){};
	~DBParamChannel(){};
public:
};

typedef std::vector<DBParamChannel*> DBParamChannelVec;

class CTTB_DBParamChannel : public DBParamChannel
{
public:
	CTTB_DBParamChannel(){};
	~CTTB_DBParamChannel(){};
public:
	std::string Freq;
	std::string AudioID;
	std::string VideoID;
};

class CMMB_DBParamChannel : public DBParamChannel
{
public:
	CMMB_DBParamChannel(){};
	~CMMB_DBParamChannel(){};
public:
	std::string Freq;
	std::string AudioID;
	std::string VideoID;
};


#endif