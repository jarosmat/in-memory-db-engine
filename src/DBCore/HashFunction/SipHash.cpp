#include "SipHash.hpp"


return_val RotateLeft(const return_val val, const std::size_t count) {
    return (val << count) | (val >> (64 - count));
}

return_val Get64bitLE(std::vector<uint8_t>::iterator &data) {
    return_val res = 0;
    for (std::ptrdiff_t i = 0; i < 8; ++i) {
        const return_val dataT = *data;
        res = res | (dataT << (i * 8));
        ++data;
    }
    return res;
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

void internalState::ApplySipRounds(const std::size_t count) {
    for (std::size_t i = 0; i < count; ++i) {
        SipRound();
    }
}

void internalState::Compress(const std::size_t c, std::vector<uint8_t> &&data) {
    const std::size_t data_size = data.size();
    const std::size_t mod = (data_size + 1) & 7;
    if (mod == 0) {
        data.push_back(data_size & 255);
    } else {
        for (std::size_t i = mod; i < 8; ++i) {
            data.push_back(0);
        }
        data.push_back(data_size & 255);
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

return_val internalState::GetValue() const {
    return v0 ^ v1 ^ v2 ^ v3;
}

SipHash::SipHash(const std::size_t k0, const std::size_t k1, const std::size_t c,
                 const std::size_t d) : c(c), d(d), k0(k0), k1(k1) {
}

return_val SipHash::operator()(std::vector<uint8_t> &&data) const {
    auto state = internalState(k0, k1);
    state.Compress(c, std::move(data));
    state.Finalize(d);

    return state.GetValue();
}

internalState::internalState(const std::uint64_t k0, const std::uint64_t k1) : v0(k0 ^ initConst1), v1(k1 ^ initConst2),
                                                                               v2(k0 ^ initConst3),
                                                                               v3(k1 ^ initConst4) {
}
