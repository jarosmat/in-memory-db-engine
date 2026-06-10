#ifndef PROJECT_CONSTANTS_HPP
#define PROJECT_CONSTANTS_HPP
#include <string>

struct API_CONSTANTS {
    static const std::string VALUE_O;

    static const std::string INT;
    static const std::string FLOAT;
    static const std::string STRING;
    static const std::string OBJECT;
    static const std::string VECT_INT;
    static const std::string VECT_FLOAT;
    static const std::string VECT_STRING;

    static const std::string TTL_O;
    static const std::string MILLISECONDS;
    static const std::string SECONDS;
    static const std::string MINUTES;
    static constexpr char SECONDS_CH = 's';
    static constexpr char MINUTES_CH = 'm';

    static const std::string STRUCT_O;
    static const std::string OPTIONAL_IS;

    static constexpr char FILE_ID = '@';
    static constexpr char FROM_DB_ID = '$';

    static const std::string FILE_ID_S;
    static const std::string FROM_DB_ID_S;

    static const std::string INSERT;
    static const std::string GET;
    static const std::string DELETE;
    static const std::string TERMINATE;

    static const std::string ADD;
    static const std::string SUBTRACT;
    static const std::string DIVIDE;
    static const std::string MULTIPLY;

    static const std::string FADD;
    static const std::string FSUBTRACT;
    static const std::string FDIVIDE;
    static const std::string FMULTIPLY;

    static const std::string CONCAT;
    static const std::string SUFFIX;
    static const std::string PREFIX;
    static const std::string AT;
    static const std::string FIND;

    static const std::string APPEND;
    static const std::string POP;
    static const std::string VINSERT;
    static const std::string REMOVE;
    static const std::string SET;
    static const std::string VGET;
    static const std::string SIZE;

    static const std::string OGET;
    static const std::string OSET;
};
#endif // PROJECT_CONSTANTS_HPP
