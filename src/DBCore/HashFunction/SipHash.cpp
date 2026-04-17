#include "SipHash.hpp"

internalState::internalState(const return_val v0, const return_val v1, const return_val v2,
                             const return_val v3) : v0(v0), v1(v1), v2(v2),
                                                    v3(v3) {
}

internalState initVals(const std::uint64_t k0, const std::uint64_t k1) {
    return {k0 ^ initConst1, k1 ^ initConst2, k0 ^ initConst3, k0 ^ initConst4};
}
