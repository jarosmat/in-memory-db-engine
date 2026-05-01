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

        auto [ok3, val3] = ParseFloat("2e3");
        CHECK(ok3 == true);
        CHECK(val3 == 2000.0);
    }

    SECTION("Invalid floats") {
        CHECK(ParseFloat("3.14.15").first == false);
        CHECK(ParseFloat("nan-ish").first == false);
    }
}

TEST_CASE("Vector Parsing Logic", "[vector]") {
    SECTION("Successful integer vector parsing") {
        std::string_view input = "{ 1, 2,  3 , 4 }";
        auto [result, vect] = ParseVect<Integer>(input, ParseInt);

        REQUIRE(result == VectorParsingRes::VALID);
        CHECK(vect.Count() == 4);
        CHECK(vect[0] == 1);
        CHECK(vect[3] == 4);
    }

    SECTION("Successful double vector parsing") {
        std::string_view input = "{1.1, 2.2, 3.3}";
        auto [result , vect] = ParseVect<Float>(input, ParseFloat);

        REQUIRE(result == VectorParsingRes::VALID);
        CHECK(vect.Count() == 3);
        CHECK_THAT(vect[1], Catch::Matchers::WithinRel(2.2));
    }

    SECTION("Empty and minimal vectors") {
        CHECK(ParseVect<Integer>("{}", ParseInt).second.Count() == 0);
        CHECK(ParseVect<Integer>("{   }", ParseInt).second.Count() == 0);

        auto [result,vect] = ParseVect<Integer>("{42}", ParseInt);
        REQUIRE(result == VectorParsingRes::VALID);
        CHECK(vect[0] == 42);
    }

    SECTION("Malformed structures") {
        CHECK(ParseVect<Integer>("1, 2, 3", ParseInt).first == VectorParsingRes::INVALID_VECT);
        CHECK(ParseVect<Integer>("{1, abc, 3}", ParseInt).first == VectorParsingRes::INVALID_ITEM);
        auto trailing = ParseVect<Integer>("{1, 2, }", ParseInt);
        CHECK(trailing.first == VectorParsingRes::INVALID_VECT);
        auto first = ParseVect<Integer>("{ , 2, 3 }", ParseInt);
        CHECK(first.first == VectorParsingRes::INVALID_VECT);
        auto middle = ParseVect<Integer>("{1, , 3}", ParseInt);
        CHECK(middle.first == VectorParsingRes::INVALID_VECT);
    }
}

namespace fs = std::filesystem;

void createTestFile(const std::string &path, const std::string &content) {
    std::ofstream ofs(path, std::ios::binary);
    ofs << content;
    ofs.close();
}

TEST_CASE("stringToBytes converts string to byte vector", "[string]") {
    SECTION("Standard string conversion") {
        std::string input = "Hello";
        auto result = stringBytes(std::move(input));

        REQUIRE(result.size() == 5);
        CHECK(result[0] == 'H');
        CHECK(result[4] == 'o');
    }

    SECTION("Empty string conversion") {
        std::string input;
        auto result = stringBytes(std::move(input));
        CHECK(result.empty());
    }
}


TEST_CASE("readFileToBytes handles various file scenarios", "[file]") {
    std::string testPath = "test_data.bin";

    SECTION("Read an existing file correctly") {
        std::string expectedContent = "BinaryData\0\1\2";
        createTestFile(testPath, expectedContent);

        auto [type, result] = readFileBytes(testPath);

        REQUIRE(type == BytesFromFileRes::VALID);
        REQUIRE(result.size() == expectedContent.size());
        CHECK(std::memcmp(result.data(), expectedContent.data(), result.size()) == 0);

        fs::remove(testPath);
    }

    SECTION("Handle non-existent file") {
        auto [type,result] = readFileBytes("this_file_does_not_exist.xyz");
        REQUIRE(type == BytesFromFileRes::COULD_NOT_OPEN);
        CHECK(result.empty());
    }

    SECTION("Handle empty file") {
        createTestFile(testPath, "");
        auto [type, result] = readFileBytes(testPath);

        REQUIRE(type == BytesFromFileRes::VALID);
        CHECK(result.empty());

        fs::remove(testPath);
    }
}

using MockDBString = SwissTable<SipHash, DBEntry<String>, std::vector<uint8_t>>;
using MockDBInteger = SwissTable<SipHash, DBEntry<Integer>, std::vector<uint8_t>>;
using MockDBFloat = SwissTable<SipHash, DBEntry<Float>, std::vector<uint8_t>>;
template<typename T>
using MockDBVect = SwissTable<SipHash, DBEntry<Vector<T>>, std::vector<uint8_t>>;



TEST_CASE("Operation Type Parsing", "[OperationType]") {
    SECTION("GetOPType correctly identifies operations") {
        std::vector<std::string> rawArgs = {"INSERT", "GET", "INVALID_OP"};
        view_t view(rawArgs.begin(), rawArgs.end());


        CHECK(GetOPType(view) == OPType::INSERT);

        CHECK(GetOPType(view) == OPType::GET);

        CHECK(GetOPType(view) == OPType::INVALID);
    }

    SECTION("ParseArg identifies argument types and values") {
        std::vector<std::string> rawArgs = {"hello", "@data.bin", "$mykey"};
        view_t view(rawArgs.begin(), rawArgs.end());

        auto [type1, val1] = ParseArg(view);
        CHECK(type1 == ArgsType::CONSTANT);
        CHECK(val1 == "hello");

        auto [type2, val2] = ParseArg(view);
        CHECK(type2 == ArgsType::KEY_FROM_FILE);
        CHECK(val2 == "data.bin");

        auto [type3, val3] = ParseArg(view);
        CHECK(type3 == ArgsType::KEY_FROM_CONSTANT);
        CHECK(val3 == "mykey");
    }
}

TEST_CASE("Parse Key") {
    SECTION("Invalid key") {
        std::vector<std::string> val = {};
        auto view = view_t(val.begin(), val.end());
        auto [res, valKey] = ParseKey(view);
        REQUIRE(res == KeyType::INVALID);
    }

    SECTION("Constant key") {
        std::vector<std::string> val = {"key"};
        auto view = view_t(val.begin(), val.end());
        auto [res, valKey] = ParseKey(view);
        REQUIRE(res == KeyType::CONSTANT);
        REQUIRE(valKey == "key");
    }

    SECTION("File path") {
        std::vector<std::string> val = {"@key"};
        auto view = view_t(val.begin(), val.end());
        auto [res, valKey] = ParseKey(view);
        REQUIRE(res == KeyType::FILE_PATH);
        REQUIRE(valKey == "key");
    }
}