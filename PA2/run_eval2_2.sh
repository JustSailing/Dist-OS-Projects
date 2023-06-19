
echo "Compiling index server"
g++ -std=c++17 index_server/main.cpp index_server/indexserver.cpp index_server/logger.cpp -o Evaluation2/clients2/indexserver

echo "Compiling peer 1"

g++ -std=c++17 main.cpp peer/peernode.cpp peer/logger.cpp nodeclient.cpp peer/nodeserver.cpp -o Evaluation2/clients2/c1

echo "Compiling peer 2"
g++ -std=c++17 main.cpp peer/peernode.cpp peer/logger.cpp nodeclient.cpp peer/nodeserver.cpp -o Evaluation2/clients2/c2

cd Evaluation2/clients2
#             log file                  csv file             ip         port
./indexserver log/index_server_log.txt log/index_server.csv 127.0.0.1 53500 &
sleep 1
#      log file         csv file     peer server log file   peer server csv file  directory   server ip   port  peer ip   peer port
./c1 log/client1.txt log/client1.csv log/client1_s.txt      log/client1_s.csv     client1/   127.0.0.1    53500 127.0.0.1 30092 1 &
./c2 log/client2.txt log/client2.csv log/client2_s.txt      log/client2_s.csv     client2/   127.0.0.1    53500 127.0.0.1 30093 1 &

sleep 5
pkill indexserver
pkill c1
pkill c2



exit
