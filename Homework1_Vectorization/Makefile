all:
	gcc -O0 -g3 -march=skylake -o compiled/with_vectorization ./with_vectorization.cpp
	gcc -O3 -g3 -march=skylake -o compiled/with_vectorization_o3 ./with_vectorization.cpp
	gcc -O0 -g3 -march=skylake -o compiled/without_vectorization ./without_vectorization.cpp
	gcc -O3 -g3 -march=skylake -o compiled/without_vectorization_o3 ./without_vectorization.cpp
