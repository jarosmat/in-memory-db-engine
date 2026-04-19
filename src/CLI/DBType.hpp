#ifndef PROJECT_DBTYPE_HPP
#define PROJECT_DBTYPE_HPP
#include <chrono>

enum class DBTypes {
    INTEGER,
    FLOAT,
    STRING,
    OBJECT,
    VECTOR_INT,
    VECTOR_FLOAT,
    VECTOR_STRING,
    INVALID
};

enum class StartupTokens {
    VALUE,
    TTL
};

using view_t = std::ranges::subrange<std::vector<std::string>::iterator>;

using ttl_t = std::variant<std::chrono::milliseconds, std::chrono::seconds, std::chrono::minutes>;

auto GetDBType(view_t &args) -> std::pair<DBTypes, ttl_t>;

auto GetTTL(view_t &args) -> ttl_t;

auto GetType(view_t &view) -> DBTypes;


#endif //PROJECT_DBTYPE_HPP
