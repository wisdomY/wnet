#include <assert.h>
#include "config.h"
#include "common.h"

#if WIN32
#include <tchar.h>
#endif

using namespace std;

#define INI_MAX_LENGTH 256

namespace qysys {
    CConfig::CConfig()
    {
    }

    CConfig::CConfig(const QYString &rFilename):m_FilePath(rFilename)
    {
    }
    
    CConfig::~CConfig()
    {
        for(INI_CONFIG_MAP::iterator it = m_ConfigMap.begin(); it != m_ConfigMap.end(); ++it) {
            delete it->second;
        }
    }

 
    //加载文件
    int CConfig::Load(const QYString &rFilename)
    {
        m_FilePath = rFilename; //保存文件路径
        
#if WIN32
        return QY_LIGHT_CONTROL_SUCCESS;
#else   
        FILE  *fp;
        fp = fopen(rFilename.c_str(), "rb");
        assert(fp != NULL);

        char data[4096];
        int line            = 0;
        STR2STR_MAP *pMap   = NULL;
        while (fgets(data, 4096, fp)) {
            ++line;
            QYString dataStr(data);
            QYString key, value, sectionName;
            int sectionRet = _GetSectionName(dataStr, sectionName);
            if (QY_LIGHT_CONTROL_SUCCESS == sectionRet) { //section 
                INI_CONFIG_MAP::iterator it = m_ConfigMap.find(sectionName);
                if(it == m_ConfigMap.end()) {
                    pMap = new STR2STR_MAP;
                    m_ConfigMap.insert(INI_CONFIG_MAP::value_type(sectionName, pMap));
                }
                else {
                    pMap = it->second;
                }
                continue;
            }
            else if (QY_LIGHT_CONTROL_ERROR == sectionRet) { //error
                LOG_ERROR << "Parse sectionName failed. line: " << line << ",  " << dataStr.c_str();
                fclose(fp);
                return QY_LIGHT_CONTROL_FAIL;
            }
            else { //context
                int contextRet = _ParseValue(dataStr, key, value);
                if (QY_LIGHT_CONTROL_SUCCESS == contextRet) {
                    assert(pMap != NULL);
                    pMap->insert(STR2STR_MAP::value_type(key, value));
                }
                else if (QY_LIGHT_CONTROL_ERROR == contextRet) {
                    LOG_ERROR << "Parse Context failed. line: " << line << ",  " << dataStr.c_str();
                    fclose(fp);
                    return QY_LIGHT_CONTROL_FAIL;
                }
                else {
                    continue;
                }
            }
        }
            
        fclose(fp);

        return QY_LIGHT_CONTROL_SUCCESS;

#endif

    }


    /***************************************************************************************************************
     * 取一个string
    ***************************************************************************************************************/
    int CConfig::GetString(const QYString &rSection, const QYString &rKey, const QYString &rDefaultValue, QYString &rValue)
    {
#if WIN32
        char szTempBuff[256] = { 0 };
        GetPrivateProfileString((LPCTSTR)(rSection.c_str()), (LPCTSTR)(rKey.c_str()), (LPCTSTR)(rDefaultValue.c_str()), szTempBuff, INI_MAX_LENGTH, (LPCTSTR)(m_FilePath.c_str()));
        //_WcharToString(szTempBuff, rValue);
        rValue = szTempBuff;
#else
        rValue = rDefaultValue;
        INI_CONFIG_MAP::iterator it = m_ConfigMap.find(rSection);
        if (it == m_ConfigMap.end()) {
            return QY_LIGHT_CONTROL_FAIL;
         }
        STR2STR_MAP::iterator subIt = it->second->find(rKey);
        if (subIt == it->second->end()) {
            return QY_LIGHT_CONTROL_FAIL;
        }

        rValue = subIt->second;
#endif

        return QY_LIGHT_CONTROL_SUCCESS;
    }

    /***************************************************************************************************************
     * 返回一个整型
    ***************************************************************************************************************/
    int CConfig::GetInt(const QYString &rSection, const QYString &rKey, int defaultValue)
    {
        int ret = defaultValue;
#if WIN32
        ret = GetPrivateProfileInt((LPCTSTR)(rSection.c_str()), (LPCTSTR)(rKey.c_str()), defaultValue, (LPCTSTR)(m_FilePath.c_str()));
#else
        QYString numStr;
        if (QY_LIGHT_CONTROL_SUCCESS != GetString(rSection, rKey, std::to_string(defaultValue), numStr)) {
            return ret;
        }
        
        ret = std::atoi(numStr.c_str());
#endif

        return ret;
    }


    /***************************************************************************************************************
     * 取一section下所有的key
    ***************************************************************************************************************/
    int CConfig::GetSectionKey(const QYString &rSection, std::vector<QYString> &rKeyVec)
    {
        INI_CONFIG_MAP::iterator it = m_ConfigMap.find(rSection);
        if (it == m_ConfigMap.end()) {
            return QY_LIGHT_CONTROL_FAIL;
        }

        for (STR2STR_MAP::iterator subit = it->second->begin(); subit != it->second->end(); ++subit) {
            rKeyVec.push_back(subit->first);
        }

        return QY_LIGHT_CONTROL_SUCCESS;
    }

    /***************************************************************************************************************
     * 得到所有section的名字
    ***************************************************************************************************************/
    int CConfig::GetSectionName(std::vector<QYString> &rSectionVec)
    {
        for (INI_CONFIG_MAP::iterator it = m_ConfigMap.begin(); it != m_ConfigMap.end(); ++it) {
            rSectionVec.push_back(it->first);
        }
        return QY_LIGHT_CONTROL_SUCCESS;
    }

    /***************************************************************************************************************
     * 完整的配置文件字符串
    ***************************************************************************************************************/
    QYString CConfig::ToString()
    {
        QYString result;
    	for(INI_CONFIG_MAP::iterator it = m_ConfigMap.begin(); it != m_ConfigMap.end(); ++it) {
            result += "[" + it->first + "]\n";
    	    for(STR2STR_MAP::iterator subit = it->second->begin(); subit != it->second->end(); ++subit) {
    	        QYString s = subit->second;
                result += "    " + subit->first + " = " + s + "\n";
            }
        }
        result += "\n";    
        return result;
    }


    /***************************************************************************************************************
     * 将指定section，key，value写入文件
    ***************************************************************************************************************/
    int CConfig::WriteProfileString(const QYString &rSection, const QYString &rKey, const QYString &rValue)
    {
        LOG_INFO << "Write config to file, Section:" << rSection.c_str() << ", Key:" << rKey.c_str() << ", Value:" << rValue.c_str();

#if WIN32
        WritePrivateProfileStringA(rSection.c_str(), rKey.c_str(), rValue.c_str(), m_FilePath.c_str());        
#else
        if (m_FilePath.empty()) {    //默认文件必须已经存在
            LOG_ERROR << "File to be writen is not exist.";
            return QY_LIGHT_CONTROL_ERROR;
        }

        std::lock_guard<std::mutex> guard(m_WriteLock);

        //去除两端空格, 防止调用方传过来的字符串首尾有空格
        QYString fileSection = rSection;
        QYString fileKey     = rKey;
        QYString fileValue   = rValue;
        QYStringOp::Trim(fileSection);
        QYStringOp::Trim(fileKey);
        QYStringOp::Trim(fileValue);

        _SetCfgToMap(fileSection, fileKey, fileValue);  //将新添加的section保存到map中
        
        QYString fileContext;    //保存文件内容
        QYString strRow;         //对应文件中的每一行内容
        size_t nSectionPos = std::string::npos;    //section对应的位置
        size_t nKeyPos = 0;     //key对应的位置

        char tmpSection[100] = {0};    
        char data[4096];

        FILE* pfile = NULL;
    
        pfile = fopen(m_FilePath.c_str(), "rb");
        QY_snprintf(tmpSection, 100, "[%s]", fileSection.c_str());

        while (fgets(data, 4096, pfile)) {
            strRow = data;
            QYStringOp::Trim(strRow);
            if (nSectionPos == std::string::npos && (nSectionPos = strRow.find(tmpSection)) != std::string::npos) {
                fileContext += tmpSection;
                fileContext += "\n";
                nSectionPos = fileContext.length();
                nKeyPos = std::string::npos;
            }
            else if (nKeyPos == std::string::npos && (nKeyPos = strRow.find(fileKey)) != std::string::npos) {
                //确保key相同，防止tmpContext是key的子串，也不能在tmpContext拼接=再查找，因为key = value，key后面的空格数不确定
                QYString str = strRow;   
                QYString key, value;
                _ParseValue(str, key, value);
                if (key == fileKey) {   //必须判断下，防止fileKey为key的子串
                    fileContext += fileKey;
                    fileContext += " = ";
                    fileContext += fileValue;
                    fileContext += "\n";
                }
                else {
                    fileContext += strRow;
                    fileContext += "\n";
                }
            }
            else {
                fileContext += strRow;
                fileContext += "\n";
            }  
        }
        
        if (nSectionPos == std::string::npos && nKeyPos == 0) {//新的section
            memset(data, 0, sizeof(data));
            sprintf(data, "%s\n%s = %s\n", tmpSection, fileKey.c_str(), fileValue.c_str());
            fileContext += data;
        }
        else if (nSectionPos != std::string::npos && nKeyPos == std::string::npos) {//已存在section，但是是新的key
            QYString strBack = fileContext.substr(nSectionPos);
            fileContext = fileContext.substr(0, nSectionPos);
            fileContext += fileKey;
            fileContext += " = ";
            fileContext += fileValue;
            fileContext += "\n" + strBack;
        }
        
        fclose(pfile);//关闭读文件操作符
        pfile = fopen(m_FilePath.c_str(), "w");

        fwrite(fileContext.c_str(), sizeof(char), fileContext.length(), pfile);
        fclose(pfile);//关闭写文件操作符
#endif

        return QY_LIGHT_CONTROL_SUCCESS;    
    }




#if WIN32
    void CConfig::_WcharToString(const wchar_t* wp, QYString &rStr)
    {
		char *pTempBuff = NULL;
        pTempBuff   = _WcharToChar(wp);
        rStr        = pTempBuff;
        SAFE_DELETE_ARRAY(pTempBuff);
    }


    char* CConfig::_WcharToChar(const wchar_t* wp)
    {
        char *m_char;
        int len = WideCharToMultiByte(CP_ACP, 0, wp, wcslen(wp), NULL, 0, NULL, NULL);
        m_char = new char[len + 1];
        WideCharToMultiByte(CP_ACP, 0, wp, wcslen(wp), m_char, len, NULL, NULL);

        m_char[len] = '\0';
        return m_char;
    }



    wchar_t* CConfig::_CharToWchar(const char* c)
    {
        wchar_t *m_wchar;
        int len = MultiByteToWideChar(CP_ACP, 0, c, strlen(c), NULL, 0);
        m_wchar = new wchar_t[len + 1];
        MultiByteToWideChar(CP_ACP, 0, c, strlen(c), m_wchar, len);

        m_wchar[len] = '\0';
        return m_wchar;
    }
    
#else

    /***************************************************************************************************************
     * 解析context
    ***************************************************************************************************************/
    int CConfig::_ParseValue(QYString &rStr, QYString &rKey, QYString &rValue)
    {
        if (rStr.empty()){
            return QY_LIGHT_CONTROL_FAIL;
        }

        QYStringOp::Trim(rStr);

        // 是注释行或空行
        if (rStr.empty() || rStr[0] == '#' || rStr[0] == ';' ) {
            return QY_LIGHT_CONTROL_FAIL;
        }

        size_t pos = rStr.find('=');
        if (pos == std::string::npos) {
            LOG_ERROR << "Error config. str:" << rStr.c_str();
            return QY_LIGHT_CONTROL_ERROR;
        }

        rKey = rStr.substr(0, pos);//获取key
        QYStringOp::Trim(rKey);

        //去除行尾的注释，获取value
        size_t pos1 = rStr.find('#');
        if (pos1 != std::string::npos) {
            assert(pos1-pos > 1);
            rValue = rStr.substr(pos+1, pos1-pos-1);
        }
        else {
            rValue = rStr.substr(pos+1, rStr.size()-pos-1);
        }

        QYStringOp::Trim(rValue);

        return QY_LIGHT_CONTROL_SUCCESS;
    }


    /***************************************************************************************************************
     * 判断与获取section名
    ***************************************************************************************************************/
    int CConfig::_GetSectionName(QYString &rStr, QYString &rSectionName)
    {
        QYStringOp::TrimLeft(rStr);
        if (rStr.empty() || rStr.size() <= 2 || rStr[0] != '[') {
            return QY_LIGHT_CONTROL_FAIL;
        }

        QYStringOp::TrimRight(rStr);
        
        if (rStr.back() != ']') {
            LOG_ERROR << "SectionName format failed. name:" << rStr.c_str();
            return QY_LIGHT_CONTROL_ERROR;
        }

        rSectionName = rStr.substr(1, rStr.size()-2); //get section name

        QYStringOp::Trim(rSectionName);
        return QY_LIGHT_CONTROL_SUCCESS;
    }


    /***************************************************************************************************************
     * 将指定section，key，value存入内存map中
    ***************************************************************************************************************/
    int CConfig::_SetCfgToMap(const QYString &rSection, const QYString &rKey, const QYString &rValue)
    {
        STR2STR_MAP *pMap   = NULL;
        INI_CONFIG_MAP::iterator it = m_ConfigMap.find(rSection);
        if(it == m_ConfigMap.end()) {
            LOG_INFO << "New Section, Section:" << rSection.c_str() << ", Key:" << rKey.c_str() << ", Value:" << rValue.c_str();
            pMap = new STR2STR_MAP;
            pMap->insert(STR2STR_MAP::value_type(rKey, rValue));
            m_ConfigMap.insert(INI_CONFIG_MAP::value_type(rSection, pMap));
        }
        else {
            pMap = it->second;
            STR2STR_MAP::iterator it2 = pMap->find(rKey);
            if(it2 == pMap->end()) {
                LOG_INFO << "New Key, Section:" << rSection.c_str() << ", Key:" << rKey.c_str() << ", Value:" << rValue.c_str();
                pMap->insert(STR2STR_MAP::value_type(rKey, rValue));
            }
            else {
                LOG_INFO << "Update config, Section:" << rSection.c_str() << ", Key:" << rKey.c_str() << ", Value:" << rValue.c_str();
                it2->second = rValue;
            }
        }
        
        return QY_LIGHT_CONTROL_SUCCESS;
    }

#endif

}



