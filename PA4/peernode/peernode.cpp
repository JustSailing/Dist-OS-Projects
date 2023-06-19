#include <iostream>
#include <arpa/inet.h>

#include "peernode.h"

std::vector<std::pair<std::string,std::string>> all_files;
std::mutex lock_all_files;
bool running = false;


PeerNode::PeerNode(const std::string &client_log_file, const std::string &client_csv_file, 
                   const std::string &server_log_file, const std::string &server_csv_file,
                   const std::string &directory, const char *ip_addr_indx_server, const char *port_number_indx_server, 
                   const char *ip_addr_peer, const char *port_number_server)
{
    s_node = std::make_unique<NodeServer>(server_log_file, server_csv_file, directory, ip_addr_indx_server, port_number_indx_server, 
                                          ip_addr_peer, port_number_server);
    c_node = std::make_unique<NodeClient>(client_log_file, client_csv_file, directory, ip_addr_indx_server, 
                                          port_number_indx_server, ip_addr_peer, port_number_server);

}

void PeerNode::Start() noexcept
{
    std::thread t1([&] {c_node->Start();});
    std::thread t2([&] {s_node->Start();});
    t1.join();
    t2.join();
}
