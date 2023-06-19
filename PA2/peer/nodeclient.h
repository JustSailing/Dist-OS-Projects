#ifndef NODECLIENT_H
#define NODECLIENT_H

#include <string>
#include <netinet/in.h>
#include "logger.h"
#include <vector>

extern std::vector<std::string> all_files;
extern std::mutex lock_all_files;
extern bool running;

class NodeClient
{
public:
    NodeClient(const std::string &log_file, const std::string &csv_file, const std::string &dir, const char *ip_addr_index_server,
               const char *port_number_indx_server, std::string ip_addr, std::string port);

    void Start() noexcept;
private:
    // peer node socket descriptor
    int m_fd;

    // port for peers to connect to by server part
    std::string m_port;

    // peer ip address for peers to connect to by server part
    std::string m_ip;

    // index server ip
    std::string m_server_ip;

    // index server port
    std::string m_server_port;

    // server info
    sockaddr_in m_server_addr;

    std::string m_dir_folder;

    // boolean to keep the peer running. initialized to false. Changed to global variable defined in PeerNode
    // bool m_running = false;

    std::unique_ptr<Logger> m_logger;

    u_int16_t m_peer_id = 0;

    void FillLog(std::string_view event, std::string_view message, std::string_view ip, 
                 int peer_sock, const std::chrono::system_clock::time_point &start) noexcept;
    void FillLog(std::string_view event, std::string_view message, std::string_view ip,
                 int peer_sock) noexcept;
    void Connect() noexcept;
    [[nodiscard]] bool Register() noexcept;
    void Unregister(const std::chrono::system_clock::time_point &start) noexcept;
    void Query(const std::string& filename, const std::chrono::system_clock::time_point &start) noexcept;
    [[nodiscard]] bool DeleteFile(const std::string &filename, int sock,
                         const std::chrono::system_clock::time_point &start) noexcept;
    [[nodiscard]] bool AddFile(const std::string &filename, const std::chrono::system_clock::time_point &start) noexcept;
    bool Download(const std::string& ip, const std::string& port,
                  const std::string& filename, const std::chrono::system_clock::time_point &start) noexcept;

};


#endif