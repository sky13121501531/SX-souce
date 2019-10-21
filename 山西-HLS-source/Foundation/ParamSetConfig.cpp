#pragma warning(disable:4996)
#include "ParamSetConfig.h"
#include "StrUtil.h"
#include "ace/Log_Msg.h"

ParamSetConfig::ParamSetConfig(void)
{
	mInitiated=false;
}

ParamSetConfig::~ParamSetConfig(void)
{

}

ParamSetConfig::ParamSetConfig(const char * PropFileName)
{
	LoadPropFile(PropFileName);
}

// <summary>���������ļ�</summary>
// <param name="PropFileName">�����ļ������ɰ���·��</param>
// <retvalue></retvalue>
bool ParamSetConfig::LoadPropFile(const char* PropFileName )
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
string ParamSetConfig::GetNodeText(const char * parNode,const char * chilNode)
{
	string nodePath="ParamSet/";
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
string ParamSetConfig::GetNodeAttribute(const char * parNode,const char * chilNode,const char * attribute)
{
	string nodePath="ParamSet/";
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
bool ParamSetConfig::SetNodeText(const char * parNode,const char * chilNode,const char * valu)
{
	string nodePath="ParamSet/";
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
bool ParamSetConfig::SetNodeAttribute(const char * parNode,const char * chilNode,const char * attribute,const char * valu)
{

	string nodePath="ParamSet/";
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

bool ParamSetConfig::SetNodeAttribute(string channelid,const char * parNode,const char * attribute,const char * valu)
{

	string type;
	string nodePath="ParamSet/";
	nodePath+=parNode;
	string ret="";
	bool result=false;
	try
	{
		pXMLNODE node=mXmlProp.GetNodeFromPath((char *)nodePath.c_str());
		pXMLNODELIST NodeList=mXmlProp.GetNodeList(node);
		int count=mXmlProp.GetChildCount(node);
		for(int i=0;i<count;i++)
		{
			string tempchannelid,freq,seviceid;
			pXMLNODE childNode=mXmlProp.GetNextNode(NodeList);
			mXmlProp.GetAttrNode(childNode,"Freq",freq);
			mXmlProp.GetAttrNode(childNode,"ServiceID",seviceid);
 			if (seviceid.length()<1 && attribute=="ServiceID")
			{
 				seviceid = valu;
 			}
			tempchannelid=freq+"_"+seviceid;
			if(tempchannelid==channelid)
			{
				result=mXmlProp.SetAttrNode(string(attribute),string(valu),childNode);
				break;
			}

		}
		if(!result)
		{
			//size_t pos=channelid.find("_");
			//string freq=channelid.substr(0,pos);
			//string serviceid=channelid.substr(pos+1);
			pXMLNODE NewNode=CreateNode(parNode,"AudioParam");
			//mXmlProp.SetAttrNode(string("Freq"),freq,NewNode);
			//mXmlProp.SetAttrNode(string("ServiceID"),serviceid,NewNode);
			result=mXmlProp.SetAttrNode(string(attribute),string(valu),NewNode);

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

bool ParamSetConfig::SetNodeAttribute( pXMLNODE node,string attribute,string valu )
{
	mXmlProp.SetAttrNode(attribute,valu,node);
	mXmlProp.SaveAll();
	return true;
}
pXMLNODE ParamSetConfig::CreateNode( const char* parNode,char* childNode )
{
	string nodePath="ParamSet/";
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

bool ParamSetConfig::DelNode( char* parNode,char* childNode )
{
	string nodePath="ParamSet/";
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