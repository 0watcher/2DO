#pragma once

#include <SQLiteCpp/SQLiteCpp.h>
#include "SQLiteCpp/Database.h"
#include "SQLiteCpp/Statement.h"
#include "Utils/result.hpp"
#include "Utils/type.hpp"

namespace SQL = SQLite;

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
    virtual ~Database() = 0;

    virtual Result<ObjectT, DbError> get_object_by_unique_column(
        const String& column_value) const noexcept = 0;

    virtual Result<ObjectT, DbError> get_object_by_id(
        int id) const noexcept = 0;
    virtual Result<Vector<ObjectT>, DbError> get_all_objects()
        const noexcept = 0;
    virtual bool is_table_empty() const noexcept = 0;

    virtual Result<void, DbError> add_object(ObjectT& obj) noexcept = 0;
    virtual Result<void, DbError> update_object(
        const ObjectT& obj) const noexcept = 0;
    virtual Result<void, DbError> delete_object(
        const ObjectT& obj) const noexcept = 0;

  protected:
    SQL::Database m_db;
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