
echo "Compiling index server"
g++ -std=c++17 index_server/main.cpp index_server/indexserver.cpp index_server/logger.cpp -o Evaluation2/clients16/indexserver

echo "Compiling peer 1"

g++ -std=c++17 main.cpp peer/peernode.cpp peer/logger.cpp nodeclient.cpp peer/nodeserver.cpp -o Evaluation2/clients16/c1

echo "Compiling peer 2"
g++ -std=c++17 main.cpp peer/peernode.cpp peer/logger.cpp nodeclient.cpp peer/nodeserver.cpp -o Evaluation2/clients16/c2

echo "Compiling peer 3"
g++ -std=c++17 main.cpp peer/peernode.cpp peer/logger.cpp nodeclient.cpp peer/nodeserver.cpp -o Evaluation2/clients16/c3

echo "Compiling peer 4"
g++ -std=c++17 main.cpp peer/peernode.cpp peer/logger.cpp nodeclient.cpp peer/nodeserver.cpp -o Evaluation2/clients16/c4

echo "Compiling peer 5"

g++ -std=c++17 main.cpp peer/peernode.cpp peer/logger.cpp nodeclient.cpp peer/nodeserver.cpp -o Evaluation2/clients16/c5

echo "Compiling peer 6"
g++ -std=c++17 main.cpp peer/peernode.cpp peer/logger.cpp nodeclient.cpp peer/nodeserver.cpp -o Evaluation2/clients16/c6

echo "Compiling peer 7"
g++ -std=c++17 main.cpp peer/peernode.cpp peer/logger.cpp nodeclient.cpp peer/nodeserver.cpp -o Evaluation2/clients16/c7

echo "Compiling peer 8"
g++ -std=c++17 main.cpp peer/peernode.cpp peer/logger.cpp nodeclient.cpp peer/nodeserver.cpp -o Evaluation2/clients16/c8

echo "Compiling peer 9"

g++ -std=c++17 main.cpp peer/peernode.cpp peer/logger.cpp nodeclient.cpp peer/nodeserver.cpp -o Evaluation2/clients16/c9

echo "Compiling peer 10"
g++ -std=c++17 main.cpp peer/peernode.cpp peer/logger.cpp nodeclient.cpp peer/nodeserver.cpp -o Evaluation2/clients16/c10

echo "Compiling peer 11"
g++ -std=c++17 main.cpp peer/peernode.cpp peer/logger.cpp nodeclient.cpp peer/nodeserver.cpp -o Evaluation2/clients16/c11

echo "Compiling peer 12"
g++ -std=c++17 main.cpp peer/peernode.cpp peer/logger.cpp nodeclient.cpp peer/nodeserver.cpp -o Evaluation2/clients16/c12

echo "Compiling peer 13"

g++ -std=c++17 main.cpp peer/peernode.cpp peer/logger.cpp nodeclient.cpp peer/nodeserver.cpp -o Evaluation2/clients16/c13

echo "Compiling peer 14"
g++ -std=c++17 main.cpp peer/peernode.cpp peer/logger.cpp nodeclient.cpp peer/nodeserver.cpp -o Evaluation2/clients16/c14

echo "Compiling peer 15"
g++ -std=c++17 main.cpp peer/peernode.cpp peer/logger.cpp nodeclient.cpp peer/nodeserver.cpp -o Evaluation2/clients16/c15

echo "Compiling peer 16"
g++ -std=c++17 main.cpp peer/peernode.cpp peer/logger.cpp nodeclient.cpp peer/nodeserver.cpp -o Evaluation2/clients16/c16


cd Evaluation2/clients16

./indexserver log/index_server_log.txt log/index_server.csv 127.0.0.1 53520 &
sleep 2

./c1 log/client1.txt log/client1.csv log/client1_s.txt log/client1_s.csv client1/ 127.0.0.1 53520 127.0.0.1 30104 1 &
./c2 log/client2.txt log/client2.csv log/client2_s.txt log/client2_s.csv client2/ 127.0.0.1 53520 127.0.0.1 30105 1 &
./c3 log/client3.txt log/client3.csv log/client3_s.txt log/client3_s.csv client3/ 127.0.0.1 53520 127.0.0.1 30106 1 &
./c4 log/client4.txt log/client4.csv log/client4_s.txt log/client4_s.csv client4/ 127.0.0.1 53520 127.0.0.1 30107 1 &
./c5 log/client5.txt log/client5.csv log/client5_s.txt log/client5_s.csv client5/ 127.0.0.1 53520 127.0.0.1 30108 1 &
./c6 log/client6.txt log/client6.csv log/client6_s.txt log/client6_s.csv client6/ 127.0.0.1 53520 127.0.0.1 30109 1 &
./c7 log/client7.txt log/client7.csv log/client7_s.txt log/client7_s.csv client7/ 127.0.0.1 53520 127.0.0.1 30110 1 &
./c8 log/client8.txt log/client8.csv log/client8_s.txt log/client8_s.csv client8/ 127.0.0.1 53520 127.0.0.1 30111 1 &
./c9 log/client9.txt log/client9.csv log/client9_s.txt log/client9_s.csv client9/ 127.0.0.1 53520 127.0.0.1 30112 1 &
./c10 log/client10.txt log/client10.csv log/client10_s.txt log/client10_s.csv client10/ 127.0.0.1 53520 127.0.0.1 30113 1 &
./c11 log/client11.txt log/client11.csv log/client11_s.txt log/client11_s.csv client11/ 127.0.0.1 53520 127.0.0.1 30114 1 &
./c12 log/client12.txt log/client12.csv log/client12_s.txt log/client12_s.csv client12/ 127.0.0.1 53520 127.0.0.1 30115 1 &
./c13 log/client13.txt log/client13.csv log/client13_s.txt log/client13_s.csv client13/ 127.0.0.1 53520 127.0.0.1 30116 1 &
./c14 log/client14.txt log/client14.csv log/client14_s.txt log/client14_s.csv client14/ 127.0.0.1 53520 127.0.0.1 30117 1 &
./c15 log/client15.txt log/client15.csv log/client15_s.txt log/client15_s.csv client15/ 127.0.0.1 53520 127.0.0.1 30118 1 &
./c16 log/client16.txt log/client16.csv log/client16_s.txt log/client16_s.csv client16/ 127.0.0.1 53520 127.0.0.1 30119 1 &
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
pkill c9
pkill c10
pkill c11
pkill c12
pkill c13
pkill c14
pkill c15
pkill c16
exit
