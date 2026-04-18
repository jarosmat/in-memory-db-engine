#ifndef PROJECT_SipHash_HPP
#define PROJECT_SipHash_HPP
#include <cstdint>
#include <vector>

using return_val = std::uint64_t;


return_val RotateLeft(return_val val, std::size_t count);

return_val Get64bit(std::vector<uint8_t>::iterator data);

struct internalState {
    void Compress(std::size_t c, std::vector<uint8_t> &&data);

    void Finalize(std::size_t d);

    return_val GetValue() const;

    internalState(std::size_t k0, std::size_t k1);

private:
    void SipRound();

    void ApplySipRounds(std::size_t count);

    return_val v0;
    return_val v1;
    return_val v2;
    return_val v3;
    static constexpr return_val initConst1 = 0x736f6d6570736575;
    static constexpr return_val initConst2 = 0x646f72616e646f6d;
    static constexpr return_val initConst3 = 0x6c7967656e657261;
    static constexpr return_val initConst4 = 0x7465646279746573;
};

class SipHash {
    std::size_t c;
    std::size_t d;
    std::size_t k0;
    std::size_t k1;

public:
    //k0 are the least significant bytes
    SipHash(std::uint64_t k0, std::uint64_t k1, std::size_t c, std::size_t d);

    return_val operator()(std::vector<uint8_t> &&data) const;
};


#endif //PROJECT_SipHash_HPP
