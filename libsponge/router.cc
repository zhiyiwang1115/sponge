#include "router.hh"
#include <vector>
#include <algorithm>
#include <iostream>

void Router::route_one_datagram(InternetDatagram &dgram){
    auto ipv4 = dgram.header().dst;
    if(dgram.header().ttl<=1)return;

    std::vector<int> v;
    for(size_t i = 0;i<32;i++){
        v.push_back(ipv4 & 1);
        ipv4 >>= 1;
    }
    reverse(v.begin(), v.end());
    Trie* cur = trie;
    size_t interface_num = 0;
    std::optional<Address> next_hop;
    for(size_t i = 0;i<v.size();i++){
        auto idx = v[i];
       if(cur->exist){
            next_hop = cur->next_hop;
            interface_num = cur->interface_num;
        }
        if(cur->children[idx]==nullptr){
            break;
        }
        cur = cur->children[idx];
    }
    if(cur->exist){
        next_hop = cur->next_hop;
        interface_num = cur->interface_num;
    }
    dgram.header().ttl--;
    if(!next_hop.has_value()){
        interface(interface_num).send_datagram(dgram, Address::from_ipv4_numeric(dgram.header().dst));
    }
    else interface(interface_num).send_datagram(dgram, next_hop.value());
}

void Router::add_route(const uint32_t route_prefix, const uint8_t prefix_length, const std::optional<Address> next_hop, const size_t interface_num){
    auto route_prefix_copy = route_prefix;
    std::vector<int> v;
    for(size_t i = 0;i<32;i++){
        v.push_back(route_prefix_copy & 1);
        route_prefix_copy >>= 1;
    }
    reverse(v.begin(), v.end());
    Trie* cur = trie;
    for(size_t i = 0;i<prefix_length;i++){
        auto idx = v[i];
        if(cur->children[idx]==nullptr){
            cur->children[idx] = new Trie();
        }
        cur = cur->children[idx];
    }
    cur->next_hop = next_hop;
    cur->interface_num = interface_num;
    cur->exist = true;
}

void Router::route(){
    for(auto& interface: _interfaces){
        auto& q = interface.datagrams_out();
        while(q.size()){
            route_one_datagram(q.front());
            q.pop();
        }
    }
}