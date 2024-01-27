#pragma once

#include <SQLiteCpp/SQLiteCpp.h>
#include "SQLiteCpp/Database.h"
#include "SQLiteCpp/Statement.h"
#include "Utils/result.hpp"
#include "Utils/type.hpp"

namespace SQL = SQLite;

enum class DbError {
    SelectFailure = 1,
    InsertFailure,
    UpdateFailure,
    DeleteFailure
};

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