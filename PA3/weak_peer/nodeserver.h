#ifndef NODESERVER_H
#define NODESERVER_H

#include <string>
#include <netinet/in.h>
#include <mutex>
#include <vector>
#include <memory>

#include "logger.h"
#include "message.h"

extern std::vector<std::string> all_files;
extern std::mutex lock_all_files;
extern bool running;

class NodeServer
{
public:
    NodeServer(const std::string &log_file, const std::string &csv_file, const std::string &directory,
               std::string super_ip, std::string super_port, std::string ip_addr, std::string port_number);
    
    void Start() noexcept;
    
private:
    // peer node socket descriptor
    int m_fd;

    // peer port to listen
    std::string m_port;

    // peer ip address to listen
    std::string m_ip;

    // peer info
    sockaddr_in m_addr;

    // super peer ip
    std::string m_super_ip;

    // super peer port
    std::string m_super_port;

    // server info
    sockaddr_in m_server_addr;

    // folder holding files with / at the end
    std::string m_dir_folder;

    // boolean to keep the peer running. initialized to false. Changed to global variable defined in Peer node
    // bool m_running = false;
    
    // Logger for peer node
    std::unique_ptr<Logger> m_logger;

    u_int16_t m_peer_id = 0;

    void Receive(int c_sock, std::string_view ip,
                 std::chrono::system_clock::time_point &time) noexcept;
    
    void Accept() noexcept;

    [[nodiscard]] bool Register() noexcept;

    void Download(const Message& msg, int peer_sock, std::string_view ip, const std::chrono::system_clock::time_point &start) noexcept;
    void FillLog(std::string_view event, std::string_view message, std::string_view ip, int peer_sock,
                          const std::chrono::system_clock::time_point &start) noexcept;
    void FillLog(std::string_view event, std::string_view message, std::string_view ip,
                          int peer_sock) noexcept;
};


#endif