#include <sys/socket.h>
#include "message.h"
#include <cstring>
#include <string>
#include <iostream>
#include "client.h"
Message messages[5];
std::atomic<int> arr = 0;
int main(int argc, char** argv) {
    Message msg{};
    std::string te = "32k.txt";
    bzero(msg.body, sizeof(msg.body));
    msg.message = SINGLE_DL;
    msg.size = (u_int16_t)te.length();
    bzero(msg.body, sizeof(msg.body));
    std::strcpy(msg.body, te.c_str());
    messages[0] = msg;


    te = "32k.txt";
    bzero(msg.body, sizeof(msg.body));
    msg.message = SINGLE_DL;
    msg.size = (u_int16_t)te.length();
    bzero(msg.body, sizeof(msg.body));
    std::strcpy(msg.body, te.c_str());
    messages[1] = msg;


    te = "32k.txt";
    bzero(msg.body, sizeof(msg.body));
    msg.message = SINGLE_DL;
    msg.size = (u_int16_t)te.length();
    bzero(msg.body, sizeof(msg.body));
    std::strcpy(msg.body, te.c_str());
    messages[2] = msg;

    te = "32k.txt";
    bzero(msg.body, sizeof(msg.body));
    msg.message = SINGLE_DL;
    msg.size = (u_int16_t)te.length();
    bzero(msg.body, sizeof(msg.body));
    std::strcpy(msg.body, te.c_str());
    messages[3] = msg;

    te = "32k.txt";
    bzero(msg.body, sizeof(msg.body));
    msg.message = SINGLE_DL;
    msg.size = (u_int16_t)te.length();
    bzero(msg.body, sizeof(msg.body));
    std::strcpy(msg.body, te.c_str());
    messages[4] = msg;
    if(argc != 5) {
        std::cout << "Not Enough arguments\n";
        std::cout << "[program] [path_to_client_dir] [path_to_client_logfile/*.txt] [IP address of Server] [Server Port]\n";
        exit(-1);
    }


    client c = client(argv[1],argv[2], argv[3], argv[4]);
    c.Start();

    return 0;
}
