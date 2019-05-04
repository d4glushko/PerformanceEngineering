all:race mutex1 mutex2 mutex3 mutex4

race:race.cpp
	g++ -std=c++11  -pthread -o race ./race.cpp

mutex1:mutex1.cpp
	g++ -std=c++11  -pthread -o mutex1 ./mutex1.cpp

mutex2:mutex2.cpp
	g++ -std=c++11  -pthread -o mutex2 ./mutex2.cpp

mutex3:mutex3.cpp
	g++ -std=c++11  -pthread -o mutex3 ./mutex3.cpp

mutex4:mutex4.cpp
	g++ -std=c++11  -pthread -o mutex4 ./mutex4.cpp

clean: 
	rm race mutex1 mutex2
