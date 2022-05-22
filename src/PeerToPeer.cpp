#include "network.h"


PeerToPeer::PeerToPeer(struct net_config *config, bool (*send_function)(const std::string &data),
                       std::string (*get_function)(const std::string &data)) {
    this->_config = config;
    this->_server = new Server(config, send_function, get_function);
}

PeerToPeer::~PeerToPeer() {
    delete _server;
    delete _config;
}

void PeerToPeer::get_connected() {
    this->_server->get_connected();
}

void PeerToPeer::run_server() {
    this->_server->run_server();
}

void PeerToPeer::close_server() {
    this->_server->close_server();
}

void PeerToPeer::send_request(const std::string &msg) {
    this->_server->send_protocol(msg, NET_DATA);
}

std::string PeerToPeer::get_request(const std::string &msg) {
    return this->_server->get_protocol(msg, NET_DATA);
}

std::vector<std::string> PeerToPeer::list_connected() const {
    return this->_server->get_list();
}

void PeerToPeer::set_list_connected(const std::vector<std::string> &data) {
    this->_server->set_list(data);
}


