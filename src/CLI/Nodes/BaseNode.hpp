#ifndef PROJECT_BASENODE_HPP
#define PROJECT_BASENODE_HPP

class BaseNode {
public:
    virtual ~BaseNode() = default;
};

using BaseNodePtr = std::unique_ptr<BaseNode>;

#endif //PROJECT_BASENODE_HPP
