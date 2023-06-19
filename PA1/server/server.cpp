#include "server.h"
#include <netinet/in.h>
#include <string>
#include <ctime>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <filesystem>
#include <cstring>
#include <openssl/sha.h>
#include <unistd.h>
#include <vector>
#include <future>
#include <atomic>

std::atomic<int> num_of_threads = 10;
Server::Server(const char *server_folder, const char *log_file, const char *ip, const char* port) {
    this->m_log = new Logger(log_file);
    this->m_addr = sockaddr_in();
    this->m_dir_folder = server_folder;
    this->m_server_fd = socket(AF_INET, SOCK_STREAM, 0);
    std::string event;
    time_t now_time;
    char *now;
    if (m_server_fd < 0) {
        event = "[ERROR] Creation of server socket failed [TIME] ";
        now_time = time(nullptr);
        now = ctime(&now_time);
        event += now;
        event += '\n';
        m_log->Log(event);
        m_log->LogClose();
        exit(2);
    }
    event = "[Success] Creation of server socket [TIME] ";
    now_time = time(nullptr);
    now = ctime(&now_time);
    event += now;
    event += '\n';
    m_log->Log(event);

    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = inet_addr(ip);
    m_addr.sin_port = htons(std::stoi(port));

    if (bind(m_server_fd, (sockaddr *) &m_addr, sizeof(m_addr)) < 0) {
        event = "[ERROR] Biding of server socket to port failed [TIME] ";
        now_time = time(nullptr);
        now = ctime(&now_time);
        event += now;
        event += '\n';
        m_log->Log(event);
        m_log->LogClose();
        exit(2);
    }

    event = "[Success] Binding the server socket to [IP] 127.0.0.1 and [PORT] 55000 [TIME] ";
    now_time = time(nullptr);
    now = ctime(&now_time);
    event += now;
    event += '\n';
    m_log->Log(event);

    // May need to have each thread have its own SHA_256 context instead of just the main thread
    // according to the openssl documentation ctx is not thread safe having to wait on a lock may be
    // to time-consuming
    if (SHA256_Init(&this->m_context) < 0) {
        event = "[ERROR] Failed to initialize [TIME] ";
        now_time = time(nullptr);
        now = ctime(&now_time);
        event += now;
        event += '\n';
        m_log->Log(event);
        m_log->LogClose();
        exit(2);
    }
}

void Server::Start() {
    this->m_running = true;
    std::string event;
    time_t now_time;
    char *now;
    // The maximum number of request is hard coded by the kernel
    // running the program. On Linux, the default is 4096.
    if (listen(m_server_fd, SOMAXCONN) < 0) {
        event = "[ERROR] Listening on server socket failed [TIME] ";
        now_time = time(nullptr);
        now = ctime(&now_time);
        event += now;
        event += '\n';
        m_log->Log(event);
        m_log->LogClose();
        exit(2);
    }
    // 192.168.1.158 <- this is changed to 127.0.0.1 in order for it to work.
    event = "[SUCCESS] Listening on server socket to [IP] 127.0.0.1 and [PORT] 55000 [TIME] ";
    now_time = time(nullptr);
    now = ctime(&now_time);
    event += now;
    event += '\n';
    m_log->Log(event);
    while(m_running) {
        while (num_of_threads > 0) {
            num_of_threads -= 1;
            std::thread t1([this] { NeededForThreading(); });
            t1.join();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
void Server::NeededForThreading() {
        std::string event;
        int client_sock;
        sockaddr_in client_{};
        auto len_addr = sizeof(client_);
        std::thread p;
        client_sock = accept( m_server_fd, (sockaddr *) &client_, (socklen_t *) &len_addr);
        std::cout << client_sock << std::endl;
        if (client_sock < 0) {
            event = "[ERROR] Accepting on server socket with [IP] 127.0.0.1 and [PORT] 55000 [TIME] ";
            auto now_time = time(nullptr);
            auto now = ctime(&now_time);
            event += now;
            event += '\n';
            m_log->Log(event);
            num_of_threads += 1;
            return;
        }
        event = "[SUCCESS] Accepting on client on ";
        std::ostringstream os;
        os << event << "[SOCKET]" << client_sock << "with [IP] " << inet_ntoa(client_.sin_addr) << " [TIME] ";
        event = os.str();
        auto now_time = time(nullptr);
        auto now = ctime(&now_time);
        event += now;
        event += '\n';
        m_log->Log(event);
        std::thread tid([this, client_sock] { InitReceive(client_sock); });
        tid.join();
        num_of_threads += 1;
}

void Server::InitReceive(int c_sock) const {
    std::string event;
    time_t now_time;
    char *now;
    char *ip;
    sockaddr_in client{};
    auto len = sizeof(client);
    if (getpeername(c_sock, (struct sockaddr *) &client, (socklen_t *) &len) < 0) {
        event = "[ERROR] Getting client info from client socket after accepting connection [TIME] ";
        now_time = time(nullptr);
        now = ctime(&now_time);
        event += now;
        event += '\n';
        m_log->Log(event);
        //Receive(c_sock);
        return;
    }
    now_time = time(nullptr);
    now = ctime(&now_time);
    std::ostringstream ss;
    // auto *msg = reinterpret_cast<Message *>(buffer);
    ip = inet_ntoa(client.sin_addr);
    ss << "[SUCCESS] Receiving  from " << ip << " on port " << htons(client.sin_port)
       << " [TIME] " << now;
    event = ss.str();
    m_log->Log(event);
    Receive(c_sock, ip);
}

void Server::Receive(int c_sock, char *ip) const {
    std::string event;
    time_t now_time;
    char *now;
    std::string ipp(ip);
    char buffer[sizeof(Message)];
    bzero(buffer, sizeof(buffer));
    if (read(c_sock, buffer, sizeof(buffer)) < 0) {
        event = "[ERROR] reading from client after accepting connection [TIME] ";
        now_time = time(nullptr);
        now = ctime(&now_time);
        event += now;
        event += '\n';
        m_log->Log(event);
        return;
    }
    auto *msg = reinterpret_cast<Message *>(buffer);
    event = "[SUCCESS] reading from client [IP] ";
    event += ipp + " [TIME] ";
    //event += "[BYTES_RECEIVED] " + std::to_string(total) + " [TIME] ";
    now_time = time(nullptr);
    now = ctime(&now_time);
    event += now;
    m_log->Log(event);
    switch (msg->message) {
        case LIST_FILES:
            SendList(c_sock, ip);
            //Receive(c_sock, ip);
            break;
        case SINGLE_DL:
            SendFile(c_sock, ip, msg);
            //Receive(c_sock, ip);
            break;
        case ADD_FILE:
            // Possible that the file is greater than 1024 bytes
            AddFile(c_sock, ip, msg);
            break;
        case DELETE_FILE:
            DeleteFile(c_sock, ip, msg);
            break;
//        case APPEND_FILE:
//            AppendFile(c_sock, ip, msg);
//            break;
        default:
            SendError(c_sock, ip);
            //Receive(c_sock, ip);
            break;
    }

}

void Server::AddFile(int c_sock, char *ip, Message *m) const {
    std::ofstream f;
    std::string ipp(ip);
    std::string dir = m_dir_folder;
    int actual_total = m->size * 1060;
    int total = m->size - 1;
    std::string name_of_file;
    std::string nameFile(m->body);
    std::stringstream fs(nameFile);
    std::vector<std::string> two;
    std::string temp;
    while (std::getline(fs, temp, ' ')) {
        two.emplace_back(temp);
    }
    name_of_file = two.at(1);
    int filesize = std::stoi(two.at(0));
    f.open(dir + name_of_file, std::ios::app);
    // Could not create the file
    if (!f.good()) {
        std::string event = "[ERROR] Adding file for client. Could not create file [MESSAGE] ERR_FILE [IP] ";
        event += ipp + " [TIME] ";
        auto now_time = time(nullptr);
        auto now = ctime(&now_time);
        event += now;
        event += '\n';
        m_log->Log(event);
        Message mess_out{};
        mess_out.message = ERR_FILE;
        std::string err = "Could not create file";
        bzero(mess_out.body, sizeof(mess_out.body));
        strcpy(mess_out.body, err.c_str());
        mess_out.size = err.length();
        if (send(c_sock, &mess_out, sizeof(mess_out), 0) <= 0) {
            event = "[ERROR] Adding file error message. Sending to client from server [MESSAGE] ERR_FILE [IP] ";
            event += ipp + " [TIME] ";
            now_time = time(nullptr);
            now = ctime(&now_time);
            event += now;
            event += '\n';
            m_log->Log(event);
            return;
        }
        event = "[SUCCESS] sending error to client from server [MESSAGE] ERR_FILE [IP] ";
        event += ipp + " [TIME] ";
        auto send_time = time(nullptr);
        now = ctime(&send_time);
        auto diff = difftime(send_time, now_time);
        auto time_taken = std::to_string(diff);
        event += now;
        event += " [SEND TIME] " + time_taken;
        m_log->Log(event);
        return;
    }
    while (total > 0) {
        char buffer[1060];
        bzero(buffer, sizeof(buffer));
        if (read(c_sock, buffer, sizeof(buffer)) < 0) {
            std::string event = "[ERROR] reading from client after accepting connection [TIME] ";
            auto now_time = time(nullptr);
            auto now = ctime(&now_time);
            event += now;
            event += '\n';
            m_log->Log(event);
            return;
        }
        total -= 1;
        auto *msg = reinterpret_cast<Message *>(buffer);
        f << msg->body;
        f.flush();
    }
    Message mess_out{};
    mess_out.message = FINISH;
    std::string out = "Successfully added file: " + name_of_file + "to server directory";
    std::strcpy(mess_out.body, out.c_str());
    if (send(c_sock, &mess_out, sizeof(mess_out), 0) <= 0) {
        std::string event = "[ERROR] Added file, however could not send back to client error message. Sending to client from server [MESSAGE] ERR_FILE [IP] ";
        event += ipp + " [TIME] ";
        auto now_time = time(nullptr);
        auto now = ctime(&now_time);
        event += now;
        event += '\n';
        m_log->Log(event);
        return;
    }
    std::string event = "[SUCCESS] Added file to server [FILE] ";
    event += name_of_file + " |from  client from server [MESSAGE] ADD_FILE [IP] ";
    event += ipp + " [BYTES_RECEIVED] " + std::to_string(actual_total) + " [TIME] ";
    auto now_time = time(nullptr);
    auto now = ctime(&now_time);
    event += now;
    m_log->Log(event);
    f.close();
}

void Server::DeleteFile(int c_sock, char *ip, Message *msg) const{
    std::string file(msg->body);
    file = file.substr(0, msg->size);
    std::string event;
    std::string ipp(ip);
    auto now_time = time(nullptr);
    if (std::remove(file.c_str()) != 0) {
        event = "[ERROR] Deleting file for client. Could not delete file [MESSAGE] ERR_FILE [IP] ";
        event += ipp + " [TIME] ";
        now_time = time(nullptr);
        auto now = ctime(&now_time);
        event += now;
        m_log->Log(event);
        Message mess_out{};
        mess_out.message = ERR_FILE;
        std::string err = "Could not delete file";
        bzero(mess_out.body, sizeof(mess_out.body));
        strcpy(mess_out.body, err.c_str());
        mess_out.size = err.length();
        if (send(c_sock, &mess_out, sizeof(mess_out), 0) <= 0) {
            event = "[ERROR] Deleting file error message. Sending to client from server [MESSAGE] ERR_FILE [IP] ";
            event += ipp + " [TIME] ";
            now_time = time(nullptr);
            now = ctime(&now_time);
            event += now;
            m_log->Log(event);
            return;
        }
        event = "[SUCCESS] sending delete error to client from server [MESSAGE] ERR_FILE [IP] ";
        event += ipp + " [TIME] ";
        auto send_time = time(nullptr);
        now = ctime(&send_time);
        auto diff = difftime(send_time, now_time);
        auto time_taken = std::to_string(diff);
        event += now;
        event += " [SEND TIME] " + time_taken;
        m_log->Log(event);
        return;
    }
    event = "[SUCCESS] sending to client from server [MESSAGE] ERR_FILE [IP] ";
    event += ipp + " [TIME] ";
    auto send_time = time(nullptr);
    auto now = ctime(&send_time);
    auto diff = difftime(send_time, now_time);
    auto time_taken = std::to_string(diff);
    event += now;
    event += " [SEND TIME] " + time_taken;
    m_log->Log(event);
}

void Server::SendError(int c_sock, char *ip) const {
    std::string ipp(ip);
    std::string event = "[ERROR] sending to client from server handling [MESSAGE] UNKNOWN [IP] ";
    event += ipp + " [TIME] ";
    auto now_time = time(nullptr);
    auto now = ctime(&now_time);
    event += now;
    m_log->Log(event);
    Message mess_out{};
    mess_out.message = ERR_FILE;
    std::string err = "Unknown message type does not exist";
    bzero(mess_out.body, sizeof(mess_out.body));
    strcpy(mess_out.body, err.c_str());
    mess_out.size = err.length();
    if (send(c_sock, &mess_out, sizeof(mess_out), 0) <= 0) {
        event = "[ERROR] Unknown message. Sending to client from server [MESSAGE] ERR_FILE [IP] ";
        event += ipp + " [TIME] ";
        now_time = time(nullptr);
        now = ctime(&now_time);
        event += now;
        m_log->Log(event);
        return;
    }
    event = "[SUCCESS] sending to client from server [MESSAGE] ERR_FILE [IP] ";
    event += ipp + " [TIME] ";
    auto send_time = time(nullptr);
    now = ctime(&send_time);
    auto diff = difftime(send_time, now_time);
    auto time_taken = std::to_string(diff);
    event += now;
    event += " [SEND TIME] " + time_taken;
    m_log->Log(event);
}

void Server::SendFile(int c_sock, char *ip, Message *m) const {
    std::string event;
    time_t now_time;
    char *now;
    std::string temp1(m->body);
    std::string file = temp1.substr(0, m->size);
    std::string ipp(ip);
    std::string full = m_dir_folder + file;
    std::ifstream f(full);
    Message mess_out{};
    if (!f.good()) {
        mess_out.message = ERR_FILE;
        std::string err = "File: " + file + " does not exist";
        bzero(mess_out.body, sizeof(mess_out.body));
        strcpy(mess_out.body, err.c_str());
        mess_out.size = err.length();
        event = "[ERROR] File not found from client request [MESSAGE] SINGLE_DL [IP] ";
        event += ipp;
        event += " [TIME] ";
        now_time = time(nullptr);
        now = ctime(&now_time);
        event += now;
        m_log->Log(event);
        if (send(c_sock, &mess_out, sizeof(mess_out), 0) <= 0) {
            event = "[ERROR] sending to client from server handling [MESSAGE] SINGLE_DL [IP] ";
            event += ipp + " [TIME] ";
            now_time = time(nullptr);
            now = ctime(&now_time);
            event += now;
            m_log->Log(event);
            return;
        }
        event = "[ERROR] SENT ERROR to client from server [MESSAGE] ERROR [IP] ";
        event += ipp + " [TIME] ";
        auto send_time = time(nullptr);
        now = ctime(&send_time);
        auto diff = difftime(send_time, now_time);
        auto time_taken = std::to_string(diff);
        event += now;
        event += " [SEND TIME] " + time_taken;
        m_log->Log(event);
        return;
    }
    std::streampos filesize = 0;
    f.seekg(0, std::ios::end);
    filesize = f.tellg() - filesize;
    f.clear();
    f.seekg(0);
    char buffer[1024];
    if (filesize < 1024) {
        char buff[(int) filesize];
        bzero(buff, sizeof(buff));
        mess_out.message = SINGLE_DL;
        mess_out.size = 1;
        f.read(buff, filesize);
        //m_log->Log(buffer);
        std::strcpy(mess_out.body, buff);
        event = "[SENDING] sending to client from server [MESSAGE] SINGLE_DL [IP] ";
        event += ipp + " [TIME] ";
        now_time = time(nullptr);
        now = ctime(&now_time);
        event += now;
        m_log->Log(event);
        if (send(c_sock, &mess_out, sizeof(mess_out), 0) <= 0) {
            event = "[ERROR] sending to client from server handling [MESSAGE] SINGLE_DL [IP] ";
            event += ipp + " [TIME] ";
            now_time = time(nullptr);
            now = ctime(&now_time);
            event += now;
            m_log->Log(event);
            return;
        }
        event = "[SUCCESS] sending to client from server [MESSAGE] SINGLE_DL [IP] ";
        event += ipp + " [TIME] ";
        auto send_time = time(nullptr);
        now = ctime(&send_time);
        auto diff = difftime(send_time, now_time);
        auto time_taken = std::to_string(diff);
        event += now;
        event += " [SEND TIME] " + time_taken;
        event += " [BYTE SENT] " + std::to_string(sizeof(mess_out));
        m_log->Log(event);
        return;
    }
    int loops = filesize / 1024;
    int lastChunk = filesize % 1024;
    Message mess[loops + lastChunk];
    for (int i = 0; i < loops; i++) {
        mess_out.message = SINGLE_DL;
        if(i == 0){mess_out.size = loops + lastChunk;}
        else{mess_out.size = 1024;}
        bzero(buffer, sizeof(buffer));
        f.read(buffer, sizeof(buffer));
        std::strcpy(mess_out.body, buffer);
        mess[i] = mess_out;
    }
    if (lastChunk > 0) {
        bzero(buffer, sizeof(buffer));
        f.read(buffer, lastChunk);
        mess_out.message = SINGLE_DL;
        mess_out.size = lastChunk;
        mess[loops] = mess_out;
    }
    if (send(c_sock,mess, sizeof(mess), 0) <= 0) {
        event = "[ERROR] sending to client from server handling [MESSAGE] SINGLE_DL [IP] ";
        event += ipp + " [TIME] ";
        now_time = time(nullptr);
        now = ctime(&now_time);
        event += now;
        m_log->Log(event);
        return;
    }
    event = "[SUCCESS] sending to client from server [MESSAGE] SINGLE_DL [IP] ";
    event += ipp + " [TIME] ";
    auto send_time = time(nullptr);
    now = ctime(&send_time);
    auto diff = difftime(send_time, now_time);
    auto time_taken = std::to_string(diff);
    event += now;
    event += " [SEND TIME] " + time_taken;
    event += " [BYTE SENT] " + std::to_string(sizeof(mess));
    m_log->Log(event);
}

void Server::SendList(int c_sock, char *ip) const {
    std::string event;
    time_t now_time;
    char *now;
    std::string ipp(ip);
    namespace fs = std::filesystem;
    std::string total_files;
    for (const auto &entry: fs::directory_iterator(this->m_dir_folder)) {
        const auto &file = entry.path();
        std::string f(file.c_str());
        total_files += f + " ";
    }
    // This can be an issue if the amount of files concatenated is greater than 1024 bytes
    // may need to check if it's greater to split the messages into multiples of 1024
    total_files += '\0';
    Message msg{};
    msg.message = MessageID::LIST_FILES;
    msg.size = total_files.length();
    bzero(msg.body, sizeof(msg.body));
    std::strcpy(msg.body, total_files.c_str());
    event = "[SENDING] sending to client from server [MESSAGE] LIST_FILES [IP] ";
    event += ipp + " [TIME] ";
    now_time = time(nullptr);
    now = ctime(&now_time);
    event += now;
    m_log->Log(event);
    if (send(c_sock, &msg, sizeof(msg), 0) <= 0) {
        event = "[ERROR] sending to client from server handling message to list all files [TIME] ";
        now_time = time(nullptr);
        now = ctime(&now_time);
        event += now;
        m_log->Log(event);
        return;
    }
    event = "[SUCCESS] sending to client from server [MESSAGE] LIST_FILES [IP] ";
    event += ipp + " [TIME] ";
    auto send_time = time(nullptr);
    now = ctime(&send_time);
    auto diff = difftime(send_time, now_time);
    auto time_taken = std::to_string(diff);
    event += now;
    event += " [SEND_TIME] " + time_taken;
    event += " [BYTES_SENT] " + std::to_string(msg.size);
    event += "\n";
    m_log->Log(event);
}



