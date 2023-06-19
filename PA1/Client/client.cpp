//
// Created by william on 2/22/23.
//

#include "client.h"
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
#include <mutex>
#include <atomic>

std::mutex task;
std::atomic<int> num_of_threads = 10;
extern Message messages[5];
std::vector<std::string> allfiles;
client::client(const char *client_folder, const char *log_file, const char* server_ip, const char* server_port) {
    this->m_log = new Logger(log_file);
    this->m_dir_folder = client_folder;
    this->m_sever_ip = server_ip;
    std::string s(server_port);

    this->m_server_port = std::stoi(s);
    std::string event;
    time_t now_time;
    char *now;

    event = "[Success] Creation of client socket [TIME] ";
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

void client::Start() {
    this->m_running = true;
    std::string event;
    time_t now_time;
    char *now;
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(this->m_sever_ip);
    server_addr.sin_port = htons(this->m_server_port);
    while (m_running) {
        while (arr < 5) {
            num_of_threads -= 1;
            std::thread t([this, server_addr] { ForThreading(server_addr); });
            t.join();
        }
    }
}

void client::ForThreading(sockaddr_in server_addr) {
    std::string event;
    time_t now_time;
    char *now;
    int c_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (c_sock < 0) {
        event = "[ERROR] Creation of client socket failed [TIME] ";
        now_time = time(nullptr);
        now = ctime(&now_time);
        event += now;
        event += '\n';
        m_log->Log(event);
        num_of_threads += 1;
        return;
        //m_log->LogClose();
        //exit(2);
    }

    if (connect(c_sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        event = "[ERROR] Connecting to server socket with [IP] 127.0.0.1 and [PORT] 55000 [TIME] ";
        now_time = time(nullptr);
        now = ctime(&now_time);
        event += now;
        event += '\n';
        m_log->Log(event);
        num_of_threads += 1;
        return;
    }
    char *ip = inet_ntoa(server_addr.sin_addr);
    event = "[SUCCESS] Connected to server from client ";
    std::ostringstream os;
    os << event << "[SOCKET]" << c_sock << "with [IP] " << ip << " [TIME] ";
    event = os.str();
    now_time = time(nullptr);
    now = ctime(&now_time);
    event += now;
    event += '\n';
    m_log->Log(event);
    task.lock();
    if(arr > 4) {
        this->m_running = false;
        return;
    }
    task.unlock();
    std::thread tid([this, ip, c_sock] { Send(c_sock, ip); });
    tid.join();
    num_of_threads += 1;
}

void client::Send(int c_sock, char *ip)  {
    std::string ipp(ip);
    Message mess{};
    task.lock();
    if(arr > 4) {
        task.unlock();
        this->m_running = false;
        return;
    }
    Message* msg = new Message;
    msg->size = messages[arr].size;
    msg->message = messages[arr].message;
    std::strcpy(msg->body, messages[arr].body);
    task.unlock();
    arr++;
    switch (msg->message) {
        case LIST_FILES:
            SendList(c_sock, ip);
            //Receive(c_sock, ip);
            break;
        case SINGLE_DL:
            SendFile(c_sock, ip, msg);
            break;
        case ADD_FILE:
            // Possible that the file is greater than 1024 bytes
            AddFile(c_sock, ip, msg);
            break;
        case DELETE_FILE:
            DeleteFile(c_sock, ip, msg);
            break;
        default:
            //SendError(c_sock, ip);
            //Receive(c_sock, ip);
            break;
    }
    if(arr > 4){this->m_running = false; return;}
}

void client::AddFile(int c_sock, char *ip, Message *m) const {
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
        std::string err = "File: " + file + " does not exist";
        event = "[ERROR] File not found. Not sent to server [MESSAGE] ADD_FILE [IP] ";
        event += ipp;
        event += " [TIME] ";
        now_time = time(nullptr);
        now = ctime(&now_time);
        event += now;
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
        mess_out.message = ADD_FILE;
        mess_out.size = 1;
        f.read(buff, filesize);
        //m_log->Log(buffer);
        std::strcpy(mess_out.body, buff);
        event = "[SENDING] sending from client to server [MESSAGE] ADD_FILE [IP] ";
        event += ipp + " [TIME] ";
        now_time = time(nullptr);
        now = ctime(&now_time);
        event += now;
        m_log->Log(event);
        if (send(c_sock, &mess_out, sizeof(mess_out), 0) <= 0) {
            event = "[ERROR] sending to from client to server handling [MESSAGE] ADD_FILE [IP] ";
            event += ipp + " [TIME] ";
            now_time = time(nullptr);
            now = ctime(&now_time);
            event += now;
            m_log->Log(event);
            return;
        }
        event = "[SUCCESS] sending from client to server [MESSAGE] ADD_FILE [IP] ";
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
        mess_out.message = ADD_FILE;
        if (i == 0) { mess_out.size = loops + lastChunk; }
        else { mess_out.size = 1024; }
        bzero(buffer, sizeof(buffer));
        f.read(buffer, sizeof(buffer));
        std::strcpy(mess_out.body, buffer);
        mess[i] = mess_out;
    }
    if (lastChunk > 0) {
        bzero(buffer, sizeof(buffer));
        f.read(buffer, lastChunk);
        mess_out.message = ADD_FILE;
        mess_out.size = lastChunk;
        mess[loops] = mess_out;
    }
    if (send(c_sock, mess, sizeof(mess), 0) <= 0) {
        event = "[ERROR] sending to server from client handling [MESSAGE] ADD_FILE [IP] ";
        event += ipp + " [TIME] ";
        now_time = time(nullptr);
        now = ctime(&now_time);
        event += now;
        m_log->Log(event);
        return;
    }
    event = "[SUCCESS] sending from client to server [MESSAGE] ADD_FILE [IP] ";
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

void client::DeleteFile(int c_sock, char *ip, Message *msg) const {
    std::string ipp(ip);
    std::string event;
    if (send(c_sock, msg, sizeof(Message), 0) <= 0) {
        event = "[ERROR] Delete file message not sent to server [MESSAGE] DELETE_FILE [IP] ";
        event += ipp + " [TIME] ";
        auto now_time = time(nullptr);
        auto now = ctime(&now_time);
        event += now;
        m_log->Log(event);
        return;
    }
    char buffer[sizeof(Message)];
    bzero(buffer, sizeof(buffer));
    if (read(c_sock, buffer, sizeof(buffer)) < 0) {
        event = "[ERROR] reading from server after connection [TIME] ";
        auto now_time = time(nullptr);
        auto now = ctime(&now_time);
        event += now;
        event += '\n';
        m_log->Log(event);
        return;
    }
    auto *temp = reinterpret_cast<Message *>(buffer);
    if (temp->message == FINISH) {
        event = "[SUCCESS] Deleting file on server from client [TIME] ";
        auto now_time = time(nullptr);
        auto now = ctime(&now_time);
        event += now;
        event += '\n';
        m_log->Log(event);
        return;
    } else {
        std::string err(temp->body);
        event = "[ERROR] ERROR message from server [INFO] " + err + " [MESSAGE] ERR_FILE [IP] ";
        event += ipp + " [TIME] ";
        auto now_time = time(nullptr);
        auto now = ctime(&now_time);
        event += now;
        m_log->Log(event);
        return;
    }

}

void client::SendFile(int c_sock, char *ip, Message *m) const {
    std::string event;
    time_t now_time;
    char *now;
    now_time = time(nullptr);
    now = ctime(&now_time);
    event += now;
    Message mess_out = *m;
    if(m->message != SINGLE_DL) return;
    if (send(c_sock, &mess_out, sizeof(mess_out), 0) <= 0) {
        event = "[ERROR] sending from client to server handling message to list all files [TIME] ";
        now_time = time(nullptr);
        now = ctime(&now_time);
        event += now;
        m_log->Log(event);
        return;
    }
    event = "[SUCCESS] receiving from server [MESSAGE] SINGLE_DL [TIME] ";
    auto send_time = time(nullptr);
    now = ctime(&send_time);
    auto diff = difftime(send_time, now_time);
    auto time_taken = std::to_string(diff);
    event += now;
    event += " [SEND_TIME] " + time_taken;
    event += " [BYTES_SENT] " + std::to_string(sizeof(Message));
    event += "\n";
    m_log->Log(event);
    char buffer[sizeof(Message)];
    bzero(buffer, sizeof(buffer));
    if (read(c_sock, buffer, sizeof(buffer)) < 0) {
        event = "[ERROR] reading server response after accepting connection [TIME] ";
        now_time = time(nullptr);
        now = ctime(&now_time);
        event += now;
        event += '\n';
        m_log->Log(event);
        return;
    }
    auto *msg = reinterpret_cast<Message *>(buffer);
//    if (!((msg->message) == SINGLE_DL)) {
//        event = "[ERROR] Downloading file from Server [MESSAGE] SINGLE_DL [TIME] ";
//        now_time = time(nullptr);
//        now = ctime(&now_time);
//        event += now;
//        event += '\n';
//        m_log->Log(event);
//        return;
//    }
    std::ofstream f;
    std::string ipp(ip);
    std::string dir = m_dir_folder;
    int actual_total = msg->size * 1060;
    int total = msg->size - 1;
    std::string name_of_file(m->body);

    f.open(dir + name_of_file, std::ios::app);
    // Could not create the file
    if (!f.good()) {
        event = "[ERROR] Downloading file at client. Could not create file [MESSAGE] ERR_FILE [IP] ";
        event += ipp + " [TIME] ";
        now_time = time(nullptr);
        now = ctime(&now_time);
        event += now;
        event += '\n';
        m_log->Log(event);
        return;
    }
    f << msg->body;

    while (total > 0) {
        bzero(buffer, sizeof(buffer));
        if (read(c_sock, buffer, sizeof(buffer)) < 0) {
            event = "[ERROR] reading at client after accepting connection [TIME] ";
            now_time = time(nullptr);
            now = ctime(&now_time);
            event += now;
            event += '\n';
            m_log->Log(event);
            return;
        }
        total -= 1;
        msg = reinterpret_cast<Message *>(buffer);
        f << msg->body;
        f.flush();
    }
    event = "[SUCCESS] Downloaded file from server [FILE] ";
    event += name_of_file + " | to  client from server [MESSAGE] ADD_FILE [IP] ";
    event += ipp + " [BYTES_RECEIVED] " + std::to_string(actual_total) + " [TIME] ";
    now_time = time(nullptr);
    now = ctime(&now_time);
    event += now;
    m_log->Log(event);
    f.close();
}

void client::SendList(int c_sock, char *ip) const {
    std::string event;
    time_t now_time;
    char *now;
    std::string ipp(ip);

    // This can be an issue if the amount of files concatenated is greater than 1024 bytes
    // may need to check if it's greater to split the messages into multiples of 1024
    Message msg{};
    msg.message = MessageID::LIST_FILES;
    msg.size = 1;
    bzero(msg.body, sizeof(msg.body));
    event = "[SENDING] sending from client to server [MESSAGE] LIST_FILES [IP] ";
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
    char buffer[sizeof(Message)];
    bzero(buffer, sizeof(buffer));
    if (read(c_sock, buffer, sizeof(buffer)) < 0) {
        event = "[ERROR] reading server response after accepting connection [TIME] ";
        now_time = time(nullptr);
        now = ctime(&now_time);
        event += now;
        event += '\n';
        m_log->Log(event);
        return;
    }
    auto mss_in = reinterpret_cast<Message *>(buffer);
    std::string s(mss_in->body);
    std::stringstream ss(s);
    std::string temp;
    std::vector<std::string> files;
    while(std::getline(ss, temp, ' ')){
        files.push_back(temp);
    }
    allfiles.clear();
    for(auto a: files){
        allfiles.push_back(a);
    }

    event = "[SUCCESS] sending from client to server [MESSAGE] LIST_FILES [TIME] ";
    auto send_time = time(nullptr);
    now = ctime(&send_time);
    auto diff = difftime(send_time, now_time);
    auto time_taken = std::to_string(diff);
    event += now;
    event += " Files received: ";
    for(auto a : allfiles){
        event += a;
    }
    event += " ";
    event += " [SEND_TIME] " + time_taken;
    event += " [BYTES_Recieved] " + std::to_string(sizeof(Message));
    event += "\n";
    m_log->Log(event);

}