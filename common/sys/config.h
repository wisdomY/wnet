#pragma once

#include "common.h"
#include "singleton.h"
#include "evpp/logging.h"

//using namespace std;

namespace qysys  {

    /** 
     * @brief ����ini�����ļ�,������������key-value����ʽ�洢���ڴ���
     */
    class CConfig {
    public:
        CConfig();
        CConfig(const QYString &rFilename);
        ~CConfig();
        
        // ����һ���ļ�
        int Load(const QYString &rFilename);
        // ȡһ���ַ���
        int GetString(const QYString &rSection, const QYString &rKey, const QYString &rDefaultValue, QYString &rValue);
        // ȡһ������
        int GetInt(const QYString &rSection, const QYString &rKey, int defaultValue);
        // ȡһsection�����е�key
        int GetSectionKey(const QYString &rSection, std::vector<QYString> &rKeyVec);
        // �õ�����section������
        int GetSectionName(std::vector<QYString> &rSectionVec);
        // �����������ļ��ַ���
        QYString ToString();

        //���ļ���дini����
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
        typedef std::map<QYString, STR2STR_MAP*> INI_CONFIG_MAP;//����map, key:section, value:map<key, value>
        INI_CONFIG_MAP m_ConfigMap;
        
        QYString m_FilePath;
        CMutex m_WriteLock;
    };
}


