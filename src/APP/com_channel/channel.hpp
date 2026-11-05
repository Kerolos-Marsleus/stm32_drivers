#ifndef CHANNEL_H
#define CHANNEL_H

constexpr int max_size = 20;

class Channel {
    private:
        int* data;
        int L_index;
        int R_index;
        int size;
    public:
    Channel();
    Channel(const Channel &obj);
    ~Channel();

    void write(int val);
    int read();

};



#endif  /*CHANNEL_H*/