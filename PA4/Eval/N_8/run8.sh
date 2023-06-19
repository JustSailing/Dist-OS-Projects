echo "Finished Compiling index server and peer"
cd Eval/N_8
./indexserver log/indexserver.txt log/indexserver.csv 127.0.0.1 33327 &
sleep 1
./peer log/peer1.txt log/peer1.csv log/peer1_s.txt log/peer1_s.csv peer1/ 127.0.0.1 33327 127.0.0.1 44456 2 &
./peer log/peer2.txt log/peer2.csv log/peer2_s.txt log/peer2_s.csv peer2/ 127.0.0.1 33327 127.0.0.1 44457 2 &
./peer log/peer3.txt log/peer3.csv log/peer3_s.txt log/peer3_s.csv peer3/ 127.0.0.1 33327 127.0.0.1 44458 2 &
./peer log/peer4.txt log/peer4.csv log/peer4_s.txt log/peer4_s.csv peer4/ 127.0.0.1 33327 127.0.0.1 44459 2 &
./peer log/peer5.txt log/peer5.csv log/peer5_s.txt log/peer5_s.csv peer5/ 127.0.0.1 33327 127.0.0.1 44460 2 &
./peer log/peer6.txt log/peer6.csv log/peer6_s.txt log/peer6_s.csv peer6/ 127.0.0.1 33327 127.0.0.1 44461 2 &
./peer log/peer7.txt log/peer7.csv log/peer7_s.txt log/peer7_s.csv peer7/ 127.0.0.1 33327 127.0.0.1 44462 2 &
./peer log/peer8.txt log/peer8.csv log/peer8_s.txt log/peer8_s.csv peer8/ 127.0.0.1 33327 127.0.0.1 44463 2 &
sleep 2
./peer log/peer9.txt log/peer9.csv log/peer9_s.txt log/peer9_s.csv peer9/ 127.0.0.1 33327 127.0.0.1 44464 1 &
sleep 25
pkill indexserver
pkill peer
exit