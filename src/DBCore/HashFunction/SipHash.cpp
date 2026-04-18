#include "SipHash.hpp"


return_val RotateLeft(const return_val val, const std::size_t count) {
    return (val << count) | (val >> (64 - count));
}

return_val Get64bitLE(std::vector<uint8_t>::iterator &data) {
    return_val res = 0;
    for (std::ptrdiff_t i = 0; i < 8; ++i) {
        res = res | (*data << (i * 8));
        ++data;
    }
    return res;
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

void internalState::ApplySipRounds(std::size_t c) {
    for (std::size_t i = 0; i < c; ++i) {
        SipRound();
    }
}

void internalState::Compress(const std::size_t c, std::vector<uint8_t> data) {
    const std::size_t data_size = data.size();
    const std::size_t mod = (data_size + 1) % 8;
    if (mod == 0) {
        data.push_back(data_size % 256);
    } else {
        for (std::size_t i = mod; i < 8; ++i) {
            data.push_back(0);
        }
        data.push_back(data_size % 256);
    }

    const std::size_t nSize = data.size();
    auto it = data.begin();
    for (std::size_t i = 0; i < nSize; ++i) {
        const auto m = Get64bitLE(it);
        v3 = v3 ^ m;
        ApplySipRounds(c);

        v0 = v0 ^ m;
    }
}

void internalState::Finalize(std::size_t d) {
    v2 = v2 ^ 0xff;

    ApplySipRounds(d);
}

internalState initVals(const std::uint64_t k0, const std::uint64_t k1) {
    return {k0 ^ initConst1, k1 ^ initConst2, k0 ^ initConst3, k0 ^ initConst4};
}
