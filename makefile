all: main

main: main.cpp src/skiplist_node.h src/skiplist.h
	g++ -g main.cpp -o main -std=c++11
	cp main bin/main

clean:
	rm main
