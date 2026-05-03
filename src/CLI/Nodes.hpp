#ifndef PROJECT_NODES_HPP
#define PROJECT_NODES_HPP
#include <ranges>
#include <utility>

#include "String.hpp"
#include "SwissTable.hpp"
#include "DBEntry.hpp"
#include "DBType.hpp"
#include "QueueEntry.hpp"
#include "SipHash.hpp"

class BaseNode {
public:
    virtual ~BaseNode() = default;
};

using BaseNodePtr = std::unique_ptr<BaseNode>;
// using RetVal = std::pair<DBTypes, AllValuePack>;

template<typename T>
class Node : BaseNode {
public:
    virtual T Execute() = 0;
};

using StringNode = Node<String>;
using StringPtr = std::unique_ptr<StringNode>;
template<typename T>
using NumberNode = Node<T>;
template<typename T>
using NumberPtr = std::unique_ptr<NumberNode<T> >;
template<typename T>
using VectNode = Node<Vector<T> >;
template<typename T>
using VectPtr = std::unique_ptr<VectNode<T> >;

template<typename T>
class FromDB : public Node<T> {
    SwissTable<SipHash, DBEntry<T>, std::vector<uint8_t> > &db;
    std::vector<uint8_t> key;

public:
    T Execute() override {
        auto entry = db.LookUp(std::move(key));
        return entry.GetValue().GetRef();
    }

    FromDB(SwissTable<SipHash, DBEntry<T>, std::vector<uint8_t> > &db,
           std::vector<uint8_t> &&key) : db(db), key(std::move(key)) {
    }
};

template<typename T>
class ConstantNode : public Node<T> {
    T val;

public:
    T Execute() override {
        return val;
    }

    explicit ConstantNode(T &&val) : val(std::move(val)) {
    }

    explicit ConstantNode(T &val) : val(val) {
    }
};

using StringConstant = ConstantNode<String>;
using IntegerConstant = ConstantNode<Integer>;
using FloatConstant = ConstantNode<Float>;
using ObjectConstant = ConstantNode<Object>;
template<typename T>
using VectorConstant = ConstantNode<Vector<T> >;

class ConcatStrings : public StringNode {
    std::unique_ptr<StringNode> left;
    std::unique_ptr<StringNode> right;

public:
    String Execute() override {
        return String::Concatenate(std::move(left->Execute()), std::move(right->Execute()));
    }

    ConcatStrings(std::unique_ptr<StringNode> left, std::unique_ptr<StringNode> right) : left(std::move(left)),
        right(std::move(right)) {
    }
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

class FindNode : public Node<Vector<Integer> > {
    std::unique_ptr<StringNode> val;
    std::unique_ptr<StringNode> toFind;

public:
    Vector<Integer> Execute() override {
        const auto where = val.get()->Execute();
        const auto what = toFind.get()->Execute();

        const auto res = where.Find(what);

        auto resV = Vector<Integer>(res.size());

        for (auto &&a: res) {
            resV.Append(a);
        }

        return resV;
    }

    FindNode(std::unique_ptr<StringNode> &&val, std::unique_ptr<StringNode> &&what) : val(std::move(val)),
        toFind(std::move(what)) {
    }
};


template<typename T>
class BinaryOp : public NumberNode<T> {
protected:
    std::unique_ptr<NumberNode<T> > left;
    std::unique_ptr<NumberNode<T> > right;

public:
    BinaryOp(std::unique_ptr<NumberNode<T> > &&left, std::unique_ptr<NumberNode<T> > &&right) : left(std::move(left)),
        right(std::move(right)) {
    }
};

template<typename T>
struct AddOp : public BinaryOp<T> {
    using BinaryOp<T>::BinaryOp;

    T Execute() override {
        return this->left.get()->Execute() + this->right.get()->Execute();
    }
};

template<typename T>
struct SubtractOp : public BinaryOp<T> {
    using BinaryOp<T>::BinaryOp;

    T Execute() override {
        return this->left.get()->Execute() - this->right.get()->Execute();
    }
};

template<typename T>
struct DivideOp : public BinaryOp<T> {
    using BinaryOp<T>::BinaryOp;

    T Execute() override {
        return this->left.get()->Execute() / this->right.get()->Execute();
    }
};

template<typename T>
struct MultiplyOp : public BinaryOp<T> {
    using BinaryOp<T>::BinaryOp;

    T Execute() override {
        return this->left.get()->Execute() * this->right.get()->Execute();
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

    AppendNode(std::unique_ptr<VectNode<T> > &&vect, std::unique_ptr<Node<T> > &&what) : vect(std::move(vect)),
        what(std::move(what)) {
    }
};

template<typename T>
class PopNode : public VectNode<T> {
    std::unique_ptr<VectNode<T> > vect;

public:
    Vector<T> Execute() override {
        auto val = vect->Execute();
        val.Remove(val.Count() - 1);
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

    VInsertNode(std::unique_ptr<VectNode<T> > &&vect, std::unique_ptr<NumberNode<Integer> > i,
                std::unique_ptr<Node<T> > what) : vect(std::move(vect)), what(std::move(what)),
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

    SetNode(std::unique_ptr<VectNode<T> > &&vect, std::unique_ptr<NumberNode<Integer> > i,
            std::unique_ptr<Node<T> > what) : vect(std::move(vect)), what(std::move(what)), i(std::move(i)) {
    }
};

template<typename T>
class VgetNode : public Node<T> {
    std::unique_ptr<VectNode<T> > vect;
    std::unique_ptr<NumberNode<Integer> > i;

public:
    T Execute() override {
        auto index = i.get()->Execute();
        auto vectVal = vect.get()->Execute();
        return vectVal[index];
    }

    VgetNode(std::unique_ptr<VectNode<T> > &&vect, std::unique_ptr<NumberNode<Integer> > &&i) : vect(std::move(vect)),
        i(std::move(i)) {
    }
};

template<typename T>
class SizeNode : public Node<Integer> {
    std::unique_ptr<VectNode<T> > vect;

public:
    Integer Execute() override {
        return vect->Execute().Count();
    }

    explicit SizeNode(std::unique_ptr<VectNode<T> > &&vect) : vect(std::move(vect)) {
    }
};

template<typename T>
class ObjectGetNode : public Node<T> {
    std::unique_ptr<Node<Object> > obj;
    Key key;

public:
    T Execute() override {
        return obj->Execute().get_member<T>(key);
    }

    ObjectGetNode(std::unique_ptr<Node<Object> > &&obj, Key key) : obj(std::move(obj)), key(std::move(key)) {
    }
};

template<typename T>
class ObjectSetNode : public Node<Object> {
    std::unique_ptr<Node<Object> > obj;
    Key key;
    T value;

public:
    Object Execute() override {
        auto res = obj->Execute();
        res.set_value<T>(key, std::move(value));

        return res;
    }

    ObjectSetNode(std::unique_ptr<Node<Object> > &&obj, Key key, T value) : obj(std::move(obj)), key(std::move(key)),
                                                                            value(value) {
    }
};

template<typename T>
using DBType = SwissTable<SipHash, DBEntry<T>, std::vector<uint8_t> >;

class DBopsNodeBase : public BaseNode {
public:
    virtual void Execute() = 0;
};

template<typename T>
class DBopsNode : public DBopsNodeBase {
protected:
    DBType<T> &db;

public:
    explicit DBopsNode(DBType<T> &db) : db(db) {
    }
};

template<typename T, typename TTL, typename QUEUE>
class InsertNode : public DBopsNode<T> {
    std::vector<uint8_t> key;
    std::unique_ptr<Node<T> > valNode;
    TTL ttl;
    QUEUE &priority_queue_;

public:
    void Execute() override {
        std::unique_ptr<QueueEntry> queueEntry = std::make_unique<QueueEntry>(
            std::chrono::duration_cast<std::chrono::milliseconds>(ttl), key);
        QueueEntry *pointer = queueEntry.get();
        priority_queue_.push(std::move(queueEntry));
        auto entry = this->db.LookUp(std::move(key));
        auto value = valNode->Execute();
        DBEntry<T> toInsert = GetEntry<T, TTL>(std::move(value), std::move(ttl), pointer);
        entry.SetValue(std::move(toInsert));
    }

    InsertNode(DBType<T> &db, std::vector<uint8_t> &&key, std::unique_ptr<Node<T> > &&valNode,
               TTL &&ttl, QUEUE &queue) : DBopsNode<T>(db),
                                          key(std::move(key)),
                                          valNode(std::move(valNode)), ttl(ttl), priority_queue_(queue) {
    }
};

template<typename T, typename QUEUE>
class InsertNode<T, ImmortalSentinel, QUEUE> : public DBopsNode<T> {
    std::vector<uint8_t> key;
    std::unique_ptr<Node<T> > valNode;
    QUEUE &priority_queue_;

public:
    void Execute() override {
        auto entry = this->db.LookUp(std::move(key));
        auto value = valNode->Execute();
        DBEntry<T> toInsert = GetEntry<T, ImmortalSentinel>(std::move(value), ImmortalSentinel(), nullptr);
        entry.SetValue(std::move(toInsert));
    }

    InsertNode(DBType<T> &db, std::vector<uint8_t> &&key, std::unique_ptr<Node<T> > &&valNode,
               ImmortalSentinel &&, QUEUE &queue) : DBopsNode<T>(db),
                                                    key(std::move(key)),
                                                    valNode(std::move(valNode)), priority_queue_(queue) {
    }
};

template<typename T>
class GetNode : public DBopsNode<T> {
    std::ostream &stream;
    std::vector<uint8_t> key;

public:
    void Execute() override {
        auto entry = this->db.LookUp(std::move(key));
        if (entry.GetResult() == USED) {
            stream << "Value is: " << entry.GetValue() << std::endl;
        } else {
            stream << "Invalid key" << std::endl;
        }
    }

    GetNode(DBType<T> &db, std::vector<uint8_t> &&key, std::ostream &stream) : DBopsNode<T>(db), stream(stream),
                                                                               key(std::move(key)) {
    }
};

template<typename T>
class DeleteNode : public DBopsNode<T> {
    std::vector<uint8_t> key;

public:
    void Execute() override {
        auto entry = this->db.LookUp(std::move(key));
        entry.GetValue().Invalidate();
        entry.DeleteEntry();
    }

    DeleteNode(DBType<T> &db, std::vector<uint8_t> &&key) : DBopsNode<T>(db), key(std::move(key)) {
    }
};

class ErrorNode : public DBopsNodeBase {
    std::ostream &stream;
    std::vector<std::string> val;

public:
    void Execute() override {
        for (auto &&s: val) {
            stream << s;
        }
        stream << std::endl;
    }

    ErrorNode(std::ostream &stream, const std::vector<std::string> &val) : stream(stream), val(val) {
    }


    ErrorNode(std::ostream &stream, std::vector<std::string> &&val) : stream(stream), val(std::move(val)) {
    }
};

template<typename T, typename OPT>
class ToStreamNode : public DBopsNode<T> {
    std::unique_ptr<Node<OPT> > op;
    std::ostream &stream;

public:
    void Execute() override {
        stream << op->Execute() << std::endl;
    }

    ToStreamNode(std::ostream &stream, std::unique_ptr<Node<OPT> > &&op, DBType<T> &db) : DBopsNode<T>(db),
        op(std::move(op)),
        stream(stream) {
    }
};


#endif //PROJECT_NODES_HPP
