#include "cping.h"
#include <time.h>
USHORT s_usPacketSeq = 0;
ULONG s_ulFirstCallTick = 0;
LONGLONG s_ullFirstCallTickMS = 0;
CPing::CPing() : 
m_szICMPData(NULL), 
m_bIsInitSucc(false)
{
	WSADATA WSAData;
	WSAStartup(MAKEWORD(1, 1), &WSAData);

	m_event = WSACreateEvent();
	m_usCurrentProcID = (USHORT)GetCurrentProcessId();

	if ((m_sockRaw = WSASocket(AF_INET, SOCK_RAW, IPPROTO_ICMP, NULL, 0, 0)) != SOCKET_ERROR)
	{
		WSAEventSelect(m_sockRaw, m_event, FD_READ);
		m_bIsInitSucc = true;

		m_szICMPData = (char*)malloc(DEF_PACKET_SIZE + sizeof(ICMPHeader));

		if (m_szICMPData == NULL)
		{
			m_bIsInitSucc = false;
		}
	}
}

CPing::~CPing()
{
	if (m_sockRaw != INVALID_SOCKET)
	{
		closesocket(m_sockRaw);
		m_sockRaw = INVALID_SOCKET;
	}
	//
	WSACleanup();
	//
	if (NULL != m_szICMPData)
	{
		free(m_szICMPData);
		m_szICMPData = NULL;
	}
}

bool CPing::Ping(DWORD dwDestIP, PingReply *pPingReply, DWORD dwTimeout)
{  
	return PingCore(dwDestIP, pPingReply, dwTimeout);
}

bool CPing::Ping(char *szDestIP, PingReply *pPingReply, DWORD dwTimeout)
{  
	if (NULL != szDestIP)
	{
		return PingCore(inet_addr(szDestIP), pPingReply, dwTimeout);
	}
	return false;    
}

bool CPing::PingCore(DWORD dwDestIP, PingReply *pPingReply, DWORD dwTimeout)
{
	//判断初始化是否成功
	if (!m_bIsInitSucc)
	{
		return false;
	}

	//配置SOCKET
	sockaddr_in sockaddrDest; 
	sockaddrDest.sin_family = AF_INET; 
	sockaddrDest.sin_addr.s_addr = dwDestIP;
	int nSockaddrDestSize = sizeof(sockaddrDest);

	//构建ICMP包
	int nICMPDataSize = DEF_PACKET_SIZE + sizeof(ICMPHeader);
	ULONG ulSendTimestamp = GetTickCountCalibrate();
	USHORT usSeq = ++s_usPacketSeq;    
	memset(m_szICMPData, 0, nICMPDataSize);
	ICMPHeader *pICMPHeader = (ICMPHeader*)m_szICMPData;
	pICMPHeader->m_byType = ECHO_REQUEST; 
	pICMPHeader->m_byCode = 0; 
	pICMPHeader->m_usID = m_usCurrentProcID;    
	pICMPHeader->m_usSeq = usSeq;
	pICMPHeader->m_ulTimeStamp = ulSendTimestamp;
	pICMPHeader->m_usChecksum = CalCheckSum((USHORT*)m_szICMPData, nICMPDataSize);

	//发送ICMP报文
	if (sendto(m_sockRaw, m_szICMPData, nICMPDataSize, 0, (struct sockaddr*)&sockaddrDest, nSockaddrDestSize) == SOCKET_ERROR)
	{
		return false;
	}

	//判断是否需要接收相应报文
	if (pPingReply == NULL)
	{
		return true;
	}

	char recvbuf[256] = {"\0"};
	time_t curTime = time(0);
	while (true)
	{
		//接收响应报文
		if (WSAWaitForMultipleEvents(1, &m_event, FALSE, 100, false) != WSA_WAIT_TIMEOUT)
		{
			WSANETWORKEVENTS netEvent;
			WSAEnumNetworkEvents(m_sockRaw, m_event, &netEvent);

			if (netEvent.lNetworkEvents & FD_READ)
			{
				ULONG nRecvTimestamp = GetTickCountCalibrate();
				int nPacketSize = recvfrom(m_sockRaw, recvbuf, 256, 0, (struct sockaddr*)&sockaddrDest, &nSockaddrDestSize);
				if (nPacketSize != SOCKET_ERROR)
				{
					IPHeader *pIPHeader = (IPHeader*)recvbuf;
					USHORT usIPHeaderLen = (USHORT)((pIPHeader->m_byVerHLen & 0x0f) * 4);
					ICMPHeader *pICMPHeader = (ICMPHeader*)(recvbuf + usIPHeaderLen);

					if (pICMPHeader->m_usID == m_usCurrentProcID //是当前进程发出的报文
						&& pICMPHeader->m_byType == ECHO_REPLY //是ICMP响应报文
						&& pICMPHeader->m_usSeq == usSeq //是本次请求报文的响应报文
						)
					{
						pPingReply->m_usSeq = usSeq;
						pPingReply->m_dwRoundTripTime = nRecvTimestamp - pICMPHeader->m_ulTimeStamp;
						pPingReply->m_dwBytes = nPacketSize - usIPHeaderLen - sizeof(ICMPHeader);
						pPingReply->m_dwTTL = pIPHeader->m_byTTL;
						return true;
					}
				}
			}
		}
		//超时
		if (GetTickCountCalibrate() - ulSendTimestamp >= dwTimeout)
		{
			return false;
		}
		if(time(0)-curTime >= 5)
		{
			return true;
		}
	}
}

USHORT CPing::CalCheckSum(USHORT *pBuffer, int nSize)
{
	unsigned long ulCheckSum=0; 
	while(nSize > 1) 
	{ 
		ulCheckSum += *pBuffer++; 
		nSize -= sizeof(USHORT); 
	}
	if(nSize ) 
	{ 
		ulCheckSum += *(UCHAR*)pBuffer; 
	} 

	ulCheckSum = (ulCheckSum >> 16) + (ulCheckSum & 0xffff); 
	ulCheckSum += (ulCheckSum >>16); 

	return (USHORT)(~ulCheckSum); 
}

ULONG CPing::GetTickCountCalibrate()
{
	SYSTEMTIME systemtime;
	FILETIME filetime;
	GetLocalTime(&systemtime);    
	SystemTimeToFileTime(&systemtime, &filetime);
	LARGE_INTEGER liCurrentTime;
	liCurrentTime.HighPart = filetime.dwHighDateTime;
	liCurrentTime.LowPart = filetime.dwLowDateTime;
	LONGLONG llCurrentTimeMS = liCurrentTime.QuadPart / 10000;

	if (s_ulFirstCallTick == 0)
	{
		s_ulFirstCallTick = GetTickCount();
	}
	if (s_ullFirstCallTickMS == 0)
	{
		s_ullFirstCallTickMS = llCurrentTimeMS;
	}

	return s_ulFirstCallTick + (ULONG)(llCurrentTimeMS - s_ullFirstCallTickMS);
}