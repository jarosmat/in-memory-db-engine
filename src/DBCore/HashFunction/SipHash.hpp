#ifndef PROJECT_SipHash_HPP
#define PROJECT_SipHash_HPP
#include <cstdint>

using return_val = std::uint64_t;

constexpr return_val initConst1 = 0x736f6d6570736575;
constexpr return_val initConst2 = 0x646f72616e646f6d;
constexpr return_val initConst3 = 0x6c7967656e657261;
constexpr return_val initConst4 = 0x7465646279746573;

struct internalState {
    return_val v0;
    return_val v1;
    return_val v2;
    return_val v3;
    internalState(return_val v0, return_val v1, return_val v2, return_val v3);
};

internalState initVals(std::uint64_t k0, std::uint64_t k1);



#endif //PROJECT_SipHash_HPP
