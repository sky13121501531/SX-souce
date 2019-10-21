//////////////////////////////////////////////////////////////////////////////////
// �ļ�����     BaseAnalyser.cpp
// �����ߣ�     yuanzy
// ����ʱ�䣺   2006-5-18
// ������       ��ǿ����������
//////////////////////////////////////////////////////////////////////////////////
#include "stdio.h"
#include "BaseAnalyser.h"
#include "math.h"

const char ZX_BEGIN		=0X7C;//���ֿ�ʼ
const char ZX_CANDO		=0X7D;//���ֳɹ�
const char ZX_DATCOME	=0X7F;//���Զ����ݱ�־
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBaseAnalyser::CBaseAnalyser(void)
{
}

CBaseAnalyser::~CBaseAnalyser(void)
{
}

/// <summary>��ǿ����ֵ</summary>
/// <param name="fLevel">[����]���صĲ���ֵ</param>
/// <param name="szFloat[]">[����]��com�ڵõ��ķ�������</param>
/// <retvalue>��</retvalue>
static inline void StrToFloat(float &fLevel,char szFloat[])
{
	float fnum=0.0;
	for(int i=0;i<4;i++)
	{
		if(szFloat[i]==0x7e)
		{
			szFloat[i]=0x0;
		}
		fnum = fnum+(float)(szFloat[i]*(100/pow(10.0,i)));
	}

	fLevel=fnum;
}

/// <summary>����д��������</summary>
/// <param name="">����˵��</param>
/// <retvalue>����ֵ˵��</retvalue>
HRESULT CBaseAnalyser::GetFreqLevel(float fFreq, float &fLevel)//
{
	if(fFreq <= 0)
	{
		fLevel = 0.0f;
		return S_OK;
	}
	char freqcmd[6];  
	char Mdata[5];
	memset(freqcmd,0,6);
	memset(Mdata,0,5);
	freqcmd[0]=0x77;
	freqcmd[1]=0x7B;
	//
	for(int i=0;i<2;i++)
	{
		if(CommandCommucation()!=S_OK)
		{
			printf("����Ƶ��ʱ����ʧ��!");
			return S_FALSE;
		}
		long lcmd=long(fFreq*16);
		freqcmd[2]=char((lcmd>>12)&0x0000000f);
		lcmd=long(fFreq*16);
		freqcmd[3]=char((lcmd>>8)&0x0000000f);
		lcmd=long(fFreq*16);
		freqcmd[4]=char((lcmd>>4)&0x0000000f);
		lcmd=long(fFreq*16);
		freqcmd[5]=char(lcmd&0x0000000f);

		for(int i=2;i<6;i++)
		{
			if(freqcmd[i]==0x0)
			{
				freqcmd[i]=0x7e;
			}
		}
        //����Ƶ������
		if(SendComman(freqcmd,6)!=S_OK)
		{
			printf("����Ƶ������ S_FALSE\n");
			return S_FALSE;
		}
        //�õ����ص�ֵ��
		if(ReadData(Mdata,5)!=S_OK)
		{
			return S_FALSE;
		}
	}
    //���ַ���ת��Ϊ�������ĵ�ƽֵ
	StrToFloat(fLevel,Mdata);

	return S_OK;
}

/// <summary>����д��������</summary>
/// <param name="">����˵��</param>
/// <retvalue>����ֵ˵��</retvalue>
HRESULT CBaseAnalyser::GetCN(float fFreq, float &fCN)//
{
	float fLevel, fNoise;
	GetFreqLevel(fFreq,fLevel);
	GetFreqLevel((float)(fFreq - 1.1),fNoise);
	fCN = fLevel - fNoise;
	if(fCN < 1.0)
	{
		fCN = 0.0f;
	}
	return S_OK;
}

/// <summary>����д��������</summary>
/// <param name="">����˵��</param>
/// <retvalue>����ֵ˵��</retvalue>
HRESULT CBaseAnalyser::GetFreqOffset(float fFreq, float &fOffset)//
{   // 0.005 0.01 
	// float fFreqOffSetTable[]={0.01f,0.05f,0.1f,0.15f,0.2f,0.5f,1.0f};
	// int nTableSize=7;    //2
	float fFreqOffSetTable[]={0.005f,0.01f,0.1f,0.15f,0.2f,0.5f,1.0f};
	int   nTableSize=2;
	float fLevel;

	GetFreqLevel(fFreq,fLevel);
	fOffset=0.0f;

	for(int i=0;i<nTableSize;i++)
	{
		for(int j=-1;j<=1;j+=2)
		{
			float fTemp;
			GetFreqLevel(fFreq+fFreqOffSetTable[i]*j,fTemp);
			//TRACE("%f - %f\n",fFreq+fFreqOffSetTable[i]*j,fTemp);

			if(fTemp>fLevel)
			{
				fLevel=fTemp;
				fOffset=fFreqOffSetTable[i]*j;
			}
		}
	}
	fOffset *= 1000; 

	return S_OK;
}

/// <summary>����д��������</summary>
/// <param name="">����˵��</param>
/// <retvalue>����ֵ˵��</retvalue>
HRESULT CBaseAnalyser::CommandCommucation()//����
{
	char szCommand=ZX_BEGIN;
	long SpanMAX=5;
	long Span=0;
	time_t start= time(0); 
	HRESULT  hRet=S_FALSE;
	char cFlag=0;
	do
	{
  		mComRW.WriteCom(&szCommand,1);
		Sleep(500);
		mComRW.ReadCom(&cFlag,1);
		Span=time(0)-start;
	}while(cFlag!=ZX_CANDO&&Span<SpanMAX);
	//
	if(cFlag==ZX_CANDO)
	{
		hRet=S_OK;
	}
	return hRet;
}

/// <summary>����д��������</summary>
/// <param name="">����˵��</param>
/// <retvalue>����ֵ˵��</retvalue>
HRESULT CBaseAnalyser::SendComman(char* bDsCommand,char bCmdLen)//��������
{
	if(bDsCommand==NULL)
	{
		return S_FALSE;
	}
	if(mComRW.WriteCom(bDsCommand,bCmdLen)!=S_OK)
	{
		printf("WriteCom\n");
		return S_FALSE;
	}
	return S_OK;
}

/// <summary>����д��������</summary>
/// <param name="">����˵��</param>
/// <retvalue>����ֵ˵��</retvalue>
HRESULT CBaseAnalyser::ReadData(char* szTextFloat,int nReceive)//���ճ�ǿ����
{
	long SpanMAX=5;
	long Span=0;
	time_t start= time(0); 
	char cRHead=0;
    //�������һ�����жϴ����Ƿ�ɶ�
	do
	{
		mComRW.ReadCom(&cRHead,1);
		Span=time(0)-start;
		Sleep(500);
	}while(cRHead!=ZX_DATCOME&&Span<SpanMAX);
	//
	if(cRHead!=ZX_DATCOME)
	{
		//printf("�ճ�ǿʱ7F��־ʧ��");
		return S_FALSE;
	}
	if(mComRW.ReadCom(szTextFloat,nReceive-1)!=S_OK)
	{
		//printf("������ʧ��");
		return S_FALSE;
	}
	return S_OK;	
}

/// <summary>����д��������</summary>
/// <param name="">����˵��</param>
/// <retvalue>����ֵ˵��</retvalue>
HRESULT CBaseAnalyser::InitDevice(int nComPort)
{
	if(mComRW.InitDevice(nComPort)!=S_OK)
	{
   		printf("���ڳ�ʼ��ʧ��!\n");
		return S_FALSE;
	}
	return S_OK;
}

/// <summary>����д��������</summary>
/// <param name="">����˵��</param>
/// <retvalue>����ֵ˵��</retvalue>
HRESULT CBaseAnalyser::CloseDevice()
{
	if(mComRW.CloseDevice()!=S_OK)
	{
        //printf("�رմ���ʧ��!\n");
		return S_FALSE;
	}

	return S_OK;
}
