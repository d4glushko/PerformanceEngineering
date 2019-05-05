all:
	gcc -O0 -g3 -march=skylake -o compiled/matrices_with_vectorization ./matrices_with_vectorization.cpp
	gcc -O3 -g3 -march=skylake -o compiled/matrices_with_vectorization_o3 ./matrices_with_vectorization.cpp
	gcc -O0 -g3 -march=skylake -o compiled/matrices_without_vectorization ./matrices_without_vectorization.cpp
	gcc -O3 -g3 -march=skylake -o compiled/matrices_without_vectorization_o3 ./matrices_without_vectorization.cpp
	gcc -O0 -g3 -march=skylake -o compiled/string_with_vectorization ./string_with_vectorization.cpp
	gcc -O3 -g3 -march=skylake -o compiled/string_with_vectorization_o3 ./string_with_vectorization.cpp
	gcc -O0 -g3 -march=skylake -o compiled/string_without_vectorization ./string_without_vectorization.cpp
	gcc -O3 -g3 -march=skylake -o compiled/string_without_vectorization_o3 ./string_without_vectorization.cpp
