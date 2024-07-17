#ifndef STRING_DEF_H
#define STRING_DEF_H

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <tchar.h>

#if UNICODE
typedef wchar_t _tchar;
typedef std::wstring _tstring;
typedef std::wostringstream _tostringstream;
typedef std::wofstream _tofstream;
typedef std::wostream _tostream;
typedef std::wistream _tistream;
typedef std::wifstream _tifstream;
typedef std::wistringstream _tistringstream;
typedef std::wstringstream _tstringstream;
#define _tcout std::wcout
#define _tcin std::wcin
#define _tcerr std::wcerr

#else
typedef char _tchar;
typedef std::string _tstring;
typedef std::ostringstream _tostringstream;
typedef std::ofstream _tofstream;
typedef std::ostream _tostream;
typedef std::istream _tistream;
typedef std::ifstream _tifstream;
typedef std::istringstream _tistringstream;
typedef std::stringstream _tstringstream;
#define _tcout std::cout
#define _tcin std::cin
#define _tcerr std::cerr
#endif

using std::endl;

#endif // STRING_DEF_H