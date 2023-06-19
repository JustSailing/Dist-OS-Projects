#include <iostream>
#include <dirent.h>
#include <cstring>
#include <sys/stat.h>
#include <queue>
#include <array>

#include "peer/peernode.h"



//client 3 requests

std::pair<u_int16_t, std::string> m3_3(QUERY, "128.txt");
//std::pair<u_int16_t, std::string> m3_4(UNREGISTER, "");

std::array<std::pair<u_int16_t ,std::string>, 20> q;
int q_size = 0;
extern std::vector<std::string> all_files;
int main(int argc, char** argv)
{

    if (argc != 11)
    {
        std::cout << "Not enough arguments: 11 arguments -> program + 10 args\n";
        std::cout << "[program] [client_peer_log_file] [client_peer_csv_file] [server_peer_log_file] [server_peer_csv_file]" <<
        " [peer_directory] [Index IP address] [Index Port Number] [Peer IP address]  [Peer server port] [client #]\n";
        exit(2);
    }
    const char* client_log_file          = argv[1];
    const char* client_csv_file          = argv[2];
    const char* server_log_file          = argv[3];
    const char* server_csv_file          = argv[4];
    const char* directory                = argv[5];
    const char* ip_addr_indx_server      = argv[6];
    const char* port_number_indx_server  = argv[7];
    const char* ip_addr_peer             = argv[8];
//    const char* port_number_client       = argv[9];
    const char* port_number_server       = argv[9];
    std::string path(directory);
    DIR *dir_ptr;
    struct dirent *diread;
    struct stat filestat{};
    std::string filepath;
    std::vector<std::string> filenames;
    if ((dir_ptr = opendir(path.c_str())) != nullptr) {
        while ((diread = readdir(dir_ptr)) != nullptr) {
            filepath = path + "/" + diread->d_name;
            stat( filepath.c_str(), &filestat);
            if (S_ISDIR( filestat.st_mode )) continue;
            all_files.push_back(diread->d_name);
        }
        closedir(dir_ptr);
    } else {
        exit(2);
    }

    for(auto i = 0; i < 20; i++)
    {
        q[i] = m3_3;
    }
    q_size = 19;

    PeerNode *server = new PeerNode(client_log_file, client_csv_file,
                                            server_log_file, server_csv_file, directory, ip_addr_indx_server, 
                                            port_number_indx_server, ip_addr_peer, port_number_server);
    server->Start();
    return 1;
}
