#include <iostream>
#include "server.h"


int main(int argc, char** argv)
{
    if(argc != 5){
        std::cout << "Not enough arguments\n";
        std::cout << "[program] [path_to_server_dir] [path_to_server_logfile/*.txt] [IP address of Server] [Server Port]\n";
    }
    const char *server_folder = argv[1];
    const char *log_file = argv[2];
    const char *ip = argv[3];
    const char *port= argv[4];
    Server* serv = new Server(server_folder, log_file, ip, port);
    serv->Start();
    return 0;
}