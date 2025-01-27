#include "byte_stream.hh"

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capacity)
: capacity_(capacity)
, data_({})
, endedWrite_(false)
, bytesWritten_(0)
, bytesRead_(0)
{}

size_t ByteStream::write(const string &data) {
    size_t lenToWrite = min(data.size(), capacity_ - data_.size());
    for(size_t i = 0;i<lenToWrite;++i){
        data_.push_back(data[i]);
    }
    bytesWritten_ += lenToWrite;
    return lenToWrite;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    string res = "";
    auto it = data_.begin();
    for(size_t i = 0;i<min(len, data_.size());++i){
        res += *it;
        it = next(it);
    }
    return res;
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) { 
    if(len>data_.size()){
        set_error();
        return;
    }
    for(size_t i = 0;i<len;++i){
        data_.pop_front();
    }
    bytesRead_ += len;
    return;
 }

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    if(len>data_.size()){
        set_error();
        return "";
    }
    string res = peek_output(len);
    pop_output(len);
    return res;
}

void ByteStream::end_input() {endedWrite_=true;}

bool ByteStream::input_ended() const { return endedWrite_; }

size_t ByteStream::buffer_size() const { return data_.size(); }

bool ByteStream::buffer_empty() const { return data_.size()==0; }

bool ByteStream::eof() const { return data_.size()==0 && endedWrite_; }

size_t ByteStream::bytes_written() const { return bytesWritten_; }

size_t ByteStream::bytes_read() const { return bytesRead_; }

size_t ByteStream::remaining_capacity() const { return capacity_ - data_.size(); }
