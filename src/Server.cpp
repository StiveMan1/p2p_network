#include "network.h"
#include "nlohmann/json.hpp"


Server::Server(struct net_config *config, bool (*send_function)(const std::string &data),
               std::string (*get_function)(const std::string &data)) {
    this->_config = config;
    this->_address.sin_family = config->domain;
    this->_address.sin_port = htons(config->port);
    this->_address.sin_addr.s_addr = htonl(config->interface);
    this->_socket = socket(config->domain, config->service, config->protocol);
    this->_send_function = send_function;
    this->_get_function = get_function;
    if (this->_socket == 0) {
        perror("Failed to connect socket...\n");
        exit(1);
    }
    if ((bind(this->_socket, (struct sockaddr *) &this->_address, sizeof(this->_address))) < 0) {
        perror("Failed to bind socket...\n");
        exit(1);
    }
    if ((listen(this->_socket, config->backlog)) < 0) {
        perror("Failed to start listening...\n");
        exit(1);
    }
}

void Server::run_server() {
    this->is_running = true;
    pthread_t server_thread;
    pthread_create(&server_thread, nullptr, Server::init_server, this);
    this->connect();
}

void Server::close_server() {
    this->is_running = false;
}

void *Server::init_server(void *arg) {
    printf("Server running.\n");
    auto *server = (Server *) arg;
    auto *address = (struct sockaddr *) &server->_address;
    auto address_length = (socklen_t) sizeof(server->_address);
    while (server->is_running) {
        int client = accept(server->_socket, address, &address_length);
        Server::accept_client(client, server);
    }
    return nullptr;
}

void Server::accept_client(int client, Server *server) {
    unsigned char flag = 0, flag_res = 0;
    std::string data = net_read(client, &flag);
    std::cout << "(" << inet_ntoa(server->_address.sin_addr) << ") : ["
              << (flag & NET_CONNECTIONS) << " "
              << (flag & NET_DATA) << " "
              << (flag & NET_REQUEST) << " "
              << (flag & NET_RESPONSE) << " "
              << (flag & NET_SEND) << " "
              << (flag & NET_GET) << " "
              << (flag & NET_ERROR) << "] " << data << std::endl;
    std::string result;
    bool send_next = true;
    if (flag & NET_CONNECTIONS) {
        flag_res |= NET_CONNECTIONS;
        if (flag & NET_SEND) {
            if (data.empty()) {
                data = inet_ntoa(server->_address.sin_addr);
            }
            if (std::find(server->known_hosts.begin(), server->known_hosts.end(), data) != server->known_hosts.end()) {
                send_next = false;
            } else {
                server->known_hosts.emplace_back(data);
            }
        }
        if (flag & NET_GET) {
            nlohmann::json json = server->known_hosts;
            result = json.dump();
        }
    }
    if (flag & NET_DATA) {
        flag_res |= NET_DATA;
        try {
            if (flag & NET_SEND) {
                send_next = server->_send_function(data);
            }
            if (flag & NET_GET) {
                result = server->_get_function(data);
            }
        } catch (std::exception &e) {
            flag_res |= NET_ERROR;
        }
    }
    if (flag & NET_GET) {
        flag_res |= NET_GET;
        flag_res |= NET_RESPONSE;
        net_send(client, result, flag_res);
    }
    close(client);
    if ((flag & NET_SEND) && send_next) {
        server->send_protocol(data, flag);
    }
}


void Server::send_protocol(const std::string &msg, unsigned char flag) {
    for (const std::string &ip_server: known_hosts) {
        Client client = Client(this->_config);
        client.make_request(ip_server);
        client.send_request(msg, flag);
        client.close_request();
    }
}

std::string Server::get_protocol(const std::string &msg, unsigned char flag) {
    unsigned char flag_res;
    for (const std::string &ip_server: known_hosts) {
        Client client = Client(this->_config);
        client.make_request(ip_server);
        flag_res = 0;
        std::string data = client.get_request(msg, flag, &flag_res);
        client.close_request();
        if ((flag_res & NET_ERROR) == 0) {
            return data;
        }
    }
    throw P2PError();
}

void Server::get_connected() {
    std::string data = get_protocol("", NET_CONNECTIONS);
    if (data.empty())return;
    nlohmann::json json = nlohmann::json::parse(data);
    this->known_hosts = json.get<std::vector<std::string>>();
}

std::vector<std::string> Server::get_list() {
    return this->known_hosts;
}

void Server::set_list(const std::vector<std::string> &data) {
    this->known_hosts = data;
}

void Server::connect() {
    known_hosts.emplace_back("127.0.0.1");
    send_protocol("", NET_CONNECTIONS);
}
