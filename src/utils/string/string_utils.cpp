#include "string_utils.h"
#include <windows.h>

#pragma warning(disable : 4996)

namespace string_utils
{
void format(string& v_str, const char* v_pszFmt, va_list v_args)
{
    if (!v_pszFmt)
    {
        return;
    }
    int nLen = ::_vscprintf(v_pszFmt, v_args) + 1;
    if (nLen > 0)
    {
        v_str.resize(nLen);
        ::vsprintf(&v_str[0], v_pszFmt, v_args);
    }
}
void format(wstring& v_wstr, const wchar_t* v_pwszFmt, va_list v_args)
{
    if (!v_pwszFmt)
    {
        return;
    }
    int nLen = ::_vscwprintf(v_pwszFmt, v_args) + 1;
    if (nLen > 0)
    {
        v_wstr.resize(nLen);
        ::vswprintf(&v_wstr[0], nLen, v_pwszFmt, v_args);
    }
}
void format(string& v_str, const char* v_pszFmt, ...)
{
    if (!v_pszFmt)
    {
        return;
    }
    FORMAT_IMPL(v_str, v_pszFmt);
}
void format(wstring& v_wstr, const wchar_t* v_pwszFmt, ...)
{
    if (!v_pwszFmt)
    {
        return;
    }
    FORMAT_IMPL(v_wstr, v_pwszFmt);
}
string format(const char* v_pszFmt, ...)
{
    if (!v_pszFmt)
    {
        return "";
    }
    string str;
    FORMAT_IMPL(str, v_pszFmt);
    return str;
}
wstring format(const wchar_t* v_pwszFmt, ...)
{
    if (!v_pwszFmt)
    {
        return L"";
    }
    wstring wstr;
    FORMAT_IMPL(wstr, v_pwszFmt);
    return wstr;
}

bool compare_no_case(const string& v_str1, const string& v_str2)
{
    return ::_stricmp(v_str1.c_str(), v_str2.c_str()) == 0;
}
bool compare_no_case(const wstring& v_wstr1, const wstring& v_wstr2)
{
    return ::_wcsicmp(v_wstr1.c_str(), v_wstr2.c_str()) == 0;
}

wstring a_to_w(const char* v_pszASCII)
{
    if (!v_pszASCII)
    {
        return L"";
    }

    int nWcharLen = ::MultiByteToWideChar(CP_ACP, 0, v_pszASCII, -1, NULL, 0);
    if (nWcharLen <= 0 || ERROR_NO_UNICODE_TRANSLATION == nWcharLen)
    {
        return L"";
    }
    wstring strBuf;
    strBuf.resize((size_t)(nWcharLen + 1));
    ::MultiByteToWideChar(CP_ACP, 0, v_pszASCII, -1, &strBuf[0], nWcharLen);
    return strBuf;
}
string a_to_utf8(const char* v_pszASCII)
{
    if (!v_pszASCII)
    {
        return "";
    }
    return w_to_utf8(a_to_w(v_pszASCII).c_str());
}
string w_to_a(const wchar_t* v_pwszUnicode)
{
    if (!v_pwszUnicode)
    {
        return "";
    }

    int nCharLen = WideCharToMultiByte(CP_ACP, 0, v_pwszUnicode, -1, NULL, 0, NULL, NULL);
    if (nCharLen <= 0)
    {
        return "";
    }
    string strBuf;
    strBuf.resize((size_t)(nCharLen + 1));
    WideCharToMultiByte(CP_ACP, 0, v_pwszUnicode, -1, &strBuf[0], nCharLen, NULL, NULL);
    return strBuf;
}
string w_to_utf8(const wchar_t* v_pwszUnicode)
{
    if (!v_pwszUnicode)
    {
        return "";
    }

    int nCharLen = WideCharToMultiByte(CP_UTF8, 0, v_pwszUnicode, -1, NULL, 0, NULL, NULL);
    if (nCharLen <= 0)
    {
        return "";
    }
    string strBuf;
    strBuf.resize((size_t)(nCharLen + 1));
    WideCharToMultiByte(CP_UTF8, 0, v_pwszUnicode, -1, &strBuf[0], nCharLen, NULL, NULL);
    return strBuf;
}
string utf8_to_a(const char* v_pszUTF8)
{
    if (!v_pszUTF8)
    {
        return "";
    }
    return w_to_a(utf8_to_w(v_pszUTF8).c_str());
}
wstring utf8_to_w(const char* v_pszUTF8)
{
    if (!v_pszUTF8)
    {
        return L"";
    }

    int nWcharLen = ::MultiByteToWideChar(CP_UTF8, 0, v_pszUTF8, -1, NULL, 0);
    if (nWcharLen <= 0 || ERROR_NO_UNICODE_TRANSLATION == nWcharLen)
    {
        return L"";
    }
    wstring strBuf;
    strBuf.resize((size_t)(nWcharLen + 1));
    ::MultiByteToWideChar(CP_UTF8, 0, v_pszUTF8, -1, &strBuf[0], nWcharLen);
    return strBuf;
}

void trim_left(wstring& v_wstr)
{
    size_t nPos = v_wstr.find_first_not_of(L" \t\r\n");
    if (nPos != wstring::npos)
    {
        v_wstr.erase(0, nPos);
    }
}
void trim_right(wstring& v_wstr)
{
    size_t nPos = v_wstr.find_last_not_of(L" \t\r\n");
    if (nPos != wstring::npos)
    {
        v_wstr.erase(nPos + 1);
    }
}
void trim(wstring& v_wstr)
{
    trim_left(v_wstr);
    trim_right(v_wstr);
}
} // namespace string_utils