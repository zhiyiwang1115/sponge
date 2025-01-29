#include "tcp_receiver.hh"
#include "wrapping_integers.hh"

std::optional<WrappingInt32> TCPReceiver::ackno() const{
    if(!_synReceived)return std::nullopt;
    if(_finReceived && _reassembler.unassembled_bytes()==0) return wrap(_reassembler.getNextIdx()+2, _isn);
    return wrap(_reassembler.getNextIdx()+1, _isn);
}

size_t TCPReceiver::window_size() const{
    return _capacity - _reassembler.stream_out().buffer_size();
}

size_t TCPReceiver::unassembled_bytes() const{
    return _reassembler.unassembled_bytes();
}

void TCPReceiver::segment_received(const TCPSegment &seg){
    auto header = seg.header();
    if(!_synReceived && header.syn){
        _synReceived = true;
        _isn = header.seqno;
    }
    auto payload = seg.payload().copy();
    if(_synReceived){
        WrappingInt32 seqno = header.syn ? header.seqno : header.seqno - 1;
        bool eof = header.fin ? true : false;
        uint64_t index = unwrap(seqno, _isn, _checkPoint);
        _reassembler.push_substring(payload, index, eof);
        if(_reassembler.getFlag()){
            _finReceived = true;
        }
        _checkPoint = _reassembler.getNextIdx();
    }
}

ByteStream& TCPReceiver::stream_out(){
    return _reassembler.stream_out();
}

const ByteStream& TCPReceiver::stream_out() const{
    return _reassembler.stream_out();
}