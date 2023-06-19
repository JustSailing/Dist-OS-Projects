#ifndef INDEX_SERVER_H
#define INDEX_SERVER_H

#include <string>
#include <netinet/in.h>
#include <memory>
#include <unordered_map>
#include <vector>
#include <atomic>
#include <chrono>

#include "logger.h"
#include "message.h"

/*
    PeerNode:
        A structured way of holding data on the peers connected to
        the index server
*/
struct PeerNode
{
    // Identifier needed for peer to connect to the server

    // Needed since if clients are on the same computer a unique identifier is given

    u_int32_t id;

    // ip and port for other peers to connect to
    std::string ip;
    std::string port;

    // peer is registered
    bool registered;

    // unordered_map of all the files that peer has
    // updated when files are added and deleted
    std::vector<std::string> files_map;
};

/*
    Index Server:
        handles queries and updates of peer nodes connected
        to it. It serves as a medium to all peers connected
*/
class IndexServer
{
public:
    /*
        Creates and initializes the Index server.
        - Both log files will be used to initialize the Logger. Failure to initialize
          the Logger correctly such as file not found, will end in the termination
          of the program.
        - socket file descriptor is created. If failure to do so the program terminates
        - binds the socket to the ip address provided. If failure, the program terminates
    */
    IndexServer(const std::string &log_file, const std::string &csv_file,
                const char *ip_addr, const char *port_number);

    /*
        Listens to the socket created in the constructor
        Creates 10 threads to recieve/send and waits until threads are finished
        Can be put into a inifite while loop constantly listening to requests
    */
    void Start(/*int num_threads*/) noexcept;

private:
    // server socket
    int m_server_fd;

    // server port
    std::string m_port_number;

    // server ip address
    std::string m_server_ip;

    // server info. not sure if I need this
    sockaddr_in m_addr;

    // boolean to keep the server running
    bool m_running = false;

    // Logger for server
    std::unique_ptr<Logger> m_logger;

    // Vector of PeerNode unique pointers. Used for get peer list who
    // hold a specific file
    std::vector<std::unique_ptr<PeerNode>> m_peer_nodes;

    std::mutex m_lock_peer_nodes;

    // map to help server keep track of registered peers
    // not needed: might use the vector above. less memory space used
    //std::unordered_map<int, bool> m_register_peer;

    //std::mutex m_lock_register_peer;

    // Used to give peers about to register a unique number for id
    std::atomic<u_int32_t> m_last_peer;

    /**                                     **/
    /**                                     **/
    /**  Private functions used internally  **/
    /**                                     **/
    /**                                     **/

    /*
        Blocks until it accpets a connection. Calls Receive to read from the
        socket and calls a function with respect to the message received
    */
    void Connection() noexcept;

    /*
        After connecting with client on socket, read messages and call function
        relative to message
    */
    void Receive(int c_sock, std::string_view ip,
                 std::chrono::system_clock::time_point &time) noexcept;

    /*
        Get list of peers of have the file
    */
    [[nodiscard]] bool GetPeerNodeList(const Message &msg, int peer_sock, std::string_view ip) noexcept;

    /*
        Registers node connected to the index server
    */
    [[nodiscard]] int RegisterNode(const Message &message, int peer_sock, std::string_view ip) noexcept;

    /*
        Unregisters node connected to the index server
    */
    [[nodiscard]] bool UnregisterNode(const Message &message, int peer_sock, std::string_view ip) noexcept;

    /*
        Adds a new file of a peer node to the list on the index server
    */
    [[nodiscard]] bool AddNewFile(const Message &message, int peer_sock, std::string_view ip) noexcept;

    /*
        Deletes a file of a peer node to the list on the index server
    */
    [[nodiscard]] bool DeleteFile(const Message &message, int peer_sock, std::string_view ip) noexcept;

    /**                                  **/
    /**                                  **/
    /**      HELPER FUNCTIONS            **/
    /**                                  **/
    /**                                  **/

    // Helper to see if peer is registered
    bool CheckRegistered(uint16_t peer_id, const std::string &message,
                         std::string_view ip, int peer_sock) noexcept;

    // Helper for logging
    void FillLog(std::string_view event, std::string_view message, std::string_view ip,
                 int peer_sock, const std::chrono::system_clock::time_point &start) noexcept;

    // Helper for logging
    void FillLog(std::string_view event, std::string_view message, std::string_view ip,
                 int peer_sock) noexcept;

    // Helper for Receive to deal with get peer list
    void Query(const Message &message, int peer_sock, std::string_view ip,
               const std::chrono::system_clock::time_point &start) noexcept;

    // Helper for Receive to deal with addfile to peer
    void Add(const Message &message, int peer_sock, std::string_view ip,
             const std::chrono::system_clock::time_point &start) noexcept;

    // Helper for Receive to deal with delete file from peer list
    void Delete(const Message &message, int peer_sock, std::string_view ip,
                const std::chrono::system_clock::time_point &start) noexcept;

    // Helper for Receive to deal with register peer
    void Register(const Message &message, int peer_sock, std::string_view ip,
                  const std::chrono::system_clock::time_point &start) noexcept;

    // Helper for Receive to deal with unregister peer
    void Unregister(const Message &message, int peer_sock, std::string_view ip,
                    const std::chrono::system_clock::time_point &start) noexcept;
};



#endif