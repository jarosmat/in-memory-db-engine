#define CATCH_CONFIG_MAIN
#include  <catch2/catch_test_macros.hpp>
#include "DBEntry.hpp"


using namespace std;
using namespace std::chrono;

template<typename T>
auto GetTTL(std::size_t val) -> T {
    return T(val);
}

void InitVec(Vector<Integer> &vec, const Integer size) {
    for (Integer i = 0; i < size; ++i) {
        vec.Append(i);
    }
}

bool CompareVec(const Vector<Integer> &left, const Vector<Integer> &right) {
    if (left.Count() != right.Count()) {
        return false;
    }
    for (size_t i = 0; i < left.Count(); ++i) {
        if (left[i] != right[i]) {
            return false;
        }
    }
    return true;
}

using namespace std::chrono;

TEST_CASE("DBEntry_t: Basic Construction and Value Access with std::chrono::duration TTL", "[dbentry_t]") {
    SECTION("Construct with int payload and seconds TTL") {
        constexpr Integer payload = 42;
        DBEntry_t<Integer, seconds> entry(payload, GetTTL<seconds>(10));
        REQUIRE(entry.GetRef() == payload);
    }

    SECTION("Construct with string payload and milliseconds TTL") {
        auto val = string("hello");
        DBEntry_t<String, milliseconds> entry(String(val), GetTTL<seconds>(500));
        REQUIRE(entry.GetRef() == String(val));
    }

    SECTION("Construct with vector payload and seconds TTL") {
        Vector<Integer> vec;
        constexpr Integer size = 6;
        InitVec(vec, size);
        DBEntry_t<Vector<Integer>, seconds> entry(vec, GetTTL<seconds>(5));
        REQUIRE(CompareVec(vec, entry.GetRef()));
    }

    SECTION("SetVal with rvalue") {
        DBEntry_t<Integer, seconds> entry(0, GetTTL<seconds>(10));
        entry.SetVal(123);
        REQUIRE(entry.GetRef() == 123);
    }

    SECTION("SetVal with lvalue") {
        DBEntry_t<Integer, seconds> entry(0, GetTTL<seconds>(10));
        Integer val = 456;
        entry.SetVal(val);
        REQUIRE(entry.GetRef() == 456);
    }
}

TEST_CASE("DBEntry_t: Expiration with std::chrono::duration TTL", "[dbentry_t][expiration]") {
    SECTION("Expired with zero TTL") {
        const DBEntry_t<Integer, seconds> entry(42, GetTTL<seconds>(0));
        REQUIRE(entry.Expired());
    }

    SECTION("Not expired with large TTL") {
        const DBEntry_t<Integer, seconds> entry(42, GetTTL<seconds>(1000000));
        REQUIRE_FALSE(entry.Expired());
    }
}

TEST_CASE("DBEntry: Type Erasure and Value Access with std::chrono::duration TTL", "[dbentry]") {
    SECTION("Construct with Integer payload and seconds TTL") {
        constexpr Integer val = 42;
        DBEntry_t<Integer, seconds> entry_t(val, GetTTL<seconds>(10));
        DBEntry<Integer> entry(entry_t);
        REQUIRE(entry.GetRef() == val);
    }

    SECTION("Construct with string payload and milliseconds TTL") {
        auto val = string("hello");
        DBEntry_t<String, milliseconds> entry_t(String(val), GetTTL<seconds>(500));
        DBEntry<String> entry(entry_t);
        REQUIRE(entry.GetRef() == String(val));
    }

    SECTION("Construct with vector payload and seconds TTL") {
        constexpr size_t size = 6;
        Vector<Integer> vec;
        InitVec(vec, size);
        DBEntry_t<Vector<Integer>, seconds> entry_t(vec, GetTTL<seconds>(5));
        DBEntry<Vector<Integer> > entry(entry_t);
        REQUIRE(CompareVec(vec, entry.GetRef()));
    }

    SECTION("SetVal with rvalue") {
        constexpr size_t size = 6;
        Vector<Integer> vec;
        InitVec(vec, size);
        DBEntry_t<Vector<Integer>, seconds> entry_t(Vector<Integer>(), GetTTL<seconds>(10));
        DBEntry<Vector<Integer> > entry(entry_t);
        entry.SetVal(std::move(vec));
        Vector<Integer> vecNew;
        InitVec(vecNew, size);
        REQUIRE(CompareVec(vecNew, entry.GetRef()));
    }

    SECTION("SetVal with lvalue") {
        DBEntry_t<Integer, seconds> entry_t(0, GetTTL<seconds>(10));
        DBEntry<Integer> entry(entry_t);
        Integer val = 456;
        entry.SetVal(val);
        REQUIRE(entry.GetRef() == val);
    }
}

TEST_CASE("DBEntry: Expiration with std::chrono::duration TTL", "[dbentry][expiration]") {
    SECTION("Expired with zero TTL") {
        const DBEntry_t<Integer, seconds> entry_t(42, GetTTL<seconds>(0));
        DBEntry<Integer> entry(entry_t);
        REQUIRE(entry.Expired());
    }

    SECTION("Not expired with large TTL") {
        const DBEntry_t<Integer, seconds> entry_t(42, GetTTL<seconds>(1000000));
        DBEntry<Integer> entry(entry_t);
        REQUIRE_FALSE(entry.Expired());
    }
}

TEST_CASE("DBEntry: Move Semantics with std::chrono::duration TTL", "[dbentry][move]") {
    SECTION("Construct with rvalue") {
        constexpr size_t size = 6;
        Vector<Integer> vec;
        InitVec(vec, size);
        DBEntry_t<Vector<Integer>, seconds> entry_t(vec, GetTTL<seconds>(10));
        DBEntry<Vector<Integer> > entry(std::move(entry_t));
        REQUIRE(CompareVec(vec, entry.GetRef()));
    }
}
