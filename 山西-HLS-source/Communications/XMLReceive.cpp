///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����XMLReceive.cpp
// �����ߣ�jiangcheng
// ����ʱ�䣺2009-05-20
// ��������������Ӧ��ϵͳ�´�XML��ʽ������
///////////////////////////////////////////////////////////////////////////////////////////
#pragma warning(disable:4996)
#include "XMLReceive.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/AppLog.h"
#include "../Foundation/OSFunction.h"
#include <fstream>

#if defined(WIN32) || defined(__WIN32__)
#include <windows.h> 
#endif

XMLReceive::XMLReceive()
{

}

XMLReceive::~XMLReceive()
{

}

int XMLReceive::svc()
{
	lastMsgID = -1;
	while (bFlag)
	{
		OSFunction::Sleep(0,50);

		try
		{
			ProcessReceiveXML();
		}
		catch (...)
		{
			std::string msg = "XMLReceive:Xml���ճ����쳣!";
			//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);
		}
	}

	return 0;
}

void XMLReceive::ProcessReceiveXML()
{
	string dirs[]={"C:\\SMSDown\\","C:\\CASDown\\","C:\\MonDown\\","C:\\DVBTHDown\\","C:\\TVMonDown\\",\
					"C:\\RadioDown\\","C:\\AMDown\\","C:\\CommonDown\\","C:\\SatDown\\","C:\\3DDown\\"};

#if defined(WIN32) || defined(__WIN32__)
	//ɨ��Ŀ¼
	HANDLE hFile;
	for (int i=0;i<(sizeof(dirs)/sizeof(string));++i)
	{
		
		string  file_name = dirs[i]+"*.xml";//����xml�ļ���Ŀ¼
		WIN32_FIND_DATA  wf;
		hFile = FindFirstFile(file_name.c_str(), &wf);//�����ļ�
		if (hFile == INVALID_HANDLE_VALUE)
		{
			continue;
		}
		do
		{
			string strXML = "";
			char xml_filename[300];
			strcpy(xml_filename,dirs[i].c_str()) ;
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

			if ( dwAttrs & FILE_ATTRIBUTE_READONLY ) //����ļ�����ֻ������Ϊ����
			{ 
				SetFileAttributes(xml_filename, FILE_ATTRIBUTE_NORMAL); 
			}

			DeleteFile(xml_filename);//ɾ���ļ� 
			
			ACE_Message_Block *mbXML = new ACE_Message_Block(strXML.length());
			memcpy(mbXML->wr_ptr(),strXML.c_str(),strXML.length());
			mbXML->wr_ptr(strXML.length());
			putq(mbXML);

			string Info = string("ϵͳ����ָ�") + strXML;
			//SYSMSGSENDER::instance()->SendMsg(Info);

		}while((FindNextFile(hFile, &wf)));
		FindClose(hFile);

	}
#endif

}