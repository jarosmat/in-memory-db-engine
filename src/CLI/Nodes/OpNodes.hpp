#ifndef PROJECT_OPNODES_HPP
#define PROJECT_OPNODES_HPP

template<typename T>
class Node : BaseNode {
public:
    virtual T Execute() = 0;
};

template<typename T>
using NodePtr = std::unique_ptr<Node<T> >;
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
        const auto where = val->Execute();
        const auto what = toFind->Execute();

        const auto res = where.Find(what);

        auto resV = Vector<Integer>(res.size());

        for (auto &&a: res) {
            resV.Append(static_cast<Integer>(a));
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
struct AddOp : BinaryOp<T> {
    using BinaryOp<T>::BinaryOp;

    T Execute() override {
        return this->left.get()->Execute() + this->right.get()->Execute();
    }
};

template<typename T>
struct SubtractOp : BinaryOp<T> {
    using BinaryOp<T>::BinaryOp;

    T Execute() override {
        return this->left.get()->Execute() - this->right.get()->Execute();
    }
};

template<typename T>
struct DivideOp : BinaryOp<T> {
    using BinaryOp<T>::BinaryOp;

    T Execute() override {
        return this->left.get()->Execute() / this->right.get()->Execute();
    }
};

template<typename T>
struct MultiplyOp : BinaryOp<T> {
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
        auto index = i->Execute();
        auto vectVal = vect->Execute();
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
    std::unique_ptr<Node > obj;
    Key key;
    std::unique_ptr<Node<T> > value;

public:
    Object Execute() override {
        auto res = obj->Execute();
        res.set_value<T>(key, std::move(value->Execute()));

        return res;
    }

    ObjectSetNode(std::unique_ptr<Node > &&obj, Key key,
                  std::unique_ptr<Node<T> > &&value) : obj(std::move(obj)), key(std::move(key)),
                                                       value(std::move(value)) {
    }
};


#endif //PROJECT_OPNODES_HPP
