///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����DownOrderReceive.cpp
// �����ߣ�jiangcheng
// ����ʱ�䣺2009-05-20
// ��������������Ӧ��ϵͳ�´�����Ļ��࣬�����ʵ�ֲ�ͬ���շ�ʽ
///////////////////////////////////////////////////////////////////////////////////////////
#include "DownOrderReceive.h"

DownOrderReceive::DownOrderReceive()
{
	bFlag = false;
}

DownOrderReceive::~DownOrderReceive()
{

}

int DownOrderReceive::Init()
{
	bFlag = true;
	return 0;
}

int DownOrderReceive::open(void*)
{
	this->activate();
	return 0;
}

int DownOrderReceive::svc()
{
	return 0;
}

int DownOrderReceive::Stop()
{
	bFlag = false;
	this->wait();

	return 0;
}
