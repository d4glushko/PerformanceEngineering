all:function_pointer function_object lambda member_function member_function_concurrent

function_pointer:function_pointer.cpp
	g++ -std=c++11  -pthread -o function_pointer ./function_pointer.cpp

function_object:function_object.cpp
	g++ -std=c++11  -pthread -o function_object ./function_object.cpp

lambda:lambda.cpp
	g++ -std=c++11  -pthread -o lambda ./lambda.cpp
	
member_function:member_function.cpp
	g++ -std=c++11  -pthread -o member_function ./member_function.cpp
	
member_function_concurrent:member_function_concurrent.cpp
	g++ -std=c++11  -pthread -o member_function_concurrent ./member_function_concurrent.cpp
	
clean: 
	rm function_pointer function_object lambda member_function
