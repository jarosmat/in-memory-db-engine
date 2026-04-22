#ifndef PROJECT_NODES_HPP
#define PROJECT_NODES_HPP
#include "String.hpp"
#include "SwissTable.hpp"
#include "DBEntry.hpp"
#include "SipHash.hpp"

template<typename T>
class Node {
public:
    virtual ~Node() = default;

    virtual T Execute() = 0;
};

using StringNode = Node<String>;
template<typename T>
using NumberNode = Node<T>;
template<typename T>
using VectNode = Node<Vector<T> >;


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
    std::unique_ptr<StringNode> left;
    std::unique_ptr<StringNode> right;

public:
    String Execute() override;

    ConcatStrings(const String &left, const String &right);

    ConcatStrings(const String &left, String &&right);

    ConcatStrings(String &&left, const String &right);

    ConcatStrings(String &&left, String &&right);
};

class SuffixNode : public StringNode {
    std::unique_ptr<StringNode> val;
    std::unique_ptr<NumberNode<Integer> > n;

public:
    String Execute() override {
        auto res = val->Execute();
        const auto c = n->Execute();
        res.RemovePrefix(res.Size() - c);
        return res;
    }

    SuffixNode(std::unique_ptr<StringNode> &&val, std::unique_ptr<NumberNode<Integer> > &&n) : val(std::move(val)),
        n(std::move(n)) {
    }
};

class PrefixNode : public StringNode {
    std::unique_ptr<StringNode> val;
    std::unique_ptr<NumberNode<Integer> > n;

public:
    String Execute() override {
        auto res = val->Execute();
        const auto c = n->Execute();
        res.RemoveSuffix(res.Size() - c);
        return res;
    }

    PrefixNode(std::unique_ptr<StringNode> &&val, std::unique_ptr<NumberNode<Integer> > &&n) : val(std::move(val)),
        n(std::move(n)) {
    }
};

class AtNode : public StringNode {
    std::unique_ptr<StringNode> val;
    std::unique_ptr<NumberNode<Integer> > i;

public:
    String Execute() override {
        const auto vala = val->Execute()[i->Execute()];
        return String(vala);
    }

    AtNode(std::unique_ptr<StringNode> &&val, std::unique_ptr<NumberNode<Integer> > &&i) : val(std::move(val)),
        i(std::move(i)) {
    }
};


template<typename T>
class BinaryOp : public NumberNode<T> {
    std::unique_ptr<NumberNode<T> > left;
    std::unique_ptr<NumberNode<T> > right;

public:
    BinaryOp(std::unique_ptr<NumberNode<T> > &&left, std::unique_ptr<NumberNode<T> > &&right);
};

template<typename T>
struct AddOp : public BinaryOp<T> {
    using BinaryOp<T>::BinaryOp;

    T Execute() override {
        return left->execute() + right->execute();
    }
};

template<typename T>
struct SubtractOp : public BinaryOp<T> {
    using BinaryOp<T>::BinaryOp;

    T Execute() override {
        return left->execute() - right->execute();
    }
};

template<typename T>
struct DivideOp : public BinaryOp<T> {
    using BinaryOp<T>::BinaryOp;

    T Execute() override {
        return left->Execute() / right->Execute();
    }
};

template<typename T>
struct MultiplyOp : public BinaryOp<T> {
    using BinaryOp<T>::BinaryOp;

    T Execute() override {
        return left->Execute() * right->Execute();
    }
};

template<typename T>
class AppendNode : public VectNode<T> {
    std::unique_ptr<VectNode<T> > vect;
    std::unique_ptr<Node<T> > what;

public:
    Vector<T> Execute() override {
        auto val = vect->Execute();
        val.Append(std::move(what->Execute()));
        return val;
    }

    AppendNode(std::unique_ptr<VectNode<T> > &&vect, std::unique_ptr<Node<T> > what) : vect(std::move(vect)),
        what(std::move(what)) {
    }
};

template<typename T>
class PopNode : public VectNode<T> {
    std::unique_ptr<VectNode<T> > vect;

public:
    Vector<T> Execute() override {
        auto val = vect->Execute();
        val.Remove();
        return val;
    }

    explicit PopNode(std::unique_ptr<VectNode<T> > &&vect) : vect(std::move(vect)) {
    }
};

template<typename T>
class VInsertNode : public VectNode<T> {
    std::unique_ptr<VectNode<T> > vect;
    std::unique_ptr<Node<T> > what;
    std::unique_ptr<NumberNode<Integer> > i;

public:
    Vector<T> Execute() override {
        auto val = vect->Execute();
        val.Insert(i->Execute(), std::move(what->Execute()));
        return val;
    }

    VinsertNode(std::unique_ptr<VectNode<T> > &&vect, std::unique_ptr<Node<T> > what,
                std::unique_ptr<NumberNode<Integer> > i) : vect(std::move(vect)), what(std::move(what)),
                                                           i(std::move(i)) {
    }
};


template<typename T>
class RemoveNode : public VectNode<T> {
    std::unique_ptr<VectNode<T> > vect;
    std::unique_ptr<NumberNode<Integer> > i;

public:
    Vector<T> Execute() override {
        auto val = vect->Execute();
        val.Remove(i->Execute());
        return val;
    }

    RemoveNode(std::unique_ptr<VectNode<T> > &&vect, std::unique_ptr<NumberNode<Integer> > i) : vect(std::move(vect)),
        i(std::move(i)) {
    }
};

template<typename T>
class SetNode : public VectNode<T> {
    std::unique_ptr<VectNode<T> > vect;
    std::unique_ptr<Node<T> > what;
    std::unique_ptr<NumberNode<Integer> > i;

public:
    Vector<T> Execute() override {
        auto val = vect->Execute();
        val[i->Execute()] = std::move(what->Execute());
        return val;
    }

    SetNode(std::unique_ptr<VectNode<T> > &&vect, std::unique_ptr<Node<T> > what,
            std::unique_ptr<NumberNode<Integer> > i) : vect(std::move(vect)), what(std::move(what)), i(std::move(i)) {
    }
};

template<typename T>
class VgetNode : public VectNode<T> {
    std::unique_ptr<VectNode<T> > vect;
    std::unique_ptr<NumberNode<Integer> > i;

public:
    T Execute() override {
        return vect->Execute()[i];
    }

    VgetNode(std::unique_ptr<VectNode<T> > &&vect, std::unique_ptr<NumberNode<Integer> > &&i) : vect(std::move(vect)),
        i(std::move(i)) {
    }
};

template<typename T>
class SizeNode : public VectNode<T> {
    std::unique_ptr<VectNode<T> > vect;

public:
    Integer Execute() override {
        return vect->Execute().Count();
    }

    explicit SizeNode(std::unique_ptr<VectNode<T> > &&vect) : vect(std::move(vect)) {
    }
};


#endif //PROJECT_NODES_HPP
