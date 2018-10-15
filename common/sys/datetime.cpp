#include <assert.h>
#include <stdlib.h>
#include "datetime.h"
#include "evpp/logging.h"
  
CDateTime::CDateTime()
{
    m_Seconds   = time(NULL);     //��䷵�ص�ֻ��һ��ʱ���
    m_Time      = localtime(&m_Seconds);
}

CDateTime::CDateTime(struct tm *pTm)
{
    m_Seconds   = mktime(pTm);
    m_Time      = localtime(&m_Seconds);
}


CDateTime::CDateTime(time_t seconds)
{
    m_Seconds   = seconds;
    m_Time      = localtime(&m_Seconds);
}

CDateTime::~CDateTime()
{
}


//��ȡ��ǰʱ�䣬����CDateTime����
CDateTime CDateTime::Now()
{
    return CDateTime();
}


time_t CDateTime::operator-(CDateTime &rTime)
{
    return m_Seconds - rTime.m_Seconds;
}


//�����ַ�����ʽ�����CDateTime����
CDateTime CDateTime::GetDateTimeFromStr(QYString &rStr, TimeFormat timeFormat)
{
    int year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;
    int length = rStr.size();
    switch(timeFormat) {
    case yyyyMMddHHmmss:
        if (length == 14) {
            year    = atoi(rStr.substr(0, 4).c_str());
            month   = atoi(rStr.substr(4, 2).c_str());
            day     = atoi(rStr.substr(6, 2).c_str());
            hour    = atoi(rStr.substr(8, 2).c_str());
            minute  = atoi(rStr.substr(10, 2).c_str());
            second  = atoi(rStr.substr(12, 2).c_str());
        }
        break;
    case yyyy_MM_dd_HH_mm_ss:
        if (length == 19) {
            year    = atoi(rStr.substr(0, 4).c_str());
            month   = atoi(rStr.substr(5, 2).c_str());
            day     = atoi(rStr.substr(8, 2).c_str());
            hour    = atoi(rStr.substr(11, 2).c_str());
            minute  = atoi(rStr.substr(14, 2).c_str());
            second  = atoi(rStr.substr(17, 2).c_str());
        }
        break;
    case yyMMddHHmmss:
        if (length == 12) {
            year    = atoi(rStr.substr(0, 2).c_str()) + 2000;
            month   = atoi(rStr.substr(2, 2).c_str());
            day     = atoi(rStr.substr(4, 2).c_str());
            hour    = atoi(rStr.substr(6, 2).c_str());
            minute  = atoi(rStr.substr(8, 2).c_str());
            second  = atoi(rStr.substr(10, 2).c_str());
        }
        break;
    case yy_MM_dd_HH_mm_ss:
        if (length == 17) {
            year    = atoi(rStr.substr(0, 2).c_str()) + 2000;
            month   = atoi(rStr.substr(3, 2).c_str());
            day     = atoi(rStr.substr(6, 2).c_str());
            hour    = atoi(rStr.substr(9, 2).c_str());
            minute  = atoi(rStr.substr(12, 2).c_str());
            second  = atoi(rStr.substr(15, 2).c_str());
        }
        break;
    case yyyyMMdd_HHmmss:
    	if (length == 15) {
            year    = atoi(rStr.substr(0, 4).c_str());
            month   = atoi(rStr.substr(4, 2).c_str());
            day     = atoi(rStr.substr(6, 2).c_str());
            hour    = atoi(rStr.substr(9, 2).c_str());
            minute  = atoi(rStr.substr(11, 2).c_str());
            second  = atoi(rStr.substr(13, 2).c_str());
        }
        break;
    case yyMMdd_HHmmss:
    	if (length == 13) {
            year    = atoi(rStr.substr(0, 2).c_str()) + 2000;
            month   = atoi(rStr.substr(2, 2).c_str());
            day     = atoi(rStr.substr(4, 2).c_str());
            hour    = atoi(rStr.substr(7, 2).c_str());
            minute  = atoi(rStr.substr(9, 2).c_str());
            second  = atoi(rStr.substr(11, 2).c_str());
        }
        break;
    default:
        break;
    }

    if (year != 0) {
        tm tmVar;
        tmVar.tm_year   = year - 1900;      // �꣬����tm�ṹ��洢���Ǵ�1900�꿪ʼ��ʱ�䣬����tm_yearΪint��ʱ������ȥ1900��
        tmVar.tm_mon    = month - 1;        // �£�����tm�ṹ����·ݴ洢��ΧΪ0-11������tm_monΪint��ʱ������ȥ1��
        tmVar.tm_mday   = day;              // �ա�
        tmVar.tm_hour   = hour;             // ʱ��
        tmVar.tm_min    = minute;           // �֡�
        tmVar.tm_sec    = second;           // �롣
        tmVar.tm_isdst  = 0;                // ������ʱ��
        return CDateTime(&tmVar);
    }

    LOG_ERROR << "GetDateTimeFromStr fialed, str:" << rStr.c_str() << ", format: " << timeFormat;
    return CDateTime();
}


//��ȡʱ���ַ���
QYString CDateTime::GetTimeStr(TimeFormat timeFormat)
{
    char buff[64] = {'\0'};
    switch(timeFormat) {
    case yyyyMMddHHmmss:
    	strftime(buff, sizeof(buff)-1, "%Y%m%d%H%M%S", m_Time);
        break;
    case yyyy_MM_dd_HH_mm_ss:
    	strftime(buff, sizeof(buff)-1, "%Y-%m-%d %H:%M:%S", m_Time);
        break;
    case yyMMddHHmmss:
    	strftime(buff, sizeof(buff)-1, "%y%m%d%H%M%S", m_Time);
        break;
    case yy_MM_dd_HH_mm_ss:
    	strftime(buff, sizeof(buff)-1, "%y-%m-%d %H:%M:%S", m_Time);
        break;
    case yyyyMMdd_HHmmss:
    	strftime(buff, sizeof(buff)-1, "%Y%m%d %H%M%S", m_Time);
        break;
    case yyMMdd_HHmmss:
    	strftime(buff, sizeof(buff)-1, "%y%m%d %H%M%S", m_Time);
        break;
    default:
        break;
    }
    return QYString(buff);
}


