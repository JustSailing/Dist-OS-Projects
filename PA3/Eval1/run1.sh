cd Eval1
echo "Finished Compiling Super Peer and Weak Peer"
./superpeer log/superpeer1.txt log/superpeer1.csv 127.0.0.1 33327 127.0.0.1 33328 127.0.0.1 33329 &
./superpeer log/superpeer2.txt log/superpeer2.csv 127.0.0.1 33328 127.0.0.1 33327 127.0.0.1 33329 &
./superpeer log/superpeer3.txt log/superpeer3.csv 127.0.0.1 33329 127.0.0.1 33328 127.0.0.1 33327 &
sleep 1
./weakpeer log/weakpeer1.txt log/weakpeer1.csv log/weakpeer1_s.txt log/weakpeer1_s.csv weak_peer1/ 127.0.0.1 33327 127.0.0.1 44456 1 &
./weakpeer log/weakpeer2.txt log/weakpeer2.csv log/weakpeer2_s.txt log/weakpeer2_s.csv weak_peer2/ 127.0.0.1 33328 127.0.0.1 44457 2 &
./weakpeer log/weakpeer3.txt log/weakpeer3.csv log/weakpeer3_s.txt log/weakpeer3_s.csv weak_peer3/ 127.0.0.1 33329 127.0.0.1 44458 3 &
sleep 7
pkill superpeer
pkill weakpeer
exit

