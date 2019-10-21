///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����CreateAlarmXMLForDVBC.h
// �����ߣ�gaoxd
// ����ʱ�䣺2011-01-27
// ���������������������ֵ��ӱ�����Ϣ
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