#pragma once
#include "byte_stream.hh"
#include "stream_reassembler.hh"
#include "tcp_segment.hh"
#include "wrapping_integers.hh"

#include <optional>

class TCPReceiver {
    StreamReassembler _reassembler;

    size_t _capacity;

    bool _synReceived;

    bool _finReceived;

    WrappingInt32 _isn;

    uint64_t _checkPoint;

  public:
    TCPReceiver(const size_t capacity)
        : _reassembler(capacity), _capacity(capacity), _synReceived(false), _finReceived(false), _isn(0), _checkPoint(0) {}

    std::optional<WrappingInt32> ackno() const;

    size_t window_size() const;
    
    size_t unassembled_bytes() const;

    void segment_received(const TCPSegment &seg);

    ByteStream &stream_out();

    const ByteStream &stream_out() const;
};