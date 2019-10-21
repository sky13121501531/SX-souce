#ifndef _XMLPARSER_H_
#define _XMLPARSER_H_

#include <string>
#include <list>

#include "../../thirdlib/xml/tinyxml.h"

using namespace std;

#define pXMLNODE TiXmlNode* 
#define pXMLELEMENT TiXmlElement* 
#define pXMLDOC TiXmlDocument* 

//�ڵ��б��࣬ͨ��������Ի�ýڵ�������е��ӽڵ㣬���ŵ�list��
class XmlNodeList
{
public:
	XmlNodeList(void)
	{
		mNodeListPosition = 0;
		mNodeListCount = 0;
	};
	virtual ~XmlNodeList(void)
	{
		if(0 < mXMLNodeList.size())
		{
			mXMLNodeList.clear();
		}

		mNodeListPosition = 0;
		mNodeListCount = 0;
	};
public:
	pXMLNODE GetFirstNode(void)
	{
		pXMLNODE retNode = NULL;

		if(mXMLNodeList.size() > 0)
		{
			retNode = mXMLNodeList.front();
		}

		mNodeListPosition = 1;

		return retNode;
	};
	pXMLNODE GetNextNode(void)
	{
		pXMLNODE retNode = NULL;

		if(mNodeListPosition < (long)mXMLNodeList.size())
		{
			list<pXMLNODE>::iterator p = mXMLNodeList.begin();
			for(int i = 0; i < mNodeListPosition; i++)
			{
				p ++;
			}
			retNode = (*p);

			mNodeListPosition ++;
		}

		return retNode;
	};

	void PushNode(pXMLNODE& node)
	{
		if(node)
		{
			mXMLNodeList.push_back(node);
			mNodeListCount ++;
		}
		else
		{
		}
	};

	long Size(void)
	{
		return mNodeListCount;
	};

	void operator = (XmlNodeList const& OtherXmlNodeList )
	{
		mXMLNodeList = OtherXmlNodeList.mXMLNodeList;
		mNodeListPosition = OtherXmlNodeList.mNodeListPosition;
		mNodeListCount = OtherXmlNodeList.mNodeListCount;
	};
private:
	list<pXMLNODE> mXMLNodeList;//����ӽڵ������
	long mNodeListPosition;//ָ��������λ��
	long mNodeListCount;//�ӽڵ���Ŀ
};

#define pXMLNODELIST XmlNodeList*
class XmlParser
{
public:
	pXMLNODE findNode(pXMLNODE parNode,char* nodename);
	// char* type must be all path exp: /Msg/Alarm/Music
	pXMLNODE nodeNameToPtr(char* nodename);   
public:

	XmlParser();
	XmlParser(const char* xml);
	virtual ~XmlParser();

	
	bool LoadFromFile(const char *FileName);//����xml�ļ�
	bool SaveAsFile(const char *FileName);//xml���浽�ļ�
	bool SaveAll();//����Ե�ǰxml�ļ����޸�
	void SaveToString(string& mResult);//xml���浽�ַ���
	bool Get_xml(string &xmlstr);//��ȡxml�ַ���
	bool Set_xml(string xmlstr);//����xml�ַ���

	string GetNodeText(char *NodeName);//��ýڵ���ı�����ֵ
	string GetNodeText( pXMLNODE   CurNode);//��ýڵ���ı�����ֵ
	bool SetNodeText(char *NodeName,char *NewText);//���ýڵ���ı�����ֵ
	bool SetNodeText(pXMLNODE CurNode, char * NewText);//���ýڵ���ı�����ֵ
	bool CreateNode(char *ParNodeName,char *NodeName);//�����½ڵ�
	bool CreateNode(pXMLNODE parNode,char * NodeName);//�����½ڵ�

	pXMLNODE CreateNodePtr(char *ParNodeName,char *NodeName);//�����½ڵ�
	pXMLNODE CreateNodePtr(pXMLNODE parNode,char * NodeName);//�����½ڵ�
	bool DeleteNode(char *ParNodeName,char *DelNodeName);//ɾ���ڵ�
	bool DeleteNode(pXMLNODE parNode, pXMLNODE childNode);//ɾ���ڵ�
	pXMLNODE GetNodeFromPath(char* NodePath);//����·���õ��ڵ�

	bool IsChildofNode(pXMLNODE parNode, pXMLNODE childNode);//�жϽڵ��Ƿ��Ǹ��ӹ�ϵ
	bool IsChildofNode(char* parNode,char* childNode);//�жϽڵ��Ƿ��Ǹ��ӹ�ϵ
	string GetRootNodeName();	//��ø��ڵ������
	string GetNodeName(pXMLNODE CurNode);//���ָ���ڵ������

	pXMLNODE GetRootNode();//��ø��ڵ�
	pXMLNODE GetNodeFirstChild(pXMLNODE CurNode);//��ýڵ�ĵ�һ���ӽڵ�
	pXMLNODELIST GetNodeList(pXMLNODE CurNode);//��ýڵ���ӽڵ��б�
	long GetChildCount(pXMLNODE CurNode);//��ýڵ���ӽڵ���
	string GetNodePath(pXMLNODE CurNode);//��ýڵ���xml�ļ��е�·��
	pXMLNODE GetParentNode(pXMLNODE CurNode);//��ýڵ�ĸ��ڵ�

	
	pXMLNODE GetFirstNode(pXMLNODELIST CurList);//���ӽڵ��б��л�õ�һ���ӽڵ�
	pXMLNODE GetNextNode(pXMLNODELIST CurList);//���ӽڵ��б��л����һ���ӽڵ�

	bool IsAttributeOfNode(pXMLNODE node,string attrname);//�ж������Ƿ����ڽڵ�

	bool GetAttrNode(pXMLNODE pNode,string nodeName,string& nodevalue);//���ָ���ڵ��ĳ������ֵ(��������ֵ��������)
	bool GetAttrNode(pXMLNODE pNode,string nodeName,int& nodevalue);//���ָ���ڵ��ĳ������ֵ
	bool GetAttrNode(pXMLNODE pNode,string nodeName,float& nodevalue);//���ָ���ڵ��ĳ������ֵ
	bool GetAttrNode(pXMLNODE pNode,string nodeName,double& nodevalue);//���ָ���ڵ��ĳ������ֵ
	bool GetAttrNode(pXMLNODE pNode,string nodeName,bool& nodevalue);//���ָ���ڵ��ĳ������ֵ
	bool GetAttrNode(pXMLNODE pNode,string nodeName,long& nodevalue);//���ָ���ڵ��ĳ������ֵ

	bool SetAttrNode(string nodeName,string nodeValue,pXMLNODE pNode);//����ָ���ڵ��ĳ������ֵ(��������ֵ��������)
	bool SetAttrNode(string nodeName,int nodeValue,pXMLNODE pNode);//����ָ���ڵ��ĳ������ֵ
	bool SetAttrNode(string nodeName,double nodeValue,pXMLNODE pNode);//����ָ���ڵ��ĳ������ֵ
	bool SetAttrNode(string nodeName,float nodeValue,pXMLNODE pNode);//����ָ���ڵ��ĳ������ֵ
	bool SetAttrNode(string nodeName,bool nodeValue,pXMLNODE pNode);//����ָ���ڵ��ĳ������ֵ
	bool SetAttrNode(string nodeName,long nodeValue,pXMLNODE pNode);//����ָ���ڵ��ĳ������ֵ
private:
	pXMLDOC mXmlDocPtr;
	char* mCurFileName;
	long listIndex;
};


#endif