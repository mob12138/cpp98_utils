#include "win_reg.h"

LSTATUS win_reg::m_lLastError = ERROR_SUCCESS;

BOOL win_reg::create_key(HKEY v_hRootKey, const char_t* v_pszSubKeyPath, REGSAM v_samDesired)
{
    if (!v_hRootKey || !v_pszSubKeyPath)
    {
        return FALSE;
    }

    close_cur_key();
    DWORD dwDisposition = 0;

    m_lLastError = ::RegCreateKeyEx(v_hRootKey, v_pszSubKeyPath, 0, NULL, REG_OPTION_NON_VOLATILE, v_samDesired, NULL,
                                    &m_hCurKey, &dwDisposition);
    if (is_successed())
    {
        m_strCurKeyPath = v_pszSubKeyPath;
        return TRUE;
    }
    return FALSE;
}

BOOL win_reg::open_key(HKEY v_hRootKey, const char_t* v_pszSubKeyPath, REGSAM v_samDesired)
{
    if (!v_hRootKey)
    {
        return FALSE;
    }

    close_cur_key();

    m_lLastError = ::RegOpenKeyEx(v_hRootKey, v_pszSubKeyPath, 0, v_samDesired, &m_hCurKey);
    if (is_successed())
    {
        m_strCurKeyPath = v_pszSubKeyPath;
        return TRUE;
    }
    return FALSE;
}

BOOL win_reg::delete_key(HKEY v_hRootKey, const char_t* v_pszSubKeyPath)
{
    if (!v_hRootKey || !v_pszSubKeyPath)
    {
        return FALSE;
    }

    m_lLastError = ::RegDeleteTree(v_hRootKey, v_pszSubKeyPath);
    return (ERROR_SUCCESS == m_lLastError);
}

BOOL win_reg::delete_value(const char_t* v_pszValueName)
{
    m_lLastError = ::RegDeleteKeyValue(m_hCurKey, NULL, v_pszValueName);
    return is_successed();
}

BOOL win_reg::enum_keys(std::vector<string_t>& v_keyList)
{
    char_t szKeyName[MAX_PATH] = {0};
    DWORD dwCchName = _countof(szKeyName);
    DWORD dwIndex = 0;

    v_keyList.clear();

    do
    {
        dwCchName = _countof(szKeyName);
        m_lLastError = ::RegEnumKeyEx(m_hCurKey, dwIndex++, szKeyName, &dwCchName, NULL, NULL, NULL, NULL);
        if (is_successed())
        {
            v_keyList.push_back(szKeyName);
        }
    } while (is_successed());

    return TRUE;
}

BOOL win_reg::enum_values(std::vector<string_t>& v_valueList)
{
    char_t szValueName[MAX_PATH] = {0};
    DWORD dwCchName = _countof(szValueName);
    DWORD dwIndex = 0;

    v_valueList.clear();

    do
    {
        dwCchName = _countof(szValueName);
        m_lLastError = ::RegEnumValue(m_hCurKey, dwIndex++, szValueName, &dwCchName, NULL, NULL, NULL, NULL);
        if (is_successed())
        {
            v_valueList.push_back(szValueName);
        }
    } while (is_successed());

    return TRUE;
}

BOOL win_reg::set_dword(const char_t* v_pszValueName, DWORD v_dwValue)
{
    m_lLastError = ::RegSetValueEx(m_hCurKey, v_pszValueName, 0, REG_DWORD, (const BYTE*)&v_dwValue, sizeof(v_dwValue));
    return is_successed();
}

BOOL win_reg::get_dword(const char_t* v_pszValueName, DWORD& v_dwValue)
{
    DWORD v_dwDataSize = sizeof(v_dwValue);
    DWORD dwType = REG_DWORD;
    m_lLastError = ::RegQueryValueEx(m_hCurKey, v_pszValueName, NULL, &dwType, (LPBYTE)&v_dwValue, &v_dwDataSize);
    return is_successed();
}

BOOL win_reg::set_qword(const char_t* v_pszValueName, ULONGLONG v_ullValue)
{
    m_lLastError =
        ::RegSetValueEx(m_hCurKey, v_pszValueName, 0, REG_QWORD, (const BYTE*)&v_ullValue, sizeof(v_ullValue));
    return is_successed();
}

BOOL win_reg::get_qword(const char_t* v_pszValueName, ULONGLONG& v_ullValue)
{
    DWORD v_dwDataSize = sizeof(v_ullValue);
    DWORD dwType = REG_QWORD;
    m_lLastError = ::RegQueryValueEx(m_hCurKey, v_pszValueName, NULL, &dwType, (LPBYTE)&v_ullValue, &v_dwDataSize);
    return is_successed();
}

BOOL win_reg::set_string(const char_t* v_pszValueName, const string_t& v_strValue)
{
    m_lLastError = ::RegSetValueEx(m_hCurKey, v_pszValueName, 0, REG_SZ, (const BYTE*)v_strValue.c_str(),
                                   (DWORD)((v_strValue.length() + 1) * sizeof(char_t)));
    return is_successed();
}

BOOL win_reg::get_string(const char_t* v_pszValueName, string_t& v_strValue)
{
    DWORD v_dwDataSize = 0;
    DWORD dwType = REG_SZ;
    m_lLastError = ::RegQueryValueEx(m_hCurKey, v_pszValueName, NULL, &dwType, NULL, &v_dwDataSize);
    if (!is_successed())
    {
        return FALSE;
    }

    v_strValue.resize(v_dwDataSize / sizeof(char_t));
    m_lLastError = ::RegQueryValueEx(m_hCurKey, v_pszValueName, NULL, &dwType, (LPBYTE)&v_strValue[0], &v_dwDataSize);
    return is_successed();
}

BOOL win_reg::set_binary(const char_t* v_pszValueName, const void* v_pData, DWORD v_dwDataSize)
{
    m_lLastError = ::RegSetValueEx(m_hCurKey, v_pszValueName, 0, REG_BINARY, (const BYTE*)v_pData, v_dwDataSize);
    return is_successed();
}

BOOL win_reg::get_binary(const char_t* v_pszValueName, void* v_pData, DWORD v_dwDataSize)
{
    DWORD dwType = REG_BINARY;
    DWORD dwSize = v_dwDataSize;
    m_lLastError = ::RegQueryValueEx(m_hCurKey, v_pszValueName, NULL, &dwType, (LPBYTE)v_pData, &dwSize);
    return is_successed();
}

BOOL win_reg::get_value_size(const char_t* v_pszValueName, DWORD& v_dwDataSize)
{
    m_lLastError = ::RegQueryValueEx(m_hCurKey, v_pszValueName, NULL, NULL, NULL, &v_dwDataSize);
    return is_successed() || (ERROR_MORE_DATA == m_lLastError);
}