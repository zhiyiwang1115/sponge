#include "network_interface.hh"

NetworkInterface::NetworkInterface(const EthernetAddress &ethernet_address, const Address &ip_address)
:_ethernet_address(ethernet_address)
,_ip_address(ip_address)
{}

void NetworkInterface::send_datagram(const InternetDatagram &dgram, const Address &next_hop){

}

std::optional<InternetDatagram> NetworkInterface::recv_frame(const EthernetFrame &frame){

}

void NetworkInterface::tick(const size_t ms_since_last_tick){

}