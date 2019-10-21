#ifndef _PARAMSETCONFIG_H_
#define _PARAMSETCONFIG_H_


#include <map>
#include <string>
#include <list>
#include "XmlParser.h"


//�����ļ�������
class ParamSetConfig
{
public:
	ParamSetConfig(void);
	virtual~ParamSetConfig(void);
	ParamSetConfig(const char * PropFileName);
	bool LoadPropFile(const char* PropFileName );//���������ļ�
	string GetNodeText(const char * parNode,const char * chilNode);//��������ļ���ĳ�ڵ���ı�����ֵ
	string GetNodeAttribute(const char * parNode,const char * chilNode,const char * attribute);//��������ļ���ĳ�ڵ������ֵ
	bool SetNodeText(const char * parNode,const char * chilNode,const char * valu);//���������ļ���ĳ�ڵ���ı�����ֵ
	bool SetNodeAttribute(const char * parNode,const char * chilNode,const char * attribute,const char * valu);//���������ļ���ĳ�ڵ������ֵ
	bool isInitiated(){return mInitiated;};//���������ļ��Ƿ�ɹ�

	pXMLNODE CreateNode(const char* parNode,char* childNode);
	bool SetNodeAttribute(pXMLNODE node,string attribute,string valu);
	bool DelNode(char* parNode,char* childNode);
	bool SetNodeAttribute(string dvbtype,const char * parNode,const char * attribute,const char * valu);

private:
	XmlParser mXmlProp;
	bool mInitiated;

};
#endif