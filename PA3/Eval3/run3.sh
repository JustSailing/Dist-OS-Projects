cd Eval3
echo "Finished Compiling Super Peer and Weak Peer"

./superpeer log/superpeer1.txt log/superpeer1.csv 127.0.0.1 33327 127.0.0.1 33328 127.0.0.1 33329 127.0.0.1 33330 127.0.0.1 33331 127.0.0.1 33332 127.0.0.1 33333 127.0.0.1 33334 127.0.0.1 33335 &
./superpeer log/superpeer2.txt log/superpeer2.csv 127.0.0.1 33328 127.0.0.1 33327 127.0.0.1 33329 127.0.0.1 33330 127.0.0.1 33331 127.0.0.1 33332 127.0.0.1 33333 127.0.0.1 33334 127.0.0.1 33335 &
./superpeer log/superpeer3.txt log/superpeer3.csv 127.0.0.1 33329 127.0.0.1 33327 127.0.0.1 33329 127.0.0.1 33330 127.0.0.1 33331 127.0.0.1 33332 127.0.0.1 33333 127.0.0.1 33334 127.0.0.1 33335 &
./superpeer log/superpeer4.txt log/superpeer4.csv 127.0.0.1 33330 127.0.0.1 33327 127.0.0.1 33328 127.0.0.1 33329 127.0.0.1 33331 127.0.0.1 33332 127.0.0.1 33333 127.0.0.1 33334 127.0.0.1 33335&
./superpeer log/superpeer5.txt log/superpeer5.csv 127.0.0.1 33331 127.0.0.1 33328 127.0.0.1 33329 127.0.0.1 33330 127.0.0.1 33327 127.0.0.1 33332 127.0.0.1 33333 127.0.0.1 33334 127.0.0.1 33335 &
./superpeer log/superpeer6.txt log/superpeer6.csv 127.0.0.1 33332 127.0.0.1 33329 127.0.0.1 33329 127.0.0.1 33330 127.0.0.1 33331 127.0.0.1 33327 127.0.0.1 33333 127.0.0.1 33334 127.0.0.1 33335 &
./superpeer log/superpeer7.txt log/superpeer7.csv 127.0.0.1 33333 127.0.0.1 33329 127.0.0.1 33327 127.0.0.1 33330 127.0.0.1 33331 127.0.0.1 33332 127.0.0.1 33328 127.0.0.1 33334 127.0.0.1 33335&
./superpeer log/superpeer8.txt log/superpeer8.csv 127.0.0.1 33334 127.0.0.1 33330 127.0.0.1 33329 127.0.0.1 33327 127.0.0.1 33331 127.0.0.1 33332 127.0.0.1 33333 127.0.0.1 33327 127.0.0.1 33335&
./superpeer log/superpeer9.txt log/superpeer9.csv 127.0.0.1 33335 127.0.0.1 33330 127.0.0.1 33329 127.0.0.1 33328 127.0.0.1 33331 127.0.0.1 33332 127.0.0.1 33333 127.0.0.1 33334 127.0.0.1 33327&

sleep 1
./weakpeer log/weakpeer1.txt log/weakpeer1.csv log/weakpeer1_s.txt log/weakpeer1_s.csv weak_peer1/ 127.0.0.1 33327 127.0.0.1 44456 1 &
./weakpeer log/weakpeer2.txt log/weakpeer2.csv log/weakpeer2_s.txt log/weakpeer2_s.csv weak_peer2/ 127.0.0.1 33328 127.0.0.1 44457 2 &
./weakpeer log/weakpeer3.txt log/weakpeer3.csv log/weakpeer3_s.txt log/weakpeer3_s.csv weak_peer3/ 127.0.0.1 33329 127.0.0.1 44458 1 &
./weakpeer log/weakpeer4.txt log/weakpeer4.csv log/weakpeer4_s.txt log/weakpeer4_s.csv weak_peer4/ 127.0.0.1 33330 127.0.0.1 44459 1 &
./weakpeer log/weakpeer5.txt log/weakpeer5.csv log/weakpeer5_s.txt log/weakpeer5_s.csv weak_peer5/ 127.0.0.1 33331 127.0.0.1 44460 1 &
./weakpeer log/weakpeer6.txt log/weakpeer6.csv log/weakpeer6_s.txt log/weakpeer6_s.csv weak_peer6/ 127.0.0.1 33332 127.0.0.1 44461 1 &
./weakpeer log/weakpeer7.txt log/weakpeer7.csv log/weakpeer7_s.txt log/weakpeer7_s.csv weak_peer7/ 127.0.0.1 33333 127.0.0.1 44462 2 &
./weakpeer log/weakpeer8.txt log/weakpeer8.csv log/weakpeer8_s.txt log/weakpeer8_s.csv weak_peer8/ 127.0.0.1 33334 127.0.0.1 44463 3 &
./weakpeer log/weakpeer9.txt log/weakpeer9.csv log/weakpeer9_s.txt log/weakpeer9_s.csv weak_peer9/ 127.0.0.1 33335 127.0.0.1 44464 3 &
sleep 7
pkill superpeer
pkill weakpeer
rm -rf weak_peer1/2k.txt
rm -rf weak_peer4/2k.txt
rm -rf weak_peer3/2k.txt
rm -rf weak_peer9/128.txt
rm -rf weak_peer5/2k.txt
rm -rf weak_peer6/2k.txt
rm -rf weak_peer7/2k.txt
rm -rf weak_peer8/128.txt

exit
