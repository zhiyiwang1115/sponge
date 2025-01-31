#pragma once
#include "byte_stream.hh"
#include "tcp_config.hh"
#include "tcp_segment.hh"
#include "wrapping_integers.hh"
#include <queue>

class TCPSender{
    private:
        WrappingInt32 _isn;

        std::queue<TCPSegment> _segments_out{};

        std::queue<TCPSegment> _segments_out_copy{};

        size_t _bytes_in_flight{0};

        unsigned int _initial_retransmission_timeout;

        unsigned int _timeout;

        ByteStream _stream;

        uint64_t _next_seqno{0};

        unsigned int _consecutive_retransmissions{0};

        uint64_t _receiver_window{1};

        bool isSyn{false};

        bool isFin{false};

        size_t sentTime{0};

        size_t accumulatedTime{0};

        void updateTime();

        bool windowZero{false};

    public:
        TCPSender(const size_t capacity = TCPConfig::DEFAULT_CAPACITY,
              const uint16_t retx_timeout = TCPConfig::TIMEOUT_DFLT,
              const std::optional<WrappingInt32> fixed_isn = {});

        ByteStream &stream_in() { return _stream; }
        const ByteStream &stream_in() const { return _stream; }
            
        void fill_window();

        void ack_received(const WrappingInt32 ackno, const uint16_t window_size);

        void tick(const size_t ms_since_last_tick);

        void send_empty_segment();      

        size_t bytes_in_flight() const;

        std::queue<TCPSegment> &segments_out() { return _segments_out; }
        
        uint64_t next_seqno_absolute() const { return _next_seqno; }

        WrappingInt32 next_seqno() const { return wrap(_next_seqno, _isn); }

        unsigned int consecutive_retransmissions() const;
  
};