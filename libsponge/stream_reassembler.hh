#include <string>
#include <deque>
#include "byte_stream.hh"
#include <cstdint>
#pragma once

class StreamReassembler{
    public:
        StreamReassembler(const size_t capacity);

        void push_substring(const std::string &data, const uint64_t index, const bool eof);

        ByteStream& stream_out();

        const ByteStream& stream_out() const;

        size_t unassembled_bytes() const;

        bool empty() const;

        uint64_t getNextIdx() const{return nextIdx_;};

        bool getFlag() const{return flag_;};
    private:
        size_t capacity_;
        ByteStream unReadBuffer_;
        std::deque<bool> occupied_;  
        std::deque<char> unAssembeledBuffer_;   
        size_t unAssembeledBufferSize_;
        uint64_t nextIdx_;
        bool flag_;
        void process();
};