
//��õ�ǰʱ��������ݣ�����2018
int CDateTime::GetYear()
{
    return m_Time->tm_year + 1900;
}

//��õ�ǰʱ�������·�, �·�ȡֵ��ΧΪ[1, 12]
int CDateTime::GetMonth()
{
    return m_Time->tm_mon + 1;
}

//��õ�ǰʱ����һ���е�����, ����ȡֵ��ΧΪ[1, 366]
int CDateTime::GetDay()
{
    return m_Time->tm_yday + 1;
}

//��ȡ��������ȡֵ��Χ[1, 31]
int CDateTime::GetDayInMonth()
{
    return m_Time->tm_mday;
}

//��õ�ǰʱ����������, ȡֵ��ΧΪ[0, 6]�������շ���0
int CDateTime::GetDayInWeek()
{
    return m_Time->tm_wday;
}

//��ȡ��1970-01-01 00:00:00����ǰʱ�������
long CDateTime::GetSeconds()
{
    return (long)m_Seconds;
}


