#include <iostream>
#include <arpa/inet.h>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <array>

#include "indexserver.h"

IndexServer::IndexServer(const std::string &log_file, const std::string &csv_file,
                         const char *ip_addr, const char *port_number) {
    std::cout << "Initializing Index Server with [ip address]: " << ip_addr << " [port]"
              << port_number << std::endl;
    m_logger = std::make_unique<Logger>(log_file, csv_file);
    m_last_peer = 0;
    m_addr = sockaddr_in{};
    m_server_ip = ip_addr;
    m_port_number = port_number;
    m_server_fd = socket(AF_INET, SOCK_STREAM, 0);
    std::string event;
    if (m_server_fd < 0) {
        event = "[ERROR] Creation of server socket failed [TIME] ";
        m_logger->Log(event);
        bool check = m_logger->CloseLog();
        if (check) {
            std::cout << "main thread closed log\n";
        }
        std::cout << "socket creation failed\n"
                  << std::endl;
        exit(2);
    }
    event = "[SUCCESS] Creation of socket [SOCKET_FD] " + std::to_string(m_server_fd) + " [TIME] ";
    m_logger->Log(event);

    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = inet_addr(ip_addr);
    m_addr.sin_port = htons(std::stoi(port_number));

    if (bind(m_server_fd, (sockaddr *) &m_addr, sizeof(m_addr)) < 0) {
        event = "[ERROR] Binding server socket to ip failed [TIME] ";
        m_logger->Log(event);
        std::cout << "binding server socket to ip address failed" << std::endl;
        exit(2);
    }
    event = "[SUCCESS] Binding server to socket with ip [IP] " + m_server_ip + " [PORT] " + m_port_number + " [TIME] ";
    m_logger->Log(event);

    if (listen(m_server_fd, SOMAXCONN) < 0) {
        event = "[ERROR] Listening on server socket failed [TIME] ";
        m_logger->Log(event);
        std::cout << "Listening on server socket failed" << std::endl;
        exit(2);
    }
    event = "[SUCCESS] Listening on server socket to [IP] " + m_server_ip + " [PORT] " + m_port_number + " [TIME] ";
    m_logger->Log(event);
    std::cout << "Initialization success" << std::endl;
}

void IndexServer::Start() noexcept {
    m_running = true;
    // I think should work better than the last programming assignment
    // After looking at it, I definitely did it wrong. I think is was just one thread
    // running at a time this should. This still will be an infinite wait since
    // Receive will be in a infinite while loop.
    std::array<std::thread, 10> all_threads;
    for (auto &t: all_threads) {
        t = std::thread([&] { Connection(); });
    }

    for (auto &t: all_threads) {
        t.join();
    }
}

void IndexServer::Connection() noexcept {
    std::cout << "ThreadId: " << std::this_thread::get_id() << '\n';
    std::string event;
    int client_sock;
    sockaddr_in client;
    std::string c_sock_ip;
    auto len_addr = sizeof(client);
    while (m_running) {
        client_sock = accept(m_server_fd, (sockaddr *) &client, (socklen_t *) &len_addr);
        if (client_sock < 0) {
            event = "[ERROR] Creating new socket with the new incoming connection [TIME] ";
            m_logger->Log(event);
            continue;
        }
        auto start = std::chrono::system_clock::now();
        c_sock_ip = inet_ntoa(client.sin_addr);
        event = "[SUCCESS] Accepting client on ";
        std::ostringstream os;
        os << event << "[SOCKET] " << client_sock << " [IP] "
           << inet_ntoa(client.sin_addr) << " [TIME] ";
        event = os.str();
        m_logger->Log(event);
        os.str("");
        os << "[SUCCESS] Receiving from [IP] " << c_sock_ip << " [PORT] "
           << htons(client.sin_port) << " [TIME] ";
        event = os.str();
        m_logger->Log(event);
        os.str("");
        Receive(client_sock, c_sock_ip, start);
        close(client_sock);
    }
}

void IndexServer::Receive(int c_sock, std::string_view ip,
                          std::chrono::system_clock::time_point &start) noexcept {
    std::string event;
    char buffer[sizeof(Message)];
    std::ostringstream os;
    memset(buffer, '\0', sizeof(Message));
    if (read(c_sock, buffer, sizeof(buffer)) < 0) {
        event = "[ERROR] reading from client after accepting connection [TIME] ";
        m_logger->Log(event);
        return;
    }
    // auto finish_read = std::chrono::system_clock::now();
    std::string ip_addr(ip.data());
    // event += "[SUCCESS] reading from client [IP] " + ip_addr;
    auto *msg = reinterpret_cast<Message *>(buffer);

    switch (msg->mess_id) {
        case QUERY: {
            if (!CheckRegistered(msg->peer_id, "Query", ip, c_sock)) {
                return;
            }

            Query(*msg, c_sock, ip, start);
            return;
        }
        case ADD_FILE: {
            if (!CheckRegistered(msg->peer_id, "AddFile", ip, c_sock)) {
                return;
            }
            Add(*msg, c_sock, ip, start);
            return;
        }
        case DELETE_FILE: {
            if (!CheckRegistered(msg->peer_id, "DeleteFile", ip, c_sock)) {
                return;
            }
            Delete(*msg, c_sock, ip, start);
            return;
        }
        case REGISTER: {
            event = "[Received] message register";
            FillLog(event, "Register", ip, c_sock);
            Register(*msg, c_sock, ip, start);
            return;
        }
        case UNREGISTER: {
            Unregister(*msg, c_sock, ip, start);
            return;
        }
    }
}

bool IndexServer::GetPeerNodeList(const Message &msg, int c_sock, std::string_view ip) noexcept {
    std::vector<std::string> list_of_peers;
    std::string pp;
    std::string _file(msg.body); // should just be the file name
    // block checks for peers who have the file and stores them in list_of_peers
    {
        const std::lock_guard<std::mutex> lck(m_lock_peer_nodes);
        for (const auto &peer: m_peer_nodes) {
            if (msg.peer_id == peer->id) {
                continue;
            }
            for (const auto &p: peer->files_map) {
                if (p == _file) {
                    pp += peer->ip + "," + peer->port  + ",";

                    list_of_peers.push_back(pp);
                     pp = "";
                } else { continue; }
            }
        }
    }
    // the size of mess would be the amount of messages sent
    // to the peer node
    std::vector<std::string> mess;
    auto counter = 0;
    std::string final_string;
    // fit as many peer info into a message body
    for (auto &peer_node: list_of_peers) {
        if ((counter + peer_node.length() + 1) < 1024) {
            final_string += peer_node;
            counter += peer_node.length();
        } else {
            mess.push_back(final_string);
            final_string = "";
            counter = 0;
        }
    }
    if (final_string != "") {
        mess.push_back(final_string);
    }
    counter = 1;
    std::string event;
    if (mess.empty()) {
        event = "[SUCCESS] Sent empty list to client in query";
        FillLog(event, "QUERY_LIST", ip, c_sock);
        Message m{};
        m.mess_id = QUERY;
        m.peer_id = msg.peer_id;
        m.size = 0;
        m.mess_in_seq = 1;
        m.total_mess = 1;
        memset(m.body, '\0', sizeof(m.body));
        // strncpy(m.body, , file.length());
        if (send(c_sock, &m, sizeof(m), 0) < 0) {
            event = "[FAILURE] failed sending query reply back to client from index server ";
            event += "[PeerID] " + std::to_string(msg.peer_id) + " ";
            FillLog(event, "Query", ip, c_sock);
            return false;
        }
    } else {
        for (auto &file: mess) {
            Message m{};
            m.mess_id = QUERY;
            m.peer_id = msg.peer_id;
            m.size = file.length();
            m.mess_in_seq = counter;
            m.total_mess = mess.size();
            memset(m.body, '\0', sizeof(m.body));
            strncpy(m.body, file.c_str(), file.length());
            if (send(c_sock, &m, sizeof(m), 0) < 0) {
                event = "[FAILURE] failed sending query reply back to client from index server ";
                event += "[PeerID] " + std::to_string(msg.peer_id) + " ";
                FillLog(event, "Query", ip, c_sock);
                return false;
            }
            FillLog(file, "[THELIST]", ip, c_sock);
            counter++;
        }
        event = "[SUCCESS] Sent non empty list to client in query";
        FillLog(event, "QUERY_LIST", ip, c_sock);
    }
    return true;
}

bool IndexServer::AddNewFile(const Message &msg, int c_sock, std::string_view ip) noexcept {
    std::ostringstream os;
    std::string event;
    std::string _file(msg.body);
    bool check = false;
    {
        const std::lock_guard<std::mutex> lck(m_lock_peer_nodes);
        for (auto &peer: m_peer_nodes) {
            if (peer->id == msg.peer_id) {
                peer->files_map.push_back(_file);
                check = true;
                break;
            }
        }
    }
    if (check) {
        std::string file = "Completed Adding of File ";
        Message m{};
        m.mess_id = COMPLETED;
        m.peer_id = msg.peer_id;
        m.size = file.length();
        m.mess_in_seq = 1;
        m.total_mess = 1;
        memset(m.body, '\0', sizeof(m.body));
        strncpy(m.body, file.c_str(), file.length());
        if (send(c_sock, &m, sizeof(m), 0) < 0) {
            event = "[SEND_FAILURE] failed sending adding file reply back to client from index server ";
            event += "[PeerID] " + std::to_string(msg.peer_id) + " ";
            FillLog(event, "ADD_FILE", ip, c_sock);
            return false;
        }
        return true;
    } else {
        std::string file = "Failed to Add File ";
        Message m{};
        m.mess_id = ERROR;
        m.peer_id = msg.peer_id;
        m.size = file.length();
        m.mess_in_seq = 1;
        m.total_mess = 1;
        memset(m.body, '\0', sizeof(m.body));
        strncpy(m.body, file.c_str(), file.length());
        if (send(c_sock, &m, sizeof(m), 0) < 0) {
            event = "[SEND_FAILURE] failed sending adding file reply back to client from index server ";
            event += "[PeerID] " + std::to_string(msg.peer_id) + " ";
            FillLog(event, "ADD_FILE", ip, c_sock);
            return false;
        }
        return false;
    }
}

bool IndexServer::DeleteFile(const Message &msg, int c_sock, std::string_view ip) noexcept {
    std::ostringstream os;
    std::string event;
    std::string _file(msg.body);

    auto check = -1;
    {
        const std::lock_guard<std::mutex> lck(m_lock_peer_nodes);
        for (auto &peer: m_peer_nodes) {
            if (peer->id == msg.peer_id) {
                for (auto i = 0; i < peer->files_map.size(); i++) {
                    if (peer->files_map[i] == _file) {
                        check = i;
                        break;
                    }
                }
                break;
            }
        }
        if(check > -1) m_peer_nodes.erase(m_peer_nodes.begin() + check);
    }
    if (check > -1) {
        std::string file = "Completed Deletion of File ";
        Message m{};
        m.mess_id = COMPLETED;
        m.peer_id = msg.peer_id;
        m.size = file.length();
        m.mess_in_seq = 1;
        m.total_mess = 1;
        memset(m.body, '\0', sizeof(m.body));
        strncpy(m.body, file.c_str(), file.length());
        if (send(c_sock, &m, sizeof(m), 0) < 0) {
            event = "[SEND_FAILURE] failed sending deletion reply back to client from index server ";
            event += "[PeerID] " + std::to_string(msg.peer_id) + " ";
            FillLog(event, "DELETION", ip, c_sock);
            return false;
        }
        return true;
    } else {
        std::string file = "Failed to Delete of File ";
        Message m{};
        m.mess_id = ERROR;
        m.peer_id = msg.peer_id;
        m.size = file.length();
        m.mess_in_seq = 1;
        m.total_mess = 1;
        memset(m.body, '\0', sizeof(m.body));
        strncpy(m.body, file.c_str(), file.length());
        if (send(c_sock, &m, sizeof(m), 0) < 0) {
            event = "[SEND_FAILURE] failed sending deletion reply back to client from index server ";
            event += "[PeerID] " + std::to_string(msg.peer_id) + " ";
            FillLog(event, "DELETION", ip, c_sock);
            return false;
        }
        return false;
    }

}

int IndexServer::RegisterNode(const Message &msg, int c_sock, std::string_view ip) noexcept {
    auto peer_id = 0;
    std::string event;

    std::unique_ptr<PeerNode> peer = std::make_unique<PeerNode>();
    std::string info(msg.body);

    std::vector<std::string> strs;
    std::string fn = "";
    for (auto x : info)
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

    // In order to register a node, the peer need's to supply their ip address
    // and port so other peers can connect and download
    // First index ip address and second index port number
    if (strs.size() != 2) {
        event = "[ERROR] peer connected failed to supply ip address and port correctly ";
        event += "[PeerID] " + std::to_string(msg.peer_id) + " ";
        FillLog(event, "REGISTER_NODE", ip, c_sock);
        Message m{};
        m.mess_id = ERROR;
        m.peer_id = 0;
        m.size = event.length();
        m.mess_in_seq = 1;
        m.total_mess = 1;
        memset(m.body, '\0', sizeof(m.body));
        strncpy(m.body, event.c_str(), event.length());
        if (send(c_sock, &m, sizeof(m), 0) < 0) {
            event = "[SEND_FAILURE] sending error client not registered from index server ";
            event += "[PeerID] " + std::to_string(msg.peer_id) + " ";
            FillLog(event, "RegisterNode_send", ip, c_sock);
            return 0;
        }
        return 0;
    }

    peer->ip = strs[0];
    peer->port = strs[1];
    event = "[SUCCESS] Got IP and Port of peer [IP] " + strs[0] + " [PORT] " + strs[1];
    FillLog(event, "REGISTER", ip, c_sock);
    // std::string comp = "Completed adding ip and port of peer to Index Server ";
    Message m{};
    // m.mess_id = COMPLETED;
    // m.peer_id = peer_id;
    // m.size = comp.length();
    // m.mess_in_seq = 1;
    // m.total_mess = 2;
    // memset(m.body, '\0', sizeof(m.body));
    // strncpy(m.body, comp.c_str(), comp.length());
    // if (send(c_sock, &m, sizeof(m), 0) < 0)
    // {
    //     event = "[SEND_FAILURE] sending error client not registered from index server ";
    //     event += "[PeerID] " + std::to_string(msg.peer_id) + " ";
    //     FillLog(event, "RegisterNode_send", ip, c_sock);
    //     return false;
    // }

    std::string comp = "Need to update peer file list ";
    m.mess_id = UPDATE;
    m.peer_id = peer_id;
    m.size = comp.length();
    m.mess_in_seq = 1;
    m.total_mess = 1;
    memset(m.body, '\0', sizeof(m.body));
    strncpy(m.body, comp.c_str(), comp.length());
    FillLog(comp, "[SENDING FROM REGISTER]", ip, c_sock);
    if (send(c_sock, &m, sizeof(m), 0) < 0) {
        event = "[SEND_FAILURE] sending error client needs to updage file list from index server ";
        event += "[PeerID] " + std::to_string(msg.peer_id) + " ";
        FillLog(event, "RegisterNode_send", ip, c_sock);
        return 0;
    }
    FillLog(comp, "[FINISHED SENDING FROM REGISTER]", ip, c_sock);
    char buffer[sizeof(Message)];
    memset(buffer, '\0', sizeof(Message));
    if (read(c_sock, buffer, sizeof(Message)) < 0) {
        event = "[ERROR] reading from client trying to register node ";
        event += "[PeerID] " + std::to_string(msg.peer_id) + " ";
        FillLog(event, "RegisterNode_read", ip, c_sock);
        return 0;
    }

    auto *mess = reinterpret_cast<Message *>(buffer);
    std::string b(mess->body);
    m_logger->Log(b);
    if (mess->mess_id != UPDATE) {
        return 0;
    }
    auto count = 2;
    FillLog(b, "body message from peer", ip, c_sock);
    fn = "";
    for (auto x : b)
    {
        if (x == ',')
        {
            peer->files_map.push_back(fn);
            fn = "";
        }
        else {
            fn = fn + x;
        }
    }
//    ss.str(mess->body);
//    while (ss.good()) {
//        std::string temp2;
//        std::getline(ss, temp2, ',');
//        peer->files_map.push_back(temp2);
//    }
    while (count <= mess->total_mess) {
        char buffer[sizeof(Message)];
        memset(buffer, '\0', sizeof(buffer));
        if (read(c_sock, buffer, sizeof(buffer)) < 0) {
            event = "[ERROR] reading from client trying to get peer's file list ";
            event += "[PeerID] " + std::to_string(msg.peer_id) + " ";
            FillLog(event, "RegisterNode_read", ip, c_sock);
            return 0;
        }
        auto *mess = reinterpret_cast<Message *>(buffer);
        if (mess->mess_id != UPDATE) {
            event = "[ERROR] reading peer's file list, message id not UPDATE ";
            event += "[PeerID] " + std::to_string(msg.peer_id) + " ";
            FillLog(event, "RegisterNode_read", ip, c_sock);
            return false;
        }
        fn = "";
        for (auto x : b) {
            if (x == ',') {
                peer->files_map.push_back(fn);
                fn = "";
            } else {
                fn = fn + x;
            }
        }
        count++;
    }
    // Debug purposes. To see if the files that are added look like files
//    {
//        std::lock_guard<std::mutex> lck(m_lock_register_peer);
//        m_last_peer++;
//        peer_id = m_last_peer;
//        peer->id = peer_id;
//        m_register_peer.insert({peer_id, true});
//    }
    std::string f = "file ";
    for(auto i : peer->files_map){
        auto event = f + i;
        FillLog(event, "file in map", ip, c_sock);
    }
    {
        std::lock_guard<std::mutex> lck(m_lock_peer_nodes);
        m_last_peer++;
        peer_id = m_last_peer;
        peer->id = peer_id;
        peer->registered = true;
        m_peer_nodes.push_back(std::move(peer));
    }
    m.mess_id = COMPLETED;
    m.peer_id = peer_id;
    m.size = 0;
    m.mess_in_seq = 1;
    m.total_mess = 1;
    memset(m.body, '\0', sizeof(m.body));
    strncpy(m.body, comp.c_str(), comp.length());
    if (send(c_sock, &m, sizeof(m), 0) < 0) {
        event = "[SEND_FAILURE] sending error: client did not get its peer id ";
        event += "[PeerID] " + std::to_string(msg.peer_id) + " ";
        FillLog(event, "RegisterNode_send", ip, c_sock);
        return 0;
    }

    return peer_id;
}

bool IndexServer::UnregisterNode(const Message &message, int peer_sock, std::string_view ip) noexcept {
    auto count = -1;
    std::string event;
    {
        std::lock_guard<std::mutex> lck(m_lock_peer_nodes);
        for (u_int64_t i = 0; i < m_peer_nodes.size(); i++) {
            if (message.peer_id == m_peer_nodes[i]->id) {
                count = i;
                break;
            }
        }
        if (count == -1) {
            event = "[ERROR] Peer not in server list ";
            event += "[PeerID] " + std::to_string(message.peer_id) + " ";
            FillLog(event, "Unregister", ip, peer_sock);
            return false;
        }
        m_peer_nodes.erase(m_peer_nodes.begin() + count);
    }
//    {
//        std::lock_guard<std::mutex> lck(m_lock_register_peer);
//        if (m_register_peer.erase(message.peer_id) < 1)
//        {
//            event = "[ERROR] failed to remove peer from server ";
//            event += "[PeerID] " + std::to_string(message.peer_id) + " ";
//            FillLog(event, "Unregister", ip, peer_sock);
//            return false;
//        }
//    }
    return true;
}

void IndexServer::Query(const Message &msg, int c_sock, std::string_view ip,
                        const std::chrono::system_clock::time_point &start) noexcept {
    std::string event;
    if (!GetPeerNodeList(msg, c_sock, ip)) {
        event = "[FAILURE] to send peer list holding a file to peer ";
        event += "[PeerID] " + std::to_string(msg.peer_id) + " ";
        FillLog(event, "Query", ip, c_sock, start);
        return;
    } else {
        event = "[SUCCESS] sending query list to client ";
        event += "[PeerID] " + std::to_string(msg.peer_id) + " ";
        FillLog(event, "Query", ip, c_sock, start);
        return;
    }
}

void IndexServer::Add(const Message &msg, int c_sock, std::string_view ip,
                      const std::chrono::system_clock::time_point &start) noexcept {
    std::string event;
    if (!AddNewFile(msg, c_sock, ip)) {
        event = "[FAILURE] adding file to peer's list ";
        event += "[PeerID] " + std::to_string(msg.peer_id) + " ";
        FillLog(event, "ADD_FILE", ip, c_sock, start);
        return;
    } else {
        event = "[SUCCESS] adding file to peer's list ";
        event += "[PeerID] " + std::to_string(msg.peer_id) + " ";
        FillLog(event, "ADD_FILE", ip, c_sock, start);
        return;
    }
}

void IndexServer::Delete(const Message &message, int peer_sock, std::string_view ip,
                         const std::chrono::system_clock::time_point &start) noexcept {
    std::string event;
    if (!DeleteFile(message, peer_sock, ip)) {

        event = "[FAILURE] deleting file to peer's list ";
        event += "[PeerID] " + std::to_string(message.peer_id) + " ";
        FillLog(event, "DELETE_FILE", ip, peer_sock, start);
        return;
    } else {
        event = "[SUCCESS] deleting file to peer's list ";
        event += "[PeerID] " + std::to_string(message.peer_id) + " ";
        FillLog(event, "DELETE_FILE", ip, peer_sock, start);
        return;
    }
}

void IndexServer::Register(const Message &message, int peer_sock, std::string_view ip,
                           const std::chrono::system_clock::time_point &start) noexcept {
    std::string event;
    auto check = RegisterNode(message, peer_sock, ip);
    if (check <= 0) {

        event = "[FAILURE] registering peer failed ";
        event += "[PeerID] " + std::to_string(message.peer_id) + " ";
        FillLog(event, "REGISTER", ip, peer_sock, start);
        return;
    } else {
        event = "[SUCCESS] registering peer ";
        event += "[PeerID] " + std::to_string(message.peer_id) + " ";
        FillLog(event, "REGISTER", ip, peer_sock, start);
        return;
    }
}

void IndexServer::Unregister(const Message &message, int peer_sock, std::string_view ip,
                             const std::chrono::system_clock::time_point &start) noexcept {
    std::ostringstream os;
    std::string event;
    if (!UnregisterNode(message, peer_sock, ip)) {
        event = "[FAILURE] unregistering peer failed ";
        event += "[PeerID] " + std::to_string(message.peer_id) + " ";
        FillLog(event, "REGISTER", ip, peer_sock, start);
        return;
    } else {
        event = "[SUCCESS] unregistering peer ";
        FillLog(event, "REGISTER", ip, peer_sock, start);
        return;
    }
}

bool IndexServer::CheckRegistered(u_int16_t peer_id, const std::string &message,
                                  std::string_view ip, int c_sock) noexcept {
    {
        const std::lock_guard<std::mutex> lck(m_lock_peer_nodes);
        for (auto &id: m_peer_nodes) {
            if (id->id == peer_id && id->registered) {
                return true;
            }
        }
    }
    // auto end = std::chrono::system_clock::now();
    std::ostringstream os;
    std::string event;
    os.str("");
    event = "[FAILURE] peer not registered with index server ";

    event += "[PeerID] " + std::to_string(peer_id) + " ";
    FillLog(event, message, ip, c_sock);

    std::string err_msg = "Peer Node not Registered Please Register with Server first ";
    Message m{};
    m.mess_id = ERROR;
    m.peer_id = peer_id;
    m.size = err_msg.length();
    m.mess_in_seq = 1;
    m.total_mess = 1;
    memset(m.body, '\0', sizeof(m.body));
    strncpy(m.body, err_msg.c_str(), err_msg.length());
    if (send(c_sock, &m, sizeof(m), 0) < 0) {
        event = "[SEND_FAILURE] sending error client not registered from index server ";
        event += "[PeerID] " + std::to_string(peer_id) + " ";
        FillLog(event, "PeerCheck_send", ip, c_sock);
        return false;
    }
    return false;
}

void IndexServer::FillLog(std::string_view event, std::string_view message, std::string_view ip, int peer_sock,
                          const std::chrono::system_clock::time_point &start) noexcept {
    std::string err;
    std::ostringstream os;
    auto end = std::chrono::system_clock::now();
    os.str("");
    os << event << "[MESSAGE] " << message << " [CLIENT_IP] " << ip << " [SOCKET] " << peer_sock;
    err = os.str();
    m_logger->Log(err, message, start, end);
}

void IndexServer::FillLog(std::string_view event, std::string_view message, std::string_view ip,
                          int peer_sock) noexcept {
    std::string err;
    std::ostringstream os;
    os.str("");
    os << event << "[MESSAGE] " << message << " [CLIENT_IP] " << ip << " [SOCKET] " << peer_sock << " [TIME] ";
    err = os.str();
    m_logger->Log(err);
}
