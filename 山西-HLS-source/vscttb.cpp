#include "cmdLine.h"
#include "./Communications/CommunicationMgr.h"
#include "./BusinessProcess/BusinessIterfaceMgr.h"
#include "./BusinessProcess/BusinessLayoutMgr.h"
#include "./Communications/TsSenderMgr.h"
#include "./DeviceAccess/TsFetcherMgr.h"
#include "./DeviceAccess/ChanScanThread.h"
#include "./DeviceAccess/UdpRecvAlarmThreadMgr.h"
#include "./Foundation/OSFunction.h"
#include "./Foundation/StrUtil.h"
#include "./Foundation/PropManager.h"
#include "./Foundation/AppLog.h"
#include "./DBAccess/QualityDbManager.h"//ָ�����ݴ��
#include "./Alarm/AlarmMgr.h"
#include "ace/OS.h"
#include <ace/Thread.h>
#include <fstream>
#include <Windows.h>
void DestroyMyWinClosedForCommand()
{
	HWND hwnd = GetConsoleWindow();
	HMENU hmenu = GetSystemMenu(hwnd, false);
	RemoveMenu(hmenu, SC_CLOSE, MF_BYCOMMAND);
	DestroyMenu(hmenu);
	ReleaseDC(hwnd, NULL);
}
void Start()
{
	std::string RunInfo = std::string("�������� ") + VSCTTB_PROG_VERSION;
	std::string SysTitel = string("title ")+VSCTTB_PROG_VERSION;
	//��ȡxml�ļ��е�����
	std::vector<std::string>  vctXML;
	ifstream rebootRFile("C:/vscttb/RebootReport.xml");
	string strRebootReport = "";
	string str = "";
	while (getline(rebootRFile,str))
	{
		strRebootReport += str;
	}
	rebootRFile.close();
	//
	if(strRebootReport.size()>0)
	{
		vctXML.push_back(strRebootReport);
	}
	for (size_t i = 0;i<vctXML.size();i++)
	{
		std::string strXML = vctXML.at(i);
		ACE_Message_Block *mbXML = new ACE_Message_Block(strXML.length());
		memcpy(mbXML->wr_ptr(),strXML.c_str(),strXML.length());
		mbXML->wr_ptr(strXML.length());

		if (mbXML != NULL)
		{
			ACE_Time_Value OutTime(ACE_OS::time(0)+1);
			COMMUNICATIONMGR::instance()->AccessOrderReceive()->putq(mbXML,&OutTime);
			OSFunction::Sleep(0,500);
		}
	}
	//

	APPLOG::instance()->WriteLog(DEVICE,LOG_EVENT_DEBUG,std::string("��ʼ�����ݿ��ʼ��������Ϣ!!"),LOG_OUTPUT_FILE);
	PROPMANAGER::instance()->InitInfoFromDB();
	APPLOG::instance()->WriteLog(DEVICE,LOG_EVENT_DEBUG,std::string("�����ݿ��ʼ��������Ϣ�ɹ�!!"),LOG_OUTPUT_FILE);

	APPLOG::instance()->WriteLog(DEVICE,LOG_EVENT_DEBUG,std::string("��ʼ�����Ҫ���ļ����Ƿ񴴽�!!"),LOG_OUTPUT_FILE);
	CheckDirectoryExsit();	//�������ļ����Ƿ���ڣ������ڣ��ʹ�����.
	APPLOG::instance()->WriteLog(DEVICE,LOG_EVENT_DEBUG,std::string("����ļ������!!"),LOG_OUTPUT_FILE);
	//
	system(SysTitel.c_str());
	//
	PROPMANAGER::instance()->GetDeviceInfo();
	//
	TSFETCHERMGR::instance()->Start();
	TSSENDERMGR::instance()->Start();
	if (COMMUNICATIONMGR::instance()->Start() != 0)	//ֻ�ж�������صĳ�ʼ��������Ĭ�ϳɹ�
	{
		OSFunction::ExitProcess("ϵͳ��ʼ��(����)ʧ��");
	}
	BUSINESSITERFACEMGR::instance()->Start();
	BUSINESSLAYOUTMGR::instance()->Start();
	ALARMMGR::instance()->Start();	
	QUALITYDBMANAGER::instance()->Start();
	CHANSCANTHREAD::instance()->Start();
	OSFunction::Sleep(0,50);
	cout<<endl<<"\n\t[VSCTTB]-:multiRF Wacth and Measure System Run."<<endl<<endl;
	LoadXMLTask();
	UDPRECVALARMTHREADMGR::instance()->Start();
}

void Stop()
{
	TSFETCHERMGR::instance()->Stop();
	TSSENDERMGR::instance()->Stop();
	COMMUNICATIONMGR::instance()->Stop();
	BUSINESSITERFACEMGR::instance()->Stop();
	BUSINESSLAYOUTMGR::instance()->Stop();
	CHANSCANTHREAD::instance()->Stop();
	UDPRECVALARMTHREADMGR::instance()->Stop();
	ALARMMGR::instance()->Stop();
	QUALITYDBMANAGER::instance()->Stop();
	OSFunction::Sleep(0,500);
	cout<<endl<<"\t[VSCTTB]-:multiRF Wacth and Measure System is stopped."<<endl<<endl;
}

int main(int argc, char *argv[])
{
	CheckSoleProcess();
	//ȥ��������رհ�ť---ͳһ������ر�
	DestroyMyWinClosedForCommand();

	string cmd;
	bool bRunning = false;

	PrintTitle();

	cout<<"-VSCTTB_CMD->"<<endl;

	OSFunction::Sleep(0,10);
	Start();
	bRunning = true;

	while(true)
	{
		cmd.clear();
		cin>>cmd;

		if(cmd == "run" && bRunning == true)
		{
			cout<<"\t[VSCTTB]-:System is running."<<endl;
			continue;
		}
		else if(cmd == "run" && bRunning == false)
		{
			Start();
			bRunning = true;
			continue;
		}
		else if(cmd == "stop" && bRunning == true)
		{
			Stop();
			bRunning = false;
			cout<<"-VSCTTB_CMD->";
			continue;
		}
		else if(cmd == "stop" && bRunning == false)
		{
			cout<<"\t[VSCTTB]-:System is stopped."<<endl;
			cout<<"-VSCTTB_CMD->";
			continue;
		}
		else if(cmd == "quit" && bRunning == true)
		{
			Stop();
			break; 
		}
		else if(cmd == "quit" && bRunning == false)
		{
			break;
		}
		else if(cmd == ">") 
		{
			cout<<"-VSDVBTH_CMD->";
			continue;
		}
		else if(cmd == "help")
		{
			PrintUsage(); 
			cout<<"-VSCTTB_CMD->"; 
			continue;
		}
		else
		{
			cout<<"\t[Cmd_Tip]-:invalid command!"<<endl; 
			cout<<"-VSCTTB_CMD->"; 
			continue;
		}
	}
	return 0;
}