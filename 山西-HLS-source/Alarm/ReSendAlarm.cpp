
#include "ReSendAlarm.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/OSFunction.h"
#include "../Communications/XMLSend.h"
#include "../Foundation/AppLog.h"
#include "ace/OS.h"
#include "fstream"

ReSendAlarm::ReSendAlarm()
{

}

ReSendAlarm::~ReSendAlarm()
{

}

int ReSendAlarm::open(void *)
{
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) �������/���������߳̿�ʼִ�� !\n"));
	this->activate();
	return 0;
}

//ͨ����xml�н��������Ĳ�ͬ���ͷ��͵���ͬ�ļ�����ͱ���������
int ReSendAlarm::svc()
{
	bFlag = true;
	while (bFlag)
	{
		try 
		{
			
			ProcessMessage();
			OSFunction::Sleep(0,200);
		}
		catch(...)
		{
		}
	}
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) �������/���������߳�ִֹͣ�� !\n"));

	return 0;
}

int ReSendAlarm::ProcessMessage()
{
	string dirs[]={"C:/AlarmUp/DVBS/","C:\\AlarmUp\\DVBC\\","C:\\AlarmUp\\CTTB\\","C:\\AlarmUp\\ATV\\","C:\\AlarmUp\\RADIO\\",\
		"C:\\AlarmUp\\AM\\"};

	//ɨ��Ŀ¼
	string Path="C:/AlarmUp/";
	string TempPath = Path + string("*.*");
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	//����ȡNIT��Ϣ
	hFind = FindFirstFile(TempPath.c_str(), &FindFileData);

	while (bFlag&&(hFind != INVALID_HANDLE_VALUE))
	{
		if (FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
		{
			if (strcmp(FindFileData.cFileName, ".") != 0 && strcmp(FindFileData.cFileName, "..") != 0)
			{
				string dvbtype= string(FindFileData.cFileName);  //�ļ�����ΪƵ��

				string FindName=Path+dvbtype+string("/")+string("*.xml");
				string DirName = Path+dvbtype+string("/");
				HANDLE hFile;
				WIN32_FIND_DATA  wf;
				hFile = FindFirstFile(FindName.c_str(), &wf);//�����ļ�
				if (hFile == INVALID_HANDLE_VALUE)
				{
					if (!FindNextFile(hFind, &FindFileData))
					{
						break;
					}
					continue;
				}
				do
				{
					string strXML = "";
					char xml_filename[300];
					strcpy(xml_filename,DirName.c_str()) ;
					strcat(xml_filename,wf.cFileName);
					DWORD dwAttrs = GetFileAttributes(wf.cFileName); 

					if ((dwAttrs & FILE_ATTRIBUTE_READONLY)) //����ļ�����ֻ������Ϊ����
					{ 
						SetFileAttributes(xml_filename, FILE_ATTRIBUTE_NORMAL); 
					} 

					//��ȡxml�ļ��е�����
					ifstream xmlFile(xml_filename);
					string strTemp = "";
					while (getline(xmlFile,strTemp))
					{
						strXML += strTemp;
					}
					xmlFile.close();
					UpAlarmInfo alarminfo;
					PROPMANAGER::instance()->GetUpAlarmInfo(OSFunction::GetEnumDVBType(dvbtype),alarminfo);
					if (PROPMANAGER::instance()->GetCurWorkVersion() == "5.0")
					{
						if (strXML.find("BreakReportQuery")!= -1)
						{
							alarminfo.alarmurl = PROPMANAGER::instance()->GetAgentInfoSet("13");
						}
						else
						{
							alarminfo.alarmurl = PROPMANAGER::instance()->GetAgentInfoSet("12");
						}
					}
					if ((strXML.empty()) || (alarminfo.alarmurl.empty()))
					{
					}
					else
					{
						if(false == XMLSend::SendXML(alarminfo.alarmurl,strXML))
						{
							if(!FindNextFile(hFile, &wf))
							{
								break;
							}
							Sleep(10000);
							continue;
						}
					}
					DeleteFile(xml_filename);//ɾ���ļ� 
				}while(bFlag&&(FindNextFile(hFile, &wf)));
				FindClose(hFile);	
			}

		}
		if (!FindNextFile(hFind, &FindFileData))
		{
			break;
		}
	}
	FindClose(hFind);
	return 0;
}

int ReSendAlarm::Stop()
{
	bFlag = false;
	this->wait();
	return 0;
}
