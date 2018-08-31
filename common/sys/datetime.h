/*****************************************************************************************************
**** 简单封装时间格式
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
        //时间格式，在使用的时候自行添加需要使用到的时间格式
        enum TimeFormat{
            yyyyMMddHHmmss, //时间格式为 20180101000000
            yyyy_MM_dd_HH_mm_ss, //时间格式为 2018-01-01 00:00:00
            yyMMddHHmmss, //时间格式为 180101000000, 年保持后两位
            yy_MM_dd_HH_mm_ss, //时间格式为 18-01-01 00:00:00, 年保持后两位
            yyyyMMdd_HHmmss, //时间格式为 20180101 000000,时间与日期之间保持一个空格
            yyMMdd_HHmmss, //时间格式为 180101 000000, 年保持后两位,时间与日期之间保持一个空格
            MAX_INDEX
        };
        
    public:
        CDateTime();
        CDateTime(struct tm *pTm);
        CDateTime(time_t seconds);
        ~CDateTime();

    public:
        //获取当前时间，返回CDateTime类型
        static CDateTime Now();
        //字符串转化为CDateTime对象格式
        static CDateTime GetDateTimeFromStr(QYString &rStr, TimeFormat timeFormat = yyyyMMddHHmmss);
        
    public:
        //获得当前时间所在年份，例如2018
        inline int GetYear();

        //获得当前时间所在月份, 月份取值范围为[1, 12]
        inline int GetMonth();

        //获得当前时间在一年中的天数, 天数取值范围为[1, 366]
        inline int GetDay();

        //获取日期数，取值范围[1, 31]
        inline int GetDayInMonth();

        //获得当前时间在星期数, 取值范围为[0, 6]，星期日返回0
        inline int GetDayInWeek();

        //获取从1970-01-01 00:00:00到当前时间的秒数
        inline long GetSeconds();

    public:
        //获取时间字符串
        QYString GetTimeStr(TimeFormat timeFormat = yyyyMMddHHmmss);

        
    private: 
        struct tm   *m_Time;
        time_t      m_Seconds;
        
    };

}
#include "datetime.inl"

