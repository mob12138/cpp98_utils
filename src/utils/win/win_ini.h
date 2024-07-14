#ifndef WIN_INI_H
#define WIN_INI_H

#include <string>
#include <windows.h>

class win_ini
{
public:
    win_ini(const std::wstring& v_wstrFileName) : m_wstrFileName(v_wstrFileName) {}
    virtual ~win_ini() {}

    BOOL get_value(const std::wstring& v_wstrSection, const std::wstring& v_wstrKey, int& v_nVal,
                   int v_nDefaultVal = 0) const
    {
        if (v_wstrSection.empty() || v_wstrKey.empty())
        {
            return FALSE;
        }

        v_nVal =
            ::GetPrivateProfileIntW(v_wstrSection.c_str(), v_wstrKey.c_str(), v_nDefaultVal, m_wstrFileName.c_str());
        return TRUE;
    }

    BOOL set_value(const std::wstring& v_wstrSection, const std::wstring& v_wstrKey, int v_nVal)
    {
        if (v_wstrSection.empty() || v_wstrKey.empty())
        {
            return FALSE;
        }

        return ::WritePrivateProfileStringW(v_wstrSection.c_str(), v_wstrKey.c_str(), std::to_wstring(v_nVal).c_str(),
                                            m_wstrFileName.c_str());
    }

    BOOL get_value(const std::wstring& v_wstrSection, const std::wstring& v_wstrKey, std::wstring& v_wstrVal,
                   const std::wstring& v_wstrDefaultVal = L"") const
    {
        if (v_wstrSection.empty() || v_wstrKey.empty())
        {
            return FALSE;
        }

        v_wstrVal.resize(MAX_PATH);
        if (::GetPrivateProfileStringW(v_wstrSection.c_str(), v_wstrKey.c_str(), v_wstrDefaultVal.c_str(),
                                       v_wstrVal.data(), MAX_PATH, m_wstrFileName.c_str()) == 0)
        {
            return FALSE;
        }

        return TRUE;
    }

    BOOL set_value(const std::wstring& v_wstrSection, const std::wstring& v_wstrKey, const std::wstring& v_wstrVal)
    {
        if (v_wstrSection.empty() || v_wstrKey.empty())
        {
            return FALSE;
        }

        return ::WritePrivateProfileStringW(v_wstrSection.c_str(), v_wstrKey.c_str(), v_wstrVal.c_str(),
                                            m_wstrFileName.c_str());
    }

private:
    std::wstring m_wstrFileName;
};

#endif // WIN_INI_H