#include <iostream>
#include <ranges>

#include "DBCore/DBEntry/String/String.hpp"
#include "DBCore/SwissTable/SwissTable.hpp"

using namespace std;

int main() {
    SwissTable<std::hash<string>, String, string> table;

    string line;
    const string insert = "INSERT";
    const string get = "GET";
    const string del = "DELETE";
    const string terminate = "TERMINATE";
    const string invalid =
            "Invalid input, input must have structure: [ACTION] [KEY] [VALUE]; actions are INSERT, GET or DELETE, [VALUE] needs to be specified only for INSERT, otherwise it is ignored. To shut down, type TERMINATE";
    const string notFound = "There does not exist a value associated with key: ";
    const string invalidAction = "Invalid action specified, valid actions are: INSERT, GET, DELETE or TERMINATE";

    getline(cin, line);
    while (line != terminate) {
        auto words = line | views::split(' ') | std::views::filter([](auto &&r) { return !r.empty(); }) | ranges::to<
                         vector<string> >();
        if (words.size() > 3 or words.size() < 2) {
            cout << invalid << endl;
            getline(cin, line);
            continue;
        }

        auto &action = words[0];
        auto &key = words[1];

        auto entry = table.LookUp(key);

        if (action == get) {
            if (entry.GetResult() == USED) {
                cout << "Value for key: " << key << " is: " << entry.GetValue() << endl;
            } else {
                cout << notFound << key << endl;
            }
        } else if (action == insert) {
            auto &value = words[2];
            entry.SetValue(String(value));
        } else if (action == del) {
            entry.DeleteEntry();
        } else {
            cout << invalidAction << endl;
        }
        getline(cin, line);
    }

    return 0;
}
