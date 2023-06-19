
all: indexserver client1 client2 client3 client4
# client4

client1:
	echo "Compiling for peer 1"
	g++ -std=c++17 peer/main.cpp peer/peernode.cpp peer/logger.cpp peer/nodeclient.cpp peer/nodeserver.cpp -o Evaluation4_peers/client1

client2:
	echo "Compiling for peer 2"
	g++ -std=c++17 peer/main.cpp peer/peernode.cpp peer/logger.cpp peer/nodeclient.cpp peer/nodeserver.cpp -o Evaluation4_peers/client2

client3:
	echo "Compiling for peer 3"
	g++ -std=c++17 peer/main.cpp peer/peernode.cpp peer/logger.cpp peer/nodeclient.cpp peer/nodeserver.cpp -o Evaluation4_peers/client3

client4:
	echo "Compiling for peer 4"
	g++ -std=c++17 peer/main.cpp peer/peernode.cpp peer/logger.cpp peer/nodeclient.cpp peer/nodeserver.cpp -o Evaluation4_peers/client4


indexserver:
	echo "Compiling Index server"
	g++ -std=c++17 index_server/main.cpp index_server/indexserver.cpp index_server/logger.cpp -o Evaluation4_peers/indexserver

clean:
	rm Evaluation4_peers/indexserver
	rm Evaluation4_peers/client1
	rm Evaluation4_peers/client2
	rm Evaluation4_peers/client3
	rm Evaluation4_peers/client4
	
run: all
	./run.sh
