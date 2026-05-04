#include <iostream>
#include <ranges>

#include "Runner.hpp"

using namespace std;

int run(vector<string> &args) {
    view_t view(args.begin(), args.end());

    if (auto [msg, ttl, obj, ref] = GetDBType(view); msg == DBTypes::OBJECT) {
        RunDB<Object>(std::move(ttl), cin, cout, obj, ref);
    } else if (msg == DBTypes::STRING) {
        RunDB<String>(std::move(ttl), cin, cout, obj, ref);
    } else if (msg == DBTypes::FLOAT) {
        RunDB<Float>(std::move(ttl), cin, cout, obj, ref);
    } else if (msg == DBTypes::INTEGER) {
        RunDB<Integer>(std::move(ttl), cin, cout, obj, ref);
    } else if (msg == DBTypes::VECTOR_FLOAT) {
        RunDB<Vector<Float> >(std::move(ttl), cin, cout, obj, ref);
    } else if (msg == DBTypes::VECTOR_INT) {
        RunDB<Vector<Integer> >(std::move(ttl), cin, cout, obj, ref);
    } else if (msg == DBTypes::VECTOR_STRING) {
        RunDB<Vector<String> >(std::move(ttl), cin, cout, obj, ref);
    } else {
        cout << "Invalid options" << std::endl;
    }

    return 0;
}

int main(const int argc, char *argv[]) {

    vector<string> args(argv + 1, argv + argc);

    return run(args);
}
