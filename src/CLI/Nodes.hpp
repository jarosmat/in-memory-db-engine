#ifndef PROJECT_NODES_HPP
#define PROJECT_NODES_HPP
#include "String.hpp"
#include "SwissTable.hpp"
#include "DBEntry.hpp"
#include "SipHash.hpp"

class Node {
public:
    virtual ~Node() = default;
};

class StringNode : public Node {
public:
    virtual String Execute() = 0;
};

class StringFromDB : public StringNode {
    SwissTable<SipHash, DBEntry<String>, std::vector<uint8_t> > &db;
    std::vector<uint8_t> key;

public:
    String Execute() override;

    StringFromDB(SwissTable<SipHash, DBEntry<String>, std::vector<uint8_t> > &db, std::vector<uint8_t> &&key);
};

class StringConstant : public StringNode {
    String val;

public:
    String Execute() override;

    explicit StringConstant(String &&val);
};

class ConcatStrings : public StringNode {
    String left;
    String right;

public:
    String Execute() override;

    ConcatStrings(const String &left, const String &right);

    ConcatStrings(const String &left, String &&right);

    ConcatStrings(String &&left, const String &right);

    ConcatStrings(String &&left, String &&right);
};

class SuffixNode : public StringNode {
    String val;
    std::size_t n;

public:
    String Execute() override;

    SuffixNode(String &&val, std::size_t n);

    SuffixNode(const String &val, std::size_t n);
};

class PrefixNode : public StringNode {
    String val;
    std::size_t n;

public:
    String Execute() override;

    PrefixNode(String &&val, std::size_t n);

    PrefixNode(const String &val, std::size_t n);
};

class AtNode : public StringNode {
    String val;
    std::size_t i;

public:
    String Execute() override;;

    AtNode(String &&val, std::size_t i);

    AtNode(const String &val, std::size_t i);
};

class IntegerNode : public Node {
public:
    virtual Integer Execute() = 0;
};

class FloatNode : public Node {
public:
    virtual Float Execute() = 0;
};

class ObjectNode : public Node {
public:
    virtual Object Execute() = 0;
};

class VectIntNode : public Node {
public:
    virtual Vector<Integer> Execute() = 0;
};

class VectFloatNode : public Node {
public:
    virtual Vector<Float> Execute() = 0;
};

class VectStringNode : public Node {
public:
    virtual Vector<String> Execute() = 0;
};

#endif //PROJECT_NODES_HPP
