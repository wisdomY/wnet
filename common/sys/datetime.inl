
//获得当前时间所在年份，例如2018
int CDateTime::GetYear()
{
    return m_Time->tm_year + 1900;
}

//获得当前时间所在月份, 月份取值范围为[1, 12]
int CDateTime::GetMonth()
{
    return m_Time->tm_mon + 1;
}

//获得当前时间在一年中的天数, 天数取值范围为[1, 366]
int CDateTime::GetDay()
{
    return m_Time->tm_yday + 1;
}

//获取日期数，取值范围[1, 31]
int CDateTime::GetDayInMonth()
{
    return m_Time->tm_mday;
}

//获得当前时间在星期数, 取值范围为[0, 6]，星期日返回0
int CDateTime::GetDayInWeek()
{
    return m_Time->tm_wday;
}

//获取从1970-01-01 00:00:00到当前时间的秒数
long CDateTime::GetSeconds()
{
    return (long)m_Seconds;
}


