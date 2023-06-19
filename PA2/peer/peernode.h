#ifndef PEERNODE_H
#define PEERNODE_H

#include "nodeclient.h"
#include "nodeserver.h"



/*
    Peer Node that connects to other peers and the index server
*/
class PeerNode
{
public:
    /*
        Creates and initializes the peer node.
   // client_log_file             keeps logs of when the peer acts as a client to index server
   // client_csv_file             keeps logs structured as csv for better evaluation in spreadsheets
   // server_log_file             keeps logs of when peer acts as a server to other peers
   // server_csv_file             keeps logs structured as csv for better evaluation in spreadsheets
   // ip_addr_indx_server         ip address of index server
   // port_number_index_server    port number of index server
   // ip_addr_peer                ip address of peer
   // port_number_client          port number for when peer acts as client
   // port_number_server          port number for when peer acts as a server
   */
    PeerNode(const std::string &client_log_file, const std::string &client_csv_file, 
             const std::string &server_log_file, const std::string &server_csv_file,
             const std::string &directory, const char *ip_addr_indx_server, const char *port_number_indx_server, 
             const char *ip_addr_peer, const char *port_number_server);
    
    void Start() noexcept;

private:

    std::unique_ptr<NodeServer> s_node;
    std::unique_ptr<NodeClient> c_node; 
};

#endif