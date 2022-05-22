#include "network.h"


void set_length(char *_msg, unsigned long length) {
    for (int i = 8; i >= 1; i--) {
        _msg[i] = (char) (length % 256);
        length /= 256;
    }
}

unsigned long get_length(const char *_msg) {
    unsigned long length = 0;
    for (int i = 1; i <= 8; i++) {
        length *= 256;
        length += (unsigned long) (unsigned char) _msg[i];
    }
    return length;
}

void net_send(int socket, const std::string &msg, unsigned char flag) {
    char *_header = (char *) malloc(16);
    set_length(_header, msg.size());
    _header[0] = (char) flag;
    send(socket, _header, 16, 0);
    send(socket, msg.c_str(), msg.size(), 0);
    free(_header);
}

std::string net_read(int socket, unsigned char *flag) {
    char *_header = (char *) malloc(16);
    read(socket, _header, 16);
    *flag = (unsigned char) _header[0];
    unsigned long msg_size = get_length(_header);
    free(_header);

    char *_msg = (char *) malloc(msg_size);
    read(socket, _msg, msg_size);
    std::string result;
    for (unsigned long i = 0; i < msg_size; i++) {
        result += _msg[i];
    }
    free(_msg);
    return result;
}
