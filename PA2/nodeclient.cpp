#include "peer/nodeclient.h"
#include "peer/message.h"

#include <iostream>
#include <arpa/inet.h>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <array>
#include <queue>

extern std::array<std::pair<u_int16_t, std::string>, 20> q;
int q_index = 0;
extern int q_size;
std::mutex lock_queue;

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
    // m_fd = socket(AF_INET, SOCK_STREAM, 0);
    std::string event = "[Success] Initialization of member variables";

    FillLog(event, "Initialization Client", m_server_ip, 0);
}

void NodeClient::Start() noexcept
{
    // m_running = true; // changed to global variable
    auto check = Register();
    if (!check) {
        std::string event = "[FAILURE] Could not register peer node ";
        FillLog(event, "Register peer", m_server_ip, m_fd);
        std::cout << "failed to register peer with index server" << std::endl;
        exit(2);
    }
    sleep(1);
    running = true;
    std::array<std::thread, 5> client_threads;
    for (auto &t : client_threads)
    {
        t = std::thread([&]
                        { Connect(); });
    }
    for (auto &t : client_threads)
    {
        t.join();
    }
    sleep(10);
}

void NodeClient::Connect() noexcept
{
    while (running)
    {
        std::pair<u_int16_t, std::string> msg;
        {
            std::lock_guard<std::mutex> lck(lock_queue);
            //if (q.size() == 0)
            //{
                //running = false;
              //  break;
            //}
            if(q_index > q_size){
                //running = false;
                break;
            }
            msg = q[q_index];
            q_index++;
        }

        switch (msg.first)
        {
        case QUERY:
        {
            auto start = std::chrono::system_clock::now();
            Query(msg.second, start);
        }break;
        case UNREGISTER:
        {
            auto start = std::chrono::system_clock::now();
            Unregister(start);
        }break;
        default:
                //running = false;
                continue;
        }

    }
    //std::string event = "[Finished] All work completed ";
    //FillLog(event, "WORK completed", 0, 0);
}

bool NodeClient::Register() noexcept
{
    // I think this is where I messed up last assignment, I was trying to send and read on the same
    // Socket which led to many issues
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
    m_server_addr.sin_addr.s_addr = inet_addr(m_server_ip.c_str());
    m_server_addr.sin_port = htons(std::stoi(m_server_port));
    while(true){
    if (connect(fd, (struct sockaddr *)&m_server_addr, sizeof(m_server_addr)) < 0)
    {
        event = "[ERROR] Connecting to index server with ";
        FillLog(event, "Connection Fail", m_server_ip, m_fd);
        //close(fd);
        //return false;
    }
    break;
}
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
        FillLog(event, "Register", m_server_ip, fd);
        return false;
    }
    char buffer[sizeof(Message)];
    memset(buffer, '\0', sizeof(buffer));
    if (read(fd, buffer, sizeof(buffer)) < 0)
    {
        event = "[ERROR] reading from index server after sending register ";
        FillLog(event, "Register", m_server_ip, fd);
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
            std::lock_guard<std::mutex> lck(lock_all_files);
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
                FillLog(event, "RegisterNode_send", m_server_ip, fd);
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
        FillLog(event, "Register", m_server_ip, fd);
        return false;
    }
    memset(buffer, '\0', sizeof(buffer));
    if (read(fd, buffer, sizeof(buffer)) < 0)
    {
        event = "[ERROR] reading from index server after sending register ";
        FillLog(event, "Register", m_server_ip, fd);
        return false;
    }
    mess = reinterpret_cast<Message *>(buffer);
    m_logger->Log(std::to_string(mess->mess_id));
    if (mess->mess_id == COMPLETED)
    {
        event = "[SUCCESS] Registering with index server ";
        m_peer_id = mess->peer_id;
        FillLog(event, "REGISTER", m_server_ip, fd, start);
        close(fd);
        return true;
    }
    else
    {
    	close(fd);
        return false;
    }
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
        event = "[ERROR] Connecting to index server with ";
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
    event = "[SUCCESS] Unregistering from index server ";
    FillLog(event, "Unregister", m_server_ip, fd, start);
    running = false;
    //exit(5);
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
    //m_server_addr.sin_family = AF_INET;
    //m_server_addr.sin_addr.s_addr = inet_addr(m_server_ip.c_str());
    //m_server_addr.sin_port = htons(std::stoi(m_server_port));
    int cc = 0;
    while(true){
    if (connect(fd, (struct sockaddr *)&m_server_addr, sizeof(m_server_addr)) < 0)
    {
        cc++;
        event = "[ERROR] Connecting to index server with ";
        FillLog(event, "Connection Fail", m_server_ip, fd);
        if(cc == 10) { close(fd); return; }

        continue;
    }
    break;
    }
    
    Message m{};
    m.peer_id = m_peer_id;
    m.mess_id = QUERY;
    m.mess_in_seq = 1;
    m.total_mess = 1;
    memset(m.body, '\0', sizeof(m.body));
    strncpy(m.body, filename.c_str(), filename.length());
    if (send(fd, &m, sizeof(m), 0) <= 0)
    {
        event = "[ERROR] sending to query to index server failed ";
        FillLog(event, "Download", m_server_ip, fd);
        return;
    }
    char buffer[sizeof(Message)];
    memset(buffer, '\0', sizeof(buffer));
    if (read(fd, buffer, sizeof(buffer)) < 0)
    {
        event = "[ERROR] reading message from index server ";
        FillLog(event, "Query", m_server_ip, fd);
        return;
    }
    auto *msg = reinterpret_cast<Message *>(buffer);
    m_logger->Log(std::to_string(msg->mess_id));
    if (msg->mess_id != QUERY)
    {
        event = "[ERROR] wrong message from index server ";
        FillLog(event, "Query", m_server_ip, fd);
        return;
    }
    event = "[SUCCESS] peers who have [FILE] " + filename + " ";
    auto d = "QUERY " + filename;
    FillLog(event, "QUERY", m_server_ip, fd, start);
    return;
    std::string files(msg->body);
    if(files == "")
    {
        event = "[ERROR] no peer has file";
        FillLog(event, "QUERY", m_server_ip, fd);
        return;
    }
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
        else {
            fn = fn + x;
        }
    }

    // while (ss.good())
    // {
    //     std::string temp2;
    //     std::getline(ss, temp2, ',');
    //     strs.emplace_back(temp2);
    // }
    bool check = false;
    std::string ip;
    std::string port;
    std::chrono::system_clock::time_point t;
    if(strs.empty())
    {
        event = "[ERROR] no peer has file";
        FillLog(event, "QUERY", m_server_ip, fd);
        return;
    }
    //event = "[SUCCESS] QUERY from Index Server of peer with [IP] " + strs[0] + " [PORT] " + strs[1];
    //FillLog(event, "QUERY", m_server_ip, fd,start);
    close(fd);

    
}

bool NodeClient::Download(const std::string &ip, const std::string &port,
                          const std::string &filename, const std::chrono::system_clock::time_point &start) noexcept
{
    std::string event;
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        event = "[FAILURE] to create socket to download file from peer ";
        FillLog(event, "DOWNLOAD", ip, 0);
        return false;
    }
    event = "[SUCCESS] Created socket in download ";
    FillLog(event, "Download", ip, fd);
    sockaddr_in peer_addr{};
    peer_addr.sin_family = AF_INET;
    peer_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    peer_addr.sin_port = htons(std::stoi(port));
    FillLog(port, "port", ip, fd);
    if (connect(fd, (sockaddr *)&peer_addr, sizeof(peer_addr)) < 0)
    {
        event = "[FAILURE] failed to connect to peer to download [FILE] " + filename + " ";
        FillLog(event, "DOWNLOAD", ip, fd);
        close(fd);
        return false;
    }
    event = "[SUCCESS] Connected to peer socket in download ";
    FillLog(event, "Download", ip, fd);
    Message m{};
    m.mess_id = DOWNLOAD;
    m.peer_id = m_peer_id;
    m.size = filename.length();
    m.mess_in_seq = 1;
    m.total_mess = 1;
    memset(m.body, '\0', sizeof(m.body));
    strncpy(m.body, filename.c_str(), filename.length());
    if (send(fd, &m, sizeof(m), 0) < 0)
    {
        event = "[SEND_FAILURE] cant send single file download for [FILE] " + filename + " ";
        FillLog(event, "DOWNLOAD", ip, fd);
        close(fd);
        return false;
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
        return false;
    }
    event = "[SUCCESS] read from peer socket in download ";
    FillLog(event, "Download", ip, fd);
    auto *msg = reinterpret_cast<Message *>(buffer);
    if (msg->mess_id != DOWNLOAD)
    {
        event = "[ERROR] other peer sent error instead of file ";
        FillLog(event, "DOWNLOAD", ip, fd);
        close(fd);
        return false;
    }
    std::ofstream f;
    std::string full = m_dir_folder + filename;
    f.open(full, std::ios::app);
    if (!f.good())
    {
        event = "[ERROR] could not open file name in the peer directory ";
        FillLog(event, "DOWNLOAD", ip, fd);
        close(fd);
        return false;
    }
    f.write(msg->body, msg->size);
    auto count = 2;
    while (count <= msg->total_mess)
    {
        memset(buffer, '\0', sizeof(buffer));
        if (read(fd, buffer, sizeof(buffer)) < 0)
        {
            event = "[ERROR] reading from buffer trying to download file from peer ";
            FillLog(event, "DOWNLOAD", ip, fd);
            close(fd);
            return false;
        }
        msg = reinterpret_cast<Message *>(buffer);
        if (msg->mess_id != DOWNLOAD)
        {
            event = "[ERROR] other peer sent error instead of file ";
            FillLog(event, "DOWNLOAD", ip, fd);
            close(fd);
            return false;
        }
        f.write(msg->body, msg->size);
        f.flush();
        count++;
    }
    event = "[SUCCESS] Downloaded file from peer [FILE] " + filename + " ";
    //auto c = "DOWNLOAD" + filename;
    FillLog(event, "DOWNLOADED", ip, fd, start);
    auto st = std::chrono::system_clock::now();
    auto check = AddFile(filename, st);
    if (!check)
    {
        event = "[ERROR] Adding file to index server after downloading ";
        FillLog(event, "Add_file", ip, fd);
        close(fd);
        return false;
    }
    event = "[SUCCESS] Added file to the list on index server ";
    //auto l = "ADD_FILE " + filename;
    FillLog(event, "ADDED_FILE" , ip, fd, st);
    close(fd);
    return true;
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
        event = "[SEND_FAILURE] sending error: index server did not get DELETE_FILE ";
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
        event = "[ERROR] index server did not complete the deletion of file on peer list ";
        FillLog(event, "Add_file", m_server_ip, sock);
        return false;
    }
    event = "[SUCCESS] Index server added file to peer list ";
    FillLog(event, "DELETE_FILE", m_server_ip, sock, start);
    {
        const std::lock_guard<std::mutex> lck(lock_all_files);
        auto index = -1;
        for (u_int64_t i = 0; i < all_files.size(); i++)
        {
            if (all_files[i] == filename)
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

bool NodeClient::AddFile(const std::string &filename,
                         const std::chrono::system_clock::time_point &start) noexcept
{

    std::string event;
    Message m{};
    m.mess_id = ADD_FILE;
    m.mess_in_seq = 1;
    m.total_mess = 1;
    m.peer_id = m_peer_id;
    m.size = filename.length();
    memset(m.body, '\0', filename.length());
    strncpy(m.body, filename.c_str(), filename.length());
    auto sock = socket(AF_INET, SOCK_STREAM, 0);
    //std::string event;
    if (sock < 0)
    {
        event = "[ERROR] Creation of peer client socket failed [TIME] ";
        m_logger->Log(event);
    }
    if (connect(sock, (struct sockaddr *)&m_server_addr, sizeof(m_server_addr)) < 0)
    {
        event = "[ERROR] Connecting to index server with ";
        FillLog(event, "Connection Fail", m_server_ip, m_fd);
        return false;
    }
    if (send(sock, &m, sizeof(m), 0) < 0)
    {
        event = "[SEND_FAILURE] sending error: index server did not get ADD_FILE ";
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
        event = "[ERROR] index server did not complete the addition of file on peer list ";
        FillLog(event, "Add_file", m_server_ip, sock);
        return false;
    }
    event = "[SUCCESS] Index server added file to peer list ";
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
