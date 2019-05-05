all:mutex conditional future async

mutex:mutex.cpp
	g++ -std=c++11  -pthread -o mutex ./mutex.cpp

conditional:conditional.cpp
	g++ -std=c++11  -pthread -o conditional ./conditional.cpp

future:future.cpp
	g++ -std=c++11  -pthread -o future ./future.cpp

async:async.cpp
	g++ -std=c++11  -pthread -o async ./async.cpp

clean: 
	rm race mutex conditional future async
