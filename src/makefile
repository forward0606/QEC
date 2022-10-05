all: a.out

a.out: main.cpp Network/Node/Node.o Network/Channel/Channel.o Path/Path.o Network/Graph/Graph.o Algorithm/AlgorithmBase/AlgorithmBase.o Algorithm/QCAST/QCAST.o Request/Request.o
	g++ -g -Wall -Wextra main.cpp Network/Node/Node.o Network/Channel/Channel.o Path/Path.o Network/Graph/Graph.o Algorithm/AlgorithmBase/AlgorithmBase.o Algorithm/QCAST/QCAST.o Request/Request.o
# qcast.out: Network/Node/Node.o Network/Channel/Channel.o Path/Path.o Network/Graph/Graph.o Algorithm/AlgorithmBase/AlgorithmBase.o
Network/Node/Node.o: Network/Node/Node.cpp Network/Node/Node.h
	g++ -c -g -Wall -Wextra Network/Node/Node.cpp Network/Node/Node.h
	mv Node.o Network/Node/Node.o

Network/Channel/Channel.o: Network/Channel/Channel.h Network/Channel/Channel.cpp
	g++ -c -g -Wall -Wextra Network/Channel/Channel.h Network/Channel/Channel.cpp
	mv Channel.o Network/Channel/Channel.o

Path/Path.o: Path/Path.cpp Path/Path.h
	g++ -c -g -Wall -Wextra Path/Path.cpp Path/Path.h
	mv Path.o Path/Path.o

Network/Graph/Graph.o: Network/Graph/Graph.cpp Network/Graph/Graph.h
	g++ -c -g -Wall -Wextra Network/Graph/Graph.cpp Network/Graph/Graph.h
	mv Graph.o Network/Graph/Graph.o

Request/Request.o: Request/Request.cpp Request/Request.h
	g++ -c -g -Wall -Wextra Request/Request.cpp Request/Request.h
	mv Request.o Request/Request.o

Algorithm/AlgorithmBase/AlgorithmBase.o: Algorithm/AlgorithmBase/AlgorithmBase.h Algorithm/AlgorithmBase/AlgorithmBase.cpp
	g++ -c -g -Wall -Wextra Algorithm/AlgorithmBase/AlgorithmBase.h Algorithm/AlgorithmBase/AlgorithmBase.cpp
	mv AlgorithmBase.o Algorithm/AlgorithmBase/AlgorithmBase.o

Algorithm/QCAST/QCAST.o: Algorithm/QCAST/QCAST.h Algorithm/QCAST/QCAST.cpp
	g++ -c -g -Wall -Wextra Algorithm/QCAST/QCAST.h Algorithm/QCAST/QCAST.cpp
	mv QCAST.o Algorithm/QCAST/QCAST.o

check_node: Node/Node.o Node/Node_test.cpp
	g++ -g -Wall -Wextra Node/Node_test.cpp Node/Node.o -o node_check.out

check_channel: Node/Node.o Channel/Channel.o Channel/Channel_test.cpp
	g++ -g -Wall -Wextra Node/Node.o Channel/Channel.o Channel/Channel_test.cpp -o channel_check.out
clean:
	rm -f a.out check_node.out channel_check.out
	rm -f Network/Node/Node.o Network/Node/Node.h.gch
	rm -f Network/Channel/Channel.o Network/Channel/Channel.h.gch
	rm -f Path/Path.o Path/Path.h.gch
	rm -f Network/Graph/Graph.o Network/Graph/Graph.h.gch
	rm -f Request/Request.o Request/Request.h.gch