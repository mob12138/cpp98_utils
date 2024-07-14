#ifndef DB_SQLITE3_H
#define DB_SQLITE3_H

#include <string>
#include <BaseTsd.h>
#include <boost/shared_ptr.hpp>
#include "sqlite3.h"
#pragma comment(lib, "sqlite3.lib")

class db_sqlite3_query_result;
class db_sqlite3_stmt_binder;

class db_sqlite3
{
    friend class db_sqlite3_stmt_binder;
public:
    db_sqlite3() : m_pDb(NULL) {}
    virtual ~db_sqlite3() { disconnect(); }

public:
    bool connect(const std::string& v_strDBName);
    bool connect(const std::wstring& v_wstrDBName);
    bool disconnect();

    bool execute(const std::string& v_strSql);
    bool execute(const char* v_pszSql, ...);
    bool execute(const std::wstring& v_wstrSql);
    bool execute(const wchar_t* v_pwszSql, ...);
    bool execute(const db_sqlite3_stmt_binder& v_binder);

    typedef boost::shared_ptr<db_sqlite3_query_result> result_ptr;
    result_ptr query(const std::string& v_strSql);
    result_ptr query(const char* v_pszSql, ...);
    result_ptr query(const std::wstring& v_wstrSql);
    result_ptr query(const wchar_t* v_pwszSql, ...);
    result_ptr query(const db_sqlite3_stmt_binder& v_binder);

    bool begin();
    bool commit();
    bool rollback();

public:
    static void set_last_error(const std::string& v_strError);
    static std::string get_last_error();
    static std::wstring get_last_error_w();
    static void reset_error();

private:
    sqlite3_stmt* prepare(const std::string& v_strSql);
    sqlite3_stmt* prepare(const std::wstring& v_wstrSql);
    bool step(sqlite3_stmt* v_pStmt);

private:
    sqlite3* m_pDb;
    static bool m_bHasError;
    static std::string m_strLastError;
};

class db_sqlite3_query_result
{
public:
    db_sqlite3_query_result(sqlite3* v_pDb, sqlite3_stmt* v_pStmt);
    virtual ~db_sqlite3_query_result() {}
public:
    bool next_row() const;
    int get_column_count() const;
    int get_column_type(int v_nCol) const;
    const char* get_column_name(int v_nCol) const;
    const wchar_t* get_column_name_w(int v_nCol) const;
    int get_column_index(const std::string& v_strColumnName) const;
    int get_column_index(const std::wstring& v_wstrColumnName) const;

    const char* get_string(int v_nCol) const;
    const wchar_t* get_wstring(int v_nCol) const;
    int get_int(int v_nCol) const;
    INT64 get_int64(int v_nCol) const;
    double get_double(int v_nCol) const;
    const void* get_blob(int v_nCol) const;

private:
    sqlite3* m_pDb;
    sqlite3_stmt* m_pStmt;
};

class db_sqlite3_stmt_binder
{
    friend class db_sqlite3;
public:
    db_sqlite3_stmt_binder(db_sqlite3* v_pDb) : m_pDb(v_pDb), m_pStmt(NULL) {}
    virtual ~db_sqlite3_stmt_binder() {}
public:
    bool prepare(const std::string& v_strSql);
    bool prepare(const char* v_pszSql, ...);
    bool prepare(const std::wstring& v_wstrSql);
    bool prepare(const wchar_t* v_pwszSql, ...);
    bool reset();

    int bind(int v_nCol, const void* v_pVal, int v_nLen, void(*v_xDel)(void*));
    int bind(int v_nCol, int v_val);
    int bind(int v_nCol, double v_val);
    int bind(int v_nCol, const std::string& v_val);
    int bind(int v_nCol, const std::wstring& v_val);

private:
    db_sqlite3* m_pDb;
    sqlite3_stmt* m_pStmt;
};

#endif // DB_SQLITE3_H
