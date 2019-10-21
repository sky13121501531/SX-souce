#pragma once
#include <string>
#include <vector>
class TranslateXMLForDevice
{
public:
	TranslateXMLForDevice(void);
	~TranslateXMLForDevice(void);
public:
	//ʵʱ��Ƶ�����׼XMLת��Ϊ��Ӳ��ͨѶ��XML
	static std::string TranslateStreamRealTimeXML(std::string strStdXML);
	//ָ���ѯ�����׼XMLת��Ϊ��Ӳ��ͨѶ��XML
	static std::string TranslateQualityRealTimeQuery(std::string strStdXML);
	//¼�������׼XMLת��Ϊ��Ӳ��ͨѶ��XML
	static std::string TranslateRecordTask(std::string strStdXML);
	//�ֲ������׼XMLת��Ϊ��Ӳ��ͨѶ��XML
	static std::vector<std::string> TranslateRoundStreamXML(std::string strStdXML);
	static std::vector<std::string> TranslateRoundRecordXML(std::string strStdXML);
	//����ָ���׼XMLת��Ϊ��Ӳ��ͨѶ��XML
	static std::vector<std::string> TranslateQualityTask(std::string strStdXML);
	static std::string TranslateSpectrumScan(std::string strStdXML);
	static std::string TranslateSpectrumTask(std::string strStdXML);

	//����¼�������׼XMLת��Ϊ��Ӳ��ͨѶ��XML
	static std::vector<std::string> TranslateRoundRoundXML(std::string strStdXML);
private:
	static std::string GetXMLHeader(std::string downType);
};
