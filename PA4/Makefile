indexserver2:
	g++ -std=c++17 index_server/main.cpp index_server/indexserver.cpp index_server/logger.cpp -o Eval/N_2/indexserver
peer2:
	g++ -std=c++17 Eval/N_2/main.cpp peernode/nodeclient.cpp peernode/nodeserver.cpp peernode/logger.cpp peernode/peernode.cpp -o Eval/N_2/peer
Eval2: clean2 indexserver2 peer2
	./Eval/N_2/run2.sh
clean2:
	rm -rf Eval/N_2/indexserver
	rm -rf Eval/N_2/peer
indexserver4:
	g++ -std=c++17 index_server/main.cpp index_server/indexserver.cpp index_server/logger.cpp -o Eval/N_4/indexserver
peer4:
	g++ -std=c++17 Eval/N_4/main.cpp peernode/nodeclient.cpp peernode/nodeserver.cpp peernode/logger.cpp peernode/peernode.cpp -o Eval/N_4/peer
Eval4: clean4 indexserver4 peer4
	./Eval/N_4/run4.sh
clean4:
	rm -rf Eval/N_4/indexserver
	rm -rf Eval/N_4/peer
indexserver8:
	g++ -std=c++17 index_server/main.cpp index_server/indexserver.cpp index_server/logger.cpp -o Eval/N_8/indexserver
peer8:
	g++ -std=c++17 Eval/N_8/main.cpp peernode/nodeclient.cpp peernode/nodeserver.cpp peernode/logger.cpp peernode/peernode.cpp -o Eval/N_8/peer
Eval8: clean8 indexserver8 peer8
	./Eval/N_8/run8.sh
clean8:
	rm -rf Eval/N_8/indexserver
	rm -rf Eval/N_8/peer
indexserver16:
	g++ -std=c++17 index_server/main.cpp index_server/indexserver.cpp index_server/logger.cpp -o Eval/N_16/indexserver
peer16:
	g++ -std=c++17 Eval/N_16/main.cpp peernode/nodeclient.cpp peernode/nodeserver.cpp peernode/logger.cpp peernode/peernode.cpp -o Eval/N_16/peer
Eval16: clean16 indexserver16 peer16
	./Eval/N_16/run16.sh
clean16:
	rm -rf Eval/N_16/indexserver
	rm -rf Eval/N_16/peer