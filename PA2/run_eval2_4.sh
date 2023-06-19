
echo "Compiling index server"
g++ -std=c++17 index_server/main.cpp index_server/indexserver.cpp index_server/logger.cpp -o Evaluation2/clients4/indexserver

echo "Compiling peer 1"

g++ -std=c++17 main.cpp peer/peernode.cpp peer/logger.cpp nodeclient.cpp peer/nodeserver.cpp -o Evaluation2/clients4/c1

echo "Compiling peer 2"
g++ -std=c++17 main.cpp peer/peernode.cpp peer/logger.cpp nodeclient.cpp peer/nodeserver.cpp -o Evaluation2/clients4/c2

echo "Compiling peer 3"
g++ -std=c++17 main.cpp peer/peernode.cpp peer/logger.cpp nodeclient.cpp peer/nodeserver.cpp -o Evaluation2/clients4/c3

echo "Compiling peer 4"
g++ -std=c++17 main.cpp peer/peernode.cpp peer/logger.cpp nodeclient.cpp peer/nodeserver.cpp -o Evaluation2/clients4/c4

cd Evaluation2/clients4

./indexserver log/index_server_log.txt log/index_server.csv 127.0.0.1 60009 &
sleep 1

./c1 log/client1.txt log/client1.csv log/client1_s.txt log/client1_s.csv client1/ 127.0.0.1 60009 127.0.0.1 30104 1 &
./c2 log/client2.txt log/client2.csv log/client2_s.txt log/client2_s.csv client2/ 127.0.0.1 60009 127.0.0.1 30105 1 &
./c3 log/client3.txt log/client3.csv log/client3_s.txt log/client3_s.csv client3/ 127.0.0.1 60009 127.0.0.1 30106 1 &
./c4 log/client4.txt log/client4.csv log/client4_s.txt log/client4_s.csv client4/ 127.0.0.1 60009 127.0.0.1 30107 1 &
sleep 5

pkill indexserver
pkill c1
pkill c2
pkill c3
pkill c4

exit
