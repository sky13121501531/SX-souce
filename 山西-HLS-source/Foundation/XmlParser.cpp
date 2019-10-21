
#pragma warning(disable:4996)

#include <sys/stat.h>
#include <vector>
#include "XmlParser.h"
#include "ace/OS.h"

XmlParser::XmlParser()
{
	mXmlDocPtr  = new TiXmlDocument;
	mCurFileName = NULL;
	listIndex = 0;
	
};
XmlParser::XmlParser(const char* xml)
{
	listIndex = 0;
	mXmlDocPtr  = new TiXmlDocument;
	if (strlen(xml) ==0)
	{
		cout << "buf empty" <<endl;
		return;
	}
	if(!mXmlDocPtr->Parse(xml))
	{
		//cout << mXmlDocPtr->ErrorDesc() << endl;
		return;
	}
}
XmlParser::~XmlParser()
{	
	if (mXmlDocPtr != NULL)
		delete mXmlDocPtr;
};
// <summary>xml���󱣴浽�ַ���</summary>
// <param name="mResult">��������xml���ַ���</param>
// <retvalue></retvalue>
void XmlParser::SaveToString(std::string& mResult)
{
	 Get_xml(mResult);
};
// <summary>�����ļ�������xml����</summary>
// <param name="FileName">�ļ���</param>
// <retvalue>�ɹ���ʧ��</retvalue>
bool XmlParser::LoadFromFile(const char *FileName)
{  
	bool ret = false;
	if(FileName != NULL)
	{
		try
		{
			if( mXmlDocPtr->LoadFile(FileName))
			{
				mCurFileName =(char*) FileName;
				ret = true;
			}
		}
		catch(...)
		{
			cout<<"�Ƿ���xml�ļ�����ָ����xml�ļ�������"<<endl;
			return false;
		}
	}
	return ret;
}
// <summary>�����ӽڵ����Ʋ����ӽڵ�</summary>
// <param name="parNode">���ڵ�</param>
// <param name="nodename">�ӽڵ�����</param>
// <retvalue>�ҵ����ӽڵ���߿�</retvalue>
pXMLNODE XmlParser::findNode(pXMLNODE parNode,char* nodename)
{
	if ((parNode==NULL)||(strlen(nodename)==0))
		return NULL;
	if((parNode->Type() == TiXmlNode::ELEMENT) && (!strcmp(parNode->Value(),nodename)))
		return parNode;

	pXMLNODE child = NULL;
	for(  child= parNode->FirstChild();child;child=child->NextSibling())
	{
		if((child->Type() == TiXmlNode::ELEMENT) && (!strcmp(child->Value(),nodename)))
			break;               
	}
	return child;
}
// <summary>xml���󱣴浽�ļ�</summary>
// <param name="FileName">�ļ���</param>
// <retvalue>�������ɹ�����ʧ��</retvalue>
bool XmlParser::SaveAsFile(const char *FileName)
{
	bool ret = false;
	if (!mXmlDocPtr) return ret;
	if (FileName!=NULL)
	{
		ret=mXmlDocPtr ->SaveFile(FileName);
	}
	return ret;
};
// <summary>�����xml�ļ����޸�</summary>
// <param name=""></param>
// <retvalue>�������ɹ�����ʧ��</retvalue>
bool XmlParser::SaveAll()
{
	bool ret = false;
	if (!mXmlDocPtr) return ret;
	if (mCurFileName!=NULL)
	{
		ret = mXmlDocPtr->SaveFile(mCurFileName);
	}	
	return ret;
};
// <summary>Ϊ�ڵ㴴���ӽڵ�</summary>
// <param name="ParNodeName">���ڵ�����</param>
// <param name="NodeName">�ӽڵ�����</param>
// <retvalue>�����ɹ�����ʧ��</retvalue>
bool XmlParser::CreateNode(char *ParNodeName,char *NodeName)
{
	if(ParNodeName!=NULL)
	{
		pXMLNODE temp =  nodeNameToPtr(ParNodeName);
		if (temp == NULL)
			return false;
		TiXmlElement pNode(NodeName);
		if (temp->InsertEndChild(pNode) != NULL)
			return true;
		else
			return false;
	}
	else
		return false;
}
// <summary>Ϊ�ڵ㴴���ӽڵ�</summary>
// <param name="parNode">���ڵ�</param>
// <param name="NodeName">�ӽڵ�����</param>
// <retvalue>�����ɹ�����ʧ��</retvalue>
bool XmlParser::CreateNode(pXMLNODE parNode,char * NodeName)
{
		TiXmlElement pNode(NodeName);
		if (parNode == NULL)
			return false;
		if (parNode->InsertEndChild(pNode) != NULL)
			return true;
		else
			return false;
}
// <summary>Ϊ�ڵ㴴���ӽڵ�</summary>
// <param name="ParNodeName">���ڵ�����</param>
// <param name="NodeName">�ӽڵ�����</param>
// <retvalue>�´����ӽڵ��ָ��</retvalue>
pXMLNODE XmlParser::CreateNodePtr(char *ParNodeName,char *NodeName)
{
	if(ParNodeName!=NULL)
	{
		pXMLNODE temp =  nodeNameToPtr(ParNodeName);
		if (temp == NULL)
			return NULL;
		TiXmlElement pNode(NodeName);
		pXMLNODE temp1 = NULL;
		temp1 = temp->InsertEndChild(pNode);
		if (temp1 != NULL)
			return temp1;
		else
			return NULL;
	}
	return NULL;
}
// <summary>Ϊ�ڵ㴴���ӽڵ�</summary>
// <param name="parNode">���ڵ�</param>
// <param name="NodeName">�ӽڵ�����</param>
// <retvalue>�´����ӽڵ��ָ��</retvalue>
pXMLNODE XmlParser::CreateNodePtr(pXMLNODE parNode,char * NodeName)
{
	if(parNode!=NULL)
	{
		TiXmlElement pNode(NodeName);
		pXMLNODE temp1 = NULL;
		temp1 = parNode->InsertEndChild(pNode);
		if (temp1 != NULL)
			return temp1;
		else
			return NULL;
	}
	else
		return NULL;
}
// <summary>ɾ���ڵ�</summary>
// <param name="ParNodeName">���ڵ�����</param>
// <param name="DelNodeName">Ҫɾ�����ӽڵ�����</param>
// <retvalue>ɾ���ɹ���ʧ��</retvalue>
bool XmlParser::DeleteNode(char *ParNodeName,char *DelNodeName)
{
	if ((ParNodeName==NULL)||(DelNodeName==NULL))
		return false;
	pXMLNODE temp =  nodeNameToPtr(ParNodeName);
	pXMLNODE temp1 =  nodeNameToPtr(DelNodeName);
	if ((temp == NULL) || (temp1 == NULL))
		return false;
	if (temp1->Parent() != temp)
		return false;
	return temp->RemoveChild(temp1);
}
// <summary>ɾ���ڵ�</summary>
// <param name="parNode">���ڵ�</param>
// <param name="childNode">Ҫɾ�����ӽڵ�</param>
// <retvalue>ɾ���ɹ���ʧ��</retvalue>
bool XmlParser::DeleteNode(pXMLNODE parNode, pXMLNODE childNode)
{
	if ((parNode == NULL) || (childNode == NULL))
		return false;
	if (childNode->Parent() != parNode)
		return false;
	return parNode->RemoveChild(childNode);
}
// <summary>���ݽڵ�·���õ��ڵ�</summary>
// <param name="NodePath">�ڵ�·��������a/b/c��ʽ</param>
// <retvalue>��Ӧ·���Ľڵ�</retvalue>
pXMLNODE XmlParser:: GetNodeFromPath(char* NodePath)
{
	return nodeNameToPtr(NodePath);
}

// <summary>��ýڵ���ı�</summary>
// <param name="CurNode">�ڵ������</param>
// <retvalue>�ڵ���ı�����</retvalue>
string XmlParser::GetNodeText(char *NodeName)
{
	string ret("");
	if(NodeName==NULL)
		return ret;
	TiXmlElement * temp =  (TiXmlElement *)nodeNameToPtr(NodeName);
	if (temp != NULL)
	{
		if (temp->GetText() != NULL)
			ret += temp->GetText();
	}
	return ret;
}

// <summary>��ýڵ���ı�</summary>
// <param name="CurNode">�ڵ��ָ��</param>
// <retvalue>�ڵ���ı�����</retvalue>
string XmlParser::GetNodeText(pXMLNODE CurNode)
{
	string ret("");
	if (CurNode != NULL)
		if(CurNode->Type() == TiXmlNode::ELEMENT)
		{
			const pXMLELEMENT box = (const pXMLELEMENT)CurNode;
			if (box->GetText() != NULL)
				ret +=box->GetText();
		}
		return ret;
}

// <summary>���ýڵ���ı�</summary>
// <param name="CurNode">�ڵ������</param>
// <param name="NewText">�ı�����</param>
// <retvalue>true�ɹ���falseʧ��</retvalue>
bool XmlParser::SetNodeText(char *NodeName,char *NewText)
{
	if ((NodeName==NULL)||(NewText==NULL))
		return false;
	pXMLNODE temp =  nodeNameToPtr(NodeName);
	if (temp == NULL)
		return false;
	TiXmlText  pText((const char*)NewText);        
	if (GetNodeText(NodeName).length() ==0)
	{
		temp->InsertEndChild(pText);
	} else
	{
		temp->Clear();
		temp->InsertEndChild(pText);
	}            
	return true;
}
// <summary>���ýڵ���ı�</summary>
// <param name="CurNode">�ڵ�ָ��</param>
// <param name="NewText">�ı�����</param>
// <retvalue>true�ɹ���falseʧ��</retvalue>
bool XmlParser::SetNodeText(pXMLNODE CurNode, char * NewText)
{
	if ((CurNode==NULL)||(NewText==NULL))
		return false;
	TiXmlText  pText((const char*)NewText);    
	if (CurNode == NULL)
		return false;
	if (GetNodeText(CurNode).length() ==0)
	{
		CurNode->InsertEndChild(pText);
	} else
	{
		CurNode->Clear();
		CurNode->InsertEndChild(pText);
	}            
	return true;
}

// <summary>��xml�����л��xml�ַ���</summary>
// <param name="xmlstr">����xml�ַ���</param>
// <retvalue>true�ɹ���falseʧ��</retvalue>
bool XmlParser::Get_xml(std::string &xmlstr)
{
	bool ret = false;
	char temp[100] = {0};
	unsigned long pid=ACE_OS::thr_self();
	ultoa(pid,temp,10);
	if (mXmlDocPtr == NULL)
		return false;
	if (mXmlDocPtr != NULL)
		mXmlDocPtr->SaveFile(temp);
	FILE* mFile = ::fopen(temp, "rb");
	if (mFile == NULL)
		return false;

	struct stat st;
	fstat(fileno(mFile), &st);

	unsigned char*  p = new unsigned char[st.st_size+1];
	memset(p,0,st.st_size+1);
	::fread(p, sizeof(char), st.st_size, mFile);
	xmlstr.clear();
	xmlstr.append((const char*)p);
	::fclose(mFile);
	unlink(temp);
	delete[] p;
	p=NULL;
	return true;
}

// <summary>����xml�ַ���</summary>
// <param name="xmlstr">���Ϲ����xml�ַ���</param>
// <retvalue>true�ɹ���falseʧ��</retvalue>
bool XmlParser::Set_xml(std::string xmlstr)
{
	if (!mXmlDocPtr->Parse(xmlstr.c_str()))
	{
		//cout<<mXmlDocPtr->ErrorDesc()<<endl;
		return false;
	}
	return true;
}

// <summary>�ж�childNode�ڵ��Ƿ���parNode�ڵ���ӽڵ�</summary>
// <param name="childNode">�ӽڵ��ָ��</param>
// <param name="parNode">���ڵ��ָ��</param>
// <retvalue>true�ǣ�false����</retvalue>
bool XmlParser::IsChildofNode(pXMLNODE parNode, pXMLNODE childNode)
{
	if ((parNode==NULL)||(childNode==NULL))
		return false;
	bool ret = false;
	pXMLNODE calParNode= NULL;
	calParNode = childNode ->Parent();
	if(calParNode)
	{
		//if (GetNodePath(calParNode) == GetNodePath(parNode))//������
		if(parNode==calParNode)
			ret =true;
	}
	return ret;
};

// <summary>�ж�childNode�ڵ��Ƿ���parNode�ڵ���ӽڵ�</summary>
// <param name="childNode">�ӽڵ������</param>
// <param name="parNode">���ڵ������</param>
// <retvalue>true�ǣ�false����</retvalue>
bool XmlParser::IsChildofNode(char* parNode,char* childNode)
{
	bool ret = false;
	if ((parNode==NULL)||(childNode==NULL))
		return ret;
	pXMLNODE calParNodeA=NULL;
	pXMLNODE calParNode=NULL;
	pXMLNODE calChildNode=NULL;
	try
	{
		calParNode = GetNodeFromPath(parNode);
		calChildNode = GetNodeFromPath(childNode);
		if ((calParNode!=NULL)&&(calChildNode!=NULL)&&(calParNode->Parent() != NULL) && (calChildNode->Parent()!=NULL ))
		{
			calParNodeA = calChildNode ->Parent();
			if (GetNodePath(calParNodeA) == GetNodePath(calParNode))
					ret =true;
		}
	} catch(...)
	{}
	return ret;
};

// <summary>��ø��ڵ������</summary>
// <param name=""></param>
// <retvalue>���ڵ������</retvalue>
std::string XmlParser::GetRootNodeName()
{
	return mXmlDocPtr->RootElement()->ValueStr();
};

// <summary>��ýڵ������</summary>
// <param name="CurNode">�ڵ�ָ��</param>
// <retvalue>�ڵ������</retvalue>
std::string XmlParser::GetNodeName(pXMLNODE CurNode)
{
	if(CurNode!=NULL)
		return CurNode->ValueStr();
	else
		return string("");
};

// <summary>��ýڵ�ĸ��ڵ�</summary>
// <param name="CurNode">�ڵ�ָ��</param>
// <retvalue>�ڵ�ĸ��ڵ�</retvalue>
pXMLNODE  XmlParser::GetParentNode(pXMLNODE CurNode)
{
	pXMLNODE ParNode=NULL;
	if(CurNode!=NULL)
		ParNode= CurNode->Parent();
	if (ParNode!=NULL)
	{
		return ParNode;
	} else
		return NULL;
};

// <summary>��ø��ڵ�</summary>
// <param name=""></param>
// <retvalue>���ڵ�</retvalue>
pXMLNODE XmlParser::GetRootNode()
{
	return mXmlDocPtr->RootElement();
};

// <summary>��ýڵ�ĵ�һ���ӽڵ�</summary>
// <param name="CurNode">�ڵ�ָ��</param>
// <retvalue>�ڵ�ĵ�һ���ӽڵ�</retvalue>
pXMLNODE XmlParser::GetNodeFirstChild(pXMLNODE CurNode)
{
	pXMLNODE ChildNode=NULL;
	if(CurNode!=NULL)
		ChildNode= CurNode->FirstChild();
	if (ChildNode)
		return ChildNode;
	else return NULL;
};

// <summary>��ýڵ���ӽڵ��б�</summary>
// <param name="CurNode">�ڵ�ָ��</param>
// <retvalue>�ڵ���ӽڵ��б�</retvalue>
pXMLNODELIST XmlParser::GetNodeList(pXMLNODE CurNode)
{
	pXMLNODELIST ret = new XmlNodeList();
	if (CurNode == NULL)
		return NULL;
	pXMLNODE child = NULL;
	for(child= CurNode->FirstChild();child;child=child->NextSibling())
	{
		if(child->Type() == TiXmlNode::ELEMENT)
			ret->PushNode(child);
	}
	return ret;
};

// <summary>��ýڵ���ӽڵ���</summary>
// <param name="CurNode">�ڵ�ָ��</param>
// <retvalue>�ڵ���ӽڵ����</retvalue>
long XmlParser::GetChildCount(pXMLNODE CurNode)
{
	if (CurNode == NULL)
		return 0;
	long ret =0;
	if (CurNode->NoChildren())
		return ret;

	pXMLNODE child = NULL;
	for(  child= CurNode->FirstChild();child;child=child->NextSibling())
		ret ++;
	return ret;
};

// <summary>��ýڵ��·����Ϣ</summary>
// <param name="CurNode">�ڵ�ָ��</param>
// <retvalue>�ڵ��·��</retvalue>
std::string XmlParser::GetNodePath(pXMLNODE CurNode)
{
	string ret1;
	if (CurNode == NULL)
		return "";
	vector<string> nodeNameList;
	pXMLNODE temp = CurNode;
	if (temp != GetRootNode())
	{
		do{
			nodeNameList.push_back(temp->ValueStr());
			temp = temp->Parent();
		}while((temp != GetRootNode())&&(temp!=NULL));
		if (temp!=NULL)
		{
			nodeNameList.push_back(temp->ValueStr());
		}
		for (size_t i = nodeNameList.size(); i > 0; i --)
		{
			string temp = nodeNameList[i-1];
			ret1.append(temp);
			if (i != 1)
				ret1.append("/");
		}
	} else
		ret1.append(temp->ValueStr());
	return ret1;
}

// <summary>���ӽڵ��б��л�õ�һ���ӽڵ�</summary>
// <param name="CurList">�ӽڵ��б�</param>
// <retvalue>�ڵ��б��еĵ�һ���ڵ�</retvalue>
pXMLNODE XmlParser::GetFirstNode(pXMLNODELIST CurList)
{
	listIndex = 0;
	pXMLNODE temp = NULL;
	if (CurList!=NULL && CurList->Size() >0)
		temp = CurList->GetFirstNode();	
	//listIndex ++;
	return temp;
}

// <summary>���ӽڵ��б��л����һ���ӽڵ�</summary>
// <param name="CurList">�ӽڵ��б�</param>
// <retvalue>�ڵ��б��е���һ���ڵ�</retvalue>
pXMLNODE XmlParser::GetNextNode(pXMLNODELIST CurList)
{       
	pXMLNODE temp = NULL;
    if(CurList!=NULL)
		temp = CurList->GetNextNode();

	return temp;
}

// <summary>��ýڵ�����ֵ</summary>
// <param name="nodename">������</param>
// <param name="nodeValue">����õ�������ֵ</param>
// <param name="pNode">ָ��Ҫ������ԵĽڵ�ָ��</param>
// <retvalue>��óɹ�����ʧ��</retvalue>
bool XmlParser::GetAttrNode(pXMLNODE pNode,string nodeName,string& nodevalue)
{
	if (pNode==NULL ||pNode->Type() != TiXmlNode::ELEMENT)
		return false;
	if (!IsAttributeOfNode(pNode,nodeName))
		return false;
	TiXmlElement * temp = (TiXmlElement *)pNode;
	nodevalue = temp->Attribute((const char*)nodeName.c_str());
	return true;
}

// <summary>��ýڵ�����ֵ</summary>
// <param name="nodename">������</param>
// <param name="nodeValue">����õ�������ֵ</param>
// <param name="pNode">ָ��Ҫ������ԵĽڵ�ָ��</param>
// <retvalue>��óɹ�����ʧ��</retvalue>
bool XmlParser::GetAttrNode(pXMLNODE pNode,string nodeName,int& nodevalue)
{   
	if (pNode==NULL ||pNode->Type() != TiXmlNode::ELEMENT)
		return false;
	TiXmlElement * temp = (TiXmlElement *)pNode;
	temp->Attribute(nodeName,&nodevalue);
	return true;
}

// <summary>��ýڵ�����ֵ</summary>
// <param name="nodename">������</param>
// <param name="nodeValue">����õ�������ֵ</param>
// <param name="pNode">ָ��Ҫ������ԵĽڵ�ָ��</param>
// <retvalue>��óɹ�����ʧ��</retvalue>
bool XmlParser::GetAttrNode(pXMLNODE pNode,string nodeName,long& nodevalue)
{   
	if (pNode==NULL ||pNode->Type() != TiXmlNode::ELEMENT)
		return false;
	string str;
    GetAttrNode(pNode,nodeName,str);
	if (str.size()>0)
	{
		nodevalue=atol(str.c_str());
	}
	return true;
}

// <summary>��ýڵ�����ֵ</summary>
// <param name="nodename">������</param>
// <param name="nodeValue">����õ�������ֵ</param>
// <param name="pNode">ָ��Ҫ������ԵĽڵ�ָ��</param>
// <retvalue>��óɹ�����ʧ��</retvalue>
bool XmlParser::GetAttrNode(pXMLNODE pNode,string nodeName,float& nodevalue)
{
	if (pNode==NULL ||pNode->Type() != TiXmlNode::ELEMENT)
		return false;
	TiXmlElement * temp = (TiXmlElement *)pNode;    
	temp->QueryFloatAttribute((const char*)nodeName.c_str(),&nodevalue);
	return true;
}

// <summary>��ýڵ�����ֵ</summary>
// <param name="nodename">������</param>
// <param name="nodeValue">����õ�������ֵ</param>
// <param name="pNode">ָ��Ҫ������ԵĽڵ�ָ��</param>
// <retvalue>��óɹ�����ʧ��</retvalue>
bool XmlParser::GetAttrNode(pXMLNODE pNode,string nodeName,double& nodevalue)
{
	if (pNode==NULL ||pNode->Type() != TiXmlNode::ELEMENT)
		return false;
	TiXmlElement * temp = (TiXmlElement *)pNode;
	temp->QueryDoubleAttribute((const char*)nodeName.c_str(),&nodevalue);
	return true;
}

// <summary>��ýڵ�����ֵ</summary>
// <param name="nodename">������</param>
// <param name="nodeValue">����õ�������ֵ</param>
// <param name="pNode">ָ��Ҫ������ԵĽڵ�ָ��</param>
// <retvalue>��óɹ�����ʧ��</retvalue>
bool XmlParser::GetAttrNode(pXMLNODE pNode,string nodeName,bool& nodevalue)
{
	if (pNode==NULL ||pNode->Type() != TiXmlNode::ELEMENT)
		return false;
	TiXmlElement * temp = (TiXmlElement *)pNode;
	string nodevaluea = temp->Attribute((const char*)nodeName.c_str());
	if (nodevaluea.compare("TRUE") ==0)
		nodevalue = true;
	else
		nodevalue = false;
	return true;
}

// <summary>���ýڵ�����</summary>
// <param name="nodename">������</param>
// <param name="nodeValue">����ֵ</param>
// <param name="pNode">Ҫ�������ԵĽڵ�ָ��</param>
// <retvalue>���óɹ�����ʧ��</retvalue>
bool XmlParser::SetAttrNode(string nodeName,string nodeValue,pXMLNODE pNode)
{
	if (pNode==NULL ||pNode->Type() != TiXmlNode::ELEMENT)
		return false;
	TiXmlElement * temp = (TiXmlElement *)pNode;
	temp->SetAttribute(nodeName,nodeValue);
	return true;
}

// <summary>���ýڵ�����</summary>
// <param name="nodename">������</param>
// <param name="nodeValue">����ֵ</param>
// <param name="pNode">Ҫ�������ԵĽڵ�ָ��</param>
// <retvalue>���óɹ�����ʧ��</retvalue>
bool XmlParser::SetAttrNode(string nodeName,int nodeValue,pXMLNODE pNode)
{
	if (pNode==NULL ||pNode->Type() != TiXmlNode::ELEMENT)
		return false;
	TiXmlElement * temp = (TiXmlElement *)pNode;
	temp->SetAttribute(nodeName,nodeValue);
	return true;
}

// <summary>���ýڵ�����</summary>
// <param name="nodename">������</param>
// <param name="nodeValue">����ֵ</param>
// <param name="pNode">Ҫ�������ԵĽڵ�ָ��</param>
// <retvalue>���óɹ�����ʧ��</retvalue>
bool XmlParser::SetAttrNode(string nodeName,long nodeValue,pXMLNODE pNode)
{
	if (pNode==NULL ||pNode->Type() != TiXmlNode::ELEMENT)
		return false;
	TiXmlElement * temp = (TiXmlElement *)pNode;
	char buff[64];
	memset(buff,0,sizeof(buff));
	sprintf(buff,"%d",nodeValue);
	temp->SetAttribute(nodeName,string(buff));
	return true;
}

// <summary>���ýڵ�����</summary>
// <param name="nodename">������</param>
// <param name="nodeValue">����ֵ</param>
// <param name="pNode">Ҫ�������ԵĽڵ�ָ��</param>
// <retvalue>���óɹ�����ʧ��</retvalue>
bool XmlParser::SetAttrNode(string nodeName,double nodeValue,pXMLNODE pNode)
{
	if (pNode==NULL ||pNode->Type() != TiXmlNode::ELEMENT)
		return false;
	TiXmlElement * temp = (TiXmlElement *)pNode;
	temp->SetDoubleAttribute((const char*)nodeName.c_str(),nodeValue);
	return true;
}

// <summary>���ýڵ�����</summary>
// <param name="nodename">������</param>
// <param name="nodeValue">����ֵ</param>
// <param name="pNode">Ҫ�������ԵĽڵ�ָ��</param>
// <retvalue>���óɹ�����ʧ��</retvalue>
bool XmlParser::SetAttrNode(string nodeName,float nodeValue,pXMLNODE pNode)
{
	if (pNode==NULL ||pNode->Type() != TiXmlNode::ELEMENT)
		return false;
	TiXmlElement * temp = (TiXmlElement *)pNode;
	temp->SetDoubleAttribute((const char*)nodeName.c_str(),(double)nodeValue);
	return true;
}

// <summary>���ýڵ�����</summary>
// <param name="nodename">������</param>
// <param name="nodeValue">����ֵ</param>
// <param name="pNode">Ҫ�������ԵĽڵ�ָ��</param>
// <retvalue>���óɹ�����ʧ��</retvalue>
bool XmlParser::SetAttrNode(string nodeName,bool nodeValue,pXMLNODE pNode)
{
	if (pNode->Type() != TiXmlNode::ELEMENT)
		return false;
	TiXmlElement * temp = (TiXmlElement *)pNode;
	string Value;
	if (nodeValue)
		Value = "TRUE";
	else
		Value = "FALSE";
	temp->SetAttribute(nodeName,Value);
	return true;
}

// <summary>�����ӽڵ����ƻ��ָ��ýڵ��ָ��</summary>
// <param name="nodename">�ڵ�����֣�����msg/return��ʽ</param>
// <retvalue>��Ӧ���ֵĽڵ��ָ��</retvalue>
pXMLNODE  XmlParser::nodeNameToPtr(char* nodename)
{
	list<string> nodes;       
	string subStr;
	string allPath(nodename);
	long lastFlag = 0;
	if (allPath[0] != '/')
		allPath.insert(0,"/");
	for (size_t i =0 ; i <allPath.length()-1; i++)
	{
		if (allPath[i] == '/')
		{       
			if (i ==0) continue;
			subStr = allPath.substr(lastFlag+1,i - lastFlag -1);
			lastFlag= (long)i;
			nodes.push_back(subStr);
		}
	}
	subStr = allPath.substr(lastFlag+1,allPath.length() -lastFlag -1);
	nodes.push_back(subStr);
	pXMLNODE parent = mXmlDocPtr->RootElement();
	for (list<string>::iterator p = nodes.begin(); p != nodes.end() ; p ++)
	{
		string nodename = *p;            
		pXMLNODE temp =  findNode(parent,(char*)nodename.c_str());
		if (temp!=NULL)
		{
			parent  = temp;
		}
		else
		{
			string xml;
			cout<<"XmlParser::nodeNameToPtr�ļ��в����ڽڵ�"<<nodename.c_str()<<endl;
			return NULL;
		}
	}
	return parent;
}

bool XmlParser::IsAttributeOfNode( pXMLNODE node,string attrname )
{
	bool ret=false;
	if (node==NULL ||node->Type() != TiXmlNode::ELEMENT)
		return ret;
	TiXmlElement* temp=(TiXmlElement*)node;
	TiXmlAttribute* attr=temp->FirstAttribute();
	if (!attr)
	{
		return false;
	}
	while (true)
	{
		if (attr->Name()==attrname)
		{
			ret=true;
			break;
		}
		if (attr==temp->LastAttribute())
		{
			break;
		}
		attr=attr->Next();
	}
	return ret;
}