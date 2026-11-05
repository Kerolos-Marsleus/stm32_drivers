#ifndef CLIENT_H
#define CLIENT_H

#include "server.hpp"


class Client{
    private:
        Channel* channel_ptr;

    public:
    Client() = delete;
    Client(Server &s, Channel_id_t id);
    void reconnect(Server &s,  Channel_id_t id);
    int listen();


};




#endif /*CLIENT_H*/