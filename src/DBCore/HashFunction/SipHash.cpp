#include "SipHash.hpp"


return_val RotateLeft(const return_val val, const std::size_t count) {
    return (val << count) | (val >> (64 - count));
}

internalState::internalState(const return_val v0, const return_val v1, const return_val v2,
                             const return_val v3) : v0(v0), v1(v1), v2(v2),
                                                    v3(v3) {
}

void internalState::SipRound() {
    v0 += v1;
    v1 = RotateLeft(v1, 13);
    v1 = v1 ^ v0;
    v0 = RotateLeft(v0, 32);
    v2 += v3;
    v3 = RotateLeft(v3, 16);
    v3 = v3 ^ v2;

    v2 += v1;
    v1 = RotateLeft(v1, 17);
    v1 = v1 ^ v2;
    v2 = RotateLeft(v2, 32);
    v0 += v3;
    v3 = RotateLeft(v3, 21);
    v3 = v3 ^ v0;
}

internalState initVals(const std::uint64_t k0, const std::uint64_t k1) {
    return {k0 ^ initConst1, k1 ^ initConst2, k0 ^ initConst3, k0 ^ initConst4};
}
