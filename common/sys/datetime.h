/*****************************************************************************************************
**** �򵥷�װʱ���ʽ
*****************************************************************************************************/

#pragma once

#include <stdio.h>
#include <time.h>
#include "common.h"

namespace qysys
{
    class CDateTime
    {
    public:
        //ʱ���ʽ����ʹ�õ�ʱ�����������Ҫʹ�õ���ʱ���ʽ
        enum TimeFormat{
            yyyyMMddHHmmss, //ʱ���ʽΪ 20180101000000
            yyyy_MM_dd_HH_mm_ss, //ʱ���ʽΪ 2018-01-01 00:00:00
            yyMMddHHmmss, //ʱ���ʽΪ 180101000000, �걣�ֺ���λ
            yy_MM_dd_HH_mm_ss, //ʱ���ʽΪ 18-01-01 00:00:00, �걣�ֺ���λ
            yyyyMMdd_HHmmss, //ʱ���ʽΪ 20180101 000000,ʱ��������֮�䱣��һ���ո�
            yyMMdd_HHmmss, //ʱ���ʽΪ 180101 000000, �걣�ֺ���λ,ʱ��������֮�䱣��һ���ո�
            MAX_INDEX
        };
        
    public:
        CDateTime();
        CDateTime(struct tm *pTm);
        CDateTime(time_t seconds);
        ~CDateTime();

    public:
        //��ȡ��ǰʱ�䣬����CDateTime����
        static CDateTime Now();
        //�ַ���ת��ΪCDateTime�����ʽ
        static CDateTime GetDateTimeFromStr(QYString &rStr, TimeFormat timeFormat = yyyyMMddHHmmss);
        
    public:
        //��õ�ǰʱ��������ݣ�����2018
        inline int GetYear();

        //��õ�ǰʱ�������·�, �·�ȡֵ��ΧΪ[1, 12]
        inline int GetMonth();

        //��õ�ǰʱ����һ���е�����, ����ȡֵ��ΧΪ[1, 366]
        inline int GetDay();

        //��ȡ��������ȡֵ��Χ[1, 31]
        inline int GetDayInMonth();

        //��õ�ǰʱ����������, ȡֵ��ΧΪ[0, 6]�������շ���0
        inline int GetDayInWeek();

        //��ȡ��1970-01-01 00:00:00����ǰʱ�������
        inline long GetSeconds();

    public:
        //��ȡʱ���ַ���
        QYString GetTimeStr(TimeFormat timeFormat = yyyyMMddHHmmss);

        
    private: 
        struct tm   *m_Time;
        time_t      m_Seconds;
        
    };

}
#include "datetime.inl"

