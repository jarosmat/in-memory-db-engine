#ifndef PROJECT_RUNNER_HPP
#define PROJECT_RUNNER_HPP

#include "OperationsParser.hpp"
#include <mutex>
#include "Constants.hpp"

void readThread(const std::stop_token &stoken, std::istream &stream, std::mutex &queueMutex,
                       std::queue<std::string> &dataQueue, std::atomic<std::size_t>& dataQueueCount);

template<typename T>
void RunDB(ttl_t &&defaultTTL, std::istream &instream, std::ostream &outstream, Object &referenceObject,
           object_reference &reference) {
    constexpr DBTypes dbType = get_db_type<T>::value;
    const auto seed = static_cast<uint64_t>(std::time(nullptr));
    auto [k1,k2] = sampleKey(seed);
    SwissTable<SipHash, DBEntry<T>, std::vector<uint8_t> > db(SipHash(k1, k2, 2, 4));
    ttlQueueType ttlQueue;
    std::queue<std::string> dataQueue;
    std::mutex queueMutex;
    std::atomic<std::size_t> dataQueueCount;

    std::jthread reader([&instream, &dataQueue, &queueMutex, &dataQueueCount](std::stop_token stoken) {
        readThread(stoken, instream, queueMutex, dataQueue, dataQueueCount);
    });


    bool run = true;


    while (run) {
        if (!ttlQueue.empty()) {
            if (ttlQueue.top()->Invalid()) {
                ttlQueue.pop();
            } else if (ttlQueue.top()->Expired()) {
                auto key = ttlQueue.top()->key;
                auto entry = db.LookUp(std::move(key));
                entry.DeleteEntry();
                ttlQueue.pop();
            }
        }

        if (dataQueueCount.load() > 0) {
            std::lock_guard lock(queueMutex);
            if (dataQueueCount.load() == 0) {
                continue;
            }
            std::string line = std::move(dataQueue.front());
            dataQueue.pop();
            --dataQueueCount;

            auto [res, lineParsed] = parseLine(line);
            if (!res) {
                outstream << "Invalid input: " << line << std::endl;
                continue;
            }
            view_t view(lineParsed.begin(), lineParsed.end());

            OPType type = GetOPType(view);


            if (type == OPType::TERMINATE) {
                run = false;
                reader.request_stop();
                continue;
            }

            auto ttl = ParseTTL(view, defaultTTL);

            TargetType targetType;
            std::string targetValue;
            if (type != OPType::GET && type != OPType::DELETE && type != OPType::INSERT) {
                auto [targetTypeP, targetValueP] = ParseTarget(view);
                targetType = targetTypeP;
                targetValue = std::move(targetValueP);

                if (targetTypeP == TargetType::INVALID) {
                    outstream << "Invalid target" << std::endl;
                    continue;
                }
            }


            std::unique_ptr<Node<T> > toExecute;
            auto resMsg = ParseOperationRes::VALID;

            try{

                if constexpr (get_db_type<T>::value == DBTypes::INTEGER) {
                    if (type == OPType::GET) {
                        auto [msg, ptr] = ParseOneKeyOP<T, GetNode<T> >(view, outstream, db);
                        if (msg == ParseOperationRes::VALID) {
                            ptr.get()->Execute();
                            continue;
                        }
                        resMsg = msg;
                    } else if (type == OPType::DELETE) {
                        auto [msg, ptr] = ParseDelete<T>(view, db);
                        if (msg == ParseOperationRes::VALID) {
                            ptr.get()->Execute();
                            continue;
                        }
                        resMsg = msg;
                    } else if (type == OPType::INSERT) {
                        auto [msg, ptr] = ParseInsert<T>(view, db, ttl, ttlQueue);
                        if (msg == ParseOperationRes::VALID) {
                            ptr.get()->Execute();
                            continue;
                        }
                        resMsg = msg;
                    } else if (type == OPType::ADD) {
                        auto [msg, ptr] = ParseIntNode<DBType<T>, AddOp<Integer> >(view, db);
                        if (msg == ParseOperationRes::VALID) {
                            toExecute = std::move(ptr);
                        } else {
                            resMsg = msg;
                        }
                    } else if (type == OPType::SUBTRACT) {
                        auto [msg, ptr] = ParseIntNode<DBType<T>, SubtractOp<Integer> >(view, db);
                        if (msg == ParseOperationRes::VALID) {
                            toExecute = std::move(ptr);
                        } else {
                            resMsg = msg;
                        }
                    } else if (type == OPType::MULTIPLY) {
                        auto [msg, ptr] = ParseIntNode<DBType<T>, MultiplyOp<Integer> >(view, db);
                        if (msg == ParseOperationRes::VALID) {
                            toExecute = std::move(ptr);
                        } else {
                            resMsg = msg;
                        }
                    } else if (type == OPType::DIVIDE) {
                        auto [msg, ptr] = ParseIntNode<DBType<T>, DivideOp<Integer> >(view, db);
                        if (msg == ParseOperationRes::VALID) {
                            toExecute = std::move(ptr);
                        } else {
                            resMsg = msg;
                        }
                    } else {
                        resMsg = ParseOperationRes::INVALID_ARGS;
                    }
                }

                if constexpr (get_db_type<T>::value == DBTypes::FLOAT) {
                    if (type == OPType::GET) {
                        auto [msg, ptr] = ParseOneKeyOP<T, GetNode<T> >(view, outstream, db);
                        if (msg == ParseOperationRes::VALID) {
                            ptr.get()->Execute();
                            continue;
                        }
                        resMsg = msg;
                    } else if (type == OPType::DELETE) {
                        auto [msg, ptr] = ParseDelete<T>(view, db);
                        if (msg == ParseOperationRes::VALID) {
                            ptr.get()->Execute();
                            continue;
                        }
                        resMsg = msg;
                    } else if (type == OPType::INSERT) {
                        auto [msg, ptr] = ParseInsert<T>(view, db, ttl, ttlQueue);
                        if (msg == ParseOperationRes::VALID) {
                            ptr.get()->Execute();
                            continue;
                        }
                    } else if (type == OPType::FADD) {
                        auto [msg, ptr] = ParseFloatNode<DBType<T>, AddOp<Float> >(view, db);
                        if (msg == ParseOperationRes::VALID) {
                            toExecute = std::move(ptr);
                        } else {
                            resMsg = msg;
                        }
                    } else if (type == OPType::FSUBTRACT) {
                        auto [msg, ptr] = ParseFloatNode<DBType<T>, SubtractOp<Float> >(view, db);
                        if (msg == ParseOperationRes::VALID) {
                            toExecute = std::move(ptr);
                        } else {
                            resMsg = msg;
                        }
                    } else if (type == OPType::FMULTIPLY) {
                        auto [msg, ptr] = ParseFloatNode<DBType<T>, MultiplyOp<Float> >(view, db);
                        if (msg == ParseOperationRes::VALID) {
                            toExecute = std::move(ptr);
                        } else {
                            resMsg = msg;
                        }
                    } else if (type == OPType::FDIVIDE) {
                        auto [msg, ptr] = ParseFloatNode<DBType<T>, DivideOp<Float> >(view, db);
                        if (msg == ParseOperationRes::VALID) {
                            toExecute = std::move(ptr);
                        } else {
                            resMsg = msg;
                        }
                    } else {
                        resMsg = ParseOperationRes::INVALID_ARGS;
                    }
                }

                if constexpr (get_db_type<T>::value == DBTypes::STRING) {
                    if (type == OPType::GET) {
                        auto [msg, ptr] = ParseOneKeyOP<T, GetNode<T> >(view, outstream, db);
                        if (msg == ParseOperationRes::VALID) {
                            ptr.get()->Execute();
                            continue;
                        }
                        resMsg = msg;
                    } else if (type == OPType::DELETE) {
                        auto [msg, ptr] = ParseDelete<T>(view, db);
                        if (msg == ParseOperationRes::VALID) {
                            ptr.get()->Execute();
                            continue;
                        }
                        resMsg = msg;
                    } else if (type == OPType::INSERT) {
                        auto [msg, ptr] = ParseInsert<T>(view, db, ttl, ttlQueue);
                        if (msg == ParseOperationRes::VALID) {
                            ptr.get()->Execute();
                            continue;
                        }
                    } else if (type == OPType::CONCAT) {
                        auto [msg, ptr] = ParseStringOpStringString<DBType<T>, ConcatStrings>(view, db);
                        if (msg == ParseOperationRes::VALID) {
                            toExecute = std::move(ptr);
                        } else {
                            resMsg = msg;
                        }
                    } else if (type == OPType::SUFFIX) {
                        auto [msg, ptr] = ParseStringOpStringNum<DBType<T>, SuffixNode>(view, db);
                        if (msg == ParseOperationRes::VALID) {
                            toExecute = std::move(ptr);
                        } else {
                            resMsg = msg;
                        }
                    } else if (type == OPType::PREFIX) {
                        auto [msg, ptr] = ParseStringOpStringNum<DBType<T>, PrefixNode>(view, db);
                        if (msg == ParseOperationRes::VALID) {
                            toExecute = std::move(ptr);
                        } else {
                            resMsg = msg;
                        }
                    } else if (type == OPType::AT) {
                        auto [msg, ptr] = ParseStringOpStringNum<DBType<T>, AtNode>(view, db);
                        if (msg == ParseOperationRes::VALID) {
                            toExecute = std::move(ptr);
                        } else {
                            resMsg = msg;
                        }
                    } else if (type == OPType::FIND) {
                        if (TargetType::KEY == targetType) {
                            outstream << "Invalid target" << std::endl;
                            continue;
                        }
                        auto [msg, ptr] = ParseStringOpStringString<DBType<T>, FindNode>(view, db);
                        if (std::ofstream outFile(targetValue); !outFile.is_open()) {
                            resMsg = ParseOperationRes::COULD_NOT_OPEN_FILE;
                        } else if (msg == ParseOperationRes::VALID) {
                            std::unique_ptr<ToStreamNode<Vector<Integer>> > toExec = std::make_unique<ToStreamNode<Vector<Integer>> >(
                                outFile, std::move(ptr));
                            toExec->Execute();
                            continue;
                        } else {
                            resMsg = msg;
                        }
                    } else {
                        resMsg = ParseOperationRes::INVALID_ARGS;
                    }
                }

                if constexpr (get_db_type<T>::value == DBTypes::VECTOR_FLOAT) {
                    if (type == OPType::GET) {
                        auto [msg, ptr] = ParseOneKeyOP<T, GetNode<T> >(view, outstream, db);
                        if (msg == ParseOperationRes::VALID) {
                            ptr.get()->Execute();
                            continue;
                        }
                        resMsg = msg;
                    } else if (type == OPType::DELETE) {
                        auto [msg, ptr] = ParseDelete<T>(view, db);
                        if (msg == ParseOperationRes::VALID) {
                            ptr.get()->Execute();
                            continue;
                        }
                        resMsg = msg;
                    } else if (type == OPType::INSERT) {
                        auto [msg, ptr] = ParseInsert<T>(view, db, ttl, ttlQueue);
                        if (msg == ParseOperationRes::VALID) {
                            ptr.get()->Execute();
                            continue;
                        }
                        resMsg = msg;
                    } else if (type == OPType::APPEND) {
                        auto [msg, ptr] = ParseVectOpVectVal<DBType<T>, AppendNode<Float>, Vector<Float> >(view, db);
                        if (msg == ParseOperationRes::VALID) {
                            toExecute = std::move(ptr);
                        } else {
                            resMsg = msg;
                        }
                    } else if (type == OPType::POP) {
                        auto [msg, ptr] = ParseVectOpVect<DBType<T>, PopNode<Float>, Vector<Float> >(view, db);
                        if (msg == ParseOperationRes::VALID) {
                            toExecute = std::move(ptr);
                        } else {
                            resMsg = msg;
                        }
                    } else if (type == OPType::VINSERT) {
                        auto [msg, ptr] = ParseVectOpVectNumVal<DBType<T>, VInsertNode<Float>, Vector<Float> >(view, db);
                        if (msg == ParseOperationRes::VALID) {
                            toExecute = std::move(ptr);
                        } else {
                            resMsg = msg;
                        }
                    } else if (type == OPType::REMOVE) {
                        auto [msg, ptr] = ParseVectOpVectNum<DBType<T>, RemoveNode<Float>, Vector<Float> >(view, db);
                        if (msg == ParseOperationRes::VALID) {
                            toExecute = std::move(ptr);
                        } else {
                            resMsg = msg;
                        }
                    } else if (type == OPType::SET) {
                        auto [msg, ptr] = ParseVectOpVectNumVal<DBType<T>, SetNode<Float>, Vector<Float> >(view, db);
                        if (msg == ParseOperationRes::VALID) {
                            toExecute = std::move(ptr);
                        } else {
                            resMsg = msg;
                        }
                    } else if (type == OPType::VGET) {
                        std::unique_ptr<Node<Float> > resGet;
                        auto msgGet = ParseOperationRes::INVALID_ARGS;
                        auto [msg, ptr] = ParseVectOpVectNum<DBType<T>, VgetNode<Float>, Vector<Float> >(view, db);
                        if (msg == ParseOperationRes::VALID) {
                            resGet = std::move(ptr);
                            msgGet = msg;
                        } else {
                            resMsg = msg;
                        }

                        if (TargetType::KEY == targetType) {
                            outstream << "Invalid target" << std::endl;
                            continue;
                        }
                        if (std::ofstream outFile(targetValue); !outFile.is_open()) {
                            resMsg = ParseOperationRes::COULD_NOT_OPEN_FILE;
                        } else if (msgGet == ParseOperationRes::VALID) {
                            auto toExec =
                                    std::make_unique<ToStreamNode<Float> >(outFile, std::move(resGet));
                            toExec->Execute();
                            continue;
                        }
                    } else if (type == OPType::SIZE) {
                        std::unique_ptr<Node<Integer> > resSize;
                        auto msgSize = ParseOperationRes::INVALID_ARGS;
                        auto [msg, ptr] = ParseVectOpVect<DBType<T>, SizeNode<Float>, Vector<Float> >(view, db);
                        if (msg == ParseOperationRes::VALID) {
                            resSize = std::move(ptr);
                            msgSize = msg;
                        } else {
                            resMsg = msg;
                        }

                        if (TargetType::KEY == targetType) {
                            outstream << "Invalid target" << std::endl;
                            continue;
                        }
                        if (std::ofstream outFile(targetValue); !outFile.is_open()) {
                            resMsg = ParseOperationRes::COULD_NOT_OPEN_FILE;
                        } else if (msgSize == ParseOperationRes::VALID) {
                            auto toExec =
                                    std::make_unique<ToStreamNode<Integer> >(outFile, std::move(resSize));
                            toExec->Execute();
                            continue;
                        }
                    } else {
                        resMsg = ParseOperationRes::INVALID_ARGS;
                    }
                } else if constexpr (get_db_type<T>::value == DBTypes::VECTOR_INT) {
                    if (type == OPType::GET) {
                        auto [msg, ptr] = ParseOneKeyOP<T, GetNode<T> >(view, outstream, db);
                        if (msg == ParseOperationRes::VALID) {
                            ptr.get()->Execute();
                            continue;
                        }
                        resMsg = msg;
                    } else if (type == OPType::DELETE) {
                        auto [msg, ptr] = ParseDelete<T>(view, db);
                        if (msg == ParseOperationRes::VALID) {
                            ptr.get()->Execute();
                            continue;
                        }
                        resMsg = msg;
                    } else if (type == OPType::INSERT) {
                        auto [msg, ptr] = ParseInsert<T>(view, db, ttl, ttlQueue);
                        if (msg == ParseOperationRes::VALID) {
                            ptr.get()->Execute();
                            continue;
                        }
                        resMsg = msg;
                    } else if (type == OPType::APPEND) {
                        auto [msg, ptr] = ParseVectOpVectVal<DBType<T>, AppendNode<Integer>, Vector<Integer> >(view, db);
                        if (msg == ParseOperationRes::VALID) {
                            toExecute = std::move(ptr);
                        } else {
                            resMsg = msg;
                        }
                    } else if (type == OPType::POP) {
                        auto [msg, ptr] = ParseVectOpVect<DBType<T>, PopNode<Integer>, Vector<Integer> >(view, db);
                        if (msg == ParseOperationRes::VALID) {
                            toExecute = std::move(ptr);
                        } else {
                            resMsg = msg;
                        }
                    } else if (type == OPType::VINSERT) {
                        auto [msg, ptr] = ParseVectOpVectNumVal<DBType<T>, VInsertNode<Integer>, Vector<
                            Integer> >(view, db);
                        if (msg == ParseOperationRes::VALID) {
                            toExecute = std::move(ptr);
                        } else {
                            resMsg = msg;
                        }
                    } else if (type == OPType::REMOVE) {
                        auto [msg, ptr] = ParseVectOpVectNum<DBType<T>, RemoveNode<Integer>, Vector<Integer> >(view, db);
                        if (msg == ParseOperationRes::VALID) {
                            toExecute = std::move(ptr);
                        } else {
                            resMsg = msg;
                        }
                    } else if (type == OPType::SET) {
                        auto [msg, ptr] = ParseVectOpVectNumVal<DBType<T>, SetNode<Integer>, Vector<Integer> >(view, db);
                        if (msg == ParseOperationRes::VALID) {
                            toExecute = std::move(ptr);
                        } else {
                            resMsg = msg;
                        }
                    } else if (type == OPType::VGET) {
                        std::unique_ptr<Node<Integer> > resGet;
                        auto msgGet = ParseOperationRes::INVALID_ARGS;
                        auto [msg, ptr] = ParseVectOpVectNum<DBType<T>, VgetNode<Integer>, Vector<Integer> >(view, db);
                        if (msg == ParseOperationRes::VALID) {
                            resGet = std::move(ptr);
                            msgGet = msg;
                        } else {
                            resMsg = msg;
                        }

                        if (TargetType::KEY == targetType) {
                            outstream << "Invalid target" << std::endl;
                            continue;
                        }
                        if (std::ofstream outFile(targetValue); !outFile.is_open()) {
                            resMsg = ParseOperationRes::COULD_NOT_OPEN_FILE;
                        } else if (msgGet == ParseOperationRes::VALID) {
                            auto toExec =
                                    std::make_unique<ToStreamNode<Integer> >(outFile, std::move(resGet));
                            toExec->Execute();
                            continue;
                        }
                    } else if (type == OPType::SIZE) {
                        std::unique_ptr<Node<Integer> > resSize;
                        auto msgSize = ParseOperationRes::INVALID_ARGS;
                        auto [msg, ptr] = ParseVectOpVect<DBType<T>, SizeNode<Integer>, Vector<Integer> >(view, db);
                        if (msg == ParseOperationRes::VALID) {
                            resSize = std::move(ptr);
                            msgSize = msg;
                        } else {
                            resMsg = msg;
                        }

                        if (TargetType::KEY == targetType) {
                            outstream << "Invalid target" << std::endl;
                            continue;
                        }
                        if (std::ofstream outFile(targetValue); !outFile.is_open()) {
                            resMsg = ParseOperationRes::COULD_NOT_OPEN_FILE;
                        } else if (msgSize == ParseOperationRes::VALID) {
                            auto toExec =
                                    std::make_unique<ToStreamNode<Integer> >(outFile, std::move(resSize));
                            toExec->Execute();
                            continue;
                        }
                    } else {
                        resMsg = ParseOperationRes::INVALID_ARGS;
                    }
                } else if constexpr (get_db_type<T>::value == DBTypes::VECTOR_STRING) {
                    if (type == OPType::GET) {
                        auto [msg, ptr] = ParseOneKeyOP<T, GetNode<T> >(view, outstream, db);
                        if (msg == ParseOperationRes::VALID) {
                            ptr.get()->Execute();
                            continue;
                        }
                        resMsg = msg;
                    } else if (type == OPType::DELETE) {
                        auto [msg, ptr] = ParseDelete<T>(view, db);
                        if (msg == ParseOperationRes::VALID) {
                            ptr.get()->Execute();
                            continue;
                        }
                        resMsg = msg;
                    } else if (type == OPType::INSERT) {
                        auto [msg, ptr] = ParseInsert<T>(view, db, ttl, ttlQueue);
                        if (msg == ParseOperationRes::VALID) {
                            ptr.get()->Execute();
                            continue;
                        }
                        resMsg = msg;
                    } else if (type == OPType::APPEND) {
                        auto [msg, ptr] = ParseVectOpVectVal<DBType<T>, AppendNode<String>, Vector<String> >(view, db);
                        if (msg == ParseOperationRes::VALID) {
                            toExecute = std::move(ptr);
                        } else {
                            resMsg = msg;
                        }
                    } else if (type == OPType::POP) {
                        auto [msg, ptr] = ParseVectOpVect<DBType<T>, PopNode<String>, Vector<String> >(view, db);
                        if (msg == ParseOperationRes::VALID) {
                            toExecute = std::move(ptr);
                        } else {
                            resMsg = msg;
                        }
                    } else if (type == OPType::VINSERT) {
                        auto [msg, ptr] = ParseVectOpVectNumVal<DBType<T>, VInsertNode<String>, Vector<String> >(view, db);
                        if (msg == ParseOperationRes::VALID) {
                            toExecute = std::move(ptr);
                        } else {
                            resMsg = msg;
                        }
                    } else if (type == OPType::REMOVE) {
                        auto [msg, ptr] = ParseVectOpVectNum<DBType<T>, RemoveNode<String>, Vector<String> >(view, db);
                        if (msg == ParseOperationRes::VALID) {
                            toExecute = std::move(ptr);
                        } else {
                            resMsg = msg;
                        }
                    } else if (type == OPType::SET) {
                        auto [msg, ptr] = ParseVectOpVectNumVal<DBType<T>, SetNode<String>, Vector<String> >(view, db);
                        if (msg == ParseOperationRes::VALID) {
                            toExecute = std::move(ptr);
                        } else {
                            resMsg = msg;
                        }
                    } else if (type == OPType::VGET) {
                        std::unique_ptr<Node<String> > resGet;
                        auto msgGet = ParseOperationRes::INVALID_ARGS;
                        auto [msg, ptr] = ParseVectOpVectNum<DBType<T>, VgetNode<String>, Vector<String> >(view, db);
                        if (msg == ParseOperationRes::VALID) {
                            resGet = std::move(ptr);
                            msgGet = msg;
                        } else {
                            resMsg = msg;
                        }

                        if (TargetType::KEY == targetType) {
                            outstream << "Invalid target" << std::endl;
                            continue;
                        }
                        if (std::ofstream outFile(targetValue); !outFile.is_open()) {
                            resMsg = ParseOperationRes::COULD_NOT_OPEN_FILE;
                        } else if (msgGet == ParseOperationRes::VALID) {
                            auto toExec =
                                    std::make_unique<ToStreamNode<String> >(outFile, std::move(resGet));
                            toExec->Execute();
                            continue;
                        }
                    } else if (type == OPType::SIZE) {
                        std::unique_ptr<Node<Integer> > resSize;
                        auto msgSize = ParseOperationRes::INVALID_ARGS;
                        auto [msg, ptr] = ParseVectOpVect<DBType<T>, SizeNode<String>, Vector<String> >(view, db);
                        if (msg == ParseOperationRes::VALID) {
                            resSize = std::move(ptr);
                            msgSize = msg;
                        } else {
                            resMsg = msg;
                        }

                        if (TargetType::KEY == targetType) {
                            outstream << "Invalid target" << std::endl;
                            continue;
                        }
                        if (std::ofstream outFile(targetValue); !outFile.is_open()) {
                            resMsg = ParseOperationRes::COULD_NOT_OPEN_FILE;
                        } else if (msgSize == ParseOperationRes::VALID) {
                            auto toExec =
                                    std::make_unique<ToStreamNode<Integer> >(outFile, std::move(resSize));
                            toExec->Execute();
                            continue;
                        }
                    } else {
                        resMsg = ParseOperationRes::INVALID_ARGS;
                    }
                } else if constexpr (get_db_type<T>::value == DBTypes::OBJECT) {
                    if (type == OPType::GET) {
                        auto [msg, ptr] = ParseOneKeyOP<T, GetNode<T> >(view, outstream, db);
                        if (msg == ParseOperationRes::VALID) {
                            ptr.get()->Execute();
                            continue;
                        }
                        resMsg = msg;
                    } else if (type == OPType::DELETE) {
                        auto [msg, ptr] = ParseDelete<T>(view, db);
                        if (msg == ParseOperationRes::VALID) {
                            ptr.get()->Execute();
                            continue;
                        }
                        resMsg = msg;
                    } else if (type == OPType::INSERT) {
                        auto [msg, ptr] = ParseInsert(view, db, ttl, ttlQueue, reference, referenceObject);
                        if (msg == ParseOperationRes::VALID) {
                            ptr.get()->Execute();
                            continue;
                        }
                        resMsg = msg;
                    } else if (type == OPType::OGET) {
                        if (targetType == TargetType::KEY || targetType == TargetType::INVALID) {
                            resMsg = ParseOperationRes::INVALID_ARGS;
                        } else if (std::ofstream outFile(targetValue); !outFile.is_open()) {
                            resMsg = ParseOperationRes::COULD_NOT_OPEN_FILE;
                        } else {
                            auto [msg, ptr] = ParseObjectGet<DBType<T> >(view, db, reference, outFile);
                            if (msg == ParseOperationRes::VALID) {
                                ptr.get()->Execute();
                                continue;
                            }
                            resMsg = msg;
                        }
                    } else if (type == OPType::OSET) {
                        auto [resMsgParse, ptrObject, fieldT, fieldN] = ParseObjectSet(view, db, reference, outstream);
                        auto parseValue = ParseArg(view);
                        std::unique_ptr<DBopsNodeBase> ptrOp;

                        if (resMsgParse != ParseOperationRes::VALID) {
                            resMsg = resMsgParse;
                        } else if (fieldT == ObjectFieldTypes::INTEGER) {
                            auto [valMsg, valPtr] = getInteger(db, std::move(parseValue));

                            if (valMsg != GetArgsReturnTypes::VALID) {
                                resMsg = ParseOperationRes::INVALID_ARGS;
                            } else if (targetType == TargetType::KEY) {
                                auto keyBytes = stringBytes(std::move(targetValue));
                                auto ptrEval = std::make_unique<ObjectSetNode<Integer> >(
                                    std::move(ptrObject), fieldN, std::move(valPtr));
                                if (ttl.second == TTLTypes::IMMORTAL) {
                                    ptrOp = std::make_unique<InsertNode<T, ImmortalSentinel, ttlQueueType> >(
                                        db, std::move(keyBytes), std::move(ptrEval), std::move(ImmortalSentinel()),
                                        ttlQueue);
                                    ptrOp->Execute();
                                    continue;
                                }
                                if (ttl.second == TTLTypes::MILLISECONDS) {
                                    ptrOp = std::make_unique<InsertNode<T, std::chrono::milliseconds, ttlQueueType> >(
                                        db, std::move(keyBytes), std::move(ptrEval),
                                        std::move(std::get<std::chrono::milliseconds>(ttl.first)), ttlQueue);
                                    ptrOp->Execute();
                                    continue;
                                }
                                if (ttl.second == TTLTypes::SECONDS) {
                                    ptrOp = std::make_unique<InsertNode<T, std::chrono::seconds, ttlQueueType> >(
                                        db, std::move(keyBytes), std::move(ptrEval),
                                        std::move(std::get<std::chrono::seconds>(ttl.first)), ttlQueue);
                                    ptrOp->Execute();
                                    continue;
                                }
                                if (ttl.second == TTLTypes::MINUTES) {
                                    ptrOp = std::make_unique<InsertNode<T, std::chrono::minutes, ttlQueueType> >(
                                        db, std::move(keyBytes), std::move(ptrEval),
                                        std::move(std::get<std::chrono::minutes>(ttl.first)), ttlQueue);
                                    ptrOp->Execute();
                                    continue;
                                }
                            } else if (targetType == TargetType::FILE) {
                                if (std::ofstream outFile(targetValue); !outFile.is_open()) {
                                    resMsg = ParseOperationRes::COULD_NOT_OPEN_FILE;
                                } else {
                                    auto ptrEval = std::make_unique<ObjectSetNode<Integer> >(
                                        std::move(ptrObject), fieldN, std::move(valPtr));

                                    ptrOp = std::make_unique<ToStreamNode<Object> >(
                                        outFile, std::move(ptrEval));

                                    ptrOp->Execute();
                                    continue;
                                }
                            }
                        } else if (fieldT == ObjectFieldTypes::FLOAT) {
                            auto [valMsg, valPtr] = getFloat(db, std::move(parseValue));

                            if (valMsg != GetArgsReturnTypes::VALID) {
                                resMsg = ParseOperationRes::INVALID_ARGS;
                            } else if (targetType == TargetType::KEY) {
                                auto keyBytes = stringBytes(std::move(targetValue));
                                auto ptrEval = std::make_unique<ObjectSetNode<Float> >(
                                    std::move(ptrObject), fieldN, std::move(valPtr));
                                if (ttl.second == TTLTypes::IMMORTAL) {
                                    ptrOp = std::make_unique<InsertNode<T, ImmortalSentinel, ttlQueueType> >(
                                        db, std::move(keyBytes), std::move(ptrEval), std::move(ImmortalSentinel()),
                                        ttlQueue);
                                    ptrOp->Execute();
                                    continue;
                                }
                                if (ttl.second == TTLTypes::MILLISECONDS) {
                                    ptrOp = std::make_unique<InsertNode<T, std::chrono::milliseconds, ttlQueueType> >(
                                        db, std::move(keyBytes), std::move(ptrEval),
                                        std::move(std::get<std::chrono::milliseconds>(ttl.first)), ttlQueue);
                                    ptrOp->Execute();
                                    continue;
                                }
                                if (ttl.second == TTLTypes::SECONDS) {
                                    ptrOp = std::make_unique<InsertNode<T, std::chrono::seconds, ttlQueueType> >(
                                        db, std::move(keyBytes), std::move(ptrEval),
                                        std::move(std::get<std::chrono::seconds>(ttl.first)), ttlQueue);
                                    ptrOp->Execute();
                                    continue;
                                }
                                if (ttl.second == TTLTypes::MINUTES) {
                                    ptrOp = std::make_unique<InsertNode<T, std::chrono::minutes, ttlQueueType> >(
                                        db, std::move(keyBytes), std::move(ptrEval),
                                        std::move(std::get<std::chrono::minutes>(ttl.first)), ttlQueue);
                                    ptrOp->Execute();
                                    continue;
                                }
                            } else if (targetType == TargetType::FILE) {
                                if (std::ofstream outFile(targetValue); !outFile.is_open()) {
                                    resMsg = ParseOperationRes::COULD_NOT_OPEN_FILE;
                                } else {
                                    auto ptrEval = std::make_unique<ObjectSetNode<Float> >(
                                        std::move(ptrObject), fieldN, std::move(valPtr));

                                    ptrOp = std::make_unique<ToStreamNode<Object> >(
                                        outFile, std::move(ptrEval));

                                    ptrOp->Execute();
                                    continue;
                                }
                            }
                        } else if (fieldT == ObjectFieldTypes::STRING) {
                            auto [valMsg, valPtr] = getString(db, std::move(parseValue));

                            if (valMsg != GetArgsReturnTypes::VALID) {
                                resMsg = ParseOperationRes::INVALID_ARGS;
                            } else if (targetType == TargetType::KEY) {
                                auto keyBytes = stringBytes(std::move(targetValue));
                                auto ptrEval = std::make_unique<ObjectSetNode<String> >(
                                    std::move(ptrObject), fieldN, std::move(valPtr));
                                if (ttl.second == TTLTypes::IMMORTAL) {
                                    ptrOp = std::make_unique<InsertNode<T, ImmortalSentinel, ttlQueueType> >(
                                        db, std::move(keyBytes), std::move(ptrEval), std::move(ImmortalSentinel()),
                                        ttlQueue);
                                    ptrOp->Execute();
                                    continue;
                                }
                                if (ttl.second == TTLTypes::MILLISECONDS) {
                                    ptrOp = std::make_unique<InsertNode<T, std::chrono::milliseconds, ttlQueueType> >(
                                        db, std::move(keyBytes), std::move(ptrEval),
                                        std::move(std::get<std::chrono::milliseconds>(ttl.first)), ttlQueue);
                                    ptrOp->Execute();
                                    continue;
                                }
                                if (ttl.second == TTLTypes::SECONDS) {
                                    ptrOp = std::make_unique<InsertNode<T, std::chrono::seconds, ttlQueueType> >(
                                        db, std::move(keyBytes), std::move(ptrEval),
                                        std::move(std::get<std::chrono::seconds>(ttl.first)), ttlQueue);
                                    ptrOp->Execute();
                                    continue;
                                }
                                if (ttl.second == TTLTypes::MINUTES) {
                                    ptrOp = std::make_unique<InsertNode<T, std::chrono::minutes, ttlQueueType> >(
                                        db, std::move(keyBytes), std::move(ptrEval),
                                        std::move(std::get<std::chrono::minutes>(ttl.first)), ttlQueue);
                                    ptrOp->Execute();
                                    continue;
                                }
                            } else if (targetType == TargetType::FILE) {
                                if (std::ofstream outFile(targetValue); !outFile.is_open()) {
                                    resMsg = ParseOperationRes::COULD_NOT_OPEN_FILE;
                                } else {
                                    auto ptrEval = std::make_unique<ObjectSetNode<String> >(
                                        std::move(ptrObject), fieldN, std::move(valPtr));

                                    ptrOp = std::make_unique<ToStreamNode<Object> >(
                                        outFile, std::move(ptrEval));

                                    ptrOp->Execute();
                                    continue;
                                }
                            }
                        }
                    } else {
                        resMsg = ParseOperationRes::INVALID_ARGS;
                    }
                }


                std::unique_ptr<DBopsNodeBase> opNode;
                std::ofstream outFile;
                if (resMsg != ParseOperationRes::VALID) {
                    opNode = GetError(resMsg, GetArgCount(type), view.size(), outstream, "");
                } else if (ValidateOPType(type, dbType) && targetType == TargetType::KEY) {
                    auto key = stringBytes(std::move(targetValue));
                    if (ttl.second == TTLTypes::MILLISECONDS) {
                        opNode = std::make_unique<InsertNode<T, std::chrono::milliseconds, ttlQueueType> >(
                            db, std::move(key), std::move(toExecute),
                            std::get<std::chrono::milliseconds>(std::move(ttl.first)), ttlQueue);
                    } else if (ttl.second == TTLTypes::SECONDS) {
                        opNode = std::make_unique<InsertNode<T, std::chrono::seconds, ttlQueueType> >(
                            db, std::move(key), std::move(toExecute), std::get<std::chrono::seconds>(std::move(ttl.first)),
                            ttlQueue);
                    } else if (ttl.second == TTLTypes::MINUTES) {
                        opNode = std::make_unique<InsertNode<T, std::chrono::minutes, ttlQueueType> >(
                            db, std::move(key), std::move(toExecute), std::get<std::chrono::minutes>(std::move(ttl.first)),
                            ttlQueue);
                    } else {
                        opNode = std::make_unique<InsertNode<T, ImmortalSentinel, ttlQueueType> >(
                            db, std::move(key), std::move(toExecute), ImmortalSentinel(), ttlQueue);
                    }
                } else if (targetType == TargetType::FILE) {
                    outFile = std::ofstream{targetValue};
                    if ( !outFile.is_open()) {
                        std::vector<std::string> err = {"Could not open file: ", targetValue};
                        opNode = std::make_unique<ErrorNode>(outstream, std::move(err));
                    } else {
                        opNode = std::make_unique<ToStreamNode<T> >(outFile, std::move(toExecute));
                    }
                } else {
                    std::vector<std::string> err = {"Error"};
                    opNode = std::make_unique<ErrorNode>(outstream, std::move(err));
                }
                opNode->Execute();
            }
            catch (const char *e) {
                outstream << e << std::endl;
            }
            catch (std::out_of_range &e) {
                outstream << e.what() << std::endl;
            }
        }
    }
}

#endif //PROJECT_RUNNER_HPP
