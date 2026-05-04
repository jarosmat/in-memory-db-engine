#include "Error.hpp"

err_t CouldNotOpenFileErr(const std::string &file) {
    return {"Could not open file: ", file};
}

err_t CouldNotReadFileErr(const std::string &file) {
    return {"Could not read file: ", file};
}

err_t InvalidArgs() {
    return {"Invalid arguments"};
}

err_t InvalidArgsCount(const std::size_t expected, const std::size_t actual) {
    return {"Invalid number of arguments, expected: ", std::to_string(expected), ", got: ", std::to_string(actual)};
}

err_t InvalidOperation() {
    return {"Invalid operation"};
}

err_t InvalidField(const std::string &field) {
    return {"Invalid field: ", field};
}
