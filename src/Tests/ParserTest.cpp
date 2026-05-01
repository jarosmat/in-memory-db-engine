#include <iostream>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "OperationType.hpp"
#include <fstream>
#include <filesystem>

#include "catch2/catch_approx.hpp"

TEST_CASE("String Parsing", "[String]") {
    SECTION("Valid Strings") {
        auto [ok1, val1] = ParseString("\'123\'");
        CHECK(ok1 == true);
        CHECK(val1 == String("123"));

        auto [ok2, val2] = ParseString(R"(''-456')");
        CHECK(ok2 == true);
        CHECK(val2 == String("\'-456"));

        auto [ok3, val3] = ParseString(R"('-456'')");
        CHECK(ok3 == true);
        CHECK(val3 == String("-456\'"));

        auto [ok4, val4] = ParseString("\'\'");
        CHECK(ok4 == true);
        CHECK(val4 == String(""));
    }

    SECTION("Invalid strings") {
        CHECK(ParseString("").first == false);
        CHECK(ParseString("\'").first == false);
        CHECK(ParseString("abs\'").first == false);
        CHECK(ParseString("\'12 3").first == false);
    }
}

TEST_CASE("Integer Parsing", "[Integer]") {
    SECTION("Valid integers") {
        auto [ok1, val1] = ParseInt("123");
        CHECK(ok1 == true);
        CHECK(val1 == 123);

        auto [ok2, val2] = ParseInt("-456");
        CHECK(ok2 == true);
        CHECK(val2 == -456);

        auto [ok3, val3] = ParseInt("0");
        CHECK(ok3 == true);
        CHECK(val3 == 0);
    }

    SECTION("Invalid integers") {
        CHECK(ParseInt("12a3").first == false);
        CHECK(ParseInt("abc").first == false);
        CHECK(ParseInt("").first == false);
        CHECK(ParseInt("12 3").first == false);
    }
}

TEST_CASE("Double Parsing", "[Float]") {
    SECTION("Valid floats") {
        auto [ok1, val1] = ParseFloat("3.1415");
        CHECK(ok1 == true);
        CHECK_THAT(val1, Catch::Matchers::WithinRel(3.1415));

        auto [ok2, val2] = ParseFloat("-0.001");
        CHECK(ok2 == true);
        CHECK_THAT(val2, Catch::Matchers::WithinRel(-0.001));

TEST_CASE("Parse Int Node") {
    DBType<Integer> db(SipHash(0, 0, 2, 4));

    SECTION("INVALID COUNT") {
        std::vector<std::string> val = {"1"};
        view_t view = view_t(val.begin(), val.end());
        auto [res, node] = ParseIntNode<DBType<Integer>, AddOp<Integer> >(view, DBTypes::INTEGER, db);
        REQUIRE(res == ParseOperationRes::INVALID_ARG_C) ;
    }
    SECTION("INVALID COUNT") {
        std::vector<std::string> val = {"1", "a"};
        view_t view = view_t(val.begin(), val.end());
        auto [res, node] = ParseIntNode<DBType<Integer>, AddOp<Integer> >(view, DBTypes::INTEGER, db);
        REQUIRE(res == ParseOperationRes::INVALID_ARGS) ;
    }
    SECTION("VALID ADD") {
        std::vector<std::string> val = {"1", "1"};
        view_t view = view_t(val.begin(), val.end());
        auto [res, node] = ParseIntNode<DBType<Integer>, AddOp<Integer> >(view, DBTypes::INTEGER, db);
        REQUIRE(res == ParseOperationRes::VALID) ;
        REQUIRE(node.get()->Execute() == 2);
    }
    SECTION("VALID SUBTRACT") {
        std::vector<std::string> val = {"1", "1"};
        view_t view = view_t(val.begin(), val.end());
        auto [res, node] = ParseIntNode<DBType<Integer>, SubtractOp<Integer> >(view, DBTypes::INTEGER, db);
        REQUIRE(res == ParseOperationRes::VALID) ;
        REQUIRE(node.get()->Execute() == 0);
    }
    SECTION("VALID MULTIPLY") {
        std::vector<std::string> val = {"1", "1"};
        view_t view = view_t(val.begin(), val.end());
        auto [res, node] = ParseIntNode<DBType<Integer>, MultiplyOp<Integer> >(view, DBTypes::INTEGER, db);
        REQUIRE(res == ParseOperationRes::VALID) ;
        REQUIRE(node.get()->Execute() == 1);
    }
    SECTION("VALID DIVIDE") {
        std::vector<std::string> val = {"1", "1"};
        view_t view = view_t(val.begin(), val.end());
        auto [res, node] = ParseIntNode<DBType<Integer>, DivideOp<Integer> >(view, DBTypes::INTEGER, db);
        REQUIRE(res == ParseOperationRes::VALID) ;
        REQUIRE(node.get()->Execute() == 1);
    }
}