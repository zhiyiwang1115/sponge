#include "tcp_connection.hh"
#include <limits>

bool TCPConnection::sendSegments(){
    bool sent = false;
    while(_sender.segments_out().size()){
        auto segment = _sender.segments_out().front();
        _sender.segments_out().pop();
       if(_receiver.ackno().has_value()){
            segment.header().ack = true;
            segment.header().ackno = _receiver.ackno().value();
        }
        segment.header().win = static_cast<uint16_t>(_receiver.window_size());
        _segments_out.push(segment);
        sent = true;
    }
    return sent;
}

void TCPConnection::connect(){
    _sender.fill_window();
    sendSegments();
}

size_t TCPConnection::write(const std::string &data){
    auto res = _sender.stream_in().write(data);
    _sender.fill_window();
    sendSegments();
    return res;
}

size_t TCPConnection::remaining_outbound_capacity() const{
    return _sender.stream_in().remaining_capacity();
}

void TCPConnection::end_input_stream(){
    _sender.stream_in().end_input();
    _sender.fill_window();
    sendSegments();
}

size_t TCPConnection::bytes_in_flight() const{
    return _sender.bytes_in_flight();
}

size_t TCPConnection::unassembled_bytes() const{
    return _receiver.unassembled_bytes();
}

size_t TCPConnection::time_since_last_segment_received() const{
    return _time_since_last_segment_received;
}

void TCPConnection::segment_received(const TCPSegment &seg){
    _time_since_last_segment_received = 0;
    auto header = seg.header();
    if(header.rst){
        _sender.stream_in().set_error();
        _receiver.stream_out().set_error();
        _active = false;
        return;
    }
    _receiver.segment_received(seg);
    if(header.ack){
        _sender.ack_received(header.ackno, header.win);
        sendSegments();
    }
    if(seg.length_in_sequence_space()){
        _sender.fill_window();
        if(!sendSegments()){
            _sender.send_empty_segment();
            sendSegments();
        }
    }

    //passive close
    if(_receiver.stream_out().eof() && _receiver.unassembled_bytes() == 0){
        if(!_sender.stream_in().eof()){
            _linger_after_streams_finish = false;
        }
    }
}

void TCPConnection::tick(const size_t ms_since_last_tick){
    _time_since_last_segment_received += ms_since_last_tick;
    if(_receiver.stream_out().eof() && unassembled_bytes() == 0 && _sender.stream_in().eof() && bytes_in_flight()==0 ){
        if(!_linger_after_streams_finish)_active = false;
        if(_linger_after_streams_finish && _time_since_last_segment_received>=10*_cfg.rt_timeout){
            _linger_after_streams_finish = false;
            _active = false;    
        }
    }

    _sender.tick(ms_since_last_tick);
    if(_sender.consecutive_retransmissions()>TCPConfig::MAX_RETX_ATTEMPTS){
        _sender.stream_in().set_error();
        _receiver.stream_out().set_error();
        _active = false;
        TCPSegment segment = TCPSegment();
        segment.header().rst = true;
        _segments_out.push(segment);
        return;
    }
    sendSegments();
}

bool TCPConnection::active() const{
    return _active;
}

TCPConnection::~TCPConnection(){
    if(_active){
        _sender.stream_in().set_error();
        _receiver.stream_out().set_error();
        _active = false;
        TCPSegment segment = TCPSegment();
        segment.header().rst = true;
        _segments_out.push(segment);  
    }   
}