#ifndef PROJECT_DBNODES_HPP
#define PROJECT_DBNODES_HPP


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

template<typename OPT>
class ToStreamNode : public DBopsNodeBase {
    std::unique_ptr<Node<OPT> > op;
    std::ostream &stream;

public:
    void Execute() override {
        stream << op->Execute() << std::endl;
    }

    ToStreamNode(std::ostream &stream, std::unique_ptr<Node<OPT> > &&op) : op(std::move(op)), stream(stream) {
    }
};

#endif //PROJECT_DBNODES_HPP
