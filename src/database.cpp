#include "database.hpp"

#include <memory>

namespace twodo
{
    Result<sql_ptr> Database::create_or_open(const char* path) const
    {
        // auto del_sql3 = [](sqlite3* sql3_ptr)
        // {
        //     sqlite3_close_v2(sql3_ptr);
        // };

        // std::shared_ptr<sqlite3> db(nullptr, del_sql3);
        // auto db_ptr = db.get();

        // int rc = sqlite3_open("db.db", &db_ptr);
        // if(rc != SQLITE_OK)
        // {
        //     return Result<*sqlite3>{ .m_err = ErrorCode::panic };
        // }

        // return Result<*sqlite3>(db_ptr);

        // db.reset(db_ptr);

       // auto db = sql_ptr{};

        sql_ptr db{};

        int rc = sqlite3_open(path, db.get());
        if (rc != SQLITE_OK)
        {
            return Result<sql_ptr>{ .m_err = ErrorCode::panic };
        }

    }
}