all:
	g++ -O3 -g3 -march=skylake -o compiled/simple_sum ./simple_sum.cpp
	g++ -O3 -g3 -march=skylake -o compiled/simple_min ./simple_min.cpp
	g++ -O3 -g3 -march=skylake -o compiled/simple_integral_image ./simple_integral_image.cpp
	mpicc -O3 -g3 -march=skylake -o compiled/mpi_sum ./mpi_sum.cpp
	mpicc -O3 -g3 -march=skylake -o compiled/mpi_min ./mpi_min.cpp
	# mpicc -O3 -g3 -march=skylake -o compiled/mpi_integral_image ./mpi_integral_image.cpp
