///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����DiskOperation.h
// �����ߣ�jiangcheng
// ����ʱ�䣺2009-06-18
// ���������������ļ�������
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ace/Synch.h"
#include "ace/Singleton.h"
#include <string>
#include <vector>

class DiskOperation
{
public:
	DiskOperation();
	virtual ~DiskOperation();
	
	bool CheckDiskIsFullForRegular(int nTimeValue);							//ָ��ʱ�����ڼ������Ƿ�����
	bool CheckDiskIsFullForRecord(std::string& validpath,int deviceid=-1);	//�������Ƿ�������������Ч·��
	long GetTotalFreeSpace();	//����������Ч·��ʣ��ռ�

	//��������ļ�
	bool ProcessInprireFiles();
	void SetLowestSpace(long lowestSpace) {mLowestSpace = lowestSpace;}	//�趨������С�ռ�(��λΪMb)

private:
	void Init();

	bool DeleteRecFile(std::string& filename);
	long CalculateFreeSpace(const std::string& dirName);
	
	bool DeleteOldestFileByDays(unsigned short delnum);		//ɾ����õ�delnum������
	bool DeleteTempFile(std::string& filename);				//ɾ����ʱ�ļ� //wz_101229 ɾ����ʱ�ļ�
	bool DeleteOldestFileInPath(unsigned short delcount, const std::string& recPath);	//ɾ��ĳ��·�������ϵ������ļ�		add by jidushan 11.05.04

	bool DeleteExpireLog();			//ɾ��������־	add by jidushan 110704
	void DeleteTrashFile();			//ɾ������¼���ļ�
	void DeleteDbData();			//ɾ�����ݿ��¼

private:
	long mLowestSpace;		//������С�ռ�(��λΪMb)
	std::vector<std::string> mDirNameVec; 
	
	time_t mPreTime;
private:
	ACE_Thread_Mutex mDelMutex;

};

typedef ACE_Singleton<DiskOperation,ACE_Mutex>  DISKOPERATION;