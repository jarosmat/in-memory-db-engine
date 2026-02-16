#include "SwissTable.hpp"
#include <catch2/catch_test_macros.hpp>

using TestTable = SwissTable<std::hash<std::string>, std::string, std::string>;

TEST_CASE("SwissTable Basic Operations", "[SwissTable]") {
    TestTable table;

    SECTION("LookUp on empty table returns empty result") {
        auto entry = table.LookUp("key1");
        REQUIRE(entry.GetResult() == LookupRes::EMPTY);
    }

    SECTION("Inserting a value via Entry") {
        auto entry = table.LookUp("key1");
        entry.SetValue("value1");

        auto verifyEntry = table.LookUp("key1");


        REQUIRE(verifyEntry.GetResult() == LookupRes::USED);
        REQUIRE(verifyEntry.GetValue() == "value1");
    }

    SECTION("Updating an existing value") {
        table.LookUp("key1").SetValue("value1");

        auto entry = table.LookUp("key1");
        entry.SetValue("value2");

        REQUIRE(table.LookUp("key1").GetValue() == "value2");
    }
}

TEST_CASE("SwissTable Deletion and Re-insertion", "[SwissTable]") {
    TestTable table;

    table.LookUp("key1").SetValue("value1");
    auto entry = table.LookUp("key1");

    SECTION("Deleting an entry") {
        entry.DeleteEntry();

        auto postDeleteEntry = table.LookUp("key1");
        REQUIRE(postDeleteEntry.GetResult() == LookupRes::EMPTY);
    }

    SECTION("Re-inserting after deletion") {
        entry.DeleteEntry();
        table.LookUp("key1").SetValue("new_value");

        REQUIRE(table.LookUp("key1").GetValue() == "new_value");
    }
}

TEST_CASE("SwissTable Stress and Rehashing", "[SwissTable]") {
    TestTable table;
    const int elementCount = 100;

    SECTION("Handling multiple inserts (Triggering Rehash)") {
        for (int i = 0; i < elementCount; ++i) {
            std::string k = "key" + std::to_string(i);
            const std::string v = "val" + std::to_string(i);
            table.LookUp(k).SetValue(v);
        }

        // Verify all elements are still there after potential rehashes
        bool allFound = true;
        for (int i = 0; i < elementCount; ++i) {
            auto e = table.LookUp("key" + std::to_string(i));
            if (e.GetResult() != LookupRes::USED || e.GetValue() != "val" + std::to_string(i)) {
                allFound = false;
                break;
            }
        }
        REQUIRE(allFound);
    }
}