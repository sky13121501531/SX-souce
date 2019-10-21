#include "HTTPDeviceAccess.h"
#include "HttpOpe.h"
#include "../Foundation/XmlParser.h"
#include "../Foundation/TypeDef.h"
#include "../Foundation/PropManager.h"

HTTPDeviceAccess::HTTPDeviceAccess(void)
{
}

HTTPDeviceAccess::~HTTPDeviceAccess(void)
{
}
HTTPDeviceAccess::HTTPDeviceAccess(int deviceid,const std::string& strIP,int nPort):DeviceAccess(deviceid,strIP,nPort)
{

}
bool HTTPDeviceAccess::SendMultiTask(std::string strtsIp,unsigned int tsport,int cardindex,int MultiIndex)
{
	return true;
}
bool HTTPDeviceAccess::SendTaskMsg(const std::string& strCmdMsg,std::string& strRetMsg)
{
	bool bRet = false;

	if (strCmdMsg.empty())
	{
		ACE_DEBUG((LM_DEBUG,"(%T| %t) ""�·�����ָ��Ϊ��!\n"));
		return bRet;
	}
	HttpOpe httpOpe(strIPAddress.c_str(),port);//http
	std::string strSubRoot = "perform";

	ACE_Guard<ACE_Thread_Mutex> guard(sendTaskMsgMutex);

	if ((httpOpe.PostHttpMessage(strSubRoot,strCmdMsg,strRetMsg)) && (!strRetMsg.empty()))//post��ʽ����http����
		bRet = true;

	return bRet;
}
bool HTTPDeviceAccess::GetTsReceiveHandle(const std::string& strAddress,ACE_SOCK_Stream& streamHandle)
{
	bool bRet = false;

	if (strAddress.empty())
	{
		ACE_DEBUG((LM_DEBUG,"(%T| %t) ""��ȡTs����ַΪ��\n"));
		return bRet;
	}

	HttpOpe httpOpe(strIPAddress.c_str(),port);
	ACE_Guard<ACE_Thread_Mutex> guard(sendTaskMsgMutex);

	if (httpOpe.GetRecvHandle(strAddress,streamHandle))//�������socket
		bRet = true;

	return bRet;
}