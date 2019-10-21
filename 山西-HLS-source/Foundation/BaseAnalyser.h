#pragma once
#include "ComReadWrite.h"
#include "ace/singleton.h"
///<summary>
///     ��ǿ����������
///<summary>
            
class CBaseAnalyser
{
    //Constructors
public:
    CBaseAnalyser(void);
    virtual ~CBaseAnalyser(void);

    //Operations
private:
	HRESULT SendComman(char* bDsCommand,char bCmdLen);      //��������
	HRESULT ReadData(char* szTextFloat,int nReceive);       //���ճ�ǿ����

    //Attributes
public:
    CComReadWrite mComRW;                                   //���ڲ�������

    //Operations
public:
	HRESULT InitDevice(int nComPort=3);                     //��ʼ��ָ������
	HRESULT CloseDevice(void);                              //�ر�ָ������
	HRESULT GetFreqLevel(float fFreq, float &fLevel);       //�õ�ĳһƵ���ƽ
	HRESULT GetCN(float fFreq, float &fCN);                 //�õ������
	HRESULT GetFreqOffset(float fFreq, float &fOffset);     //�õ�Ƶ����
	HRESULT CommandCommucation();                           //��ǿͨѶ����

    //Overrides
public:

    //Implementation
public:

};


typedef ACE_Singleton<CBaseAnalyser,ACE_Mutex>  DEVICEFREQLEVEL;