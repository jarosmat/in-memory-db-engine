#include <catch2/catch_all.hpp>
#include "ValueTypes.hpp"
#include "Object.hpp"
#include <vector>

auto GetInitData() {
    constexpr std::size_t initValCount = 5;
    constexpr Integer initIntVal = 10;
    const auto initStringVal = String("init");
    constexpr float initFloatVal = 22.8;
    Value intVal{};
    intVal = static_cast<Integer>(42);
    Value floatVal{};
    floatVal = 1.75;
    Value stringVal(String("Alice"));
    Value vectorIntVal(Vector<Integer>(initValCount, initIntVal));
    Value vectorStringVal(Vector<String>(initValCount, initStringVal));
    Value vectorFloatVal(Vector<Float>(initValCount, initFloatVal));
    return TypeForConstruct{
        {"age", intVal}, {"height", floatVal}, {"name", stringVal}, {"VectorInt", vectorIntVal},
        {"VectorString", vectorStringVal},
        {"VectorFloat", vectorFloatVal}
    };
}

TEST_CASE("Object class tests", "[Object]") {
    SECTION("Construction from iterator range") {
        auto init_data = GetInitData();
        Object obj(init_data.begin(), init_data.end());

        REQUIRE(obj.has_member("age"));
        REQUIRE(obj.has_member("height"));
        REQUIRE(obj.has_member("name"));
        REQUIRE(obj.has_member("VectorInt"));
        REQUIRE(obj.has_member("VectorString"));
        REQUIRE(obj.has_member("VectorFloat"));
    }

    SECTION("Get member by name") {
        auto init_data = GetInitData();
        Object obj(init_data.begin(), init_data.end());

        REQUIRE(obj.get_member<Integer>("age") == 42);
        REQUIRE(obj.get_member<Float>("height") == 1.75);
        REQUIRE(obj.get_member<String>("name") == String("Alice"));
    }

    SECTION("Set member by name") {
        auto init_data = GetInitData();
        Object obj(init_data.begin(), init_data.end());

        obj.set_value("age", static_cast<Integer>(43));
        obj.set_value("height", 1.80);
        obj.set_value("name", String("Bob"));

        REQUIRE(obj.get_member<Integer>("age") == 43);
        REQUIRE(obj.get_member<Float>("height") == 1.80);
        REQUIRE(obj.get_member<String>("name") == String("Bob"));
    }

    SECTION("Check non-existent member") {
        auto init_data = GetInitData();
        Object obj(init_data.begin(), init_data.end());

        REQUIRE(!obj.has_member("weight"));
    }
}
