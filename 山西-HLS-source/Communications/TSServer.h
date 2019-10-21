#pragma once
#include "ace/Synch.h"
#include "ace/Singleton.h"
#include "RTSPServerInter.h"
#include <map>
#include <string>
class TSServer
{
public:
	TSServer(void);
	~TSServer(void);
public:
	// ����RTSP������
	bool CreateTsServer(char *szIP, int nPort);
	// ����ʵʱ��Ƶ��
	int CreateTsStream(char *szStrName);
	// ɾ��ʵʱ��Ƶ��
	bool DeleteTsStream(char *szStrName);
	bool DeleteTsStream(int nStrID);
	// ��ʵʱ��Ƶ���������
	bool AddTsData(std::string szStrName, char *szBuf, int nDataLen);
	bool DelTsData(std::string szStrName);
	// �ͷ�RTSP������
	bool ReleaseTsServer();
	// �����ļ�����·��
	bool SetTsFilePath(char *szFilePath);
private:
	std::map<std::string,int> mapStreamID;//��id�����ֶ�Ӧ��
	//ACE_Thread_Mutex StreamMutex;
};
typedef ACE_Singleton<TSServer,ACE_Mutex>  TSSERVERMGR;