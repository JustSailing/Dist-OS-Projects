make clean
make
cd Evaluation4_peers
echo "Finished Compiling Index Server and 4 peers"
./indexserver log/index_server_log.txt log/index_server.csv 127.0.0.1 33327 &
sleep 1
./client1 log/client1.txt log/client1.csv log/client1_s.txt log/client1_s.csv peer1/ 127.0.0.1 33327 127.0.0.1 44456 1 &
./client2 log/client2.txt log/client2.csv log/client2_s.txt log/client2_s.csv peer2/ 127.0.0.1 33327 127.0.0.1 44457 2 &
./client3 log/client3.txt log/client3.csv log/client3_s.txt log/client3_s.csv peer3/ 127.0.0.1 33327 127.0.0.1 44458 3 &
./client4 log/client4.txt log/client4.csv log/client4_s.txt log/client4_s.csv peer4/ 127.0.0.1 33327 127.0.0.1 44459 4 &


sleep 3


pkill indexserver
pkill client1
pkill client2
pkill client3
pkill client4

#wait
exit
