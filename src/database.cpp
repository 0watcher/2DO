#include "database.hpp"

#include <memory>

namespace twodo
{
    Result<sql3_ptr> Database::create_or_open(const char *path) const
    {
        sqlite3 *buffer = nullptr;
        int err = sqlite3_open(path, &buffer);
        if (err)
        {
            return Result<sql3_ptr>{.m_err = ErrorCode::panic};
        }

        return Result<sql3_ptr>(sql3_ptr(buffer));
    }
}