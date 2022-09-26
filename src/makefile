all: a.out

a.out: main.cpp Node/Node.o Channel/Channel.o Path/Path.o Graph/Graph.o
	g++ -g -Wall -Wextra main.cpp Node/Node.o Channel/Channel.o Path/Path.o Graph/Graph.o

Node/Node.o: Node/Node.cpp Node/Node.h
	g++ -c -g -Wall -Wextra Node/Node.cpp Node/Node.h
	mv Node.o Node/Node.o

Channel/Channel.o: Channel/Channel.h Channel/Channel.cpp
	g++ -c -g -Wall -Wextra Channel/Channel.h Channel/Channel.cpp
	mv Channel.o Channel/Channel.o

Path/Path.o: Path/Path.cpp Path/Path.h
	g++ -c -g -Wall -Wextra Path/Path.cpp Path/Path.h
	mv Path.o Path/Path.o

Graph/Graph.o: Graph/Graph.cpp Graph/Graph.h
	g++ -c -g -Wall -Wextra Graph/Graph.cpp Graph/Graph.h
	mv Graph.o Graph/Graph.o

check_node: Node/Node.o Node/Node_test.cpp
	g++ -g -Wall -Wextra Node/Node_test.cpp Node/Node.o -o node_check.out

check_channel: Node/Node.o Channel/Channel.o Channel/Channel_test.cpp
	g++ -g -Wall -Wextra Node/Node.o Channel/Channel.o Channel/Channel_test.cpp -o channel_check.out
clean:
	rm -f a.out check_node.out channel_check.out
	rm -f Node/Node.o Node/Node.h.gch
	rm -f Channel/Channel.o Channel/Channel.h.gch
	rm -f Path/Path.o Path/Path.h.gch
	rm -f Graph/Graph.o Graph/Graph.h.gch