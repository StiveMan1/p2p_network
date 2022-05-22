//
// Created by stiveman1 on 30.04.2022.
//

#ifndef SKR_NETWORK_H
#define SKR_NETWORK_H

#include <string>
#include "vector"
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>

#define NET_CONNECTIONS     0b00000001
#define NET_DATA            0b00000010
#define NET_REQUEST         0b00000100
#define NET_RESPONSE        0b00001000
#define NET_SEND            0b00010000
#define NET_GET             0b00100000
#define NET_ERROR           0b01000000


struct net_config {
    int domain;
    int service;
    int protocol;
    unsigned long interface;
    int port;
    int backlog;
};


class P2PError : public std::exception {
};

void set_length(char *_msg, unsigned long length);

unsigned long get_length(const char *_msg);

void net_send(int socket, const std::string &msg, unsigned char flag);

std::string net_read(int socket, unsigned char *flag);

class Client {
    struct net_config *_config;
    int _socket;
public:
    explicit Client(struct net_config *config);

    int make_request(const std::string &server_ip);

    std::string get_request(const std::string &msg, unsigned char flag, unsigned char *flag_res) const;

    void send_request(const std::string &msg, unsigned char flag) const;

    void close_request() const;

};

class Server {
    struct net_config *_config;
    struct sockaddr_in _address{};
    int _socket;
    bool is_running = false;
    std::vector<std::string> known_hosts;

    bool (*_send_function)(const std::string &data);

    std::string (*_get_function)(const std::string &data);

private:

    static void *init_server(void *);

    static void accept_client(int client, Server *server);

public:
    explicit Server(struct net_config *config, bool (*send_function)(const std::string &data),
                    std::string (*get_function)(const std::string &data));

    void run_server();

    void close_server();


    void send_protocol(const std::string &msg, unsigned char flag);

    std::string get_protocol(const std::string &msg, unsigned char flag);


    void get_connected();

    void connect();

    std::vector<std::string> get_list();

    void set_list(const std::vector<std::string> &data);

};

class PeerToPeer {
    struct net_config *_config;
    Server *_server;
public:
    void run_server();

    void close_server();

    void get_connected();

    PeerToPeer(struct net_config *config, bool (*send_function)(const std::string &data),
               std::string (*get_function)(const std::string &data));

    ~PeerToPeer();

    void send_request(const std::string &msg);

    std::string get_request(const std::string &msg);

    std::vector<std::string> list_connected() const;

    void set_list_connected(const std::vector<std::string> &data);
};

#endif //SKR_NETWORK_H
