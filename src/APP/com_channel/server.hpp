#ifndef SERVER_H
#define SERVER_H

#include "channel.hpp"

typedef enum {
    SPEED,
    TEMP,
    RPM,
    NUM_OF_TYPES
}Channel_id_t;

class Server{
    private:
        Channel channels[NUM_OF_TYPES];


    public: 

        void Write_channel(Channel_id_t id, int data);
        Channel* connect(Channel_id_t id);
};


#endif