#pragma once

#include <SQLiteCpp/SQLiteCpp.h>
#include "SQLiteCpp/Database.h"
#include "SQLiteCpp/Statement.h"
#include "Utils/result.hpp"
#include "Utils/type.hpp"

class DbError : public std::runtime_error {
public:
    DbError(const std::string& message)
        : std::runtime_error{message} {}

    const char* what() const noexcept override {
        return std::runtime_error::what();
    }
};