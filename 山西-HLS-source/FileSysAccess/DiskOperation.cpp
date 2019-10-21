///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����DiskOperation.cpp
// �����ߣ�jiangcheng
// ����ʱ�䣺2009-06-18
// ���������������ļ�������
///////////////////////////////////////////////////////////////////////////////////////////
#include "DiskOperation.h"
#include "../DBAccess/DBManager.h"
#include "../Foundation/FileOperater.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/OSFunction.h"
#include "../Foundation/AppLog.h"


#if defined(WIN32) || defined(__WIN32__)
#include <windows.h>
#else
#include <sys/vfs.h> 
#endif
#include <time.h>  

DiskOperation::DiskOperation()
{
	Init();
}

DiskOperation::~DiskOperation()
{

}

void DiskOperation::Init()
{
	mPreTime = time(0);
	
	mDirNameVec.clear();//gxd 20110331 ���¼��洢·��
	PROPMANAGER::instance()->GetRecFileLocVec(mDirNameVec);
	mLowestSpace=StrUtil::Str2Int(PROPMANAGER::instance()->GetMaxAvailableSize());//���¼���ļ����ڴ��̵���С����ֵ
}

long DiskOperation::CalculateFreeSpace(const std::string& dirName)
{
#if defined(WIN32) || defined(__WIN32__)
	unsigned __int64 FreeBytesAvailableToCaller = 0;
	std::string dPath = dirName.substr(0,dirName.find_first_of(":/",0));//¼���ļ��洢���̷�
	dPath.append(":\\");
	::GetDiskFreeSpaceEx(dPath.c_str(),
		(ULARGE_INTEGER *)&FreeBytesAvailableToCaller,NULL,NULL);//��ô��̿��ÿռ�

	return (long)(FreeBytesAvailableToCaller/(1024*1024*1024));
#else
	long long FreeBytesAvailableToCaller = 0;
	struct statfs buf;
	memset(&buf,0,sizeof(buf));
	if (-1 != statfs(dirName.c_str(), &buf))
		FreeBytesAvailableToCaller = (long long)(buf.f_bsize * buf.f_bavail)/(1024*1024);
	return FreeBytesAvailableToCaller;
#endif
}

bool DiskOperation::DeleteOldestFileInPath(unsigned short delcount, const std::string& recPath)
{
	bool bRet = false;

	std::vector<std::string> vecfilename;
	DBMANAGER::instance()->QueryRecordTopInPath(delcount, recPath, vecfilename);
	//�����·�������ݿ����Ѿ�û��¼���¼�ˣ�������Ȼ�ռ䲻�㣬
	//����Ϊ���ļ��������������ļ�Ϊ�����ļ���ɾ���ļ����������ļ�
	if ( vecfilename.empty() )	
	{
		std::string strMsg = recPath;
		strMsg += "�����ݿ����Ѿ�û�м�¼��������Ȼ�ռ䲻��,����ɾ�����ļ����������ļ���";
		std::cout<<"===================="<<std::endl;
		std::cout<<strMsg<<std::endl;
		std::cout<<"===================="<<std::endl;
		APPLOG::instance()->WriteLog(RECORD,LOG_EVENT_DEBUG,strMsg,LOG_OUTPUT_FILE);

		std::string strDelCmd = "del ";
		strDelCmd += recPath;
		strDelCmd += " \\Q";

		system( strDelCmd.c_str() );
	}

	std::vector<std::string>::iterator iter=vecfilename.begin();
	for (;iter!=vecfilename.end(); ++iter)
	{
		if(DeleteRecFile(*iter))								//ɾ�������ļ�
			DBMANAGER::instance()->DeleteRecord(*iter);			//ɾ�����ݿ�

		ACE_DEBUG ((LM_DEBUG,"(%T| %t) ɾ��[%s]!\n",(*iter).c_str()));
		OSFunction::Sleep(0,10);
	}
	bRet = true;
	return bRet;
}


bool DiskOperation::DeleteOldestFileByDays(unsigned short delnum)
{
	bool bRet = false;	

	std::vector<std::string> filenames;
	DBMANAGER::instance()->QueryRecordTop(delnum,filenames);

	for (std::vector<std::string>::iterator iter=filenames.begin();iter!=filenames.end(); ++iter)
	{
		if(DeleteRecFile(*iter))								//ɾ�������ļ�
			DBMANAGER::instance()->DeleteRecord(*iter);			//ɾ�����ݿ�

		ACE_DEBUG ((LM_DEBUG,"(%T| %t) ɾ��123[%s]!\n",(*iter).c_str()));
		OSFunction::Sleep(0,10);
	}
	bRet = true;
	return bRet;
}

bool DiskOperation::CheckDiskIsFullForRegular(int nTimeValue)
{
	bool bRet = false;

	if (nTimeValue > 0)//���ʱ����
	{
		time_t currentTime = time(0);
		if ((currentTime - mPreTime) >= nTimeValue)
		{
			mPreTime = currentTime;								//�����ϴμ��ʱ��
		}
		else
		{
			return bRet;
		}
	}

	std::string storetype = PROPMANAGER::instance()->GetStoreType();	//��ȡ¼��洢��ʽ����ɢ ���� ˳��)
	
	if (storetype == "0" || storetype.length()<1)	//˳��洢����δ���壬����ȫ��ʱ��ɾ���߼�
	{
		for(int i=0;i<100;i++)//while��Ϊfor��ֹɾ����Ч������ֹ��ɾ��
		{
			std::vector<std::string>::iterator diriter=mDirNameVec.begin();
			for (;diriter!=mDirNameVec.end(); ++diriter)
			{
				long dirSpace = CalculateFreeSpace(*diriter);		//���ÿ��·����ʣ��ռ�
				if(dirSpace > mLowestSpace+2)							//ĳ���̷��ռ����Ҫ��������ɾ��
				{
					break;
				}
				OSFunction::Sleep(0,10);
			}
			if (diriter == mDirNameVec.end())						//������д��̿ռ䶼������Ҫ��
			{
				cout<<"���̿ռ䲻�㿪ʼɾ���ļ�storetype == 0 || storetype==.............."<<endl;
				DeleteOldestFileByDays(100);						//��ʱ���ʱɾ��100����ʷ��¼
				cout<<"����ɾ���ļ�storetype == 0 || storetype==.............."<<endl;
				Sleep(100);
				continue;
			}
			break;													//����κ�һ�����̿ռ����Ҫ�����˳����μ��
		}
	}
	else if (storetype == "1")				//��ɢ�洢��ÿ������ռ�Ĵ��̶���Ҫɾ��
	{
		std::vector<std::string>::iterator diriter = mDirNameVec.begin();
		long dirSpace = 0;
		for (;diriter!=mDirNameVec.end(); ++diriter)
		{
			long cursp = dirSpace;
			dirSpace = CalculateFreeSpace(*diriter);		//���ÿ��·����ʣ��ռ�
			if(dirSpace <= mLowestSpace+5)						//ĳ���̷��ռ䲻��
			{
				cout<<"���̿ռ䲻�㿪ʼɾ���ļ�storetype == 1.............."<<endl;
				DeleteOldestFileInPath(100,*diriter);			//��ʱ���ʱɾ��100����ʷ��¼
				cout<<"����ɾ���ļ�storetype == 1.............."<<endl;
				diriter = mDirNameVec.begin();					//���¼��
				OSFunction::Sleep(0,10);
				if(dirSpace <= cursp)//д�ļ����ٶȴ���ɾ�����ٶ��ǲ�̫���ܵ�,Ӧ����ɾ����Ч,�˳�.
				{
					cout<<"����ɾ���ļ�,ɾ���ļ���Ч�˳�.............."<<endl;
					break;
				}
				continue;		
			}
			OSFunction::Sleep(0,10);
		}									
	}
	return true;
}
long DiskOperation::GetTotalFreeSpace()
{
	long TotalSpace = 0;
	std::vector<std::string>::iterator diriter=mDirNameVec.begin();
	for (;diriter!=mDirNameVec.end(); ++diriter)
	{
		TotalSpace += CalculateFreeSpace(*diriter);
		OSFunction::Sleep(0,5);
	}
	return TotalSpace;
}
bool DiskOperation::CheckDiskIsFullForRecord(std::string& validpath,int deviceid)
{
	bool ret = false;
	int CheckTimes = 50;	//������
	if (deviceid == -1)		//˳��
	{
		while(1)
		{
			std::vector<std::string>::iterator diriter=mDirNameVec.begin();
			for (;diriter!=mDirNameVec.end(); ++diriter)
			{
				long dirSpace = CalculateFreeSpace(*diriter);		//���ÿ��·����ʣ��ռ�
				if(dirSpace > mLowestSpace)							//ĳ���̷��ռ����Ҫ��
				{
					validpath = *diriter;
					break;
				}
				OSFunction::Sleep(0,5);
			}
			if (diriter == mDirNameVec.end() && --CheckTimes)		//������д��̿ռ䶼������Ҫ��
			{
				DeleteOldestFileByDays(50);							//¼�Ƽ��ʱɾ��50����ʷ��¼
				OSFunction::Sleep(0,10);
				continue;
			}
			break;													//����κ�һ�����̿ռ����Ҫ�����˳����μ��
		}
	}
	else		//��ɢ
	{
		std::string recPath = PROPMANAGER::instance()->GetRecPathByDevId(deviceid);		//ͨ��ͨ���Ż�ȡ�洢·��
		if ( recPath.empty() )
			return false;

		while (1)
		{
			long lDirSpace = CalculateFreeSpace(recPath);		//��ȡ���пռ�
			if (lDirSpace > mLowestSpace)
			{
				validpath = recPath;
				break;
			}
			else if( --CheckTimes)
			{
				DeleteOldestFileInPath(50, recPath);		//ɾ��recpath��50����ʷ��¼
				OSFunction::Sleep(0,10);
				continue;
			}
			break;
		}	
	}
	
	return true;	//���ú������ж�validpath�Ƿ�Ϊ��
}

bool DiskOperation::ProcessInprireFiles()
{
	bool bRet = false;
	std::vector<std::string> filenames;

	/* �������¼���ļ� */
	DBMANAGER::instance()->QueryRecordExpire(filenames);		//��ѯ����¼��¼���ļ�
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ��ʼɾ��¼��¼���ļ�,��[%d]��!\n",(int)filenames.size()));
	for (std::vector<std::string>::iterator iter=filenames.begin(); iter!=filenames.end(); ++iter)
	{
		DeleteRecFile(*iter);									//ɾ������¼��¼���ļ�
		DBMANAGER::instance()->DeleteRecord(*iter);				//ɾ�����ݿ��е�¼��¼���¼
		//std::cout<<"ɾ��¼��¼���ļ���"<<*iter<<std::endl;
		OSFunction::Sleep(0,500);
	}
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ����ɾ��¼��¼���ļ�,��[%d]��!\n",(int)filenames.size()));

	filenames.clear();

	/* ���������ʱ�ļ� */
	DBMANAGER::instance()->QueryTempFileExpire(filenames);		//��ѯ������ʱ�ļ�
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ��ʼɾ����ʱ�ļ�,��[%d]��!\n",(int)filenames.size()));
	for (std::vector<std::string>::iterator iter=filenames.begin(); iter!=filenames.end(); ++iter)
	{
		DeleteTempFile(*iter);									//ɾ��������ʱ�ļ�
		DBMANAGER::instance()->DeleteTempFile(*iter);			//ɾ�����ݿ��е���ʱ�ļ���¼
		OSFunction::Sleep(0,500);
	}
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ����ɾ����ʱ�ļ�,��[%d]��!\n",(int)filenames.size()));

	/* ���������־�ļ� */
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ��ʼɾ��������־�ļ�!\n"));
	DeleteExpireLog();
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ����ɾ��������־�ļ�!\n"));

	/* ��������¼���ļ� */
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ��ʼɾ������¼���ļ�!\n"));
	DeleteTrashFile();
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ����ɾ������¼���ļ�!\n"));

	/* ����7��ǰ�������� */
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ��ʼɾ�����ݿ��¼!\n"));
	DeleteDbData();
	ACE_DEBUG ((LM_DEBUG,"(%T| %t) ����ɾ�����ݿ��¼!\n"));

	bRet = true;
	return bRet;
}

bool DiskOperation::DeleteRecFile( std::string& filename )
{
	ACE_Guard<ACE_Thread_Mutex> guard(mDelMutex);
	bool bRet = true;

#if defined(WIN32) || defined(__WIN32__)
	 if( (_access( filename.c_str(),F_OK )) == -1 )		
		 return true/*false*/;		//����ļ��������ˣ�Ҳ��Ϊ���Ѿ�ɾ����
#endif

	if (!filename.empty())
	{
#if defined(WIN32) || defined(__WIN32__)
		if (0== DeleteFile(filename.c_str()))
		{
			string msg = "ɾ��¼���ļ�����:"+filename;
			//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);

			bRet=false;
		}
#else
		if (-1 == remove(filename.c_str()))
		{
			bRet=false;
		}
#endif
	}

	return bRet;
}


bool DiskOperation::DeleteTempFile(std::string& filename)
{
	ACE_Guard<ACE_Thread_Mutex> guard(mDelMutex);
	bool bRet = true;

#if defined(WIN32) || defined(__WIN32__)
	if( (_access( filename.c_str(),F_OK )) == -1 )
		return false;
#endif

	if (!filename.empty())
	{
#if defined(WIN32) || defined(__WIN32__)
		if (0 == DeleteFile(filename.c_str()))
		{
			bRet=false;
		}
#else
		if (-1== remove(filename.c_str()))
		{
			bRet=false;
		}
#endif
	}

	return bRet;
}

//ɾ��������־�ļ�
bool DiskOperation::DeleteExpireLog()			
{
	string logpath = PROPMANAGER::instance()->GetLogPath();		//c:/vscttb/log/
	logpath += "*.*";

	string logExpire = PROPMANAGER::instance()->GetLogExpire();
	//ͨ�����·���ȡ��ֵ
	int expire = (0 - StrUtil::Str2Int(logExpire) );

	//�����������
	string CurDateTime = TimeUtil::GetCurDateTime();
	string logExpireDateTime = TimeUtil::CalDay(CurDateTime, expire);
	string logExpireDate = TimeUtil::GetDateFromDatetime(logExpireDateTime);

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile(logpath.c_str(), &FindFileData);
	while (hFind != INVALID_HANDLE_VALUE)
	{
		if (FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
		{
			if (strcmp(FindFileData.cFileName, ".") != 0 && strcmp(FindFileData.cFileName, "..") != 0)
			{
				//��ȡlog�Ĳ�ͬ���͵��ļ�������
				string logtype = string(FindFileData.cFileName);
				string templogpath = logpath;
				templogpath.erase(15);
				//�����ļ���·��
				string DirName = templogpath + logtype + string("/")+string("*.*");

				WIN32_FIND_DATA ChildFindFileData;
				HANDLE ChildhFind;

				//��ȡ������־�ļ�
				ChildhFind = FindFirstFile(DirName.c_str(), &ChildFindFileData);
				while(ChildhFind !=INVALID_HANDLE_VALUE)
				{
					if (strcmp(ChildFindFileData.cFileName, ".") != 0 && strcmp(ChildFindFileData.cFileName, "..") != 0)
					{
						//ʵ���ļ�������ʽΪ��xxxx-xx-xx.txt
						string FileNameTemp = string(ChildFindFileData.cFileName);

						//ȡ���ļ����е�������Ϣ�����ڱȽ�
						string fileDate = FileNameTemp.substr(0, 10);
						if (TimeUtil::DiffDate(logExpireDate, fileDate) > 0)	//�������
						{
							string tempDirName = DirName;
							tempDirName.erase( tempDirName.find_last_of("/")+1 );
							string fullFileName = tempDirName + FileNameTemp;	//deletefile���������������ʵ��·��

							int ret = DeleteFile( fullFileName.c_str() );	//ɾ��
							if ( !ret )
							{
								string msg = "ɾ����־�ļ�ʧ�ܣ��ļ���Ϊ" + FileNameTemp;
								APPLOG::instance()->WriteLog(DEVICE,LOG_EVENT_DEBUG,msg,LOG_OUTPUT_FILE);
								//SYSMSGSENDER::instance()->SendMsg(msg,UNKNOWN,VS_MSG_SYSALARM);
							}

							//д��־
							std::string msg = string("ɾ����־�ļ��ɹ����ļ���Ϊ��") + fullFileName;
							APPLOG::instance()->WriteLog(DEVICE,LOG_EVENT_DEBUG,msg,LOG_OUTPUT_FILE);
							//SYSMSGSENDER::instance()->SendMsg(msg);

						}
					}
					if (!FindNextFile(ChildhFind, &ChildFindFileData))
					{
						break;
					}
				}
				FindClose(ChildhFind);
			}
		}
		if (!FindNextFile(hFind, &FindFileData))
		{
			break;
		}
	}
	FindClose(hFind);

	return true;
}

void DiskOperation::DeleteTrashFile()
{
	//��ȡ¼���ļ��洢·������
	std::vector<std::string> vecStrFilePath;
	PROPMANAGER::instance()->GetRecFileLocVec( vecStrFilePath );	

	//���������ļ��洢·����ɾ�������ļ�
	std::vector<std::string>::iterator iterPath = vecStrFilePath.begin();
	for(; iterPath!=vecStrFilePath.end(); iterPath++)
	{
		//��ȡ��·���¡����ݿ��������¼���¼�Ŀ�ʼʱ��
		std::string strLastDateTime ;
		bool ret = DBMANAGER::instance()->QueryLastRecordDateTime(*iterPath, strLastDateTime);
		if (!ret)
		{
			std::string strError = "��ȡ���ݿ���·����";
			strError += *iterPath;
			strError += "�������¼���¼�Ŀ�ʼʱ��ʧ��!!";
			APPLOG::instance()->WriteLog(DATABASE,LOG_EVENT_DEBUG,strError,LOG_OUTPUT_FILE);
			return;
		}
		std::string strMsg = "��ȡ�����ݿ���·����";
		strMsg += *iterPath;
		strMsg += "�������¼���¼�Ŀ�ʼʱ��: ";
		strMsg += strLastDateTime;
		APPLOG::instance()->WriteLog(DATABASE,LOG_EVENT_DEBUG,strMsg,LOG_OUTPUT_FILE);
		
		if (strLastDateTime.length()<1)
		{
			strLastDateTime = TimeUtil::GetCurDateTime();
		}

		//���һ�������
		std::string strLastDate = TimeUtil::GetDateFromDatetime(strLastDateTime);

		int nDeleteCount = 5000;		//ÿ��·��ɾ��5000���ļ�
		std::string strFindPath = *iterPath + "*.*";
		std::string strLastDelFileName = "";
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind = FindFirstFile( strFindPath.c_str(), &FindFileData);
		while (hFind != INVALID_HANDLE_VALUE)
		{
			if (strcmp(FindFileData.cFileName, ".") != 0 && strcmp(FindFileData.cFileName, "..") != 0)
			{
				if (nDeleteCount <= 0)	//ֻɾ��nDeleteCount������ֹռ��̫��ʱ��
				{
					break;
				}

				//��ȡ�ļ��Ĵ���ʱ��
				FILETIME fileTime;
				memset(&fileTime, 0, sizeof(fileTime) );
				fileTime = FindFileData.ftCreationTime;
				
				//��FILETIMEת��ΪSYSTEMTIME
				SYSTEMTIME sysTime;
				memset(&sysTime, 0, sizeof(sysTime) );
				FileTimeToSystemTime(&fileTime, &sysTime);
				

				//SYSTEMTIMEת��Ϊstring
				std::string strTempFileDate;
				char now[64];	
				sprintf(now, "%04d-%02d-%02d %02d:%02d:%02d", sysTime.wYear, sysTime.wMonth, 
					sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
				strTempFileDate = now;

				//��ʱ��ת��Ϊ����ʱ��
				strTempFileDate = TimeUtil::CalHour(strTempFileDate, 8);
				strTempFileDate = TimeUtil::GetDateFromDatetime(strTempFileDate);

				//����ļ�����ʱ���������ݿ������¼�Ŀ�ʼʱ�䣬��ɾ���ļ�
				std::string strCurFileName = FindFileData.cFileName;
				if ( TimeUtil::DiffDate(strTempFileDate,strLastDate) < 0 )
				{
					strLastDelFileName = strCurFileName;
					strCurFileName = *iterPath + strCurFileName;
					DeleteRecFile(strCurFileName);
					nDeleteCount --;	//����ɾ����񣬶���nDeleteCount��һ	
				}
				
				OSFunction::Sleep(0, 10);
			}
			
			if (!FindNextFile(hFind, &FindFileData))
			{
				break;
			}
		}
		FindClose(hFind);

		std::string strDelMsg = "·����";
		strDelMsg += *iterPath;
		strDelMsg += "�����ɾ��������¼���ļ���: ";
		strDelMsg += strLastDelFileName;
		APPLOG::instance()->WriteLog(DATABASE,LOG_EVENT_DEBUG,strDelMsg,LOG_OUTPUT_FILE);
	}
	
	return ;
}

void DiskOperation::DeleteDbData()
{
	std::string strCurDateTime = TimeUtil::GetCurDateTime();
	std::string strAlarmTime = TimeUtil::CalDay(strCurDateTime, -7);
	
	DBMANAGER::instance()->DeleteAlarmInfo( strAlarmTime );

	return;
}