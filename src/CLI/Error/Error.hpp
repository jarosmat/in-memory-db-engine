#ifndef PROJECT_ERROR_HPP
#define PROJECT_ERROR_HPP
#include <string>
#include <vector>

using err_t = std::vector<std::string>;

err_t CouldNotOpenFileErr(const std::string &file);

err_t CouldNotReadFileErr(const std::string &file);

err_t InvalidArgs();

err_t InvalidArgsCount(std::size_t expected, std::size_t actual);

err_t InvalidOperation();

err_t InvalidField(const std::string &field);


#endif //PROJECT_ERROR_HPP