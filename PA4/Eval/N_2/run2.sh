echo "Finished Compiling index server and peer"
cd Eval/N_2
./indexserver log/indexserver.txt log/indexserver.csv 127.0.0.1 33327 &
sleep 1
./peer log/peer1.txt log/peer1.csv log/peer1_s.txt log/peer1_s.csv peer1/ 127.0.0.1 33327 127.0.0.1 44456 2 &
./peer log/peer2.txt log/peer2.csv log/peer2_s.txt log/peer2_s.csv peer2/ 127.0.0.1 33327 127.0.0.1 44457 2 &
sleep 2
./peer log/peer3.txt log/peer3.csv log/peer3_s.txt log/peer3_s.csv peer3/ 127.0.0.1 33327 127.0.0.1 44458 1 &
sleep 25
pkill indexserver
pkill peer
exit