///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����TranslateDownXML.h
// �����ߣ�jiangcheng
// ����ʱ�䣺2009-06-04
// �������������·�XMLת��Ϊ���������XML
///////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <vector>
#include <string>
#include "../Foundation/XmlParser.h"

class TranslateDownXML
{
public:
	//���ر�׼��xml
	static bool TranslateTaskXML(const std::string& strOriginalXML,std::string& strStandardXML);

	//���ر�׼��¼��ִ������XML
	static bool TranslateRecordTask(const std::string& strOriginalXML,std::vector<std::string>& strStandardXMLVec);
	//���ر�׼�Ă���¼��ִ������XML
	static bool TranslateRoundRecordTask(const std::string& strOriginalXML,std::vector<std::string>& strStandardXMLVec);
	//���ر�׼�Ķ�ʱָ���ѯ����xml
	static bool TranslateQualityTask(const std::string& strOriginalXML,std::vector<std::string>& strStandardXMLVec);
	//���ر�׼�Ķ�ʱƵ������xml
	static bool TranslateSpectrumTask(const std::string& strOriginalXML,std::vector<std::string>& strStandardXMLVec);
};