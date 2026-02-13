#include <catch2/catch_test_macros.hpp>
#include "Vector.hpp"
#include "../DBEntry/String.hpp"

using namespace std;
TEST_CASE("Accessing Value") {
    String initialVal("xd");
    constexpr size_t size = 10;
    const Vector<String> testV(size, initialVal);

    REQUIRE(size == testV.Count());
    for (size_t i = 0; i < testV.Count(); i++) {
        REQUIRE(initialVal == testV[i]);
    }
}

TEST_CASE("Appending Values") {
    constexpr size_t startVal = 100;
    constexpr size_t addedEntries = 3000;
    Vector<size_t> val;

    for (size_t i = startVal; i < startVal + addedEntries; i++) {
        val.Append(i);
    }
    REQUIRE(val.Count() == addedEntries);
    for (size_t i = 0; i < addedEntries; i++) {
        REQUIRE(val[i] == i + startVal);
    }

    for (size_t i = val.Count() - 1; 0 < i; i--) {
        REQUIRE(val.CLastEntry() == startVal + i);
        val.Remove(i);
    }
}






TEST_CASE("Vector Initialization") {
    SECTION("Default constructor starts with size 0") {
        Vector<int> v;
        REQUIRE(v.Count() == 0);
    }

    SECTION("Constructor with size initializes capacity") {
        Vector<int> v(10);
        REQUIRE(v.Count() == 0);
    }

    SECTION("Constructor with initial value") {
        int val = 42;
        constexpr size_t size = 5;
        Vector<int> v(size, val);
        REQUIRE(v.Count() == 5);
        for (size_t i = 0; i < size; i++ ) {
            REQUIRE(v[i] == val);
        }
    }
}

TEST_CASE("Adding and Accessing Elements") {
    Vector<string> v;

    SECTION("Append lvalue") {
        string s = "hello";
        v.Append(s);
        REQUIRE(v.Count() == 1);
        REQUIRE(v[0] == "hello");
    }

    SECTION("Append rvalue") {
        v.Append(std::move(string("world")));
        REQUIRE(v.Count() == 1);
        REQUIRE(v[0] == "world");
    }

    SECTION("LastEntry and CLastEntry") {
        v.Append("first");
        v.Append("last");
        REQUIRE(v.LastEntry() == "last");

        v.LastEntry() = "new_last";
        REQUIRE(v.CLastEntry() == "new_last");
    }
}

TEST_CASE("Resizing Logic") {
    Vector<size_t> v;

    SECTION("Vector expands correctly beyond initialSize (4)") {
        constexpr size_t count = 10;
        for (size_t i = 0; i < count; ++i) {
            v.Append(i);
        }
        REQUIRE(v.Count() == 10);
        for (size_t i = 0; i <count;i++) {
            REQUIRE(v[i] == i);
        }
    }
}

TEST_CASE("Removing Elements") {
    Vector<int> v;
    v.Append(10);
    v.Append(20);
    v.Append(30);

    SECTION("Remove middle element") {
        v.Remove(1);
        REQUIRE(v.Count() == 2);
        REQUIRE(v[0] == 10);
        REQUIRE(v[1] == 30);
    }

    SECTION("Remove last element") {
        v.Remove(2);
        REQUIRE(v.Count() == 2);
        REQUIRE(v.LastEntry() == 20);
    }
}

TEST_CASE("Const Correctness", "[Vector]") {
    Vector<int> v;
    v.Append(100);

    const Vector<int>& const_v = v;

    SECTION("Const access via operator[]") {
        REQUIRE(const_v[0] == 100);
    }

    SECTION("Const access via CLastEntry") {
        REQUIRE(const_v.CLastEntry() == 100);
    }
}
