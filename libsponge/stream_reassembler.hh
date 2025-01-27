#include <string>
#include <deque>
#include "byte_stream.hh"
#pragma once

class StreamReassembler{
    public:
        StreamReassembler(const size_t capacity);

        void push_substring(const std::string &data, const uint64_t index, const bool eof);

        ByteStream& stream_out();

        size_t unassembled_bytes() const;

        bool empty() const;
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