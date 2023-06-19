echo "Finished Compiling index server and peer"
cd Eval/N_4
./indexserver log/indexserver.txt log/indexserver.csv 127.0.0.1 33327 &
sleep 1
./peer log/peer1.txt log/peer1.csv log/peer1_s.txt log/peer1_s.csv peer1/ 127.0.0.1 33327 127.0.0.1 44456 2 &
./peer log/peer2.txt log/peer2.csv log/peer2_s.txt log/peer2_s.csv peer2/ 127.0.0.1 33327 127.0.0.1 44457 2 &
./peer log/peer3.txt log/peer3.csv log/peer3_s.txt log/peer3_s.csv peer3/ 127.0.0.1 33327 127.0.0.1 44458 2 &
./peer log/peer4.txt log/peer4.csv log/peer4_s.txt log/peer4_s.csv peer4/ 127.0.0.1 33327 127.0.0.1 44459 2 &
sleep 2
./peer log/peer5.txt log/peer5.csv log/peer5_s.txt log/peer5_s.csv peer5/ 127.0.0.1 33327 127.0.0.1 44460 1 &
sleep 25
pkill indexserver
pkill peer
exit