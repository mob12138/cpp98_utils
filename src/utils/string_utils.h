#ifndef STRINGUTIL_H
#define STRINGUTIL_H

#include <sstream>
#include <string>

#define FORMAT_IMPL(v_str, v_pszFmt)                                                                                   \
    do                                                                                                                 \
    {                                                                                                                  \
        va_list args;                                                                                                  \
        va_start(args, v_pszFmt);                                                                                      \
        string_utils::format(v_str, v_pszFmt, args);                                                                   \
        va_end(args);                                                                                                  \
    } while (0)

namespace string_utils
{
using std::string;
using std::wstring;

void format(string& v_str, const char* v_pszFmt, va_list v_args);
void format(wstring& v_wstr, const wchar_t* v_pwszFmt, va_list v_args);
void format(string& v_str, const char* v_pszFmt, ...);
void format(wstring& v_wstr, const wchar_t* v_pwszFmt, ...);
string format(const char* v_pszFmt, ...);
wstring format(const wchar_t* v_pwszFmt, ...);

void to_lower(string& v_str);
void to_lower(wstring& v_wstr);
void to_upper(string& v_str);
void to_upper(wstring& v_wstr);

bool compare_no_case(const string& v_str1, const string& v_str2);
bool compare_no_case(const wstring& v_wstr1, const wstring& v_wstr2);

wstring a_to_w(const char* v_pszASCII);
string a_to_utf8(const char* v_pszASCII);
string w_to_a(const wchar_t* v_pszUnicode);
string w_to_utf8(const wchar_t* v_pszUnicode);
string utf8_to_a(const char* v_pszUTF8);
wstring utf8_to_w(const char* v_pszUTF8);

template <typename T>
wstring to_wstring(const T& v_val, size_t v_nPrecision = 0)
{
    std::wostringstream oss;
    oss.precision(v_nPrecision);
    oss << v_val;
    return oss.str();
}
template <typename T>
string to_string(const T& v_val, size_t v_nPrecision = 0)
{
    std::ostringstream oss;
    oss.precision(v_nPrecision);
    oss << v_val;
    return oss.str();
}

void trim_left(wstring& v_wstr);
void trim_right(wstring& v_wstr);
void trim(wstring& v_wstr);
}; // namespace string_utils

#endif // STRINGUTIL_H