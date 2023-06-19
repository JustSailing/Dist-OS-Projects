#include <iostream>
#include "indexserver.h"


int main(int argc, char** argv)
{
    // if (argc != 5)
    // {
    //     std::cout << "Not enough arguments\n";
    //     std::cout << "[program] [path_to_log_file] [path_to_csv_file] [IP address] [Port Number]\n";
    //     exit(2);
    // }
    const char* log_file = argv[1];
    const char* csv_file = argv[2];
    const char* ip       = argv[3];
    const char* port     = argv[4];
    int index = 5;
    std::vector<SuperNode> super;
    for(auto i = 0; i < argc - index; i++)
    {
        std::cout << "super peer connected: " << i << argv[index] << " " << argv[index + 1] << std::endl;
        SuperNode sup = {argv[index], argv[index + 1]};
        super.emplace_back(sup);
        index += 2;
    }
    std::unique_ptr<IndexServer> server = std::make_unique<IndexServer>(log_file, csv_file, ip, port, super);
    server->Start();
    return 1;
}