#pragma once

#include <cstdint>
#include <ostream>

class WrappingInt32 {
  private:
    uint32_t _raw_value; 

  public:
    explicit WrappingInt32(uint32_t raw_value) : _raw_value(raw_value) {}

    uint32_t raw_value() const { return _raw_value; }
};

WrappingInt32 wrap(uint64_t n, WrappingInt32 isn);

uint64_t unwrap(WrappingInt32 n, WrappingInt32 isn, uint64_t checkpoint);

inline int32_t operator-(WrappingInt32 a, WrappingInt32 b) { return a.raw_value() - b.raw_value(); }

inline bool operator==(WrappingInt32 a, WrappingInt32 b) { return a.raw_value() == b.raw_value(); }

inline bool operator!=(WrappingInt32 a, WrappingInt32 b) { return !(a == b); }

inline std::ostream &operator<<(std::ostream &os, WrappingInt32 a) { return os << a.raw_value(); }

inline WrappingInt32 operator+(WrappingInt32 a, uint32_t b) { return WrappingInt32{a.raw_value() + b}; }

inline WrappingInt32 operator-(WrappingInt32 a, uint32_t b) { return a + -b; }