#include "client.hpp"


void Client::reconnect(Server &s,  Channel_id_t id){
    channel_ptr = s.connect(id);
}

int Client::listen(){
    return channel_ptr->read();
}

Client::Client(Server &s, Channel_id_t id): channel_ptr{s.connect(id)}
{

}

