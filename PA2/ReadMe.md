# Program Assignment 2
## Test with 4 peers
There should be a 4 folders in Evaluation4_peers labeled as peer1, peer2, peer3, peer4. Each folders contains:
- peer1: 32k file, 128 file
- peer2: 2k file
- peer3: 8k file
- peer4: 512 file
- log folder is empty (after ./run.sh it should be populated with logs)

After running the script, all folders should have a copy of each file. The log folder should be populated with log files of each peer acting as client, server, and the index server log file.

### To run type command in terminal and run (make sure to chmod +x run.sh)
./run.sh

- you can change the ip and ports of index servers and peers in the run.sh They are labeled.

## Test with N peers

The folder called Evaluation2 should have folders:
Each clientN should have 1 file 128.txt

- clients2
  - client1
  - client2
  - log
- client4
  - client1
  - client2
  - client3
  - client4
  - log
- client8
  - client1
  - client2
  - client3
  - client4
  - client5
  - client6
  - client7
  - client8
  - log
- client16
  - client1
  - client2
  - client3
  - client4
  - client5
  - client6
  - client7
  - client8
  - client9
  - client10
  - client11
  - client12
  - client13
  - client14
  - client15
  - client16
  - log

### ./run_eval2_2.sh 
#### runs 2 peers concurrently querying a file
### ./run_eval2_4.sh 
#### runs 4 peers concurrently querying a file
### ./run_eval2_8.sh
#### runs 8 peers concurrently querying a file
### ./run_eval2_16.sh 
#### runs 16 peers concurrently querying a file

#### Log files should be populated with index server logs and peer logs
