#include "server.hpp"
void Server::Write_channel(Channel_id_t id, int data){
    channels[id].write(data);
}


Channel* Server::connect(Channel_id_t id){
    return channels + id;
}