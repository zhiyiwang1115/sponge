#include "tcp_sender.hh"

TCPSender::TCPSender(const size_t capacity, const uint16_t retx_timeout, const std::optional<WrappingInt32> fixed_isn)
              :_isn(fixed_isn.has_value() ? fixed_isn.value() : WrappingInt32(0))
              , _initial_retransmission_timeout(retx_timeout)\
              , _timeout(retx_timeout)
              , _stream(ByteStream(capacity)) {}

void TCPSender::fill_window(){
    if(isFin || _next_seqno>=_receiver_window)return;
    TCPSegment segment = TCPSegment();
    if(!isSyn){
        segment.header().syn = true; 
    }
    else{
        size_t bytesToFill = std::min(_stream.buffer_size()+_stream.remaining_capacity(), _receiver_window - _next_seqno);
        std::string s = _stream.read(std::min(bytesToFill, _stream.buffer_size()));
        segment.payload() = Buffer(std::move(s));
        if(_stream.eof()){
            segment.header().fin = true; 
            isFin = true;
        }    
    }
    if(segment.length_in_sequence_space()==0)return;
    segment.header().seqno = wrap(_next_seqno, _isn);
    _next_seqno += segment.length_in_sequence_space();
    _bytes_in_flight += segment.length_in_sequence_space();
    _segments_out.push(segment);
    _segments_out_copy.push(segment);
    if(_segments_out_copy.size()==1)sentTime = accumulatedTime;
    return;
}

void TCPSender::ack_received(const WrappingInt32 ackno, const uint16_t window_size){
    isSyn = true;
    auto ackno64 = unwrap(ackno, _isn, _next_seqno);
    if(ackno64>_next_seqno)return;
    _receiver_window = ackno64 + window_size;
    while(_segments_out_copy.size() 
    && unwrap(_segments_out_copy.front().header().seqno, _isn, _next_seqno)+_segments_out_copy.front().length_in_sequence_space()<=ackno64){
        auto t = _segments_out_copy.front();
        _bytes_in_flight -= t.length_in_sequence_space();
        _segments_out_copy.pop();
        _consecutive_retransmissions = 0;
        _timeout = _initial_retransmission_timeout;
    }

    if(_segments_out_copy.size()){
        sentTime = accumulatedTime;
    }
}

void TCPSender::tick(const size_t ms_since_last_tick){
    accumulatedTime += ms_since_last_tick;
    if(accumulatedTime-sentTime>=_timeout){
        _segments_out.push(_segments_out_copy.front());
        sentTime = accumulatedTime;
        _timeout *= 2;
        ++_consecutive_retransmissions;
    }
}

void TCPSender::send_empty_segment(){
    TCPSegment segment = TCPSegment();
    segment.header().seqno = wrap(_next_seqno, _isn);
    _segments_out.push(segment);
}

size_t TCPSender::bytes_in_flight() const{
    return _bytes_in_flight;
}

unsigned int TCPSender::consecutive_retransmissions() const{
    return _consecutive_retransmissions;
}