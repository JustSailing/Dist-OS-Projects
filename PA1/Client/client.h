//
// Created by william on 2/22/23.
//

#ifndef CLIENT_CLIENT_H
#define CLIENT_CLIENT_H
#include "message.h"
#include <thread>
#include "logger.h"
#include <openssl/sha.h>
#include <netinet/in.h>
#include <vector>

//
extern Message messages[];
extern std::atomic<int> arr;

class client {
public:
    // Initializes the client class
    client(const char *client_folder, const char *log_file, const char *sever_ip, const char *server_port);
    void Start();
    //bool Stop();
    ~client()
    {
        free(m_log);
    }
private:
    // Used to keep client alive
    bool m_running = false;
    // ip address of server to connect
    const char* m_sever_ip;
    // port number of the server to connect
    u_int16_t m_server_port;
    // pointer to logger to do logging
    Logger *m_log;
    // checksum to check the integrity of message
    SHA256_CTX m_context{};
    // client directory to hold files downloaded from server
    const char* m_dir_folder;
    // get the list of files on the server
    void SendList(int c_sock, char* ip) const;
    // get a file from the server
    void SendFile(int c_sock, char* ip, Message* m) const;
    // used as a switch statement to go to respective function depending on message
    void Send(int c_sock, char* ip);
    // sends a message to server that you want to add a file and send it
    void AddFile(int c_sock, char* ip, Message* m) const;
    // sends a delete request to the server for a file
    void DeleteFile(int c_sock, char* ip, Message* m) const;
    // help with multithreading
    void ForThreading(sockaddr_in sockaddrIn);

};


#endif //CLIENT_CLIENT_H
