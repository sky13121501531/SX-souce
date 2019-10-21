#ifndef _H_LSQL_H_
#define _H_LSQL_H_

/************************************************************************/
/* 
    FileName:   LSql.h
    Description:���׷�װSQLITE3����
    Author:     lynn    2008.10.31
    Relative:   LSql.lib,LSql.dll,Sqlite3.dll,SQlite3.lib,Iconv.dll
                                                                        */
/************************************************************************/
#ifndef L_DLL
#define L_DLL _declspec(dllimport)
#endif

#include <string>
#include <iostream>

# define SQLITE_THREADSAFE 1

//��������ֵ
#define LSQL_OK      0   /*��ȷ����*/
#define LSQL_ERROR   1   /*һ�����*/
#define LSQL_BUSY    5   /*���ݿⷱæ������д������*/

/*
 * 1. ����SetTimeOut(int ms) ���������ڷ�����ʱ����������³���ȴ�ʱ�䡣�˺�����open()������ȷ���غ������Ч.
 * 2. ���к����������LSQL_BUSY,��ʾ����д���ݿ⣬���ݿ���ʱ�������������Ե�һ���ٴ�ִ�С�
 * 3. Ĭ������£��ڷ������ݿⱻ��ʱ����ʱ��ִ�����ݿ����ʱ������ȴ�500ms�����500ms�����ݿ�δ���������������LSQL_BUSY
 * 4. Ĭ������£����ᷢ�����ݿ���ʱ��������������һ�����ύ����'��������'����'�����޸�'�������ܻᷢ��������һ������100��INSERT���SQL����
 * 5. �����SQL��䣬���������� �����ִ�Сд������IsTableExist,GetCollect(fieldname...),Blob �ֶβ����������⡣Ӧ������֤��Сдһ��
 */
class L_DLL LSql
{
public:
    LSql();
    ~LSql();
    
    /*����ļ������ڣ��Զ��������ļ�������ļ�����Ŀ¼�����ڷ���,filename�ļ�����ʧ�ܲ��ҷ���0xe */
    int Open(const char *filename);
    int Close();

    /*
     * ���ó�ʱʱ��,��λ������,����������ǰ�ĳ�ʱֵ
     * Ӧ�������ݿ�ɹ��򿪺����
     *
     * �����ȡ����ʱ���ݿⱻLOCK����ô�ȴ�ms����ʱ�䣬
     * ����TIMEOUTʱ�����Ȼ����LOCK״̬�����ú�������LSQL_BUSY
     */
    int SetTimeOut(int ms=500);
    /*��ȡ��ʱʱ��,��λ������*/
    int GetTimeOut();
    
    /*ִ��SQL��䣬�������ؼ�¼����һ������Insert,Delete,Update,Create,Drop */
    int ExecNonQuery(const char *szSql);
    /*ִ��SQL���,���ؼ�¼��,һ������Select */
    int ExecSql(const char *szSql);
    
    /*���ش�����Ϣָ��,������ã�Ӧ��ʱ����ָ��ָ������ݣ����ⱻ�������������Ĵ�����Ϣ���ǣ��������ڴ���� */
    const char *ErrorMessage();
    /*�����ϴ�ִ��ʱ�����Ĵ������ */
    int ErrorCode();

    /*��¼���α���� */
    int MoveFirst();
    int MoveLast();
    int MoveNext();
    int MovePrevious();

    /*������������'��'����LSQL_OK��'��'����LSQL_ERROR�����ݿⱻ��������LSQL_BUSY*/
    int IsBOF();
    int IsEOF();
    /*ע�⣺�����ڴ�ʱ�����ִ�Сд��'����'����LSQL_OK��'������'����LSQL_ERROR,�ⱻ��������LSQL_BUSY*/
    int IsTableExist(const char *szTbl);
    /*���������������ֵ�����ڻ�ȡ�ղ���ļ�¼�������ֶ�ֵ*/
    int LastInsertRowId();

    /*ȡ�ü�¼����*/
    int GetRowCount();
    /*ȡ������*/
    int GetColumnCount();

    /*������ţ�ȡ��������nBufSize:���������ȣ����Ϊ0�����ǻ���������*/
    int GetColumnName(int nIndex,char *pBuf,unsigned int nBufSize=0);
    /*
     * ��ExecSql��������Ч��ѯ֮�󣬸�����Ŷ�ȡĳ�ֶε�ֵ,
     * ���nBufSize��Ϊ0������fieldvalue��д��nBufSize����ֵ������д��ȫ��  
     */
    int GetCollect(const int nIndex,char *pBuf,unsigned int nBufSize=0);
    /* 
     * fieldname ���ִ�Сд
     * ��������ȡ���ֶ�ֵ
     */
    int GetCollect(const char *fieldname,char *pBuf,unsigned int nBufSize=0);


/*Blob �ֶβ��� */
    /*
    * ���nBufSize��Ϊ0������pBuf��д��nBufSize����ֵ������д��ȫ�� 
    * ���ôκ����ɹ�����Ҫ����Free�ͷ�(void *)(*pBuf)
    * �������where������ѯ�������¼��ֻ���ص�һ����¼
    * fieldname ���ִ�Сд
    */
    int GetBlob(const char *tblname,const char *fieldname,const char *szWhere,void **pBuf,unsigned int *pBufSize);
    /*
    * �����ݿ��ж�ȡblob�ֶΣ�д���ļ���
    * ��������ļ�ʧ�ܻ��߶�ȡ���ݿ�ʧ�ܣ�������LSQLITE_ERROR
    * �������where������ѯ�������¼��ֻ���ص�һ����¼
    */
    int GetBlobToFile(const char *tblname,const char *fieldname,const char *szWhere,const char *filename);
    /*�����ݿ���д��blob�ֶ�*/
    int SetBlob(const char *tblname,const char *fieldname,const char *szWhere,const void *pBuf,const unsigned int nBufSize);
    /*��һ���ļ�д��blob�ֶ�*/
    int SetBlobFromFile(const char *tblname,const char *fieldname,const char *szWhere,const char *filename);

    /*�ͷ���Blob�����������ڴ�*/
    void Free(void *pbuf);
    
protected:
    void *m_db; 
    char *m_errmsg;
    char **m_result;
    int m_nErrorCode;
    int m_nRow;
    int m_nCol;
    int m_nCursor;
    int m_nTimeOut;
};




class DeriveLsql : public LSql
{
public:
	DeriveLsql() : LSql()
	{
		m_pFieldValue = new char[1000];
	}

	~DeriveLsql()
	{
		if (m_pFieldValue != NULL)
		{
			delete []m_pFieldValue;
		}
	}

	int GetCollect(std::string fieldname, std::string& value)
	{
		try
		{
			memset(m_pFieldValue, 0, 1000);
			LSql::GetCollect(fieldname.c_str(), m_pFieldValue);
			value = m_pFieldValue;
		}
		catch ( ... )
		{
			std::cout<<"DeriveLsql::GetCollect() ��ȡ�ֶ��쳣!!"<<std::endl;
			return LSQL_ERROR;
		}
		
		return LSQL_OK;
	};

private:
	char* m_pFieldValue;	//���ڻ�ȡ�ֶε�ֵ
};



#endif/*__ #ifndef _H_LSQL_H_ */