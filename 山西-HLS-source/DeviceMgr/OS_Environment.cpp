
#include "OS_Environment.h"

#include "../Foundation/TypeDef.h"
#include "../Foundation/StrUtil.h"
#include "../Foundation/OSFunction.h"

#if defined(WIN32) || defined(__WIN32__)
#include <windows.h>
#include <conio.h>
#include <stdio.h>
#define SystemBasicInformation       0
#define SystemPerformanceInformation 2
#define SystemTimeInformation        3
#define Li2Double(x) ((double)((x).HighPart) * 4.294967296E9 + (double)((x).LowPart))
#else
#include <sys/vfs.h> 
#endif



OS_Environment::OS_Environment()
{
}

OS_Environment::~OS_Environment()
{

}

long OS_Environment::MemoryRealtimeUsage()
{
#if defined(WIN32) || defined(__WIN32__)
  MEMORYSTATUSEX statusEx;
  statusEx.dwLength = sizeof(MEMORYSTATUSEX);
  GlobalMemoryStatusEx(&statusEx);//���ϵͳ�ڴ���Ϣ
  long percentage = (long)statusEx.dwMemoryLoad;
//  printf("MemoryRealtimeUsage: %d\n",percentage);
  return percentage;
#else
  return 0;
#endif
}


long OS_Environment::CPURealtimeUsage()
{
#if defined(WIN32) || defined(__WIN32__)
	SYSTEM_PERFORMANCE_INFORMATION SysPerfInfo;//������Ϣ
	SYSTEM_TIME_INFORMATION        SysTimeInfo;//ʱ����Ϣ
	SYSTEM_BASIC_INFORMATION       SysBaseInfo;//������Ϣ
	typedef LONG (WINAPI *PROCNTQSI)(UINT,PVOID,ULONG,PULONG);
	PROCNTQSI NtQuerySystemInformation;
	double                         dbIdleTime;
	double                         dbSystemTime;
	static LARGE_INTEGER                  liOldIdleTime = {0,0};
	static LARGE_INTEGER                  liOldSystemTime = {0,0};
	LONG                           status;
	NtQuerySystemInformation = (PROCNTQSI)GetProcAddress(GetModuleHandle("ntdll"),"NtQuerySystemInformation");//����ntdll��
	if (!NtQuerySystemInformation)      return 0;
	// ��ô���������
	status = NtQuerySystemInformation(SystemBasicInformation,&SysBaseInfo,sizeof(SysBaseInfo),NULL);//���ϵͳ�Ļ�����Ϣ
	if (status != NO_ERROR)       return 0 ;
	//�����µ�ϵͳʱ�䣺
	status = NtQuerySystemInformation(SystemTimeInformation,&SysTimeInfo,sizeof(SysTimeInfo),0);//���ϵͳ��ʱ����Ϣ
	if (status!=NO_ERROR)      return 0;
	status = NtQuerySystemInformation(SystemPerformanceInformation,&SysPerfInfo,sizeof(SysPerfInfo),NULL);//���ϵͳ��������Ϣ
	if (status != NO_ERROR)    return 0; 
	dbIdleTime = Li2Double(SysPerfInfo.liIdleTime) - Li2Double(liOldIdleTime);//���ϵͳ����ʱ��
	dbSystemTime = Li2Double(SysTimeInfo.liKeSystemTime) - Li2Double(liOldSystemTime);//���ϵͳ����ʱ��
	dbIdleTime = dbIdleTime / dbSystemTime;
	dbIdleTime = 100.0 - dbIdleTime * 100.0 / (double)SysBaseInfo.bKeNumberProcessors;//����ϵͳ��cpuʹ����
	long percentage=(long)dbIdleTime;

	liOldIdleTime=SysPerfInfo.liIdleTime;//���¿���ʱ��
	liOldSystemTime=SysTimeInfo.liKeSystemTime;//����ϵͳʱ��

//	printf("CPURealtimeUsage: %d\n",percentage);

	return percentage;

#else
	return 0;
#endif
}