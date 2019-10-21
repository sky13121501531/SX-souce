#pragma once
#include <string>
#include "TypeDef.h"
#include "../DeviceAccess/CardType.h"
class OSFunction
{
public:
	static void Sleep(int sencond,int millisecond=0);
	static void CompressFile(std::string dstZipPath,std::string srcFilePath);  //ѹ���ļ������
	static void GetFileName(eDVBType dvbType,std::string taskType,std::string& strFileName);

	static string GetXmlMsgID(string datetime);

	static eDVBType GetEnumDVBType(string StrDVBType);
	static string   GetStrDVBType(eDVBType eDVBType);
	static string   GetAlarmTypeID(eDVBType type,string alarmtype);
	static bool SetOSDTimeMode(int mode,OSDInfo& osd);

	//�����忨����ָ��xml
	static string CreateTaskTypeXml(int deviceid);
	static string CreateCardRebootXml(int deviceid);
	//�����忨��汾xml
	static string CreateTableVersionXml(enumDVBType dvbtype);

	static void ExitProcess(string info);
};