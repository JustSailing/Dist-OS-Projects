#include "nodeclient.h"
#include "message.h"

#include <iostream>
#include <arpa/inet.h>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <array>
#include <queue>
#include <uuid/uuid.h>
#include <random>
#include <fstream>

extern std::array<std::pair<u_int16_t, std::string>, 4> q;
extern int q_index;
extern int q_size;
std::mutex lock_queue;
extern uint16_t peer_id;

NodeClient::NodeClient(const std::string &log_file, const std::string &csv_file, const std::string &dir, const char *ip_addr_index_server,
                       const char *port_number_indx_server, std::string ip_addr, std::string port)
{
    m_logger = std::make_unique<Logger>(log_file, csv_file);
    m_server_addr = sockaddr_in{};
    m_ip = ip_addr;
    m_port = port;
    m_dir_folder = dir;
    m_server_ip = ip_addr_index_server;
    m_server_port = port_number_indx_server;
    m_server_addr.sin_family = AF_INET;
    m_server_addr.sin_addr.s_addr = inet_addr(m_server_ip.c_str());
    m_server_addr.sin_port = htons(std::stoi(m_server_port));
    // m_fd = socket(AF_INET, SOCK_STREAM, 0);
    std::string event = "[Success] Initialization of member variables";

    FillLog(event, "Initialization Client", m_server_ip, 0);
}

void NodeClient::Start() noexcept
{
    // while(!running) continue;
    std::array<std::thread, 10> client_threads;
    for (auto &t : client_threads)
    {
        t = std::thread([&]
                        { Connect(); });
    }
    for (auto &t : client_threads)
    {
        t.join();
    }
    sleep(3); // not sure if I need this still since there's a join for server and client side threads in the peernode cpp
}

void NodeClient::Connect() noexcept
{
    sleep(2);
    m_peer_id = peer_id;
    while (running)
    {
        std::pair<u_int16_t, std::string> msg;
        {
            const std::lock_guard<std::mutex> lck(lock_queue);

            if (q_index > q_size)
                break;
            msg = q[q_index];
            q_index++;
        }
        // std::cout << msg.first << std::endl;
        // REGISTER handled on the server side
        switch (msg.first)
        {
        case QUERY:
        {
            auto start = std::chrono::system_clock::now();
            Query(msg.second, start);
        }
        break;
        case UNREGISTER:
        {
            auto start = std::chrono::system_clock::now();
            Unregister(start);
        }
        break;
        default:
            running = false;
            break;
        }
    }
    // std::string event = "[Finished] All work completed ";
    // FillLog(event, "WORK completed", 0, 0);
}

void NodeClient::Unregister(const std::chrono::system_clock::time_point &start) noexcept
{
    std::string event;
    auto fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        event = "[ERROR] Creation of peer client socket failed [TIME] ";
        m_logger->Log(event);
        bool check = m_logger->CloseLog();
        if (check)
        {
            std::cout << "main thread closing log\n";
        }
        std::cout << "peer client socket failed to be created\n"
                  << std::endl;
        exit(2);
    }
    if (connect(fd, (struct sockaddr *)&m_server_addr, sizeof(m_server_addr)) < 0)
    {
        event = "[ERROR] Connecting to super peer with ";
        FillLog(event, "Connection Fail", m_server_ip, fd);
        return;
    }
    Message m{};
    m.mess_id = UNREGISTER;
    m.peer_id = m_peer_id;
    m.size = 0;
    m.mess_in_seq = 1;
    m.total_mess = 1;
    memset(m.body, '\0', sizeof(m.body));
    if (send(fd, &m, sizeof(m), 0) < 0)
    {
        event = "[SEND_FAILURE] sending error trying to unregister peer ";
        event += "[PeerID] " + std::to_string(m_peer_id) + " ";
        FillLog(event, "RegisterNode_send", m_server_ip, fd);
        running = false;
    }
    event = "[SUCCESS] Unregistering from super peer ";
    FillLog(event, "Unregister", m_server_ip, fd, start);
    running = false;
    // exit(5);
}

void NodeClient::Query(const std::string &filename, const std::chrono::system_clock::time_point &start) noexcept
{
    std::string event;
    auto fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        event = "[ERROR] Creation of peer client socket failed [TIME] ";
        m_logger->Log(event);
        bool check = m_logger->CloseLog();
        if (check)
        {
            std::cout << "main thread closing log\n";
        }
        std::cout << "peer client socket failed to be created\n"
                  << std::endl;
        exit(2);
    }
    // m_server_addr.sin_family = AF_INET;
    // m_server_addr.sin_addr.s_addr = inet_addr(m_server_ip.c_str());
    // m_server_addr.sin_port = htons(std::stoi(m_server_port));
    if (connect(fd, (struct sockaddr *)&m_server_addr, sizeof(m_server_addr)) < 0)
    {
        event = "[ERROR] Connecting to super peer with ";
        FillLog(event, "Connection Fail", m_server_ip, fd);
        return;
    }
    event = "[SUCCESS] Connecting to super peer ";
    FillLog(event, "Connection", m_server_ip, fd);
    Message m{};
    m.peer_id = peer_id;
    m.mess_id = QUERY;
    m.mess_in_seq = 1;
    m.total_mess = 1;
    memset(m.body, '\0', sizeof(m.body));
    strncpy(m.body, filename.c_str(), filename.length());
    std::string f(m.body);
    event = "message sent weak peer: " + f;
    FillLog(event, "debug", m_server_ip, fd);
    if (send(fd, &m, sizeof(m), 0) <= 0)
    {
        event = "[ERROR] sending to query to super peer failed ";
        FillLog(event, "Download", m_server_ip, fd);
        return;
    }
    event = "[Success] Sending file name to super peer ";
    FillLog(event, "QuerySend", m_server_ip, fd);
    char buffer[sizeof(Message)];

    memset(buffer, '\0', sizeof(buffer));

    if (read(fd, buffer, sizeof(buffer)) < 0)
    {
        event = "[ERROR] reading message from super peer ";
        FillLog(event, "Query", m_server_ip, fd);
        return;
    }
    auto *msg = reinterpret_cast<Message *>(buffer);
    std::string files = msg->body;
    event = "[TIME] to query all peers and get results ";
    FillLog(event, "QUERYTIME", "", fd, start);
    std::string ss(files);
    std::vector<std::string> strs;
    std::string fn = "";
    for (auto x : ss)
    {
        if (x == ',')
        {
            strs.push_back(fn);
            fn = "";
        }
        else
        {
            fn = fn + x;
        }
    }

    bool check = false;
    std::string ip;
    std::string port;
    std::chrono::system_clock::time_point t;
    if (strs.empty())
    {
        event = "[ERROR] no peer has file";
        FillLog(event, "QUERY", m_server_ip, fd);
        return;
    }
    int count = 2;

    while (count <= msg->total_mess)
    {
        char buffer[sizeof(Message)];
        memset(buffer, '\0', sizeof(buffer));
    
        if (read(fd, buffer, sizeof(buffer)) < 0)
        {
            event = "[ERROR] reading message from super peer ";
            FillLog(event, "Query", m_server_ip, fd);
            return;
        }
        msg = reinterpret_cast<Message *>(buffer);
        files = msg->body;
        event = "[TIME] to query all peers and get results ";
        FillLog(event, "QUERYTIME", "", fd, start);
        std::string ss1(files);
        // std::vector<std::string> strs;
        fn = "";
        for (auto x : ss1)
        {
            if (x == ',')
            {
                strs.push_back(fn);
                fn = "";
            }
            else
            {
                fn = fn + x;
            }
        }
    }
    
    auto f_size = msg->filesize;
    uint64_t amount_of_chunks = f_size / CHUCK_SIZE;
    auto extra = f_size % CHUCK_SIZE;
    amount_of_chunks += extra;
    auto total = amount_of_chunks; // used to help evaluate which chuck was downloaded total - amount of chunks
    std::mutex for_file;
    std::mutex for_chunk_number;
    // auto number_of_threads = strs.size() / 2; // should always be an event number since all ip and ports come in pairs
    std::vector<std::thread> ths;
    int vec_index = 0;


    for (u_int64_t i = 0; i < strs.size(); i += 2)
    {
        ip = strs[i];
        if (ip == "")
        {
            event = "[ERROR] ip was null";
            FillLog(event, "QUERY", m_server_ip, fd);
            continue;
        }
        port = strs[i + 1];
        if (port == "")
        {
            event = "[ERROR] port was null";
            FillLog(event, "QUERY", m_server_ip, fd);
            continue;
        }

        event = "[Port] " + port + "[IP]" + ip;
        FillLog(event, "ip port list", ip, 0);
        t = std::chrono::system_clock::now();

        std::thread th([&, ip, port, f_size, total, t]
                       { Download(ip, port, filename, f_size, amount_of_chunks, for_chunk_number, for_file, total, t); });
        ths.push_back(std::move(th));
    }
    for (auto &thread : ths)
    {
        thread.join();
    }
    if (amount_of_chunks <= 0)
    {
        event = "[SUCCESS] Downloaded file from peers [FILE] " + filename + " ";
        // auto c = "DOWNLOAD" + filename;
        FillLog(event, "DOWNLOADED", ip, fd, start);
        auto st = std::chrono::system_clock::now();
        auto check = AddFile(filename, f_size, st);
        if (check)
        {
            event = "[SUCCESS] Added file to index server [FILE] " + filename + " ";
            FillLog(event, "ADDFILE", ip, fd);
            return;
        }
        else
        {
            event = "[FAILURE] Did not add file to index server [FILE] " + filename + " ";
            FillLog(event, "ADDFILE", ip, fd);
        }
    }
    else
    {
        event = "[FAILURE] Did not download all file [CHUNKS_LEFT#] " + std::to_string(amount_of_chunks);
        FillLog(event, "DOWNLOADING", ip, fd);
    }
}

void NodeClient::Download(const std::string &ip, const std::string &port,
                          const std::string &filename, const uint64_t filesize, uint64_t &chunks, std::mutex &chks, std::mutex &fl,
                          int total, const std::chrono::system_clock::time_point &start) noexcept
{
    while (true)
    {
        std::string event;
        // FillLog("[THREAD] ", port, ip, 0);
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd < 0)
        {
            event = "[FAILURE] to create socket to download file from peer ";
            FillLog(event, "DOWNLOAD", ip, 0);
            return;
        }
        event = "[SUCCESS] Created socket in download ";
        FillLog(event, "Download", ip, fd);
        sockaddr_in peer_addr{};
        peer_addr.sin_family = AF_INET;
        peer_addr.sin_addr.s_addr = inet_addr(ip.c_str());
        peer_addr.sin_port = htons(std::stoi(port));
        // FillLog(port, "port", ip, fd);
        if (connect(fd, (sockaddr *)&peer_addr, sizeof(peer_addr)) < 0)
        {
            event = "[FAILURE] failed to connect to peer to download [FILE] " + filename + " ";
            FillLog(event, "DOWNLOAD", ip, fd);
            close(fd);
            return;
        }
        event = "[SUCCESS] Connected to peer socket in download ";
        FillLog(event, "Download", ip, fd);

        chks.lock();
        if (chunks <= 0)
        {
            chks.unlock();
            break;
        }
        auto temp = chunks;
        chunks -= 1;
        chks.unlock();
        Message m{};
        m.mess_id = DOWNLOAD;
        m.peer_id = m_peer_id;
        m.size = filename.length();
        m.offset = total - temp;
        m.mess_in_seq = 1;
        m.total_mess = 1;
        memset(m.body, '\0', sizeof(m.body));
        strncpy(m.body, filename.c_str(), filename.length());
        if (send(fd, &m, sizeof(m), 0) < 0)
        {
            event = "[SEND_FAILURE] cant send single file download for [FILE] " + filename + " ";
            FillLog(event, "DOWNLOAD", ip, fd);
            close(fd);
            return;
        }
        event = "[SUCCESS] Sent from peer socket to download ";
        FillLog(event, "Download", ip, fd);
        char buffer[sizeof(Message)];
        memset(buffer, '\0', sizeof(buffer));
        if (read(fd, buffer, sizeof(buffer)) < 0)
        {
            event = "[ERROR] reading message from other peer node ";
            FillLog(event, "DOWNLOAD", ip, fd);
            close(fd);
            return;
        }
        event = "[SUCCESS] read from peer socket in download ";
        FillLog(event, "Download", ip, fd);
        auto *msg = reinterpret_cast<Message *>(buffer);
        if (msg->mess_id != DOWNLOAD)
        {
            event = "[ERROR] other peer sent error instead of file ";
            FillLog(event, "DOWNLOAD", ip, fd);
            close(fd);
            return;
        }
        // FILE *f;
        std::string full = m_dir_folder + filename;
        std::ofstream f(full, std::ios_base::out | std::ios_base::app);
        // FillLog(msg->body, "DEBUG", ip, fd);
        if (!f.good())
        {
            event = "[ERROR] could not open file name in the peer directory ";
            FillLog(event, "DOWNLOAD", ip, fd);
            close(fd);
            return;
        }
        std::string content(msg->body);
        fl.lock();
        f.seekp(0);
        f.seekp((msg->offset * CHUCK_SIZE), std::ios::end);
        // f.clear();
        f.write(content.c_str(), content.length());
        f.clear();
        f.flush();
        f.close();
        fl.unlock();
        event = "[SUCCESS] Partial Download [FILENAME] " + filename + " [CHUNK#] " + std::to_string(total - temp) + " [BYTES] " + std::to_string(msg->size) + " [PEER] " + std::to_string(msg->peer_id);
        FillLog(event, "Partial Download", ip, fd);
        close(fd);
    }
    return;
}

bool NodeClient::DeleteFile(const std::string &filename, int sock,
                            const std::chrono::system_clock::time_point &start) noexcept
{
    std::string event;
    Message m{};
    m.mess_id = DELETE_FILE;
    m.mess_in_seq = 1;
    m.total_mess = 1;
    m.peer_id = m_peer_id;
    m.size = filename.length();
    memset(m.body, '\0', filename.length());
    strncpy(m.body, filename.c_str(), filename.length());
    if (send(sock, &m, sizeof(m), 0) < 0)
    {
        event = "[SEND_FAILURE] sending error: super peer did not get DELETE_FILE ";
        event += "[PeerID] " + std::to_string(m_peer_id) + " ";
        FillLog(event, "Add_file", m_server_ip, sock);
        return false;
    }
    char buffer[sizeof(Message)];
    if (read(sock, buffer, sizeof(buffer)) < 0)
    {
        event = "[ERROR] reading at client after deletion file send ";
        FillLog(event, "Add_file", m_server_ip, sock);
        return false;
    }
    auto *msg = reinterpret_cast<Message *>(buffer);
    if (msg->mess_id != COMPLETED)
    {
        event = "[ERROR] super peer did not complete the deletion of file on peer list ";
        FillLog(event, "Add_file", m_server_ip, sock);
        return false;
    }
    event = "[SUCCESS] super peer added file to peer list ";
    FillLog(event, "DELETE_FILE", m_server_ip, sock, start);
    {
        const std::lock_guard<std::mutex> lck(lock_all_files);
        auto index = -1;
        for (u_int64_t i = 0; i < all_files.size(); i++)
        {
            if (all_files[i].first == filename)
            {
                index = i;
                break;
            }
        }
        if (index == -1)
        {
            event = "[FAILURE] File not in all files anyway ";
            FillLog(event, "DELETE_FILE", m_server_ip, sock);
            return true;
        }
        else
        {
            all_files.erase(all_files.begin() + index);
        }
    }
    return true;
}

bool NodeClient::AddFile(const std::string &filename, const uint64_t &filesize,
                         const std::chrono::system_clock::time_point &start) noexcept
{

    std::string event;
    Message m{};
    m.mess_id = ADD_FILE;
    m.mess_in_seq = 1;
    m.total_mess = 1;
    m.peer_id = m_peer_id;
    m.filesize = filesize;
    m.size = filename.length();
    m.filesize = filesize;
    memset(m.body, '\0', filename.length());
    strncpy(m.body, filename.c_str(), filename.length());
    auto sock = socket(AF_INET, SOCK_STREAM, 0);
    // std::string event;
    if (sock < 0)
    {
        event = "[ERROR] Creation of peer client socket failed [TIME] ";
        m_logger->Log(event);
    }
    if (connect(sock, (struct sockaddr *)&m_server_addr, sizeof(m_server_addr)) < 0)
    {
        event = "[ERROR] Connecting to super peer with ";
        FillLog(event, "Connection Fail", m_server_ip, m_fd);
        return false;
    }
    if (send(sock, &m, sizeof(m), 0) < 0)
    {
        event = "[SEND_FAILURE] sending error: super peer did not get ADD_FILE ";
        event += "[PeerID] " + std::to_string(m_peer_id) + " ";
        FillLog(event, "Add_file", m_server_ip, sock);
        return false;
    }
    char buffer[sizeof(Message)];
    if (read(sock, buffer, sizeof(buffer)) < 0)
    {
        event = "[ERROR] reading at client after Adding file send ";
        FillLog(event, "Add_file", m_server_ip, sock);
        return false;
    }
    auto *msg = reinterpret_cast<Message *>(buffer);
    if (msg->mess_id != COMPLETED)
    {
        event = "[ERROR] super peer did not complete the addition of file on peer list ";
        FillLog(event, "Add_file", m_server_ip, sock);
        return false;
    }
    event = "[SUCCESS] super peer added file to peer list ";
    FillLog(event, "ADD_FILE", m_server_ip, sock, start);
    return true;
}

void NodeClient::FillLog(std::string_view event, std::string_view message, std::string_view ip, int peer_sock,
                         const std::chrono::system_clock::time_point &start) noexcept
{
    std::string err;
    std::ostringstream os;
    auto end = std::chrono::system_clock::now();
    os.str("");
    os << event << "[MESSAGE] " << message << " [INDEX_IP] " << ip << " [SOCKET] " << peer_sock;
    err = os.str();
    m_logger->Log(err, message, start, end);
}

void NodeClient::FillLog(std::string_view event, std::string_view message, std::string_view ip,
                         int peer_sock) noexcept
{
    std::string err;
    std::ostringstream os;
    os.str("");
    os << event << "[MESSAGE] " << message << " [INDEX_IP] " << ip << " [SOCKET] " << peer_sock << " [TIME] ";
    err = os.str();
    m_logger->Log(err);
}