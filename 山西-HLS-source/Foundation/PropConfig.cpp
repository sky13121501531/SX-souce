#pragma warning(disable:4996)
#include "PropConfig.h"
#include "StrUtil.h"
#include "ace/Log_Msg.h"

PropConfig::PropConfig(void)
{
	mInitiated=false;
}

PropConfig::~PropConfig(void)
{
	
}

PropConfig::PropConfig(const char * PropFileName)
{
	LoadPropFile(PropFileName);
}

// <summary>���������ļ�</summary>
// <param name="PropFileName">�����ļ������ɰ���·��</param>
// <retvalue></retvalue>
bool PropConfig::LoadPropFile(const char* PropFileName )
{
	try
	{
		if (!fopen(PropFileName,"r"))
		{
			cout<<"�Ҳ��������ļ�"<<endl;
			return false;
		}
		mInitiated=mXmlProp.LoadFromFile(PropFileName);
		if (!mInitiated)
		{
			cout<<"���������ļ���ʽ����ȷ"<<endl;
			return false;
		}
	}
	catch (...)
	{
		cout<<"�Ҳ��������ļ��������ļ�����"<<endl;
	}
	return true;
}

// <summary>��������ļ��нڵ���ı�ֵ</summary>
// <param name="parNode">���ڵ�����</param>
// <param name="chilNode">�ӽڵ�����</param>
// <retvalue>�ڵ���ı�ֵ</retvalue>
string PropConfig::GetNodeText(const char * parNode,const char * chilNode)
{
	string nodePath="properties/";
	nodePath+=parNode;
	nodePath+="/";
	nodePath+=chilNode;
	string ret="";
	try
	{
		pXMLNODE node=mXmlProp.GetNodeFromPath((char *)nodePath.c_str());
		if (node!=NULL)
		{
			ret=mXmlProp.GetNodeText(node);
		}		
	}
	catch (...)
	{
		
	}
	return ret;
}

// <summary>��������ļ��нڵ������ֵ</summary>
// <param name="parNode">���ڵ�����</param>
// <param name="chilNode">�ӽڵ�����</param>
// <retvalue>��Ӧ�ڵ������ֵ</retvalue>
string PropConfig::GetNodeAttribute(const char * parNode,const char * chilNode,const char * attribute)
{
	string nodePath="properties/";
	nodePath+=parNode;
	nodePath+="/";
	nodePath+=chilNode;
	string ret="";
	bool result;
	try
	{
		pXMLNODE node=mXmlProp.GetNodeFromPath((char *)nodePath.c_str());
		if (node!=NULL)
		   result=mXmlProp.GetAttrNode(node,string(attribute),ret);
	}
	catch (...)
	{

	}
	return ret;
}

// <summary>���������ļ��нڵ���ı�ֵ</summary>
// <param name="parNode">���ڵ�����</param>
// <param name="chilNode">�ӽڵ�����</param>
// <param name="valu">Ҫ���õĽڵ���ı�ֵ</param>
// <retvalue>���óɹ�true��ʧ��false</retvalue>
bool PropConfig::SetNodeText(const char * parNode,const char * chilNode,const char * valu)
{
	string nodePath="properties/";
	nodePath+=parNode;
	nodePath+="/";
	nodePath+=chilNode;
	string ret="";
	bool result;
	try
	{
		pXMLNODE node=mXmlProp.GetNodeFromPath((char *)nodePath.c_str());
		if(node!=NULL)
			result=mXmlProp.SetNodeText(node,(char *)valu);
		if (result)
		{
			mXmlProp.SaveAll();
		}
	}
	catch (...)
	{

	}
	return result;
}

// <summary>���������ļ��нڵ������ֵ</summary>
// <param name="parNode">���ڵ�����</param>
// <param name="chilNode">�ӽڵ�����</param>
// <param name="attribute">Ҫ���õĽڵ��������</param>
// <param name="atvalutribute">Ҫ���õĽڵ������ֵ</param>
// <retvalue>���óɹ�true��ʧ��false</retvalue>
bool PropConfig::SetNodeAttribute(const char * parNode,const char * chilNode,const char * attribute,const char * valu)
{

	string nodePath="properties/";
	nodePath+=parNode;
	nodePath+="/";
	nodePath+=chilNode;
	string ret="";
	bool result;
	try
	{
		pXMLNODE node=mXmlProp.GetNodeFromPath((char *)nodePath.c_str());
		if(node!=NULL)
			result=mXmlProp.SetAttrNode(string(attribute),string(valu),node);
		if (result)
		{
			mXmlProp.SaveAll();
		}
	}
	catch (...)
	{

	}
	return result;
}

bool PropConfig::SetNodeAttribute(string dvbtype,const char * parNode,const char * attribute,const char * valu)
{

	string type;
	string nodePath="properties/";
	nodePath+=parNode;
	string ret="";
	bool result;
	try
	{
		pXMLNODE node=mXmlProp.GetNodeFromPath((char *)nodePath.c_str());
		pXMLNODELIST NodeList=mXmlProp.GetNodeList(node);
		int count=mXmlProp.GetChildCount(node);
		for(int i=0;i<count;i++)
		{
			pXMLNODE childNode=mXmlProp.GetNextNode(NodeList);
			mXmlProp.GetAttrNode(childNode,"dvbtype",type);
			if(type==dvbtype)
			{
				result=mXmlProp.SetAttrNode(string(attribute),string(valu),childNode);
				break;
			}
			
		}
		if (result)
		{
			mXmlProp.SaveAll();
		}
	}
	catch (...)
	{

	}
	return result;
}
bool PropConfig::SetNodeAttribute(string dvbtype,string freq,const char * parNode,const char * attribute,const char * valu)
{

	string type,tempfreq;
	string nodePath="properties/";
	nodePath+=parNode;
	string ret="";
	bool result;
	try
	{
		pXMLNODE node=mXmlProp.GetNodeFromPath((char *)nodePath.c_str());
		pXMLNODELIST NodeList=mXmlProp.GetNodeList(node);
		int count=mXmlProp.GetChildCount(node);
		for(int i=0;i<count;i++)
		{
			pXMLNODE childNode=mXmlProp.GetNextNode(NodeList);
			mXmlProp.GetAttrNode(childNode,"dvbtype",type);
			mXmlProp.GetAttrNode(childNode,"freq",tempfreq);
			if(type==dvbtype&&tempfreq==freq)
			{
				result=mXmlProp.SetAttrNode(string(attribute),string(valu),childNode);
				break;
			}

		}
		if (result)
		{
			mXmlProp.SaveAll();
		}
	}
	catch (...)
	{

	}
	return result;
}

bool PropConfig::SetNodeAttribute( pXMLNODE node,string attribute,string valu )
{
	mXmlProp.SetAttrNode(attribute,valu,node);
	mXmlProp.SaveAll();
	return true;
}


#ifdef ZONG_JU_ZHAO_BIAO
bool PropConfig::SetNodeAttribute( string parNode, string strAttrName, string strAttrValue, string strSetAttrName, string strSetValue/*, string strNoUse*/)
{
	string type;
	string nodePath="properties/";
	nodePath+=parNode;
	string ret="";
	bool result;
	try
	{
		pXMLNODE node=mXmlProp.GetNodeFromPath((char *)nodePath.c_str());
		pXMLNODELIST NodeList=mXmlProp.GetNodeList(node);
		int count=mXmlProp.GetChildCount(node);
		for(int i=0; i<count; i++)
		{
			pXMLNODE childNode=mXmlProp.GetNextNode(NodeList);
			mXmlProp.GetAttrNode(childNode, strAttrName.c_str() ,type);
			if(type==strAttrValue)	//������ͬ���޸ĸ�����ֵ
			{
				result=mXmlProp.SetAttrNode(strSetAttrName, strSetValue, childNode);
				break;
			}

		}
// 		if (i == count)	//���û�и����͵���Ϣ���ʹ���xml�ڵ㣬�����Ϣ
// 		{
// 			pXMLNODE newCenterNode = mXmlProp.CreateNodePtr(node, "centercode");
// 			result=mXmlProp.SetAttrNode(strAttrName, strAttrValue, newCenterNode);
// 			result=mXmlProp.SetAttrNode(strSetAttrName, strSetValue, newCenterNode);
// 		}
		if (result)
		{
			mXmlProp.SaveAll();
		}
	}
	catch (...)
	{

	}
	return result;
}
#endif



pXMLNODE PropConfig::CreateNode( char* parNode,char* childNode )
{
	string nodePath="properties/";
	nodePath+=parNode;
	string ret="";
	try
	{
		pXMLNODE node=mXmlProp.GetNodeFromPath((char *)nodePath.c_str());
		pXMLNODE retNode=mXmlProp.CreateNodePtr(node,childNode);
		if (retNode)
		{
			mXmlProp.SaveAll();
			return retNode;
		}
	}
	catch (...)
	{

	}
	return NULL;
}

bool PropConfig::DelNode( char* parNode,char* childNode )
{
	string nodePath="properties/";
	nodePath+=parNode;
	bool ret=false;
	try
	{
		pXMLNODE node=mXmlProp.GetNodeFromPath((char *)nodePath.c_str());
		nodePath+="/";
		nodePath+=childNode;
		pXMLNODE child=mXmlProp.GetNodeFromPath((char *)nodePath.c_str());
		ret=mXmlProp.DeleteNode(node,child);
		if (ret)
		{
			mXmlProp.SaveAll();
			return ret;
		}
	}
	catch (...)
	{

	}
	return ret;
}

pXMLNODE PropConfig::CreateNode( char* parNode,char* childNode,string dvbtype,string freq,string alarmthreshold )
{
	string nodePath="properties/";
	nodePath+=parNode;
	string ret="";
	try
	{
		pXMLNODE node=mXmlProp.GetNodeFromPath((char *)nodePath.c_str());
		pXMLNODE retNode=mXmlProp.CreateNodePtr(node,childNode);
		mXmlProp.SetAttrNode("dvbtype",dvbtype,retNode);
		mXmlProp.SetAttrNode("freq",freq,retNode);
		mXmlProp.SetAttrNode("downthreshold",alarmthreshold,retNode);
		if (retNode)
		{
			mXmlProp.SaveAll();
			return retNode;
		}
	}
	catch (...)
	{

	}
	return NULL;
}