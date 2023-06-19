superpeer:
	g++ -std=c++17 super_peer/main.cpp super_peer/indexserver.cpp super_peer/logger.cpp -luuid -o Eval1/superpeer
weakpeer:
	g++ -std=c++17 weak_peer/main.cpp weak_peer/nodeclient.cpp weak_peer/nodeserver.cpp weak_peer/peernode.cpp weak_peer/logger.cpp -luuid -o Eval1/weakpeer
superpeer1:
	g++ -std=c++17 super_peer/main.cpp super_peer/indexserver.cpp super_peer/logger.cpp -luuid -o Eval2/superpeer
weakpeer1:
	g++ -std=c++17 Eval2/main.cpp weak_peer/nodeclient.cpp weak_peer/nodeserver.cpp weak_peer/peernode.cpp weak_peer/logger.cpp -luuid -o Eval2/weakpeer
superpeer2:
	g++ -std=c++17 super_peer/main.cpp super_peer/indexserver.cpp super_peer/logger.cpp -luuid -o Eval3/superpeer
weakpeer2:
	g++ -std=c++17 Eval3/main.cpp weak_peer/nodeclient.cpp weak_peer/nodeserver.cpp weak_peer/peernode.cpp weak_peer/logger.cpp -luuid -o Eval3/weakpeer
Eval1: cleanEval1 weakpeer superpeer
	./Eval1/run1.sh
Eval2: cleanEval2 weakpeer1 superpeer1
	./Eval2/run2.sh
Eval3: cleanEval3 weakpeer2 superpeer2
	./Eval3/run3.sh
cleanEval1:
	rm -rf Eval1/superpeer
	rm -rf Eval1/weakpeer
cleanEval2:
	rm -rf Eval2/superpeer
	rm -rf Eval2/weakpeer
cleanEval3:
	rm -rf Eval3/superpeer
	rm -rf Eval3/weakpeer