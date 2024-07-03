#ifndef STRING_DEF_H
#define STRING_DEF_H

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <tchar.h>

#if UNICODE
typedef wchar_t char_t;
typedef std::wstring string_t;
typedef std::wostringstream ostringstream_t;
typedef std::wofstream ofstream_t;
typedef std::wostream ostream_t;
typedef std::wistream istream_t;
typedef std::wifstream ifstream_t;
typedef std::wistringstream istringstream_t;
typedef std::wstringstream stringstream_t;
#define cout_t std::wcout
#define cin_t std::wcin
#define cerr_t std::wcerr

#else
typedef char char_t;
typedef std::string string_t;
typedef std::ostringstream ostringstream_t;
typedef std::ofstream ofstream_t;
typedef std::ostream ostream_t;
typedef std::istream istream_t;
typedef std::ifstream ifstream_t;
typedef std::istringstream istringstream_t;
typedef std::stringstream stringstream_t;
#define cout_t std::cout
#define cin_t std::cin
#define cerr_t std::cerr
#endif

using std::endl;

#endif // STRING_DEF_H