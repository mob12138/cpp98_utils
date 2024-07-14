#include "../../string_utils.h"
#include <iostream>
#include <boost/make_shared.hpp>
#include "db_sqlite3.h"

#pragma comment(lib, "sqlite3.lib")

bool db_sqlite3::m_bHasError = false;
std::string db_sqlite3::m_strLastError;

struct statement_guard
{
    sqlite3_stmt* m_pStmt;
    statement_guard(sqlite3_stmt* v_pStmt) : m_pStmt(v_pStmt) { db_sqlite3::reset_error(); }
    ~statement_guard()
    {
        if (m_pStmt)
        {
            int nStatus = sqlite3_finalize(m_pStmt);
            m_pStmt = NULL;
            if (nStatus)
            {
                db_sqlite3::set_last_error(std::string("Failed to finalize statement: ") +
                                           string_utils::to_string(nStatus));
            }
        }
    }
};

bool db_sqlite3::connect(const std::string& v_strDBName)
{
    reset_error();
    int nRet = sqlite3_open(v_strDBName.c_str(), &m_pDb);
    if (SQLITE_OK != nRet)
    {
        set_last_error(sqlite3_errmsg(m_pDb));
        return false;
    }
    return true;
}
bool db_sqlite3::connect(const std::wstring& v_wstrDBName)
{
    reset_error();
    int nRet = sqlite3_open16(v_wstrDBName.c_str(), &m_pDb);
    if (SQLITE_OK != nRet)
    {
        set_last_error(sqlite3_errmsg(m_pDb));
        return false;
    }
    return true;
}
bool db_sqlite3::disconnect()
{
    bool bRet = true;
    if (m_pDb)
    {
        int nRet = sqlite3_close(m_pDb);
        if (SQLITE_OK != nRet)
        {
            set_last_error(sqlite3_errmsg(m_pDb));
            bRet = false;
        }
        m_pDb = NULL;
    }
    return bRet;
}
bool db_sqlite3::execute(const std::string& v_strSql)
{
    reset_error();
    sqlite3_stmt* pStmt = prepare(v_strSql);
    if (step(pStmt))
    {
        return true;
    }

    return false;
}
bool db_sqlite3::execute(const char* v_pszSql, ...)
{
    if (!v_pszSql) { return false; }
    std::string strSql;
    FORMAT_IMPL(strSql, v_pszSql);
    return execute(strSql);
}
bool db_sqlite3::execute(const std::wstring& v_wstrSql)
{
    reset_error();
    sqlite3_stmt* pStmt = prepare(v_wstrSql);
    if (step(pStmt))
    {
        return true;
    }

    return false;
}
bool db_sqlite3::execute(const wchar_t* v_pwszSql, ...)
{
    if (!v_pwszSql) { return false; }
    std::wstring wstrSql;
    FORMAT_IMPL(wstrSql, v_pwszSql);
    return execute(wstrSql);
}
bool db_sqlite3::execute(const db_sqlite3_stmt_binder& v_binder)
{
    reset_error();
    return step(v_binder.m_pStmt);
}
db_sqlite3::result_ptr db_sqlite3::query(const std::string& v_strSql)
{
    reset_error();
    sqlite3_stmt* pStmt = prepare(v_strSql);
    if (!pStmt) { return db_sqlite3::result_ptr(); }
    return boost::make_shared<db_sqlite3_query_result>(m_pDb, pStmt);
}
db_sqlite3::result_ptr db_sqlite3::query(const char* v_pszSql, ...)
{
    if (!v_pszSql) { return db_sqlite3::result_ptr(); }
    std::string strSql;
    FORMAT_IMPL(strSql, v_pszSql);
    return query(strSql);
}
db_sqlite3::result_ptr db_sqlite3::query(const std::wstring& v_wstrSql)
{
    reset_error();
    sqlite3_stmt* pStmt = prepare(v_wstrSql);
    if (!pStmt) { return db_sqlite3::result_ptr(); }
    return boost::make_shared<db_sqlite3_query_result>(m_pDb, pStmt);
}
db_sqlite3::result_ptr db_sqlite3::query(const wchar_t* v_pwszSql, ...)
{
    if (!v_pwszSql) { return db_sqlite3::result_ptr(); }
    std::wstring wstrSql;
    FORMAT_IMPL(wstrSql, v_pwszSql);
    return query(wstrSql);
}
db_sqlite3::result_ptr db_sqlite3::query(const db_sqlite3_stmt_binder& v_binder)
{
    if (!v_binder.m_pStmt) { return db_sqlite3::result_ptr(); }
    reset_error();
    return boost::make_shared<db_sqlite3_query_result>(m_pDb, v_binder.m_pStmt);
}

sqlite3_stmt* db_sqlite3::prepare(const std::string& v_strSql)
{
    if (v_strSql.empty()) { return NULL; }

    sqlite3_stmt* pStmt;
    int nRet = sqlite3_prepare_v2(m_pDb, &v_strSql[0], (int)v_strSql.size(), &pStmt, NULL);
    if (SQLITE_OK != nRet)
    {
        set_last_error(sqlite3_errmsg(m_pDb));
        return NULL;
    }
    return pStmt;
}
sqlite3_stmt* db_sqlite3::prepare(const std::wstring& v_wstrSql)
{
    if (v_wstrSql.empty()) { return NULL; }

    sqlite3_stmt* pStmt;
    int nRet = sqlite3_prepare16_v2(m_pDb, &v_wstrSql[0], (int)v_wstrSql.size() * sizeof(wchar_t), &pStmt, NULL);
    if (SQLITE_OK != nRet)
    {
        set_last_error(sqlite3_errmsg(m_pDb));
        return NULL;
    }
    return pStmt;
}
bool db_sqlite3::step(sqlite3_stmt* v_pStmt)
{
    if (!v_pStmt) { return false; }

    statement_guard guard(v_pStmt);
    int nRet = sqlite3_step(v_pStmt);
    if (SQLITE_DONE != nRet)
    {
        set_last_error(sqlite3_errmsg(m_pDb));
        return false;
    }
    return true;
}

bool db_sqlite3::begin()
{
    reset_error();
    if (sqlite3_exec(m_pDb, "BEGIN", NULL, NULL, NULL) != SQLITE_OK)
    {
        set_last_error(sqlite3_errmsg(m_pDb));
        return false;
    }
    return true;
}
bool db_sqlite3::commit()
{
    reset_error();
    if (sqlite3_exec(m_pDb, "COMMIT", NULL, NULL, NULL) != SQLITE_OK)
    {
        set_last_error(sqlite3_errmsg(m_pDb));
        return false;
    }
    return true;
}
bool db_sqlite3::rollback()
{
    reset_error();
    if (sqlite3_exec(m_pDb, "ROLLBACK", NULL, NULL, NULL) != SQLITE_OK)
    {
        set_last_error(sqlite3_errmsg(m_pDb));
        return false;
    }
    return true;
}

std::string db_sqlite3::get_last_error() { return m_strLastError; }
std::wstring db_sqlite3::get_last_error_w()
{
    return string_utils::a_to_w(m_strLastError.c_str());
}
void db_sqlite3::set_last_error(const std::string& v_strError)
{
    m_bHasError = true;
    m_strLastError = v_strError;
    std::cout << m_strLastError << std::endl;
}
void db_sqlite3::reset_error()
{
    m_bHasError = false;
    m_strLastError.clear();
}

db_sqlite3_query_result::db_sqlite3_query_result(sqlite3* v_pDb, sqlite3_stmt* v_pStmt)
    : m_pDb(v_pDb), m_pStmt(v_pStmt)
{}
bool db_sqlite3_query_result::next() const
{
    db_sqlite3::reset_error();
    int nRet = sqlite3_step(m_pStmt);
    switch (nRet)
    {
    case SQLITE_ROW: { return true; }
    case SQLITE_DONE:
    {
        statement_guard guard(m_pStmt);
        return false;
    }
    default:
    {
        statement_guard guard(m_pStmt);
        db_sqlite3::set_last_error(sqlite3_errmsg(m_pDb));
        return false;
    }
    }
}
int db_sqlite3_query_result::get_column_count() const
{
    return sqlite3_column_count(m_pStmt);
}
int db_sqlite3_query_result::get_column_type(int v_nCol) const
{
    return sqlite3_column_type(m_pStmt, v_nCol);
}
const char* db_sqlite3_query_result::get_column_name(int v_nCol) const
{
    return sqlite3_column_name(m_pStmt, v_nCol);
}
const wchar_t* db_sqlite3_query_result::get_column_name_w(int v_nCol) const
{
    return (const wchar_t*)sqlite3_column_name16(m_pStmt, v_nCol);
}
int db_sqlite3_query_result::get_column_index(const std::string& v_strColumnName) const
{
    if (v_strColumnName.empty()) { return -1; }
    for (int i = 0; i < get_column_count(); ++i)
    {
        if (v_strColumnName == get_column_name(i))
        {
            return i;
        }
    }
    return -1;
}
int db_sqlite3_query_result::get_column_index(const std::wstring& v_wstrColumnName) const
{
    if (v_wstrColumnName.empty()) { return -1; }
    for (int i = 0; i < get_column_count(); ++i)
    {
        if (v_wstrColumnName == get_column_name_w(i))
        {
            return i;
        }
    }
    return -1;
}
const char* db_sqlite3_query_result::get_string(int v_nCol) const
{
    return (const char*)sqlite3_column_text(m_pStmt, v_nCol);
}
const wchar_t* db_sqlite3_query_result::get_wstring(int v_nCol) const
{
    return (const wchar_t*)sqlite3_column_text16(m_pStmt, v_nCol);
}
int db_sqlite3_query_result::get_int(int v_nCol) const
{
    return sqlite3_column_int(m_pStmt, v_nCol);
}
INT64 db_sqlite3_query_result::get_int64(int v_nCol) const
{
    return sqlite3_column_int64(m_pStmt, v_nCol);
}
double db_sqlite3_query_result::get_double(int v_nCol) const
{
    return sqlite3_column_double(m_pStmt, v_nCol);
}
const void* db_sqlite3_query_result::get_blob(int v_nCol) const
{
    return sqlite3_column_blob(m_pStmt, v_nCol);
}

bool db_sqlite3_stmt_binder::prepare(const std::string& v_strSql)
{
    if (!m_pDb || m_pStmt || v_strSql.empty()) { return false; }

    m_pStmt = m_pDb->prepare(v_strSql);
    return m_pStmt ? true : false;
}
bool db_sqlite3_stmt_binder::prepare(const char* v_pszSql, ...)
{
    if (!v_pszSql) { return false; }
    std::string strSql;
    FORMAT_IMPL(strSql, v_pszSql);
    return prepare(strSql);
}
bool db_sqlite3_stmt_binder::prepare(const std::wstring& v_wstrSql)
{
    if (!m_pDb || m_pStmt || v_wstrSql.empty()) { return false; }

    m_pStmt = m_pDb->prepare(v_wstrSql);
    return m_pStmt ? true : false;
}
bool db_sqlite3_stmt_binder::prepare(const wchar_t* v_pwszSql, ...)
{
    if (!v_pwszSql) { return false; }
    std::wstring wstrSql;
    FORMAT_IMPL(wstrSql, v_pwszSql);
    return prepare(wstrSql);
}
bool db_sqlite3_stmt_binder::reset()
{
    if (m_pStmt)
    {
        int nStatus = sqlite3_reset(m_pStmt);
        if (nStatus)
        {
            db_sqlite3::set_last_error(std::string("Failed to reset statement: ") +
                                       string_utils::to_string(nStatus));
            return false;
        }
    }
    return true;
}

int db_sqlite3_stmt_binder::bind(int v_nCol, const void* v_pVal, int v_nLen, void(*v_xDel)(void*))
{
    return sqlite3_bind_blob(m_pStmt, v_nCol, v_pVal, v_nLen, v_xDel);
}
int db_sqlite3_stmt_binder::bind(int v_nCol, int v_val)
{
    return sqlite3_bind_int(m_pStmt, v_nCol, v_val);
}
int db_sqlite3_stmt_binder::bind(int v_nCol, double v_val)
{
    return sqlite3_bind_double(m_pStmt, v_nCol, v_val);
}
int db_sqlite3_stmt_binder::bind(int v_nCol, const std::string& v_val)
{
    return sqlite3_bind_text(m_pStmt, v_nCol, v_val.c_str(), -1, SQLITE_TRANSIENT);
}
int db_sqlite3_stmt_binder::bind(int v_nCol, const std::wstring& v_val)
{
    return sqlite3_bind_text16(m_pStmt, v_nCol, v_val.c_str(), -1, SQLITE_TRANSIENT);
}