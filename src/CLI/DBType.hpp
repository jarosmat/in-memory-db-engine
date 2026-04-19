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

enum class TTLTypes {
    MILLISECONDS,
    SECONDS,
    MINUTES,
    INVALID
};

using view_t = std::ranges::subrange<std::vector<std::string>::iterator>;

using variant_t = std::variant<std::chrono::milliseconds, std::chrono::seconds, std::chrono::minutes>;
using ttl_t = std::pair<variant_t, TTLTypes>;

auto GetDBType(view_t &args) -> std::pair<DBTypes, ttl_t>;

auto GetTTL(view_t &args) -> ttl_t;

auto GetType(view_t &view) -> DBTypes;

auto ParseSignedTTL(std::string &val) -> ttl_t;

auto ParseUnsignedTTL(std::string &val) -> ttl_t;


#endif //PROJECT_DBTYPE_HPP
