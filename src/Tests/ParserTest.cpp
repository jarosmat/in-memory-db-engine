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
        auto [fst, snd] = ParseVect<Integer>("{1, 2, }", ParseInt);
        CHECK(fst == VectorParsingRes::INVALID_VECT);
        auto [fst1, snd1] = ParseVect<Integer>("{ , 2, 3 }", ParseInt);
        CHECK(fst1 == VectorParsingRes::INVALID_VECT);
        auto [fst2, snd2] = ParseVect<Integer>("{1, , 3}", ParseInt);
        CHECK(fst2 == VectorParsingRes::INVALID_VECT);
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

using MockDBString = SwissTable<SipHash, DBEntry<String>, std::vector<uint8_t> >;
using MockDBInteger = SwissTable<SipHash, DBEntry<Integer>, std::vector<uint8_t> >;
using MockDBFloat = SwissTable<SipHash, DBEntry<Float>, std::vector<uint8_t> >;
template<typename T>
using MockDBVect = SwissTable<SipHash, DBEntry<Vector<T> >, std::vector<uint8_t> >;


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

TEST_CASE("Node Generation Functions", "[OperationType]") {
    MockDBString dbS(SipHash(0, 0, 2, 4));
    MockDBInteger dbI(SipHash(0, 0, 2, 4));
    MockDBFloat dbF(SipHash(0, 0, 2, 4));

    SECTION("getString generation") {
        SECTION("Constant string argument") {
            args_parsed args = {ArgsType::CONSTANT, "test_val"};
            auto [status, node] = getString(dbS, std::move(args));

            REQUIRE(status == GetArgsReturnTypes::VALID);
            REQUIRE(node != nullptr);
            CHECK(node->Execute() == String("test_val"));
        }

        SECTION("Key from constant (Database Lookup)") {
            args_parsed args = {ArgsType::KEY_FROM_CONSTANT, "db_key"};
            auto [status, node] = getString(dbS, std::move(args));
            CHECK(status == GetArgsReturnTypes::VALID);
            CHECK(node != nullptr);

            args_parsed args2 = {ArgsType::KEY_FROM_CONSTANT, "db_key"};
            auto [status2, node2] = getString(dbI, std::move(args2));
            CHECK(status2 == GetArgsReturnTypes::INVALID);
        }
    }

    SECTION("getInteger generation") {
        SECTION("Valid constant integer") {
            args_parsed args = {ArgsType::CONSTANT, "123"};
            auto [status, node] = getInteger(dbI, std::move(args));

            REQUIRE(status == GetArgsReturnTypes::VALID);
            CHECK(node->Execute() == 123);
        }

        SECTION("Empty argument failure") {
            args_parsed args = {ArgsType::EMPTY, ""};
            auto [status, node] = getInteger(dbI, std::move(args));
            CHECK(status == GetArgsReturnTypes::INVALID);
        }
    }

    SECTION("getFloat generation") {
        SECTION("Valid constant float") {
            args_parsed args = {ArgsType::CONSTANT, "3.14"};
            auto [status, node] = getFloat(dbF, std::move(args));

            REQUIRE(status == GetArgsReturnTypes::VALID);
            CHECK(node->Execute() == Catch::Approx(3.14));
        }

        SECTION("Invalid DB Type for Key lookup") {
            args_parsed args = {ArgsType::KEY_FROM_CONSTANT, "some_key"};
            auto [status, node] = getFloat(dbS, std::move(args));
            CHECK(status == GetArgsReturnTypes::INVALID);
        }
    }
}

TEST_CASE("Vector Node Generation", "[OperationType]") {
    MockDBVect<Integer> db(SipHash(0, 0, 2, 4));

    SECTION("getVector basic logic") {
        args_parsed args = {ArgsType::CONSTANT, "{1,2,3}"};

        if (auto [status, node] = getVector<Integer, MockDBVect<Integer> >(db, std::move(args), ParseInt);
            status == GetArgsReturnTypes::VALID) {
            SUCCEED("Vector node created");
        }
    }
}

TEST_CASE("parseLine handles space-delimited words", "[parseLine]") {
    SECTION("Simple space-delimited words") {
        auto [hasUnclosedQuotes, tokens] = parseLine("hello world");
        REQUIRE(hasUnclosedQuotes);
        REQUIRE(tokens == std::vector<std::string>{"hello", "world"});
    }

    SECTION("Multiple spaces between words") {
        auto [hasUnclosedQuotes, tokens] = parseLine("hello   world");
        REQUIRE(hasUnclosedQuotes);
        REQUIRE(tokens == std::vector<std::string>{"hello", "world"});
    }
}

TEST_CASE("parseLine handles quoted phrases", "[parseLine]") {
    SECTION("Single quoted phrase") {
        auto [hasUnclosedQuotes, tokens] = parseLine("\"hello world\"");
        REQUIRE(hasUnclosedQuotes);
        REQUIRE(tokens == std::vector<std::string>{"hello world"});
    }

    SECTION("Quoted phrase with surrounding words") {
        auto [hasUnclosedQuotes, tokens] = parseLine("hello \"world of tests\" 123");
        REQUIRE(hasUnclosedQuotes);
        REQUIRE(tokens == std::vector<std::string>{"hello", "world of tests", "123"});
    }

    SECTION("Multiple quoted phrases") {
        auto [hasUnclosedQuotes, tokens] = parseLine(R"("hello world" "another test")");
        REQUIRE(hasUnclosedQuotes);
        REQUIRE(tokens == std::vector<std::string>{"hello world", "another test"});
    }
}

TEST_CASE("parseLine handles mixed input", "[parseLine]") {
    SECTION("Mixed quoted and unquoted words") {
        auto [hasUnclosedQuotes, tokens] = parseLine(R"(hello "world of tests" 123 "another one")");
        REQUIRE(hasUnclosedQuotes);
        REQUIRE(tokens == std::vector<std::string>{"hello", "world of tests", "123", "another one"});
    }
}

TEST_CASE("parseLine handles edge cases", "[parseLine]") {
    SECTION("Empty string") {
        auto [hasUnclosedQuotes, tokens] = parseLine("");
        REQUIRE(hasUnclosedQuotes);
        REQUIRE(tokens.empty());
    }

    SECTION("Only spaces") {
        auto [hasUnclosedQuotes, tokens] = parseLine("   ");
        REQUIRE(hasUnclosedQuotes);
        REQUIRE(tokens.empty());
    }

    SECTION("Only quoted empty string") {
        auto [hasUnclosedQuotes, tokens] = parseLine("\"\"");
        REQUIRE(hasUnclosedQuotes);
        REQUIRE(tokens == std::vector<std::string>{""});
    }

    SECTION("Unclosed quote") {
        auto [hasUnclosedQuotes, tokens] = parseLine("\"hello world");
        REQUIRE_FALSE(hasUnclosedQuotes);
    }
}

TEST_CASE("Parse Int Node") {
    DBType<Integer> db(SipHash(0, 0, 2, 4));

    SECTION("INVALID COUNT") {
        std::vector<std::string> val = {"1"};
        auto view = view_t(val.begin(), val.end());
        auto [res, node] = ParseIntNode<DBType<Integer>, AddOp<Integer> >(view, db);
        REQUIRE(res == ParseOperationRes::INVALID_ARG_C) ;
    }
    SECTION("INVALID COUNT") {
        std::vector<std::string> val = {"1", "a"};
        auto view = view_t(val.begin(), val.end());
        auto [res, node] = ParseIntNode<DBType<Integer>, AddOp<Integer> >(view, db);
        REQUIRE(res == ParseOperationRes::INVALID_ARGS) ;
    }
    SECTION("VALID ADD") {
        std::vector<std::string> val = {"1", "1"};
        auto view = view_t(val.begin(), val.end());
        auto [res, node] = ParseIntNode<DBType<Integer>, AddOp<Integer> >(view, db);
        REQUIRE(res == ParseOperationRes::VALID) ;
        REQUIRE(node.get()->Execute() == 2);
    }
    SECTION("VALID SUBTRACT") {
        std::vector<std::string> val = {"1", "1"};
        auto view = view_t(val.begin(), val.end());
        auto [res, node] = ParseIntNode<DBType<Integer>, SubtractOp<Integer> >(view, db);
        REQUIRE(res == ParseOperationRes::VALID) ;
        REQUIRE(node.get()->Execute() == 0);
    }
    SECTION("VALID MULTIPLY") {
        std::vector<std::string> val = {"1", "1"};
        auto view = view_t(val.begin(), val.end());
        auto [res, node] = ParseIntNode<DBType<Integer>, MultiplyOp<Integer> >(view, db);
        REQUIRE(res == ParseOperationRes::VALID) ;
        REQUIRE(node.get()->Execute() == 1);
    }
    SECTION("VALID DIVIDE") {
        std::vector<std::string> val = {"1", "1"};
        auto view = view_t(val.begin(), val.end());
        auto [res, node] = ParseIntNode<DBType<Integer>, DivideOp<Integer> >(view, db);
        REQUIRE(res == ParseOperationRes::VALID) ;
        REQUIRE(node.get()->Execute() == 1);
    }
}

TEST_CASE("Parse Float Node") {
    DBType<Float> db(SipHash(0, 0, 2, 4));

    SECTION("INVALID COUNT") {
        std::vector<std::string> val = {"1.0"};
        auto view = view_t(val.begin(), val.end());
        auto [res, node] = ParseFloatNode<DBType<Float>, AddOp<Float> >(view, db);
        REQUIRE(res == ParseOperationRes::INVALID_ARG_C) ;
    }
    SECTION("INVALID COUNT") {
        std::vector<std::string> val = {"1", "a"};
        auto view = view_t(val.begin(), val.end());
        auto [res, node] = ParseFloatNode<DBType<Float>, AddOp<Float> >(view, db);
        REQUIRE(res == ParseOperationRes::INVALID_ARGS) ;
    }
    SECTION("VALID ADD") {
        std::vector<std::string> val = {"1.0", "1.0"};
        auto view = view_t(val.begin(), val.end());
        auto [res, node] = ParseFloatNode<DBType<Float>, AddOp<Float> >(view, db);
        REQUIRE(res == ParseOperationRes::VALID) ;
        REQUIRE(node.get()->Execute() == 2.0);
    }
    SECTION("VALID SUBTRACT") {
        std::vector<std::string> val = {"1.0", "1.0"};
        auto view = view_t(val.begin(), val.end());
        auto [res, node] = ParseFloatNode<DBType<Float>, SubtractOp<Float> >(view, db);
        REQUIRE(res == ParseOperationRes::VALID) ;
        REQUIRE(node.get()->Execute() == 0.0);
    }
    SECTION("VALID MULTIPLY") {
        std::vector<std::string> val = {"1.0", "1.0"};
        auto view = view_t(val.begin(), val.end());
        auto [res, node] = ParseFloatNode<DBType<Float>, MultiplyOp<Float> >(view, db);
        REQUIRE(res == ParseOperationRes::VALID) ;
        REQUIRE(node.get()->Execute() == 1.0);
    }
    SECTION("VALID DIVIDE") {
        std::vector<std::string> val = {"1.0", "1.0"};
        auto view = view_t(val.begin(), val.end());
        auto [res, node] = ParseFloatNode<DBType<Float>, DivideOp<Float> >(view, db);
        REQUIRE(res == ParseOperationRes::VALID) ;
        REQUIRE(node.get()->Execute() == 1.0);
    }
}


TEST_CASE("Parse String Op String Num") {
    DBType<String> db(SipHash(0, 0, 2, 4));

    SECTION("Invalid Count") {
        std::vector<std::string> val = {"String"};
        auto view = view_t(val.begin(), val.end());
        auto [res, node] = ParseStringOpStringNum<DBType<String>, PrefixNode>(view, db);
        REQUIRE(res == ParseOperationRes::INVALID_ARG_C);
    }

    SECTION("Invalid Args") {
        std::vector<std::string> val = {"String", "XD"};
        auto view = view_t(val.begin(), val.end());
        auto [res, node] = ParseStringOpStringNum<DBType<String>, PrefixNode>(view, db);
        REQUIRE(res == ParseOperationRes::INVALID_ARGS);
    }

    SECTION("Valid Count Prefix") {
        std::vector<std::string> val = {"String", "1"};
        auto view = view_t(val.begin(), val.end());
        auto [res, node] = ParseStringOpStringNum<DBType<String>, PrefixNode>(view, db);
        REQUIRE(res == ParseOperationRes::VALID);
        REQUIRE(node.get()->Execute() == String("S"));
    }

    SECTION("Valid Count Suffix") {
        std::vector<std::string> val = {"String", "1"};
        auto view = view_t(val.begin(), val.end());
        auto [res, node] = ParseStringOpStringNum<DBType<String>, SuffixNode>(view, db);
        REQUIRE(res == ParseOperationRes::VALID);
        REQUIRE(node.get()->Execute() == String("g"));
    }

    SECTION("Valid Count At") {
        std::vector<std::string> val = {"String", "1"};
        auto view = view_t(val.begin(), val.end());
        auto [res, node] = ParseStringOpStringNum<DBType<String>, AtNode>(view, db);
        REQUIRE(res == ParseOperationRes::VALID);
        REQUIRE(node.get()->Execute() == String("t"));
    }
}

TEST_CASE("Parse String Op String String") {
    DBType<String> db(SipHash(0, 0, 2, 4));

    SECTION("Invalid Count") {
        std::vector<std::string> val = {"String"};
        auto view = view_t(val.begin(), val.end());
        auto [res, node] = ParseStringOpStringString<DBType<String>, ConcatStrings>(view, db);
        REQUIRE(res == ParseOperationRes::INVALID_ARG_C);
    }

    SECTION("Valid Count Concat") {
        std::vector<std::string> val = {"String", "1"};
        auto view = view_t(val.begin(), val.end());
        auto [res, node] = ParseStringOpStringString<DBType<String>, ConcatStrings>(view, db);
        REQUIRE(res == ParseOperationRes::VALID);
        REQUIRE(node.get()->Execute() == String("String1"));
    }

    SECTION("Valid Count Find") {
        std::vector<std::string> val = {"StringString", "S"};
        auto view = view_t(val.begin(), val.end());
        auto [res, node] = ParseStringOpStringString<DBType<String>, FindNode>(view, db);

        Vector<Integer> resV;
        resV.Append(0);
        resV.Append(6);

        auto resC = node.get()->Execute();

        REQUIRE(resC.Count() == resV.Count());
        REQUIRE((resC[0] == resV[0] || resC[0] == resV[1]));
        REQUIRE((resC[1] == resV[0] || resC[1] == resV[1]));


        REQUIRE(res == ParseOperationRes::VALID);
    }
}

TEST_CASE("Parse Vector operation Vector value") {
    SECTION("Vector Integer") {
        DBType<Vector<Integer> > db(SipHash(0, 0, 2, 4));
        std::vector<std::uint8_t> key = {'k', 'e', 'y'};
        auto mss = std::chrono::milliseconds(1);
        auto entry = std::make_unique<QueueEntry>(mss, key);
        auto dbEntry = db.LookUp(std::move(key));
        Vector<Integer> vec;
        for (std::size_t i = 0; i < 4; ++i) {
            vec.Append(0);
        }
        auto inEntry = GetEntry(std::move(vec), ImmortalSentinel(), entry.get());
        dbEntry.SetValue(std::move(inEntry));


        SECTION("Invalid args count") {
            std::vector<std::string> val = {"$key"};
            auto view = view_t(val.begin(), val.end());
            auto [msg, res] = ParseVectOpVectVal<DBType<Vector<Integer> >, AppendNode<Integer>, Vector<
                Integer> >(view, db);
            REQUIRE(msg == ParseOperationRes::INVALID_ARG_C);
        }
        SECTION("Valid args count") {
            std::vector<std::string> val = {"$key", "xd"};
            auto view = view_t(val.begin(), val.end());
            auto [msg, res] = ParseVectOpVectVal<DBType<Vector<Integer> >, AppendNode<Integer>, Vector<
                Integer> >(view, db);
            REQUIRE(msg == ParseOperationRes::INVALID_ARGS);
        }

        SECTION("Valid args Append") {
            std::vector<std::string> val = {"$key", "1"};
            auto view = view_t(val.begin(), val.end());
            auto [msg, res] = ParseVectOpVectVal<DBType<Vector<Integer> >, AppendNode<Integer>, Vector<
                Integer> >(view, db);
            REQUIRE(msg == ParseOperationRes::VALID);

            REQUIRE(res.get()->Execute().Count() == 5);
        }
    }
}

TEST_CASE("Parse Vector operation Vector") {
    SECTION("Vector Integer") {
        DBType<Vector<Integer> > db(SipHash(0, 0, 2, 4));
        std::vector<std::uint8_t> key = {'k', 'e', 'y'};
        auto mss = std::chrono::milliseconds(1);
        auto entry = std::make_unique<QueueEntry>(mss, key);

        auto dbEntry = db.LookUp(std::move(key));
        Vector<Integer> vec;
        for (std::size_t i = 0; i < 4; ++i) {
            vec.Append(0);
        }
        auto inEntry = GetEntry(std::move(vec), ImmortalSentinel(), entry.get());
        dbEntry.SetValue(std::move(inEntry));

        SECTION("Invalid args count") {
            std::vector<std::string> val = {"$key", "8"};
            auto view = view_t(val.begin(), val.end());
            auto [msg, res] = ParseVectOpVect<DBType<Vector<Integer> >, PopNode<Integer>, Vector<Integer> >(view, db);
            REQUIRE(msg == ParseOperationRes::INVALID_ARG_C);
        }
        SECTION("Valid args") {
            std::vector<std::string> val = {"key"};
            auto view = view_t(val.begin(), val.end());
            auto [msg, res] = ParseVectOpVect<DBType<Vector<Integer> >, PopNode<Integer>, Vector<Integer> >(view, db);
            REQUIRE(msg == ParseOperationRes::INVALID_ARGS);
        }

        SECTION("Valid args Pop") {
            std::vector<std::string> val = {"$key"};
            auto view = view_t(val.begin(), val.end());
            auto [msg, res] = ParseVectOpVect<DBType<Vector<Integer> >, PopNode<Integer>, Vector<Integer> >(view, db);
            REQUIRE(msg == ParseOperationRes::VALID);

            REQUIRE(res.get()->Execute().Count() == 3);
        }


        SECTION("Valid args Size") {
            std::vector<std::string> val = {"$key"};
            auto view = view_t(val.begin(), val.end());
            auto [msg, res] = ParseVectOpVect<DBType<Vector<Integer> >, SizeNode<Integer>, Vector<Integer> >(view, db);
            REQUIRE(msg == ParseOperationRes::VALID);

            REQUIRE(res.get()->Execute() == 4);
        }
    }
}

TEST_CASE("Parse Vector operation Vector Num Val") {
    SECTION("Vector Integer") {
        DBType<Vector<Integer> > db(SipHash(0, 0, 2, 4));
        std::vector<std::uint8_t> key = {'k', 'e', 'y'};
        auto mss = std::chrono::milliseconds(1);
        auto entry = std::make_unique<QueueEntry>(mss, key);

        auto dbEntry = db.LookUp(std::move(key));
        Vector<Integer> vec;
        for (std::size_t i = 0; i < 4; ++i) {
            vec.Append(0);
        }
        auto inEntry = GetEntry(std::move(vec), ImmortalSentinel(), entry.get());
        dbEntry.SetValue(std::move(inEntry));

        SECTION("Invalid args count") {
            std::vector<std::string> val = {"$key", "8"};
            auto view = view_t(val.begin(), val.end());
            auto [msg, res] = ParseVectOpVectNumVal<DBType<Vector<Integer> >, VInsertNode<Integer>, Vector<
                Integer> >(view, db);
            REQUIRE(msg == ParseOperationRes::INVALID_ARG_C);
        }
        SECTION("Valid args") {
            std::vector<std::string> val = {"key", "xd", "xd"};
            auto view = view_t(val.begin(), val.end());
            auto [msg, res] = ParseVectOpVectNumVal<DBType<Vector<Integer> >, VInsertNode<Integer>, Vector<
                Integer> >(view, db);
            REQUIRE(msg == ParseOperationRes::INVALID_ARGS);
        }

        SECTION("Valid args VInsert") {
            std::vector<std::string> val = {"$key", "0", "1"};
            auto view = view_t(val.begin(), val.end());
            auto [msg, res] = ParseVectOpVectNumVal<DBType<Vector<Integer> >, VInsertNode<Integer>, Vector<
                Integer> >(view, db);
            REQUIRE(msg == ParseOperationRes::VALID);

            auto resVal = res.get()->Execute();

            REQUIRE(resVal.Count() == 5);
            REQUIRE(resVal[0] == 1);
        }


        SECTION("Valid args Set") {
            std::vector<std::string> val = {"$key", "0", "1"};
            auto view = view_t(val.begin(), val.end());
            auto [msg, res] = ParseVectOpVectNumVal<DBType<Vector<Integer> >, SetNode<Integer>, Vector<
                Integer> >(view, db);
            REQUIRE(msg == ParseOperationRes::VALID);

            auto resVal = res.get()->Execute();

            REQUIRE(resVal[0] == 1);
        }
    }
}


TEST_CASE("Parse Vector operation Vector Num") {
    SECTION("Vector Integer") {
        DBType<Vector<Integer> > db(SipHash(0, 0, 2, 4));
        std::vector<std::uint8_t> key = {'k', 'e', 'y'};
        auto mss = std::chrono::milliseconds(1);
        auto entry = std::make_unique<QueueEntry>(mss, key);

        auto dbEntry = db.LookUp(std::move(key));
        Vector<Integer> vec;
        for (std::size_t i = 0; i < 4; ++i) {
            vec.Append(0);
        }
        auto inEntry = GetEntry(std::move(vec), ImmortalSentinel(), entry.get());
        dbEntry.SetValue(std::move(inEntry));

        SECTION("Invalid args count") {
            std::vector<std::string> val = {"$key"};
            auto view = view_t(val.begin(), val.end());
            auto [msg, res] = ParseVectOpVectNum<DBType<Vector<Integer> >, RemoveNode<Integer>, Vector<
                Integer> >(view, db);
            REQUIRE(msg == ParseOperationRes::INVALID_ARG_C);
        }
        SECTION("Valid args") {
            std::vector<std::string> val = {"key", "xd"};
            auto view = view_t(val.begin(), val.end());
            auto [msg, res] = ParseVectOpVectNum<DBType<Vector<Integer> >, RemoveNode<Integer>, Vector<
                Integer> >(view, db);
            REQUIRE(msg == ParseOperationRes::INVALID_ARGS);
        }

        SECTION("Valid args Remove") {
            std::vector<std::string> val = {"$key", "0"};
            auto view = view_t(val.begin(), val.end());
            auto [msg, res] = ParseVectOpVectNum<DBType<Vector<Integer> >, RemoveNode<Integer>, Vector<
                Integer> >(view, db);
            REQUIRE(msg == ParseOperationRes::VALID);

            auto resVal = res.get()->Execute();

            REQUIRE(resVal.Count() == 3);
        }


        SECTION("Valid args VGet") {
            std::vector<std::string> val = {"$key", "0"};
            auto view = view_t(val.begin(), val.end());
            auto [msg, res] = ParseVectOpVectNum<DBType<Vector<Integer> >, VgetNode<Integer>, Vector<
                Integer> >(view, db);
            REQUIRE(msg == ParseOperationRes::VALID);

            auto resVal = res.get()->Execute();

            REQUIRE(resVal == 0);
        }
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

TEST_CASE("Parse Target") {
    SECTION("Invalid target") {
        std::vector<std::string> val = {};
        auto view = view_t(val.begin(), val.end());
        auto [res, valKey] = ParseTarget(view);
        REQUIRE(res == TargetType::INVALID);
    }

    SECTION("Constant target") {
        std::vector<std::string> val = {"key"};
        auto view = view_t(val.begin(), val.end());
        auto [res, valKey] = ParseTarget(view);
        REQUIRE(res == TargetType::KEY);
        REQUIRE(valKey == "key");
    }

    SECTION("File path") {
        std::vector<std::string> val = {"@key"};
        auto view = view_t(val.begin(), val.end());
        auto [res, valKey] = ParseTarget(view);
        REQUIRE(res == TargetType::FILE);
        REQUIRE(valKey == "key");
    }
}

TEST_CASE("Parse One key op") {
    DBType<Integer> db(SipHash(0, 0, 2, 4));
    std::vector<std::uint8_t> key = {'k', 'e', 'y'};
    auto mss = std::chrono::milliseconds(1);
    auto entry = std::make_unique<QueueEntry>(mss, key);
    auto dbEntry = db.LookUp(std::move(key));
    auto inEntry = GetEntry(static_cast<Integer>(8), ImmortalSentinel(), entry.get());
    dbEntry.SetValue(std::move(inEntry));

    SECTION("Invalid args count") {
        std::vector<std::string> val = {"$key", "8"};
        auto view = view_t(val.begin(), val.end());
        auto [msg, res] = ParseOneKeyOP<Integer, GetNode<Integer> >(view, std::cout, db);
        REQUIRE(msg == ParseOperationRes::INVALID_ARG_C);
    }

    SECTION("Valid args Get") {
        std::stringstream stream;
        std::vector<std::string> val = {"key"};
        auto view = view_t(val.begin(), val.end());
        auto [msg, res] = ParseOneKeyOP<Integer, GetNode<Integer> >(view, stream, db);
        REQUIRE(msg == ParseOperationRes::VALID);

        res.get()->Execute();
        auto resView = stream.view();
        REQUIRE(resView == "Value is: 8\n");
    }

    SECTION("Invalid key Get") {
        std::stringstream stream;
        std::vector<std::string> val = {"keyy"};
        auto view = view_t(val.begin(), val.end());
        auto [msg, res] = ParseOneKeyOP<Integer, GetNode<Integer> >(view, stream, db);
        REQUIRE(msg == ParseOperationRes::VALID);

        res.get()->Execute();
        auto resView = stream.view();
        REQUIRE(resView == "Invalid key\n");
    }
}

TEST_CASE("Parse Delete") {
    std::vector<std::uint8_t> key = {'k', 'e', 'y'};
    auto mss = std::chrono::milliseconds(1);
    auto entry = std::make_unique<QueueEntry>(mss, key);
    DBType<Integer> db(SipHash(0, 0, 2, 4));
    auto dbEntry = db.LookUp(std::move(key));
    auto inEntry = GetEntry(static_cast<Integer>(8), ImmortalSentinel(), entry.get());
    dbEntry.SetValue(std::move(inEntry));

    SECTION("Invalid args count") {
        std::vector<std::string> val = {"$key", "8"};
        auto view = view_t(val.begin(), val.end());
        auto [msg, res] = ParseDelete<Integer>(view, db);
        REQUIRE(msg == ParseOperationRes::INVALID_ARG_C);
    }

    SECTION("Valid args Delete") {
        std::vector<std::string> val = {"key"};
        auto view = view_t(val.begin(), val.end());
        auto [msg, res] = ParseDelete<Integer>(view, db);
        REQUIRE(msg == ParseOperationRes::VALID);

        res.get()->Execute();

        std::vector<std::uint8_t> keyL = {'k', 'e', 'y'};
        auto dbEntryL = db.LookUp(std::move(keyL));
        REQUIRE_FALSE(dbEntryL.GetResult() == LookupRes::USED);
    }
}

TEST_CASE("Parse Insert") {
    DBType<Integer> db(SipHash(0, 0, 2, 4));
    ttlQueueType queue;

    SECTION("Invalid args count") {
        std::vector<std::string> val = {"$key"};
        auto view = view_t(val.begin(), val.end());
        auto [msg, res] = ParseInsert<Integer>(view, db, {ImmortalSentinel(), TTLTypes::IMMORTAL}, queue);
        REQUIRE(msg == ParseOperationRes::INVALID_ARG_C);
    }

    SECTION("Valid args Insert") {
        std::vector<std::string> val = {"key", "8"};
        auto view = view_t(val.begin(), val.end());
        auto [msg, res] = ParseInsert<Integer>(view, db, {ImmortalSentinel(), TTLTypes::IMMORTAL}, queue);
        REQUIRE(msg == ParseOperationRes::VALID);

        res.get()->Execute();

        std::vector<std::uint8_t> key = {'k', 'e', 'y'};
        auto dbEntry = db.LookUp(std::move(key));
        REQUIRE(dbEntry.GetResult() == LookupRes::USED);
        REQUIRE(dbEntry.GetValue().GetRef() == 8);
    }
}

TEST_CASE("Parse Field Startup") {
    SECTION("Valid Int") {
        std::string val = " name : int ";
        std::string_view view(val.begin(), val.end());

        auto [name, type, optional] = ParseFieldStartup(view);

        REQUIRE(name == "name");
        REQUIRE(type == ObjectFieldTypes::INTEGER);
        REQUIRE_FALSE(optional);
    }

    SECTION("Valid Float") {
        std::string val = " name : float ";
        std::string_view view(val.begin(), val.end());

        auto [name, type, optional] = ParseFieldStartup(view);

        REQUIRE(name == "name");
        REQUIRE(type == ObjectFieldTypes::FLOAT);
        REQUIRE_FALSE(optional);
    }

    SECTION("Valid String") {
        std::string val = " name : string ";
        std::string_view view(val.begin(), val.end());

        auto [name, type, optional] = ParseFieldStartup(view);

        REQUIRE(name == "name");
        REQUIRE(type == ObjectFieldTypes::STRING);
        REQUIRE_FALSE(optional);
    }


    SECTION("Valid Int optional") {
        std::string val = " name : int-O ";
        std::string_view view(val.begin(), val.end());

        auto [name, type, optional] = ParseFieldStartup(view);

        REQUIRE(name == "name");
        REQUIRE(type == ObjectFieldTypes::INTEGER);
        REQUIRE(optional);
    }

    SECTION("Valid Float optional") {
        std::string val = " name : float-O ";
        std::string_view view(val.begin(), val.end());

        auto [name, type, optional] = ParseFieldStartup(view);

        REQUIRE(name == "name");
        REQUIRE(type == ObjectFieldTypes::FLOAT);
        REQUIRE(optional);
    }

    SECTION("Valid String optional") {
        std::string val = " name : string-O ";
        std::string_view view(val.begin(), val.end());

        auto [name, type, optional] = ParseFieldStartup(view);

        REQUIRE(name == "name");
        REQUIRE(type == ObjectFieldTypes::STRING);
        REQUIRE(optional);
    }

    SECTION("Invalid field") {
        std::string val = " name";
        std::string_view view(val.begin(), val.end());

        auto [name, type, optional] = ParseFieldStartup(view);
        REQUIRE(type == ObjectFieldTypes::INVALID_FIELD);
    }


    SECTION("Invalid type") {
        std::string val = " name:iiint";
        std::string_view view(val.begin(), val.end());

        auto [name, type, optional] = ParseFieldStartup(view);
        REQUIRE(type == ObjectFieldTypes::INVALID_TYPE);
    }
}

TEST_CASE("Parse Field") {
    SECTION("Valid Int") {
        std::string val = " name : val ";
        std::string_view view(val.begin(), val.end());

        auto [name, type, value] = ParseField(view);

        REQUIRE(name == "name");
        REQUIRE(type == ObjectFieldTypes::VALID);
        REQUIRE(value == "val");
    }


    SECTION("Invalid field") {
        std::string val = " name";
        std::string_view view(val.begin(), val.end());

        auto [name, type, value] = ParseField(view);
        REQUIRE(type == ObjectFieldTypes::INVALID_FIELD);
    }


}