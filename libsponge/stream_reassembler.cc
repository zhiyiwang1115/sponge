#include "stream_reassembler.hh"

StreamReassembler::StreamReassembler(const size_t capacity)
: capacity_(capacity)
, unReadBuffer_(ByteStream(capacity_))
, occupied_({})
, unAssembeledBuffer_({})
, unAssembeledBufferSize_(0)
, nextIdx_(0)
, flag_(false)
{}

void StreamReassembler::push_substring(const std::string &data, const uint64_t index, const bool eof){
    process();
    
    for(uint64_t i = index>=nextIdx_ ? 0 : nextIdx_ - index;i<data.size();++i){
        if(unReadBuffer_.buffer_size()+index+i-nextIdx_+1>capacity_){
            break;
        }
        while(unAssembeledBuffer_.size()<=index+i-nextIdx_){
            unAssembeledBuffer_.push_back(0);
            occupied_.push_back(0);
        }
        if(!occupied_[index+i- nextIdx_]){
            occupied_[index+i- nextIdx_] = true;
            unAssembeledBuffer_[index + i - nextIdx_] = data[i];
            ++unAssembeledBufferSize_;
        }
    }

    if(eof && index+data.size()==nextIdx_+unAssembeledBuffer_.size()){
        flag_ = true;
    }

    process();
}

ByteStream& StreamReassembler::stream_out(){
    return unReadBuffer_;
}

const ByteStream& StreamReassembler::stream_out() const{
    return unReadBuffer_;
}

size_t StreamReassembler::unassembled_bytes() const{
    return unAssembeledBufferSize_;
}

bool StreamReassembler::empty() const{
    return unAssembeledBufferSize_==0;
}

void StreamReassembler::process() {
    std::string s = "";
    while(occupied_.size() && occupied_.front()){
        char c = unAssembeledBuffer_.front();
        s += c;
        nextIdx_++;
        unAssembeledBuffer_.pop_front();
        occupied_.pop_front();
        --unAssembeledBufferSize_;
    }
    unReadBuffer_.write(s);
    if(flag_ && unAssembeledBuffer_.size()==0){
        unReadBuffer_.end_input();
    }
}