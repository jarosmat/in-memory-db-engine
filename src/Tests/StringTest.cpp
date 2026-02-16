#include <catch2/catch_test_macros.hpp>
#include "String.hpp"

using namespace std;
TEST_CASE("Accessing Value") {
    const string testVal = "Hello World";

    const String string(testVal);

    for (size_t i = 0; i < testVal.size(); i++) {
        REQUIRE(string[i] == testVal[i]);
    }
}

TEST_CASE("Concatenating Value") {
    string leftPart = "Hello ";
    string rightPart = "World";
    const string resultVal = "Hello World";

    REQUIRE(String::Concatenate(leftPart, rightPart) == String(resultVal));

    REQUIRE(String::Concatenate(std::move(leftPart), std::move(rightPart)) == String(resultVal));
}

TEST_CASE("Remove Prefix") {
    const string value = "Hello World";

    SECTION("Normal situation") {
        const size_t n = value.size() / 2;
        String val(value);
        const string valCorrect = value.substr(n);

        val.RemovePrefix(n);

        REQUIRE(val == String(valCorrect));
    }

    SECTION("Remove more that len of string") {
        const size_t n = value.size() * 2;
        String val(value);
        const string valCorrect = "";

        val.RemovePrefix(n);

        REQUIRE(val == String(valCorrect));

    }
}

TEST_CASE("Remove Suffix") {
    const string value = "Hello World";

    SECTION("Normal situation") {
        const size_t n = value.size() / 2;
        String val(value);
        const string valCorrect = value.substr(0, value.size() - n);

        val.RemoveSuffix(n);
        REQUIRE(val == String(valCorrect));
    }

    SECTION("Remove more that len of string") {
        const size_t n = value.size() * 2;
        String val(value);
        const string valCorrect = "";

        val.RemoveSuffix(n);

        REQUIRE(val == String(valCorrect));
    }
}

using fRes = set<size_t>;
void testCase(String&& val, const String& text, const fRes& res) {
    REQUIRE(res == text.Find(val));
}

TEST_CASE("Find substring") {
    String text("");
    String val("");

    SECTION("Simple One") {
        text = String("dashiki q woken");
        val = String("iki q wo");

        const fRes res{4};
        testCase(std::move(val), text, res);
    }

    SECTION("Repetition") {
        text = String("aaaaaaaaaa");
        val = String("a");

        const fRes res{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        testCase(std::move(val),text, res);
    }

    SECTION("Finding self") {
        val = String("Hello World");
        text = String(val);
        const fRes res{0};

        testCase(std::move(val), text, res);
    }

    SECTION("Found none") {
        text = String("djweqowencqrubq 213 3o4j 3323 o3123");
        val = String("xDDDD");
        const fRes res{};

        testCase(std::move(val), text, res);
    }
}
