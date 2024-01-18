#pragma once

#include <SQLiteCpp/SQLiteCpp.h>
#include "SQLiteCpp/Database.h"
#include "SQLiteCpp/Statement.h"
#include "Utils/result.hpp"
#include "Utils/type.hpp"

namespace twodoutils {
enum class DbError {
    SelectFailure = 1,
    InsertFailure,
    UpdateFailure,
    DeleteFailure
};

template <typename ObjectT>
class Database {
  public:
    Database();
    ~Database();

    virtual Result<ObjectT, DbError> get_object_by_column_name(
        const String& column_name,
        const String& column_value) const noexcept;

    virtual Result<ObjectT, DbError> get_object_by_id(int id) const noexcept;
    virtual Result<Vector<ObjectT>, DbError> get_all_objects(
        const String& table_name) const noexcept;

    virtual Result<void, DbError> add_object(ObjectT& obj) noexcept;
    virtual Result<void, DbError> update_object(
        const ObjectT& obj) const noexcept;
    virtual Result<void, DbError> delete_object(
        const ObjectT& obj) const noexcept;

  protected:
    SQLite::Database m_db;
};
};  // namespace twodoutils

#ifdef SQLITECPP_ENABLE_ASSERT_HANDLER
namespace SQLite {
void assertion_failed(const char* apFile,
                      const long apLine,
                      const char* apFunc,
                      const char* apExpr,
                      const char* apMsg) {
    std::cerr << apFile << ":" << apLine << ":"
              << " error: assertion failed (" << apExpr << ") in " << apFunc
              << "() with message \"" << apMsg << "\"\n";
    std::abort();
}
}  // namespace SQLite
#endif