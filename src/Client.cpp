#include "network.h"


Client::Client(struct net_config *config) {
    this->_config = config;
    this->_socket = socket(this->_config->domain, this->_config->service, this->_config->protocol);
}

int Client::make_request(const std::string &server_ip) {
    struct sockaddr_in server_address{};
    server_address.sin_family = this->_config->domain;
    server_address.sin_port = htons(this->_config->port);
    server_address.sin_addr.s_addr = (int) this->_config->interface;
    inet_pton(this->_config->domain, server_ip.c_str(), &server_address.sin_addr);
    return connect(this->_socket, (struct sockaddr *) &server_address, sizeof(server_address));
}

std::string Client::get_request(const std::string &msg, unsigned char flag, unsigned char *flag_res) const {
    net_send(this->_socket, msg, NET_REQUEST | NET_GET | flag);
    return net_read(this->_socket, flag_res);
}

void Client::send_request(const std::string &msg, unsigned char flag) const {
    net_send(this->_socket, msg, NET_REQUEST | NET_SEND | flag);
}

void Client::close_request() const {
    close(this->_socket);
}
