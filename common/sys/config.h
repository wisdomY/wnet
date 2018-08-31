#pragma once

#include "common.h"
#include "singleton.h"
#include "evpp/logging.h"

//using namespace std;

namespace qysys  {

    /** 
     * @brief 解析ini配置文件,并将配置项以key-value的形式存储到内存中
     */
    class CConfig {
    public:
        CConfig();
        CConfig(const QYString &rFilename);
        ~CConfig();
        
        // 加载一个文件
        int Load(const QYString &rFilename);
        // 取一个字符串
        int GetString(const QYString &rSection, const QYString &rKey, const QYString &rDefaultValue, QYString &rValue);
        // 取一个整型
        int GetInt(const QYString &rSection, const QYString &rKey, int defaultValue);
        // 取一section下所有的key
        int GetSectionKey(const QYString &rSection, std::vector<QYString> &rKeyVec);
        // 得到所有section的名字
        int GetSectionName(std::vector<QYString> &rSectionVec);
        // 完整的配置文件字符串
        QYString ToString();

        //向文件中写ini内容
        int WriteProfileString(const QYString &rSection, const QYString &rKey, const QYString &rValue);
        
    private:
#if WIN32
        static void _WcharToString(const wchar_t* wp, QYString &rStr);
        static char* _WcharToChar(const wchar_t* wp);
	    static wchar_t* _CharToWchar(const char* c);
#else
        int _ParseValue(QYString &rStr, QYString &rKey, QYString &rValue);
        int _GetSectionName(QYString &rStr, QYString &rSectionName);
        int _SetCfgToMap(const QYString &rSection, const QYString &rKey, const QYString &rValue);
#endif
    
    private:
        typedef std::map<QYString, QYString> STR2STR_MAP;
        typedef std::map<QYString, STR2STR_MAP*> INI_CONFIG_MAP;//两层map, key:section, value:map<key, value>
        INI_CONFIG_MAP m_ConfigMap;
        
        QYString m_FilePath;
        CMutex m_WriteLock;
    };
}


