#include "../../src/utils/db/sqlite3/db_sqlite3.h"
#include <iostream>

int main()
{
    db_sqlite3 db;
    db.connect(L"test.db");
    db.execute(L"CREATE TABLE IF NOT EXISTS test (id INTEGER PRIMARY KEY, name TEXT, age INTEGER)");
    db.execute(L"INSERT INTO test (name, age) VALUES ('Tom', 25)");
    db.execute(L"INSERT INTO test (name, age) VALUES ('Jerry', 30)");

    db_sqlite3::result_ptr result = db.query(L"SELECT * FROM test");
    if (!result) {
        std::cout << "query failed" << std::endl;
        return 1;
    }
    while (result->next())
    {
        int id = result->get_int(result->get_column_index(L"id"));
        std::wstring wname = result->get_wstring(result->get_column_index(L"name"));
        int age = result->get_int(result->get_column_index(L"age"));
        std::wcout << L"id: " << id << L", name: " << wname << L", age: " << age << std::endl;
    }

}