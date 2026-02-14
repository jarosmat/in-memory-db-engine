#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include "Group/Group.hpp"
#include <algorithm>

using namespace std;

TEST_CASE("Initial state is empty") {
    constexpr size_t start = 0;
    constexpr size_t end = 8;
    constexpr size_t size = end - start;
    Group g(start, end);

    auto [status, positions] = g.LookUp(0x12);

    CHECK(status == LookupRes::EMPTY);
    CHECK(positions.size() == size);

    for (size_t i = 0; i < size; ++i) {
        CHECK(find(positions.begin(), positions.end(), i) != positions.end());
    }
}

TEST_CASE("Adding and finding values") {
    constexpr size_t start = 0;
    constexpr size_t end = 8;
    constexpr size_t size = end - start;
    Group g(start, end);

    hash2Type h1 = 0x01;
    hash2Type h2 = 0x02;

    g.SetVal(h1, 0);
    g.SetVal(h2, 1);
    g.SetVal(h1, 2);

    SECTION("LookUp finds single entry") {
        auto [status, positions] = g.LookUp(h2);
        CHECK(status == LookupRes::USED);
        CHECK(positions == posArr{1});
    }

    SECTION("LookUp finds multiple entries for same hash") {
        auto [status, positions] = g.LookUp(h1);
        CHECK(status == LookupRes::USED);
        CHECK(positions.size() == 2);
        CHECK(find(positions.begin(), positions.end(), 0) != positions.end());
        CHECK(find(positions.begin(), positions.end(), 2) != positions.end());
    }

    SECTION("LookUp returns EMPTY slots when hash is not present") {
        auto [status, positions] = g.LookUp(0x7F);
        CHECK(status == LookupRes::EMPTY);

        CHECK(positions.size() == 5);
        for (size_t i = 3; i < size; i++) {
            CHECK(find(positions.begin(), positions.end(), i) != positions.end());
        }
    }
}


TEST_CASE("Adding and finding values second group") {
    constexpr size_t start = 8;
    constexpr size_t end = 16;
    constexpr size_t size = end - start;
    Group g(start, end);

    hash2Type h1 = 0x01;
    hash2Type h2 = 0x02;

    g.SetVal(h1, 8);
    g.SetVal(h2, 9);
    g.SetVal(h1, 10);

    SECTION("LookUp finds single entry") {
        auto [status, positions] = g.LookUp(h2);
        CHECK(status == LookupRes::USED);
        CHECK(positions == posArr{9});
    }

    SECTION("LookUp finds multiple entries for same hash") {
        auto [status, positions] = g.LookUp(h1);
        CHECK(status == LookupRes::USED);
        CHECK(positions.size() == 2);
        CHECK(find(positions.begin(), positions.end(), 8) != positions.end());
        CHECK(find(positions.begin(), positions.end(), 10) != positions.end());
    }

    SECTION("LookUp returns EMPTY slots when hash is not present") {
        auto [status, positions] = g.LookUp(0x7F);
        CHECK(status == LookupRes::EMPTY);

        CHECK(positions.size() == 5);
        for (size_t i = 3 + 8; i < size + 8; i++) {
            CHECK(find(positions.begin(), positions.end(), i) != positions.end());
        }
    }
}


TEST_CASE("Deletion and Tombstones") {
    constexpr size_t start = 0;
    constexpr size_t end = 8;
    constexpr size_t size = end - start;
    Group g(start, end);

    hash2Type h1 = 0x05;
    size_t testPos = 3;
    g.SetVal(h1, testPos);

    SECTION("Find the value") {
        auto [status, positions] = g.LookUp(h1);
        CHECK(status == LookupRes::USED);
        CHECK(positions.size() == 1);
        CHECK(positions[0] == testPos);
    }

    g.Delete(testPos);

    SECTION("Deleted slots are not returned as USED") {
        auto [status, positions] = g.LookUp(h1);
        CHECK(status == LookupRes::EMPTY);
        CHECK(positions.size() == size - 1);
        for (unsigned long i: positions) {
            CHECK(i != testPos);
        }
    }
}


TEST_CASE("Full Group behavior") {
    constexpr size_t start = 0;
    constexpr size_t end = 8;
    constexpr size_t size = end - start;
    Group g(start, end);

    for (size_t i = 0; i < size; ++i) {
        g.SetVal(static_cast<hash2Type>(i), i);
    }

    SECTION("LookUp returns FULL when no match and no EMPTY slots") {
        auto [status, positions] = g.LookUp(0x7F);
        CHECK(status == LookupRes::FULL);
        CHECK(positions.empty());
    }
}
