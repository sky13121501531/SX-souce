///////////////////////////////////////////////////////////////////////////////////////////
// 文件名：CreateAlarmXMLForDVBC.h
// 创建者：gaoxd
// 创建时间：2011-01-27
// 内容描述：生成有线数字电视报警信息
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <string>
#include "../Foundation/TypeDef.h"

class CreateAlarmXMLForDVBC
{
public:
	static std::string CreateFreqAlarmXML(const sCheckParam& checkparam,std::string alarmvalue,std::string alarmid);
	static std::string CreateProgramAlarmXML(const sCheckParam& checkparam,std::string alarmvalue,std::string alarmid);
	static std::string CreateEnvironmentAlarmXML(const sCheckParam& checkparam,std::string alarmvalue);
	static std::string CreateEquipmentAlarmXML(const sCheckParam& checkparam,std::string alarmvalue);
};