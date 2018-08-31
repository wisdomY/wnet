/*******************************************************************************
* File:common.h
* Copyright:
* Author:zhiwei.yin
* Email:zhiwei.yin@yingheying.com
* Description:���ܵƿ�ϵͳDAG, ���峣�õ�����������ö�ٳ���
* Date:2018-08-14
******************************************************************************/

#pragma once

#include <string>
#include <vector>
#include <map>
#include <list>
#include <algorithm>
#include <mutex>
#include <thread>

#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <unistd.h>
#endif



#define QY_LIGHT_CONTROL_SUCCESS       0    //ret code: �ɹ�
#define QY_LIGHT_CONTROL_FAIL          -1   //ret code: ʧ��
#define QY_LIGHT_CONTROL_ERROR         -2   //ret code: ����

#define SAFE_DELETE_ARRAY(p)\
    if (p) { \
        delete[] p; \
        p = NULL; \
    }
 

typedef std::string     QYString;
typedef std::mutex      CMutex;

#ifdef WIN32
typedef void*           HANDLE;
#define QY_snprintf(buf,sizeOfBuf,...)      sprintf_s(buf,sizeOfBuf,##__VA_ARGS__)
#define QY_SLEEP_MS(x)                      Sleep(x)
#else
#define QY_snprintf(buf,sizeOfBuf,...)      snprintf(buf,sizeOfBuf,##__VA_ARGS__)
#define QY_SLEEP_MS(x)                      usleep((x)*1000)
#endif


//�����ַ�������
class QYStringOp
{
public:
    QYStringOp()
    {
    };
    ~QYStringOp()
    {
    };

public:
    static QYString& Trim(QYString &s)
    {
        return TrimLeft(TrimRight(s));
    }

    // trim from start
    static QYString& TrimLeft(QYString &s) 
    {
        s.erase(0, s.find_first_not_of(" \t\f\v\n\r"));
        return s;
    }

    // trim from end
    static QYString& TrimRight(QYString &s) 
    {
        s.erase(s.find_last_not_of(" \t\f\v\n\r") + 1);
        return s;
    }
};


