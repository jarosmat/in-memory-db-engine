#ifndef PROJECT_DBTYPE_HPP
#define PROJECT_DBTYPE_HPP
#include <chrono>
#include "DBEntry.hpp"
#include "SipHash.hpp"
#include "ValueTypes.hpp"
#include "Utils.hpp"

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

template<typename T>
struct get_db_type {
    static constexpr auto value = DBTypes::INVALID;
};

template<>
struct get_db_type<Integer> {
    static constexpr auto value = DBTypes::INTEGER;
};

template<>
struct get_db_type<Float> {
    static constexpr auto value = DBTypes::FLOAT;
};

template<>
struct get_db_type<String> {
    static constexpr auto value = DBTypes::STRING;
};

template<>
struct get_db_type<Object> {
    static constexpr auto value = DBTypes::OBJECT;
};

template<>
struct get_db_type<Vector<Integer> > {
    static constexpr auto value = DBTypes::VECTOR_INT;
};

template<>
struct get_db_type<Vector<Float> > {
    static constexpr auto value = DBTypes::VECTOR_FLOAT;
};

template<>
struct get_db_type<Vector<String> > {
    static constexpr auto value = DBTypes::VECTOR_STRING;
};

template<typename T>
struct get_db_type<SwissTable<SipHash, DBEntry<T>, std::vector<uint8_t> > > {
    static constexpr auto value = get_db_type<T>::value;
};

enum class StartupTokens {
    VALUE,
    TTL,
    STRUCT,
    INVALID
};

enum class TTLTypes {
    MILLISECONDS,
    SECONDS,
    MINUTES,
    IMMORTAL,
    INVALID
};

enum class ObjectFieldTypes {
    INTEGER,
    FLOAT,
    STRING,
    INVALID_TYPE,
    INVALID_FIELD,
    VALID
};

enum class SplitFieldRes {
    VALID,
    INVALID_PARENTHESIS,
    EMPTY_STRING,
};

enum class ObjectParseRes {
    VALID,
    INVALID_TYPE,
    INVALID_FIELD,
    INVALID_STRUCTURE,
};

using view_t = std::ranges::subrange<std::vector<std::string>::iterator>;

using variant_t = std::variant<std::chrono::milliseconds, std::chrono::seconds, std::chrono::minutes, ImmortalSentinel>;
using ttl_t = std::pair<variant_t, TTLTypes>;

using object_reference = std::unordered_map<std::string, ObjectFieldTypes>;

auto GetOptionToken(view_t &args) -> StartupTokens;

auto GetDBType(view_t &args) -> std::tuple<DBTypes, ttl_t, Object, object_reference>;

auto GetTTL(view_t &args) -> ttl_t;

auto GetType(view_t &view) -> DBTypes;

auto ParseSignedTTL(std::string &val) -> ttl_t;

auto ParseUnsignedTTL(std::string &val) -> ttl_t;

std::tuple<std::string, ObjectFieldTypes, bool> ParseFieldStartup(std::string_view view);

std::pair<SplitFieldRes, std::vector<std::string> > SplitToFields(std::string_view view);

std::tuple<ObjectParseRes, Object, object_reference> ParseObjectStructure(view_t &viewIn);


#endif //PROJECT_DBTYPE_HPP
