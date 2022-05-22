#include "My/PeerToPeer.h"

bool send_function(std::string data){
    std::cout<<data<<std::endl;
    return false;
}
std::string get_function(std::string data){
    return "Hello";
}
int main() {
    struct net_config config = {AF_INET, SOCK_STREAM, 0, INADDR_ANY, 1240, 20};
    PeerToPeer p2p = PeerToPeer(&config, send_function, get_function);
    p2p.run_server();
    while(true){
        std::string msg;
        std::cin>>msg;
        p2p.send_request(msg);
    }
}
