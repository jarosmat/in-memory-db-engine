#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include <thread>
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

TEST_CASE("DBEntry_t: RemainingTime with std::chrono::milliseconds", "[dbentry_t][remaining][milliseconds]") {
    SECTION("Time remaining (not expired)") {
        DBEntry_t<Integer, milliseconds> entry(42, 100ms);
        auto remaining = entry.RemainingTime();
        REQUIRE(remaining >= 95ms);
        REQUIRE(remaining <= 100ms);
    }

    SECTION("Expired (zero remaining)") {
        DBEntry_t<Integer, milliseconds> entry(42, 100ms);
        std::this_thread::sleep_for(150ms);
        auto remaining = entry.RemainingTime();
        REQUIRE(remaining == 0ms);
    }
}

TEST_CASE("DBEntry_t: RemainingTime with std::chrono::seconds", "[dbentry_t][remaining][seconds]") {
    SECTION("Time remaining (not expired)") {
        DBEntry_t<Integer, seconds> entry(42, 2s);
        auto remaining = entry.RemainingTime();
        REQUIRE(remaining >= 1s);
        REQUIRE(remaining <= 2s);
    }

    SECTION("Expired (zero remaining)") {
        DBEntry_t<Integer, seconds> entry(42, 2s);
        std::this_thread::sleep_for(2100ms);
        auto remaining = entry.RemainingTime();
        REQUIRE(remaining == 0s);
    }
}

TEST_CASE("DBEntry_t: RemainingTime with std::chrono::minutes", "[dbentry_t][remaining][minutes]") {
    SECTION("Time remaining (not expired)") {
        DBEntry_t<Integer, minutes> entry(42, 1min);
        auto remaining = entry.RemainingTime();
        REQUIRE(remaining >= 59s);
        REQUIRE(remaining <= 60s);
    }

    SECTION("Expired (zero remaining)") {
        DBEntry_t<Integer, minutes> entry(42, 1min);
        std::this_thread::sleep_for(61s);
        auto remaining = entry.RemainingTime();
        REQUIRE(remaining == 0min);
    }
}


TEST_CASE("DBEntry: RemainingTime with std::chrono::milliseconds", "[dbentry][remaining][milliseconds]") {
    SECTION("Time remaining (not expired)") {
        const DBEntry_t<Integer, milliseconds> entry_t(42, 100ms);
        DBEntry<Integer> entry(entry_t);
        auto remaining = entry.RemainingTime();
        REQUIRE(remaining >= 95ms);
        REQUIRE(remaining <= 100ms);
    }

    SECTION("Expired (zero remaining)") {
        DBEntry_t<Integer, milliseconds> entry_t(42, 100ms);
        DBEntry<Integer> entry(entry_t);
        std::this_thread::sleep_for(150ms);
        auto remaining = entry.RemainingTime();
        REQUIRE(remaining == 0ms);
    }
}

TEST_CASE("DBEntry: RemainingTime with std::chrono::seconds", "[dbentry][remaining][seconds]") {
    SECTION("Time remaining (not expired)") {
        DBEntry_t<Integer, seconds> entry(42, 2s);
        auto remaining = entry.RemainingTime();
        REQUIRE(remaining >= 1s);
        REQUIRE(remaining <= 2s);
    }

    SECTION("Expired (zero remaining)") {
        DBEntry_t<Integer, seconds> entry(42, 2s);
        std::this_thread::sleep_for(2100ms);
        auto remaining = entry.RemainingTime();
        REQUIRE(remaining == 0s);
    }
}

TEST_CASE("DBEntry: RemainingTime with std::chrono::minutes", "[dbentry][remaining][minutes]") {
    SECTION("Time remaining (not expired)") {
        DBEntry_t<Integer, minutes> entry(42, 1min);
        auto remaining = entry.RemainingTime();
        REQUIRE(remaining >= 59s);
        REQUIRE(remaining <= 60s);
    }

    SECTION("Expired (zero remaining)") {
        DBEntry_t<Integer, minutes> entry(42, 1min);
        std::this_thread::sleep_for(61s);
        auto remaining = entry.RemainingTime();
        REQUIRE(remaining == 0min);
    }
}

TEST_CASE("Appending multiple types of DBEntry_t into one Vector") {
    Vector<DBEntry<Integer> > vec;
    const DBEntry_t<Integer, seconds> entry_t(0, GetTTL<seconds>(10));
    DBEntry<Integer> entry(entry_t);

    DBEntry_t<Integer, milliseconds> entry_milliseconds_t(0, GetTTL<seconds>(10));
    DBEntry<Integer> entry_milliseconds(entry_t);

    const DBEntry_t<Integer, ImmortalSentinel> entry_imm_t(0);
    DBEntry<Integer> entry_immortal(entry_imm_t);

    vec.Append(std::move(entry));
    vec.Append(std::move(entry_milliseconds));
    vec.Append(std::move(entry_immortal));

    REQUIRE(vec[0].GetRef() == 0);
    REQUIRE(vec[1].GetRef() == 0);
    REQUIRE(vec[2].GetRef() == 0);
}

TEST_CASE("Test immortal entry") {
    const DBEntry_t<Integer, ImmortalSentinel> entry_imm_t(0);
    DBEntry<Integer> entry_immortal(entry_imm_t);

    REQUIRE_FALSE(entry_immortal.Expired());
    REQUIRE(entry_immortal.RemainingTime() > 0ms);
    std::this_thread::sleep_for(1000ms);
    REQUIRE_FALSE(entry_immortal.Expired());
    REQUIRE(entry_immortal.RemainingTime() > 0ms);
}
