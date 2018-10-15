#include "common.h"

#ifndef WIN32
#include <iconv.h>
#include <string.h>

bool _Convert(char *fcharset, char *tcharset, QYString &rFromStr, QYString &rToStr)  
{
    iconv_t cd;
    cd = iconv_open(tcharset, fcharset);  
    if (0 == cd) {
        return false;
    }

    // 获取源数据
    size_t nSrcSize = rFromStr.size();
    char *pcInBuf = new char[nSrcSize + 1];
    memset(pcInBuf, 0, nSrcSize + 1);
    memcpy(pcInBuf, rFromStr.c_str(), nSrcSize);

    // 开辟目的存储空间
    size_t nDestSize = 2 * (nSrcSize + 1);
    char *pcOutBuf = new char[nDestSize];
    if (!pcOutBuf) {
        delete[] pcInBuf;
        return false;
    }

    // 开始转换
    size_t nRet;
    memset(pcOutBuf, 0, nDestSize);
    nRet = iconv(cd, &pcInBuf, &nSrcSize, &pcOutBuf, &nDestSize);
    if (-1 == nRet) {
        delete[] pcInBuf;
        delete[] pcOutBuf;
        return false;
    }

    rToStr = pcOutBuf;
    delete[] pcInBuf;
    delete[] pcOutBuf;
    return true;  
}
#endif



bool GBK2UTF8(QYString &rGBKStr, QYString &rUTF8Str)
{
#ifdef WIN32
    int n = MultiByteToWideChar(CP_ACP, 0, rGBKStr.c_str(), -1, NULL, 0);

    WCHAR *str1 = new WCHAR[n];
    if (str1 == NULL) {
        return false;
    }
    MultiByteToWideChar(CP_ACP, 0, rGBKStr.c_str(), -1, str1, n);
    
    n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);
    char * str2 = new char[n];
    if (str2 == NULL) {
        delete[]str1;
        return false;
    }
    WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, NULL, NULL); 

    rUTF8Str = str2;
    delete[]str1;
    delete[]str2;
    
    return true;
#else
    return _Convert("gb2312", "utf-8", rGBKStr,rUTF8Str);
#endif
}



bool UTF82GBK(QYString &rUTF8Str, QYString &rGBKStr)
{
#ifdef WIN32
    int len = MultiByteToWideChar(CP_UTF8, 0, rUTF8Str.c_str(), -1, NULL, 0);

    wchar_t * pTempOne = new wchar_t[len];
    if (pTempOne == NULL) {
        return false;
    }
    memset(pTempOne, 0, len);

    MultiByteToWideChar(CP_UTF8, 0, rUTF8Str.c_str(), -1, pTempOne, len);
    len = WideCharToMultiByte(CP_ACP, 0, pTempOne, -1, NULL, 0, NULL, NULL);

    char *pTempTwo = new char[len + 1];
    if (pTempTwo == NULL) {
        delete[]pTempOne;
        return false;
    }
    memset(pTempTwo, 0, len + 1);

    WideCharToMultiByte(CP_ACP, 0, pTempOne, -1, pTempTwo, len, NULL, NULL);

    rGBKStr = pTempTwo;
    delete[]pTempOne;
    delete[]pTempTwo;

    return true;
#else
    return _Convert("utf-8", "gb2312", rUTF8Str, rGBKStr);
#endif
}


