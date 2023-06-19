

#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H
#include "message.h"
#include <thread>
#include "logger.h"
#include <openssl/sha.h>
#include <netinet/in.h>

class Server
{
public:
    // initialize server socket, logger, and binding of server socket to a port
    // connections and accepts should happen in Start function
    // Not sure if I should put the connection part in the Server constructor, but it
    // shouldn't make a difference
    Server(const char *server_folder, const char *log, const char *ip, const char* port);
    void Start();
    //bool Stop();
    //bool Send(MessageID id);
    ~Server()
    {
        free(m_log);
    }

private:
    // boolean to keep server running
    bool m_running = false;
    // server socket to connect to
    int m_server_fd;
    // server port to connect
    u_int16_t m_server_port;
    // the server ip address
    const char* m_server_ip;
    // logger to log action/events
    Logger *m_log;
    // info on server to pass around in functions
    sockaddr_in m_addr{};
    // checksum to check the integrity of the message
    SHA256_CTX m_context{};
    // directory holding server files
    const char* m_dir_folder;
    // sends the list of files in server directory to client
    void SendList(int c_sock, char* ip) const;
    // sends the requested files from server to client
    void SendFile(int c_sock, char* ip, Message* m) const;
    // sends errors to client if they appear from the beginning (example: unrecognized message)
    void SendError(int c_sock, char* ip) const ;
    // Mainly used as a switch to send to different functions depending on the message
    void Receive(int c_sock, char* ip) const;
    // Happens before receive. Gathers information on the client ip, port number
    void InitReceive(int c_sock) const;
    // Handles when a user wants to add a file (from the client's directory) to the server
    // Downloads file from client
    void AddFile(int c_sock, char* ip, Message* m) const;
    // Deletes file that client wants deleted
    void DeleteFile(int c_sock, char* ip, Message* m) const;
    // Threading (there was to many blocks happening and the program would hang).
    void NeededForThreading();
};
#endif //CS550_03_HADDAD_WILLIAM_PA1_SERVER_H
