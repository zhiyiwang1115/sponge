#include "tcp_connection.hh"
#include <iostream>

    void TCPConnection::connect(){
        _sender.fill_window();
        _segments_out.push(_sender.segments_out().front());
        _sender.segments_out().pop();
    }

    size_t TCPConnection::write(const std::string &data){
        return _sender.stream_in().write(data);
    }

    size_t TCPConnection::remaining_outbound_capacity() const{
        return 0;
    }

    void TCPConnection::end_input_stream(){
        _sender.stream_in().end_input();
    }

    size_t TCPConnection::bytes_in_flight() const{
        return _sender.bytes_in_flight();
    }

    size_t TCPConnection::unassembled_bytes() const{
        return _receiver.unassembled_bytes();
    }

    size_t TCPConnection::time_since_last_segment_received() const{
        return 0;
    }

    void TCPConnection::segment_received(const TCPSegment &seg){
        auto header = seg.header();
        // if(header.syn){
        //     _active = true;
        // }
        if(header.rst){
            _sender.stream_in().set_error();
            _receiver.stream_out().set_error();
            _active = false;
            return;
        }
        _receiver.segment_received(seg);
        if(header.ack){
            _sender.ack_received(header.ackno, header.win);
        }
        if(header.seqno.raw_value()){
            _sender.send_empty_segment();
        }
    }

    void TCPConnection::tick(const size_t ms_since_last_tick){
        std::cout << ms_since_last_tick << std::endl;
    }

    bool TCPConnection::active() const{
        return _active;
    }

    TCPConnection::~TCPConnection(){}