all:
	g++ -O3 -g3 -march=skylake -o compiled/single_thread_sum ./single_thread_sum.cpp
	g++ -O3 -g3 -march=skylake -o compiled/single_thread_min ./single_thread_min.cpp
	g++ -O3 -g3 -march=skylake -o compiled/single_thread_integral_image ./single_thread_integral_image.cpp
	g++ -std=c++11  -pthread -O3 -g3 -march=skylake -o compiled/multi_thread_sum ./multi_thread_sum.cpp
	g++ -std=c++11  -pthread -O3 -g3 -march=skylake -o compiled/multi_thread_min ./multi_thread_min.cpp
	g++ -std=c++11  -pthread -O3 -g3 -march=skylake -o compiled/multi_thread_integral_image ./multi_thread_integral_image.cpp
