#pragma once

#include <string>
#include <deque>

class ByteStream {
  private:
    size_t capacity_;
    std::deque<char> data_;
    bool endedWrite_;
    size_t bytesWritten_, bytesRead_;
    bool _error{}; 

  public:
    ByteStream(const size_t capacity);

    size_t write(const std::string &data);

    size_t remaining_capacity() const;

    void end_input();

    void set_error() { _error = true; }

    std::string peek_output(const size_t len) const;

    void pop_output(const size_t len);

    std::string read(const size_t len);

    bool input_ended() const;

    bool error() const { return _error; }

    size_t buffer_size() const;

    bool buffer_empty() const;

    bool eof() const;

    size_t bytes_written() const;

    size_t bytes_read() const;
};

