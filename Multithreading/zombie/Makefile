all: zombie zombie_1 nozombie background

zombie:zombie.c
	gcc -pthread -o zombie ./zombie.c

zombie_1:zombie_1.c
	gcc -pthread -o zombie_1 ./zombie_1.c

nozombie:nozombie.c
	gcc -pthread -o nozombie ./nozombie.c

zombie2:background.c
	gcc -pthread -o background ./background.c

clean:
	rm zombie nozombie zombie_1 background
