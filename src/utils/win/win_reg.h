/**
 * @file win_reg.h
 * @brief Windows 注册表操作类
 * @author zhengw
 * @date 2024-06-13
 */
#ifndef _WIN_REG_H_
#define _WIN_REG_H_

#include "../string_defs.h"
#include <windows.h>

#define HKCR (HKEY_CLASSES_ROOT)
#define HKCU (HKEY_CURRENT_USER)
#define HKLM (HKEY_LOCAL_MACHINE)
#define HKU (HKEY_USERS)

#define HKPD (HKEY_PERFORMANCE_DATA)
#define HKPT (HKEY_PERFORMANCE_TEXT)
#define HKPN (HKEY_PERFORMANCE_NLSTEXT)

#if (WINVER >= 0x0400)
#define HKCC (HKEY_CURRENT_CONFIG)
#define HKDD (HKEY_DYN_DATA)
#define HKCULS (HKEY_CURRENT_USER_LOCAL_SETTINGS)
#endif

/**
 * @brief windows 注册表操作类
 */
class win_reg
{
public:
    /**
     * @brief 构造函数
     * @param [in] v_hRootKey 根节点，如 HKCR, HKCU, HKLM, HKU, HKPD, HKPT, HKPN,
     * HKCC, HKDD, HKCULS
     * @param [in] v_pszSubKeyPath 子节点路径
     * @param [in] v_samDesired 访问权限
     */
    win_reg(HKEY v_hRootKey = NULL, const char_t* v_pszSubKeyPath = NULL, REGSAM v_samDesired = KEY_READ | KEY_WRITE)
        : m_hCurKey(NULL)
    {
        open_key(v_hRootKey, v_pszSubKeyPath, v_samDesired);
    }
    virtual ~win_reg()
    {
        close_cur_key();
    }

public:
    /**
     * @brief 获取最后一次操作的错误代码
     * @return LSTATUS 错误代码
     */
    static LSTATUS get_last_error()
    {
        return m_lLastError;
    }

public:
    /**
     * @brief 创建注册表节点
     * @param [in] v_hRootKey 根节点，如 HKCR, HKCU, HKLM, HKU, HKPD, HKPT, HKPN,
     * HKCC, HKDD, HKCULS
     * @param [in] v_pszSubKeyPath 子节点路径
     * @param [in] v_samDesired 访问权限
     * @return BOOL TRUE成功，FALSE失败
     */
    BOOL create_key(HKEY v_hRootKey, const char_t* v_pszSubKeyPath, REGSAM v_samDesired = KEY_ALL_ACCESS);

    /**
     * @brief 打开注册表节点
     * @param [in] v_hRootKey 根节点，如 HKCR, HKCU, HKLM, HKU, HKPD, HKPT, HKPN,
     * HKCC, HKDD, HKCULS
     * @param [in] v_pszSubKeyPath 子节点路径
     * @param [in] v_samDesired 访问权限
     * @return BOOL TRUE成功，FALSE失败
     */
    BOOL open_key(HKEY v_hRootKey, const char_t* v_pszSubKeyPath = NULL, REGSAM v_samDesired = KEY_READ | KEY_WRITE);

    /**
     * @brief 删除注册表节点
     * @param [in] v_hRootKey 根节点，如 HKCR, HKCU, HKLM, HKU, HKPD, HKPT, HKPN,
     * HKCC, HKDD, HKCULS
     * @param [in] v_pszSubKeyPath 子节点路径
     * @return BOOL TRUE成功，FALSE失败
     */
    static BOOL delete_key(HKEY v_hRootKey, const char_t* v_pszSubKeyPath);

    /**
     * @brief 删除当前打开/创建的注册表节点的指定值
     * @param [in] v_pszValueName 值名称
     * @return BOOL TRUE成功，FALSE失败
     */
    BOOL delete_value(const char_t* v_pszValueName);

    /**
     * @brief 枚举当前打开/创建的注册表节点的所有子节点名称
     * @param [out] v_keyList 子节点名称列表
     * @return BOOL TRUE成功，FALSE失败
     */
    BOOL enum_keys(std::vector<string_t>& v_keyList);

    /**
     * @brief 枚举当前打开/创建的注册表节点的所有值名称
     * @param [out] v_valueList 值名称列表
     * @return BOOL TRUE成功，FALSE失败
     */
    BOOL enum_values(std::vector<string_t>& v_valueList);

    /**
     * @brief 设置当前打开/注册的注册表节点的值名称对应的DWORD值
     * @param [in] v_pszValueName 值名称
     * @param [in] v_dwValue 值
     * @return BOOL TRUE成功，FALSE失败
     */
    BOOL set_dword(const char_t* v_pszValueName, DWORD v_dwValue);

    /**
     * @brief 获取当前打开/注册的注册表节点的值名称对应的DWORD值
     * @param [in] v_pszValueName 值名称
     * @param [out] v_dwValue 值
     * @return BOOL TRUE成功，FALSE失败
     */
    BOOL get_dword(const char_t* v_pszValueName, DWORD& v_dwValue);

    /**
     * @brief 设置当前打开/注册的注册表节点的值名称对应的QWORD值
     * @param [in] v_pszValueName 值名称
     * @param [in] v_ullValue 值
     * @return BOOL TRUE成功，FALSE失败
     */
    BOOL set_qword(const char_t* v_pszValueName, ULONGLONG v_ullValue);

    /**
     * @brief 获取当前打开/注册的注册表节点的值名称对应的QWORD值
     * @param [in] v_pszValueName 值名称
     * @param [out] v_ullValue 值
     * @return BOOL TRUE成功，FALSE失败
     */
    BOOL get_qword(const char_t* v_pszValueName, ULONGLONG& v_ullValue);

    /**
     * @brief 设置当前打开/注册的注册表节点的值名称对应的字符串值
     * @param [in] v_pszValueName 值名称
     * @param [in] v_strValue 值
     * @return BOOL TRUE成功，FALSE失败
     */
    BOOL set_string(const char_t* v_pszValueName, const string_t& v_strValue);

    /**
     * @brief 获取当前打开/注册的注册表节点的值名称对应的字符串值
     * @param [in] v_pszValueName 值名称
     * @param [out] v_strValue 值
     * @return BOOL TRUE成功，FALSE失败
     */
    BOOL get_string(const char_t* v_pszValueName, string_t& v_strValue);

    /**
     * @brief 设置当前打开/注册的注册表节点的值名称对应的二进制值
     * @param [in] v_pszValueName 值名称
     * @param [in] v_pData 值
     * @param [in] v_dwDataSize 值大小
     * @return BOOL TRUE成功，FALSE失败
     */
    BOOL set_binary(const char_t* v_pszValueName, const void* v_pData, DWORD v_dwDataSize);

    /**
     * @brief 获取当前打开/注册的注册表节点的值名称对应的二进制值
     * @param [in] v_pszValueName 值名称
     * @param [out] v_pData 值
     * @param [in] v_dwDataSize 值大小
     * @return BOOL TRUE成功，FALSE失败
     */
    BOOL get_binary(const char_t* v_pszValueName, void* v_pData, DWORD v_dwDataSize);

    /**
     * @brief 获取当前打开/注册的注册表节点的值名称对应的大小
     * @param [in] v_pszValueName 值名称
     * @param [out] v_dwDataSize 值大小
     * @return BOOL TRUE成功，FALSE失败
     */
    BOOL get_value_size(const char_t* v_pszValueName, DWORD& v_dwDataSize);

private:
    /**
     * @brief 关闭当前打开的注册表节点
     */
    void close_cur_key()
    {
        if (m_hCurKey)
        {
            ::RegCloseKey(m_hCurKey);
            m_hCurKey = NULL;
            m_strCurKeyPath.clear();
        }
    }

    /**
     * @brief 判断操作是否成功
     * @return BOOL TRUE成功，FALSE失败
     */
    BOOL is_successed() const
    {
        return ERROR_SUCCESS == m_lLastError;
    }

private:
    HKEY m_hCurKey;              // 当前打开的注册表节点
    string_t m_strCurKeyPath;    // 当前打开的注册表节点路径
    static LSTATUS m_lLastError; // 最后一次操作的错误代码
};

#endif // _WIN_REG_H_