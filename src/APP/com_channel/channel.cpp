#include "channel.hpp"


Channel::Channel():
    data{new int[max_size]},
    L_index{0},
    R_index{0},
    size{0} {}

Channel::Channel(const Channel &obj):
    data(new int[max_size]),
    L_index{obj.L_index},
    R_index{obj.R_index},
    size{obj.size}
{
        for(int i = 0; i < max_size; i++){
            data[i] = obj.data[i];
        }
}


Channel::~Channel()
{
        delete[] data;
}



void Channel::write(int val){
    if(size == max_size){
        // it is fulled
        return;
    }
    if(R_index == max_size){
        R_index= 0;
    }
    data[R_index] = val;
    R_index++;
    size++;
}

int Channel::read(){
    // return -1 if there is nothing to read
    if(size == 0){
        return -1;
    }
    if(L_index == max_size){
        L_index = 0;
    }
    int val = data[L_index];
    L_index++;
    size--;
    return val;
}