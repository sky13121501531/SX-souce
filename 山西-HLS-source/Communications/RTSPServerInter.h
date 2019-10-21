#ifndef RTSPSERVER_H
#define RTSPSERVER_H

#include <string>
#include <vector>
#include "../Foundation/TypeDef.h"
class RTSPServer;
class CStreamManager;
// RTSP������ָ��
extern RTSPServer* g_rtspServer;
// ��������ָ��
extern CStreamManager* g_pStrManager;
// ����ֹͣ��ʶ
extern char		g_cStart;
// ����ʵʱ��Ƶ��
int RTSPCreateStream(char *szStrName);
// ɾ��ʵʱ��Ƶ��
bool RTSPDeleteStream(char *szStrName);
bool RTSPDeleteStream(int nStrID);
// ��ʵʱ��Ƶ���������
bool RTSPAddData(char *szStrName, char *szBuf, int nDataLen);
bool RTSPAddData(int nStrID, char *szBuf, int nDataLen);
// ���ʵʱ��
bool RTSPClearData(char *szStrName);
bool RTSPClearData(int nStrID);

// ����RTSP������
bool RTSPCreateServer(char *szIP, int nPort = 554);
// �ͷ�RTSP������
bool RTSPReleaseServer();
// �����ļ�����·��
bool RTSPSetFilePath(char *szFilePath);

//////////////////////////////////////////////////////////////////////////
//	�û�����ӿ�
//	Ŀǰֻ��ͨ���š���ַ�Ͷ˿�����

// ȡ�����û�����Ŀ
int RTSPStrUserCount(int nStrID);
int RTSPStrUserCount(char *szStrName);

// ȡ�ø������������û�����Ϣ
bool RTSPGetStrUser(int nStrID, std::vector<sVedioUserInfo> &vUsersInfo);
bool RTSPGetStrUser(char *szStrName, std::vector<sVedioUserInfo> &vUsersInfo);

#endif