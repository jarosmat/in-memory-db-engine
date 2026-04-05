#include <complex.h>
#include <catch2/catch_all.hpp>
#include "ValueTypes.hpp"
#include "Object.hpp"
#include <vector>

auto GetInitData() {
    Value val1{};
    val1 = static_cast<Integer>(42);
    Value val2{};
    val2 = 1.75;
    Value val3(String("Alice"));
    return TypeForConstruct{{"age", val1}, {"height", val2}, {"name", val3}};
}

TEST_CASE("Object class tests", "[Object]") {
    SECTION("Construction from iterator range") {
        auto init_data = GetInitData();
        Object obj(init_data.begin(), init_data.end());

        REQUIRE(obj.has_member("age"));
        REQUIRE(obj.has_member("height"));
        REQUIRE(obj.has_member("name"));
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
