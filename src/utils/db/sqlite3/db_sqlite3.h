#ifndef DB_SQLITE3_H
#define DB_SQLITE3_H

#include <string>
#include <BaseTsd.h>

struct sqlite3;
class sqlite3_stmt;
class db_sqlite3_stmt_binder;
class db_sqlite3_query;

class db_sqlite3
{
    friend class db_sqlite3_stmt_binder;

public:
    db_sqlite3();
    virtual ~db_sqlite3();

public:
    bool connect(const std::string& v_strDBName);
    bool connect(const std::wstring& v_wstrDBName);
    bool disconnect();
    bool connected() const;

    bool execute(const std::string& v_strSql);
    bool execute(const char* v_pszSql, ...);
    bool execute(const std::wstring& v_wstrSql);
    bool execute(const wchar_t* v_pwszSql, ...);
    bool execute(const db_sqlite3_stmt_binder& v_binder);

    db_sqlite3_query query(const std::string& v_strSql);
    db_sqlite3_query query(const char* v_pszSql, ...);
    db_sqlite3_query query(const std::wstring& v_wstrSql);
    db_sqlite3_query query(const wchar_t* v_pwszSql, ...);
    db_sqlite3_query query(const db_sqlite3_stmt_binder& v_binder);

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

class db_sqlite3_query
{
public:
    db_sqlite3_query(sqlite3* v_pDb = NULL, sqlite3_stmt* v_pStmt = NULL);
    virtual ~db_sqlite3_query();

public:
    bool next();
    int get_column_count() const;
    int get_column_type(int v_nCol) const;
    const char* get_column_name(int v_nCol) const;
    const wchar_t* get_column_name_w(int v_nCol) const;
    int get_column_index(const std::string& v_strColumnName) const;
    int get_column_index(const std::wstring& v_wstrColumnName) const;

    const char* get_text(int v_nCol) const;
    const wchar_t* get_textw(int v_nCol) const;
    void get_string(int v_nCol, std::string& v_strVal) const;
    void get_string(int v_nCol, std::wstring& v_wstrVal) const;
    int get_int(int v_nCol) const;
    INT64 get_int64(int v_nCol) const;
    double get_double(int v_nCol) const;
    const void* get_blob(int v_nCol) const;

public:
    bool is_valid() const;

private:
    sqlite3* m_pDb;
    sqlite3_stmt* m_pStmt;
    bool m_bRelease;
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

    bool bind(int v_nCol, const void* v_pVal, int v_nLen, void (*v_xDel)(void*));
    bool bind(int v_nCol, int v_iVal);
    bool bind(int v_nCol, double v_dVal);
    bool bind(int v_nCol, const std::string& v_strVal);
    bool bind(int v_nCol, const std::wstring& v_wstrVal);

private:
    db_sqlite3* m_pDb;
    sqlite3_stmt* m_pStmt;
};

#endif // DB_SQLITE3_H
