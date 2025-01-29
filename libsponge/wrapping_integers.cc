#include "wrapping_integers.hh"

WrappingInt32 wrap(uint64_t n, WrappingInt32 isn){
    uint64_t mod = static_cast<uint64_t>(1)<<32;
    return WrappingInt32(isn+static_cast<uint32_t>(n%mod));
}

uint64_t unwrap(WrappingInt32 n, WrappingInt32 isn, uint64_t checkpoint){
    uint32_t diff32 = n - isn;
    uint64_t diff = static_cast<uint64_t>(diff32);
    uint64_t base = static_cast<uint64_t>(1)<<32;
    if(checkpoint<=diff)return diff;
    uint64_t t = (checkpoint - diff) / base;
    uint64_t candidate1 = t*base+diff, candidate2 = (t+1)*base+diff;
    return checkpoint-candidate1<=candidate2-checkpoint ? candidate1 : candidate2;
}
