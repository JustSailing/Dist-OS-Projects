#include <iostream>
#include <dirent.h>
#include <cstring>
#include <sys/stat.h>
#include <queue>
#include <utility>

#include "../../peernode/peernode.h"

//client2 to requests
std::pair<u_int16_t, std::string> m1_1(QUERY, "1_1.txt");
std::pair<u_int16_t, std::string> m1_2(QUERY, "1_2.txt");
std::pair<u_int16_t, std::string> m1_3(QUERY, "1_3.txt");
std::pair<u_int16_t, std::string> m1_4(QUERY, "1_4.txt");
//std::pair<u_int16_t, std::string> m2_4(UNREGISTER, "");

//client 1 requests
std::pair<u_int16_t, std::string> m1_5(QUERY, "1_5.txt");
std::pair<u_int16_t, std::string> m1_6(QUERY, "1_6.txt");
std::pair<u_int16_t, std::string> m1_7(QUERY, "1_7.txt");
//std::pair<u_int16_t, std::string> m1_4(UNREGISTER, "");

//client 3 requests
std::pair<u_int16_t, std::string> m1_8(QUERY, "1_8.txt");
std::pair<u_int16_t, std::string> m1_9(QUERY, "1_9.txt");
std::pair<u_int16_t, std::string> m1_10(QUERY, "1_10.txt");


std::pair<u_int16_t ,std::string> q[10];
int q_size = 0;
int q_index = 0;
extern std::vector<std::pair<std::string, std::string>> all_files;
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
            all_files.push_back(std::make_pair(diread->d_name, std::to_string(filestat.st_size)));
        }
        closedir(dir_ptr);
    } else {
        exit(2);
    }

    // for(auto file : all_files)
    // {
    //     std::cout << file << '\n';
    // }
    switch(std::stoi(argv[10]))
    {
        case 1:
        {
            q[0] = m1_1;
            q[1] = m1_2;
            q[2] = m1_3;
            q[3] = m1_4;
            q[4] = m1_5;
            q[5] = m1_6;
            q[6] = m1_7;
            q[7] = m1_8;
            q[8] = m1_9;
            q[9] = m1_10;
            q_size = 9;
        }break;
        case 2:
        {
            //q[0] = m2_1;
            //q[1] = m2_2;
            //q[1] = m2_3;
            //q[3] = m2_4;
            q_size = 0;
            q_index = 1;
        }break;
               
    }
    std::unique_ptr<PeerNode> server = std::make_unique<PeerNode>(client_log_file, client_csv_file,
                                            server_log_file, server_csv_file, directory, ip_addr_indx_server, 
                                            port_number_indx_server, ip_addr_peer, port_number_server);
    server->Start();
    return 1;
}
