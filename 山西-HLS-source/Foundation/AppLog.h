#ifndef _APPLOG_H_
#define _APPLOG_H_

///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����AppLog.h
// �����ߣ�����
// ����ʱ�䣺2009-05-20
// ������������־��¼
// ��ע��ʵ��������־��¼��ʽ��
// 1�����������ļ�ȷ����Ϣ�������ʽ����Ļ���ļ���
// 2������һ���Ըı���־��Ϣ�����ʽ
// 3������̬�ı���־��Ϣ�����ʽ
// 4�������ʽѡ�����ȼ��𣺶�̬�ı� > һ���Ըı� > �����ļ�
///////////////////////////////////////////////////////////////////////////////////////////

#include <string>
#include "TypeDef.h"
#include "ace/Singleton.h"

class AppLog
{
public:
	AppLog(void);
	virtual~AppLog(void);
private:
	eLogType mLogType;  //��־����
	enumLogOutPut mLogOutPut; //��־�������
	enumLogOutPut mOldLogOutPut; //�������ļ���ʼ��������
private:
	//����ģ�����ƻ��log�ļ����ƣ�����·��������Ӧ��ģ���ַ��������Լ���־��Ϣ��������
	void GetLogBasicInfo(eModuleType Module,eLogType LogType,std::string& strLogName,std::string& strModule,std::string& strLogTpye);
	bool WriteLogToFile(eModuleType Module,eLogType LogType,const std::string& EventLog);
	//����ӿ�
public:
	eLogType GetLogType(void){return mLogType;};						//�õ���־������
	bool SetLogType(eLogType LogType){mLogType=LogType;return true;};   //������־������

public:
	void ManualSetOutType(enumLogOutPut LogOutPut){mLogOutPut = LogOutPut;};	//�ֶ��ı���־�����ʽ
	void CloseManualSet(){mLogOutPut = mOldLogOutPut;};							//�ر��ֶ��ı�������ʽ����ԭΪĬ��

	bool WriteLog(eModuleType Module,eLogType LogType,const std::string& EventLog,enumLogOutPut LogOutPut = LOG_OUTPUT_UNDEFINE);
};
typedef ACE_Singleton <AppLog, ACE_Thread_Mutex>  APPLOG;
#endif
