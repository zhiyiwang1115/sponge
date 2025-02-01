#include "network_interface.hh"
#include <iostream>
#include "arp_message.hh"

NetworkInterface::NetworkInterface(const EthernetAddress &ethernet_address, const Address &ip_address)
:_ethernet_address(ethernet_address)
,_ip_address(ip_address)
{}

void NetworkInterface::send_datagram(const InternetDatagram &dgram, const Address &next_hop){
    auto addressNumeric = next_hop.ipv4_numeric();
    EthernetFrame frame = EthernetFrame();
    frame.header().src = _ethernet_address;
    if(ipv4toEthernet.count(addressNumeric)){
        frame.payload() = dgram.serialize();
        frame.header().dst = ipv4toEthernet[addressNumeric];
        frame.header().type = EthernetHeader::TYPE_IPv4;
        _frames_out.push(frame);
        return;
    }
    if(!ipv4Request2Time.count(addressNumeric) || currentTime-ipv4Request2Time[addressNumeric]>TRANS_TIME){
        frame.header().type = EthernetHeader::TYPE_ARP;
        frame.header().dst = ETHERNET_BROADCAST;
        ARPMessage message = ARPMessage();
        message.opcode = ARPMessage::OPCODE_REQUEST;
        message.sender_ethernet_address = _ethernet_address;
        message.sender_ip_address = _ip_address.ipv4_numeric();
        message.target_ip_address = addressNumeric;
        message.target_ethernet_address = {0, 0, 0, 0, 0, 0};
        frame.payload() = message.serialize();
        _frames_out.push(frame);
        pendingDatagrams[addressNumeric].push({dgram,next_hop});
        ipv4Request2Time[addressNumeric] = currentTime;
        return;
    }    
}

std::optional<InternetDatagram> NetworkInterface::recv_frame(const EthernetFrame &frame){
    auto header = frame.header();
    if(header.dst!=_ethernet_address && header.dst!=ETHERNET_BROADCAST){
        return std::nullopt;
    }
    if(header.type == EthernetHeader::TYPE_IPv4){
        InternetDatagram dgram = InternetDatagram();
        if(dgram.parse(frame.payload())==ParseResult::NoError){
            return dgram;
        }
        return std::nullopt;
    }
    ARPMessage message = ARPMessage();
    if(message.parse(frame.payload())==ParseResult::NoError){
        if(message.opcode == ARPMessage::OPCODE_REQUEST){
            if(message.target_ip_address==_ip_address.ipv4_numeric()){
                EthernetFrame replyFrame = EthernetFrame();
                replyFrame.header().src = _ethernet_address;
                replyFrame.header().type = EthernetHeader::TYPE_ARP;
                replyFrame.header().dst = message.sender_ethernet_address;
                ARPMessage replyMessage = ARPMessage();
                replyMessage.opcode = ARPMessage::OPCODE_REPLY;
                replyMessage.sender_ethernet_address = _ethernet_address;
                replyMessage.sender_ip_address = _ip_address.ipv4_numeric();
                replyMessage.target_ip_address = message.sender_ip_address;
                replyMessage.target_ethernet_address = message.sender_ethernet_address;
                replyFrame.payload() = replyMessage.serialize();
                _frames_out.push(replyFrame);
            }
        }
        ipv4TsQueue.push(currentTime);
        ipv4Queue.push(message.sender_ip_address);
        ipv4toEthernet[message.sender_ip_address] = message.sender_ethernet_address;
        while(pendingDatagrams[message.sender_ip_address].size()){
            auto t = pendingDatagrams[message.sender_ip_address].front();
            pendingDatagrams[message.sender_ip_address].pop();
            send_datagram(t.first, t.second);
        }
    }
    return std::nullopt;
}

void NetworkInterface::tick(const size_t ms_since_last_tick){
    currentTime += ms_since_last_tick;
    while(ipv4TsQueue.size() && currentTime-ipv4TsQueue.front()>CACHE_TIME){
        ipv4TsQueue.pop();
        auto ipv4 = ipv4Queue.front();
        ipv4Queue.pop();
        ipv4toEthernet.erase(ipv4);
    }
}