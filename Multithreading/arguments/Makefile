all:simple pointer1 pointer2 heap  references1 references2

simple:simple.cpp
	g++ -std=c++11  -pthread -o simple ./simple.cpp

pointer1:pointer1.cpp
	g++ -std=c++11  -pthread -o pointer1 ./pointer1.cpp

pointer2:pointer2.cpp
	g++ -std=c++11  -pthread -o pointer2 ./pointer2.cpp

heap:heap.cpp
	g++ -std=c++11  -pthread -o heap ./heap.cpp
	
references1:references1.cpp
	g++ -std=c++11  -pthread -o references1 ./references1.cpp

references2:references2.cpp
	g++ -std=c++11  -pthread -o references2 ./references2.cpp

clean: 
	rm simple pointer1 pointer2 heap references1 references2
