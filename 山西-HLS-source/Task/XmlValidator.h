#ifndef XmlValidator_h__
#define XmlValidator_h__



#include <string>


//xml��֤��
class XmlValidator
{
public:
	XmlValidator();
	~XmlValidator();

public:
	//��֤Ƶ��ɨ����xml����ȷ��
	static std::string ValidChanScanXml(const std::string& strRetXml);

};
























#endif // XmlValidator_h__