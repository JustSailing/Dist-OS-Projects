#include "nodeserver.h"
#include <iostream>
#include <arpa/inet.h>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <array>
uint16_t peer_id;
NodeServer::NodeServer(const std::string &log_file, const std::string &csv_file,
                       const std::string &dir, std::string super_ip, std::string super_port,
                       std::string ip_addr, std::string port_number)
{
    m_logger = std::make_unique<Logger>(log_file, csv_file);
    m_dir_folder = dir;
    m_addr = sockaddr_in{};
    m_ip = ip_addr;
    m_port = port_number;
    m_super_ip = super_ip;
    m_super_port = super_port;
    m_fd = socket(AF_INET, SOCK_STREAM, 0);
    m_addr = sockaddr_in{};
    std::string event;
    if (m_fd < 0)
    {
        event = "[ERROR] Creation of peer server socket failed [TIME] ";
        m_logger->Log(event);
        bool check = m_logger->CloseLog();
        if (check)
        {
            std::cout << "main thread closing log\n";
        }
        std::cout << "peer server socket failed to be created\n"
                  << std::endl;
        exit(2);
    }
    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = inet_addr(ip_addr.c_str());
    m_addr.sin_port = htons(std::stoi(port_number));
    if (bind(m_fd, (sockaddr *)&m_addr, sizeof(m_addr)) < 0)
    {
        event = "[ERROR] Binding peer server socket to ip failed [TIME] ";
        m_logger->Log(event);
        bool check = m_logger->CloseLog();
        if (check)
        {
            std::cout << "main thread closing log\n";
        }
        std::cout << "binding peer server socket to ip address failed" << std::endl;
        exit(2);
    }
    event = "[SUCCESS] Binding peer server to socket with ip [IP] " + m_ip + " [PORT] " + m_port + " [TIME] ";
    m_logger->Log(event);
    if (listen(m_fd, SOMAXCONN) < 0)
    {
        event = "[ERROR] Listening on peer server socket failed [TIME] ";
        m_logger->Log(event);
        std::cout << "Listening on peer server socket failed" << std::endl;
        exit(2);
    }
    event = "[SUCCESS] Listening on peer server socket to [IP] " + m_ip + " [PORT] " + m_port + " [TIME] ";
    m_logger->Log(event);
    std::cout << "Initialization success" << std::endl;
    auto check = Register();
    if (!check) {
        std::string event = "[FAILURE] Could not register peer node ";
        FillLog(event, "Register peer", m_super_ip, m_fd);
        std::cout << "failed to register peer with index server" << std::endl;
        exit(2);
    }
    running = true;
}

bool NodeServer::Register() noexcept
{
    auto start = std::chrono::system_clock::now();
    auto fd = socket(AF_INET, SOCK_STREAM, 0);
    std::string event;
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
    m_server_addr.sin_family = AF_INET;
    m_server_addr.sin_addr.s_addr = inet_addr(m_super_ip.c_str());
    m_server_addr.sin_port = htons(std::stoi(m_super_port));
    //while(true){
    if (connect(fd, (struct sockaddr *)&m_server_addr, sizeof(m_server_addr)) < 0)
    {
        event = "[ERROR] Connecting to index server with ";
        FillLog(event, "Connection Fail", m_super_ip, m_fd);
        close(fd);
        return false;
    }
    //break;
    //}

    Message m{};
    m.mess_id = REGISTER;
    m.peer_id = m_peer_id;
    m.size = 0;
    m.mess_in_seq = 1;
    m.total_mess = 1;
    std::string peer_node_info;
    peer_node_info += m_ip + "," + m_port+ ",";
    memset(m.body, '\0', sizeof(m.body));
    strncpy(m.body, peer_node_info.c_str(), peer_node_info.length());
    if (send(fd, &m, sizeof(m), 0) < 0)
    {
        event = "[FAILURE] sending error: Register Peer with  ";
        FillLog(event, "Register", m_super_ip, fd);
        return false;
    }
    char buffer[sizeof(Message)];
    memset(buffer, '\0', sizeof(buffer));
    if (read(fd, buffer, sizeof(buffer)) < 0)
    {
        event = "[ERROR] reading from super peer server after sending register ";
        FillLog(event, "Register", m_super_ip, fd);
        return false;
    }
    m_logger->Log("Before printing body ");
    auto *mess = reinterpret_cast<Message *>(buffer);
    m_logger->Log(mess->body);
    if (mess->mess_id == UPDATE)
    {
        m_logger->Log("in update message");
        std::string f;
        std::vector<std::string> all;
        {
            auto counter = 0;
            const std::lock_guard<std::mutex> lck(lock_all_files);
            for (const auto &file : all_files)
            {
                if ((counter + file.length()) < 1024)
                {
                    f += file + ",";
                    counter += file.length();
                }
                else
                {
                    all.push_back(f);
                    f = "";
                    counter = 0;
                }
            }
        }
        if(f != ""){
            all.push_back(f);
        }
        for(auto &i : all) {
            m_logger->Log(i);
        }
        m_logger->Log(" after all_files");
        u_int64_t i = 0;
        auto counter = 1;
        m_logger->Log(std::to_string(all.size()));
        while (i < all.size())
        {
            m.mess_id = UPDATE;
            m.peer_id = m_peer_id;
            m.size = all[i].length();
            m.mess_in_seq = counter;
            m.total_mess = all.size();
            memset(m.body, '\0', sizeof(m.body));
            strncpy(m.body, all[i].c_str(), all[i].length());
            if (send(fd, &m, sizeof(m), 0) < 0)
            {
                event = "[SEND_FAILURE] sending file list from index server ";
                event += "[PeerID] " + std::to_string(m_peer_id) + " ";
                FillLog(event, "RegisterNode_send", m_super_ip, fd);
                return false;
            }
            m_logger->Log("after send");
            m_logger->Log(all[i]);

            counter++;
            i++;
        }
    }
    else
    {
        event = "[ERROR] index server did not reply with update file list after Register ";
        FillLog(event, "Register", m_super_ip, fd);
        return false;
    }
    memset(buffer, '\0', sizeof(buffer));
    if (read(fd, buffer, sizeof(buffer)) < 0)
    {
        event = "[ERROR] reading from index server after sending register ";
        FillLog(event, "Register", m_super_ip, fd);
        return false;
    }
    mess = reinterpret_cast<Message *>(buffer);
    m_logger->Log(std::to_string(mess->mess_id));
    if (mess->mess_id == COMPLETED)
    {
        event = "[SUCCESS] Registering with index server ";
        m_peer_id = mess->peer_id;
        peer_id = mess->peer_id;
        FillLog(event, "REGISTER", m_super_ip, fd, start);
        return true;
    }
    else
    {
        return false;
    }
}

void NodeServer::Start() noexcept
{
    std::array<std::thread, 5> server_threads;
    for (auto &t : server_threads)
    {
        t = std::thread([&]
                        { Accept(); });
    }
    for (auto &t : server_threads)
    {
        t.join();
    }
}

void NodeServer::Accept() noexcept
{
    std::cout << "ThreadId: " << std::this_thread::get_id() << '\n';
    std::string event;
    int peer_sock;
    sockaddr_in peer_client{};
    std::string peer_sock_ip;
    auto len_addr = sizeof(peer_client);
    running = true;
    while (running)
    {
        peer_sock = accept(m_fd, (sockaddr *)&peer_client, (socklen_t *)&len_addr);
        if (peer_sock < 0)
        {
            event = "[ERROR] Creating new socket with the new incoming connection [TIME] ";
            m_logger->Log(event);
            continue;
        }
        auto start = std::chrono::system_clock::now();
        peer_sock_ip = inet_ntoa(peer_client.sin_addr);
        event = "[SUCCESS] Accepting client on ";
        std::ostringstream os;
        os << event << "[SOCKET] " << peer_sock << " [IP] "
           << inet_ntoa(peer_client.sin_addr) << " [TIME] ";
        event = os.str();
        m_logger->Log(event);
        os.str("");
        os << "[SUCCESS] Receiving from [IP] " << peer_sock_ip << " [PORT] "
           << htons(peer_client.sin_port) << " [TIME] ";
        event = os.str();
        m_logger->Log(event);
        os.str("");
        Receive(peer_sock, peer_sock_ip, start);
        close(peer_sock);
    }
}

void NodeServer::Receive(int peer_sock, std::string_view ip,
                         std::chrono::system_clock::time_point &start) noexcept
{
    std::string event;
    char buffer[sizeof(Message)];
    std::ostringstream os;
    memset(buffer, '\0', sizeof(Message));
    if (read(peer_sock, buffer, sizeof(buffer)) < 0)
    {
        event = "[ERROR] reading from client after accepting connection [TIME] ";
        m_logger->Log(event);
        return;
    }
    // auto finish_read = std::chrono::system_clock::now();
    std::string ip_addr(ip.data());
    // event += "[SUCCESS] reading from client [IP] " + ip_addr;
    auto *msg = reinterpret_cast<Message *>(buffer);

    switch (msg->mess_id)
    {
    case DOWNLOAD:
    {
        event = "[RECEIVED] Download request [TIME] ";
        FillLog(event, "DOWNLOAD", ip, peer_sock);
        Download(*msg, peer_sock, ip, start);
        return;
    }break;
    default:
    {
        event = "[ERROR] Peer acting as a server should only serve download request [TIME] ";
        m_logger->Log(event);
        return;
    }
    }
}

void NodeServer::Download(const Message &msg, int peer_sock, std::string_view ip,
                          const std::chrono::system_clock::time_point &start) noexcept
{
    std::string event;
    std::string file_to_send(msg.body);
    bool exists = false;
    // check if peer has the file requested
    {
        const std::lock_guard<std::mutex> lck(lock_all_files);
        for (const auto &file : all_files)
        {
            if (file == file_to_send)
            {
                exists = true;
            }
        }
    }
    if (!exists)
    {
        Message m{};
        m.mess_id = ERROR;
        m.peer_id = msg.peer_id;
        m.size = 0;
        m.mess_in_seq = 1;
        m.total_mess = 1;
        memset(m.body, '\0', sizeof(m.body));
        if (send(peer_sock, &m, sizeof(m), 0) < 0)
        {
            event = "[FAILURE] sending error file does not exist to ";
            event += "[PeerID] " + std::to_string(msg.peer_id) + " ";
            FillLog(event, "Download", ip, peer_sock);
            return;
        }
        return;
    }
    FillLog(file_to_send, "file to send", "", 0);
    std::string full_path = m_dir_folder + file_to_send;
    std::ifstream f(full_path);
    if (!f.good())
    {
        event = "[ERROR] error getting file on peer server side ";
        FillLog(event, "Download", ip, peer_sock);
        Message m{};
        m.mess_id = ERROR;
        m.peer_id = msg.peer_id;
        m.size = event.length();
        m.mess_in_seq = 1;
        m.total_mess = 1;
        memset(m.body, '\0', sizeof(m.body));
        strncpy(m.body, event.c_str(), event.length());
        if (send(peer_sock, &m, sizeof(m), 0) < 0)
        {
            event = "[FAILURE] could not send message to peer that couldn't open file ";
            event += "[PeerID] " + std::to_string(msg.peer_id) + " ";
            FillLog(event, "Download", ip, peer_sock);
            return;
        }
        return;
    }
    std::streampos filesize = 0;
    f.seekg(0, std::ios::end);
    filesize = f.tellg() - filesize;
    f.clear();
    f.seekg(0);
    char buffer[1024];
    Message mess_out{};
    if (filesize < 1024)
    {
        char buff[(int)filesize];
        bzero(buff, sizeof(buff));
        mess_out.mess_id = DOWNLOAD;
        mess_out.size = filesize;
        mess_out.mess_in_seq = 1;
        mess_out.total_mess = 1;
        mess_out.peer_id = msg.peer_id;
        f.read(buff, filesize);
        // m_log->Log(buffer);
        std::strncpy(mess_out.body, buff, sizeof(buff));
        if (send(peer_sock, &mess_out, sizeof(mess_out), 0) <= 0)
        {
            event = "[ERROR] sending to from client to server handling ";
            FillLog(event, "Download", ip, peer_sock);
            return;
        }
        event = "[SUCCESS] sending from server to other peer [MESSAGE] Download [IP] ";
        FillLog(event, "Download", ip, peer_sock, start);
        return;
    }
    int loops = filesize / 1024;
    int lastChunk = filesize % 1024;
    // I messed up on first programming assignment this should be if ((lastChunk > 0) ? 1 : 0)
    Message mess[loops + ((lastChunk > 0) ? 1 : 0)];
    for (int i = 0; i < loops; i++)
    {
        mess_out.mess_id = DOWNLOAD;
        mess_out.mess_in_seq = i + 1;
        mess_out.total_mess = loops + ((lastChunk > 0) ? 1 : 0);
        mess_out.size = sizeof(buffer);
        memset(buffer, '\0', sizeof(buffer));
        f.read(buffer, sizeof(buffer));
        std::strncpy(mess_out.body, buffer, sizeof(buffer));
        mess[i] = mess_out;
    }
    if (lastChunk > 0)
    {
        memset(buffer, '\0', sizeof(buffer));
        f.read(buffer, lastChunk);
        mess_out.mess_id = DOWNLOAD;
        mess_out.size = lastChunk;
        mess_out.mess_in_seq = loops + 1;
        mess_out.total_mess = loops + 1;
        mess[loops] = mess_out;
    }
    if (send(peer_sock, mess, sizeof(mess), 0) <= 0)
    {
        event = "[ERROR] sending to file to peer failed ";
        FillLog(event, "Download", ip, peer_sock);
        return;
    }
    event = "[SUCCESS] sending from peer to peer [MESSAGE] Download [IP] ";
    FillLog(event, "Download", ip, peer_sock, start);
}


void NodeServer::FillLog(std::string_view event, std::string_view message, std::string_view ip, int peer_sock,
                         const std::chrono::system_clock::time_point &start) noexcept
{
    std::string err;
    std::ostringstream os;
    auto end = std::chrono::system_clock::now();
    os.str("");
    os << event << "[MESSAGE] " << message << " [PEER_IP] " << ip << " [SOCKET] " << peer_sock;
    err = os.str();
    m_logger->Log(err, message, start, end);
}

void NodeServer::FillLog(std::string_view event, std::string_view message, std::string_view ip,
                         int peer_sock) noexcept
{
    std::string err;
    std::ostringstream os;
    os.str("");
    os << event << "[MESSAGE] " << message << " [PEER_IP] " << ip << " [SOCKET] " << peer_sock << " [TIME] ";
    err = os.str();
    m_logger->Log(err);
}