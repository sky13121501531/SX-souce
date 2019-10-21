///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����DeviceIndependentTask.cpp
// �����ߣ�jiangcheng
// ����ʱ�䣺2009-05-26
// �������������豸�����޹ص�������
///////////////////////////////////////////////////////////////////////////////////////////
#include "DeviceIndependentTask.h"
#include "../Foundation/AppLog.h"
#include "../Foundation/PropManager.h"
#include "../Foundation/StrUtil.h"
#include "ace/SOCK_Connector.h"
#include "ace/INET_Addr.h"
#include "ace/Time_value.h"

DeviceIndependentTask::DeviceIndependentTask() : XMLTask()
{

}

DeviceIndependentTask::DeviceIndependentTask(std::string strXML) : XMLTask(strXML)
{
	DeviceID = -100;//���豸�޹ص�DeviceIDΪ-100���������������ٴ����á�
}

DeviceIndependentTask::~DeviceIndependentTask()
{

}
bool DeviceIndependentTask::SendCASXML(const std::string& dstUrl,const std::string& upXML)
{
	std::string hostip,content;;
	int port = 0;
	size_t pos1,pos2,i,temp;
	bool bEnd = false;
	//���upxmlû��xmlͷ�������xmlͷ
	std::string tempXML = upXML;
	if(tempXML.find("encoding=\"GB2312\"") == -1)
	{
		int ppos = (int)tempXML.find("standalone=\"yes\"");
		if(ppos != -1)
			tempXML.insert(ppos-1,string(" encoding=\"GB2312\""));
	}

	pos1 = 0;
	pos2 = 0;
	//��÷���Ŀ��ĵ�ַ�Ͷ˿ں�
	char *http = "http://"; 
	size_t httpLen = strlen(http);

	std::string url = dstUrl;

	if(url.find(http) == 0)
	{
		temp = 0;
		for(i=0;i<url.length();i++)
		{
			if( url[i] == ':')
				temp++;
			if (temp == 2) {
				pos1 = i; 
				break;
			}
		}

		temp = 0;
		for(i=0;i<url.length();i++)
		{
			if( url[i] == '/')
				temp++;
			if (temp == 3) {
				pos2 = i; 
				break;
			}
		}

		if (pos1 == 0)//���û�еڶ�����:��
		{
			port = 80;

			if (pos2 == 0)//���û�к���ġ�/��
			{
				temp = url.length() - httpLen;
				hostip = url.substr(httpLen,temp);
			}else//����к���ġ�/��
			{
				temp = pos2 - httpLen;
				hostip = url.substr(httpLen,temp);
			}
		}else//����еڶ�����:��
		{
			if (pos2 == 0)//���û�к���ġ�/��
			{
				port = atol(url.substr(pos1 + 1,url.length() - pos1).c_str());

				hostip = url.substr(httpLen,pos1 - httpLen); 
			}else//����к���ġ�/��
			{
				port = atol(url.substr(pos1+1,(pos2-pos1-1)).c_str());

				temp = pos1 - httpLen;
				hostip = url.substr(httpLen,temp);
			}
		}

		std::string errorMsg = "����XML�ļ�ʧ�ܣ�\n",errorPos("");

		try
		{
			ACE_SOCK_Stream clientStream;
			ACE_INET_Addr remoteAddr(port,hostip.c_str());//Ŀ�ĵ�ַ
			ACE_SOCK_Connector connector;
			int nResult = -1;

			errorPos = "connect";
			if (connector.connect (clientStream, remoteAddr) == -1)//���ӷ�����
			{
				clientStream.close_reader();
				clientStream.close_writer();
				clientStream.dump();
				clientStream.close();
				return false;
				ACE_ERROR_RETURN((LM_ERROR,"(%P|%t) %p\n","XML�����������ӷ�����ʧ��"),false);
			}
			else
			{
				;//ACE_DEBUG((LM_DEBUG,"(%T| %t) ���ӵ� %s\n",remoteAddr.get_host_name ()));
			}

			std::string tmp = dstUrl;
			content = "POST " + tmp + " HTTP/1.1\r\nUser-Agent: Mozila\r\nConnection: close\r\nHost: " + hostip;
			content = content + ":" + StrUtil::Int2Str(port) + "\r\nContent-Length: " + StrUtil::Int2Str((int)tempXML.length()) + "\r\n\r\n" + tempXML;//post��ʽ��http����ͷ

			for(int j=0;j<PROPMANAGER::instance()->GetXmlSendTimes();j++)
			{
				nResult = clientStream.send(content.c_str(),(int)content.length(),0);
				cout<<content<<endl<<nResult<<endl;
				if(nResult > 0)//��������
				{
					ACE_Time_Value overTime(PROPMANAGER::instance()->GetXmlOvertime(),0);
					char buf[1024*16]={0};
					int ret = clientStream.recv(buf,1024*16,0,&overTime);//���շ�������Ӧ
					cout<<buf<<endl;
					if (ret > 0)
					{
						//�ж��յ��ķ������ظ��ļ����Ƿ���200��˵����������Ӧ����
						for(int i = 0;i < ret;i++)
						{
							if(buf[i]=='2')
								if(i+1<ret&&buf[i+1]=='0')
									if(i+2<ret&&buf[i+2]=='0')
										if(i+3<ret&&buf[i+3]==' ')
										{
											bEnd = true;
											break;
										}
						}
						if(bEnd)
							break;
					}
				}
			}

			clientStream.close();
		}
		catch (...)
		{
			bEnd = false;
			errorMsg += errorPos;
			//SYSMSGSENDER::instance()->SendMsg(errorMsg,UNKNOWN,VS_MSG_SYSALARM);
		}
	}

	return bEnd;
}

