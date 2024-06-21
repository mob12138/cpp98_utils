/**
 * @file win_file.h
 * @brief Windows文件操作类
 * @author zhengw
 * @date 2024-06-07
 */
#ifndef WIN_FILE_H
#define WIN_FILE_H

#include <windows.h>
#include <crtdbg.h>

class win_file
{
public:
    explicit win_file() : m_hFile(INVALID_HANDLE_VALUE) {}
    explicit win_file(LPCWSTR v_lpFileName,
                      DWORD v_dwDesiredAccess,
                      DWORD v_dwShareMode,
                      LPSECURITY_ATTRIBUTES v_lpSecurityAttributes,
                      DWORD v_dwCreationDisposition,
                      DWORD v_dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL,
                      HANDLE v_hTemplateFile = NULL) : m_hFile(INVALID_HANDLE_VALUE)
    {
        open(v_lpFileName,
             v_dwDesiredAccess,
             v_dwShareMode,
             v_lpSecurityAttributes,
             v_dwCreationDisposition,
             v_dwFlagsAndAttributes,
             v_hTemplateFile);
    }
    virtual ~win_file() { close(); }

public:
    /**
     * @brief 打开文件
     * @param [in] v_pwszFilePath - 文件路径
     * @param [in] v_iDAcc - 访问权限
     * @param [in] v_FileMode - 文件模式
     * @param [in] v_bShareRead - 是否共享读
     * @return TRUE 成功，FALSE 失败
     */
    BOOL open(LPCWSTR v_lpFileName,
              DWORD v_dwDesiredAccess,
              DWORD v_dwShareMode,
              LPSECURITY_ATTRIBUTES v_lpSecurityAttributes,
              DWORD v_dwCreationDisposition,
              DWORD v_dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL,
              HANDLE v_hTemplateFile = NULL)
    {
        m_hFile = CreateFileW(v_lpFileName,
                              v_dwDesiredAccess,
                              v_dwShareMode,
                              v_lpSecurityAttributes,
                              v_dwCreationDisposition,
                              v_dwFlagsAndAttributes,
                              v_hTemplateFile);

        return valid();
    }
    /**
     * @brief 关闭文件，释放资源
     */
    void close()
    {
        if (valid())
        {
            ::CloseHandle(m_hFile);
            m_hFile = INVALID_HANDLE_VALUE;
        }
    }
    /**
     * @brief 移动文件指针
     * @param [in] v_llOff - 偏移量
     * @param [in] v_dwMoveMethod - 移动方式
     * @return TRUE 成功，FALSE 失败
     */
    BOOL seek(LONGLONG v_llOff, DWORD v_dwMoveMethod = FILE_BEGIN) const
    {
        LARGE_INTEGER liOff = { 0 };
        liOff.QuadPart = v_llOff;
        liOff.LowPart = ::SetFilePointer(m_hFile, liOff.LowPart, &liOff.HighPart, v_dwMoveMethod);

        return liOff.LowPart != INVALID_SET_FILE_POINTER;
    }
    /**
     * @brief 读取文件
     * @param [out] v_pBuffer - 缓冲区
     * @param [in]  v_nBuffer - 缓冲区大小
     * @return TRUE 成功，FALSE 失败
     */
    BOOL read(void* v_pBuffer, size_t v_nBuffer) const
    {
        DWORD dwBufferSize = (DWORD)v_nBuffer;
        if (v_nBuffer != dwBufferSize) { return FALSE; }

        DWORD dwRet = 0;
        return ::ReadFile(m_hFile, v_pBuffer, dwBufferSize, &dwRet, 0) && dwRet == dwBufferSize;
    }
    /**
     * @brief 写文件
     * @param [in] v_pBuffer - 缓冲区
     * @param [in] v_nBuffer - 缓冲区大小
     * @return TRUE 成功，FALSE 失败
     */
    BOOL write(const void* v_pBuffer, size_t v_nBuffer) const
    {
        DWORD dwBufferSize = (DWORD)v_nBuffer;
        if (v_nBuffer != dwBufferSize) { return FALSE; }

        DWORD dwRet = 0;
        return ::WriteFile(m_hFile, v_pBuffer, dwBufferSize, &dwRet, 0) && dwRet == dwBufferSize;
    }
    /**
     * @brief 将缓冲区中的数据写入文件
     * @return TRUE 成功，FALSE 失败
     */
    BOOL flush() const { return ::FlushFileBuffers(m_hFile); }
    /**
     * @brief 获取文件长度
     * @return 文件长度
     */
    ULONGLONG size() const
    {
        ULARGE_INTEGER liSize = { 0 };
        liSize.LowPart = ::GetFileSize(m_hFile, &liSize.HighPart);
        return liSize.QuadPart;
    }
    /**
     * @brief 获取文件当前位置
     * @return 文件当前位置
     */
    ULONGLONG position() const
    {
        LARGE_INTEGER liPos = { 0 };
        liPos.QuadPart = 0;
        liPos.LowPart = ::SetFilePointer(m_hFile, liPos.LowPart, &liPos.HighPart, FILE_CURRENT);

        return liPos.QuadPart;
    }
    /**
     * @brief 设置文件最后修改时间
     * @param [out] v_pFileTime - 文件时间
     * @return
     *		@retval TRUE 成功
     *		@retval FALSE 失败
     */
    BOOL set_last_modify_time(FILETIME* v_pFileTime) const { return ::SetFileTime(m_hFile, NULL, NULL, v_pFileTime); }
    /**
     * @brief 判断文件是否有效
     * @return TRUE 成功，FALSE 失败
     */
    BOOL valid() const { return m_hFile && m_hFile != INVALID_HANDLE_VALUE; }
private:
    HANDLE m_hFile;	// 文件句柄
};

#endif // WIN_FILE_H