
echo "Compiling index server"
g++ -std=c++17 index_server/main.cpp index_server/indexserver.cpp index_server/logger.cpp -o Evaluation2/clients8/indexserver

echo "Compiling peer 1"

g++ -std=c++17 main.cpp peer/peernode.cpp peer/logger.cpp nodeclient.cpp peer/nodeserver.cpp -o Evaluation2/clients8/c1

echo "Compiling peer 2"
g++ -std=c++17 main.cpp peer/peernode.cpp peer/logger.cpp nodeclient.cpp peer/nodeserver.cpp -o Evaluation2/clients8/c2

echo "Compiling peer 3"
g++ -std=c++17 main.cpp peer/peernode.cpp peer/logger.cpp nodeclient.cpp peer/nodeserver.cpp -o Evaluation2/clients8/c3

echo "Compiling peer 4"
g++ -std=c++17 main.cpp peer/peernode.cpp peer/logger.cpp nodeclient.cpp peer/nodeserver.cpp -o Evaluation2/clients8/c4

echo "Compiling peer 5"

g++ -std=c++17 main.cpp peer/peernode.cpp peer/logger.cpp nodeclient.cpp peer/nodeserver.cpp -o Evaluation2/clients8/c5

echo "Compiling peer 6"
g++ -std=c++17 main.cpp peer/peernode.cpp peer/logger.cpp nodeclient.cpp peer/nodeserver.cpp -o Evaluation2/clients8/c6

echo "Compiling peer 7"
g++ -std=c++17 main.cpp peer/peernode.cpp peer/logger.cpp nodeclient.cpp peer/nodeserver.cpp -o Evaluation2/clients8/c7

echo "Compiling peer 8"
g++ -std=c++17 main.cpp peer/peernode.cpp peer/logger.cpp nodeclient.cpp peer/nodeserver.cpp -o Evaluation2/clients8/c8


cd Evaluation2/clients8

./indexserver log/index_server_log.txt log/index_server.csv 127.0.0.1 60010 &
sleep 1

./c1 log/client1.txt log/client1.csv log/client1_s.txt log/client1_s.csv client1/ 127.0.0.1 60010 127.0.0.1 30104 1 &
./c2 log/client2.txt log/client2.csv log/client2_s.txt log/client2_s.csv client2/ 127.0.0.1 60010 127.0.0.1 30105 1 &
./c3 log/client3.txt log/client3.csv log/client3_s.txt log/client3_s.csv client3/ 127.0.0.1 60010 127.0.0.1 30106 1 &
./c4 log/client4.txt log/client4.csv log/client4_s.txt log/client4_s.csv client4/ 127.0.0.1 60010 127.0.0.1 30107 1 &
./c5 log/client5.txt log/client5.csv log/client5_s.txt log/client5_s.csv client5/ 127.0.0.1 60010 127.0.0.1 30108 1 &
./c6 log/client6.txt log/client6.csv log/client6_s.txt log/client6_s.csv client6/ 127.0.0.1 60010 127.0.0.1 30109 1 &
./c7 log/client7.txt log/client7.csv log/client7_s.txt log/client7_s.csv client7/ 127.0.0.1 60010 127.0.0.1 30110 1 &
./c8 log/client8.txt log/client8.csv log/client8_s.txt log/client8_s.csv client8/ 127.0.0.1 60010 127.0.0.1 30111 1 &
sleep 5

pkill indexserver
pkill c1
pkill c2
pkill c3
pkill c4
pkill c5
pkill c6
pkill c7
pkill c8
exit
