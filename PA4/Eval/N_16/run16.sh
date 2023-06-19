echo "Finished Compiling index server and peer"
cd Eval/N_16
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
./peer log/peer9.txt log/peer9.csv log/peer9_s.txt log/peer9_s.csv peer9/ 127.0.0.1 33327 127.0.0.1 44464 2 &
./peer log/peer10.txt log/peer10.csv log/peer10_s.txt log/peer10_s.csv peer10/ 127.0.0.1 33327 127.0.0.1 44465 2 &
./peer log/peer11.txt log/peer11.csv log/peer11_s.txt log/peer11_s.csv peer11/ 127.0.0.1 33327 127.0.0.1 44466 2 &
./peer log/peer12.txt log/peer12.csv log/peer12_s.txt log/peer12_s.csv peer12/ 127.0.0.1 33327 127.0.0.1 44467 2 &
./peer log/peer13.txt log/peer13.csv log/peer13_s.txt log/peer13_s.csv peer13/ 127.0.0.1 33327 127.0.0.1 44468 2 &
./peer log/peer14.txt log/peer14.csv log/peer14_s.txt log/peer14_s.csv peer14/ 127.0.0.1 33327 127.0.0.1 44469 2 &
./peer log/peer15.txt log/peer15.csv log/peer15_s.txt log/peer15_s.csv peer15/ 127.0.0.1 33327 127.0.0.1 44470 2 &
./peer log/peer16.txt log/peer16.csv log/peer16_s.txt log/peer16_s.csv peer16/ 127.0.0.1 33327 127.0.0.1 44471 2 &
sleep 2
./peer log/peer17.txt log/peer17.csv log/peer17_s.txt log/peer17_s.csv peer17/ 127.0.0.1 33327 127.0.0.1 44472 1 &
sleep 25
pkill indexserver
pkill peer
exit