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
