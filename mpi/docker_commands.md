## Compile MPI 
`mpicc ./hello_world.c -o ./hello_world`

## Start MPI program
`mpirun -np 2 ./basic_message`

## Docker build image
`docker build -t mpi:v1 -f ./Dockerfile .`

mpi:v1 - image name

./Dockerfile - how to create image

. - workdir

## Delete docker image
`docker rm <image_name/image_id>`

## Stop docker container
`docker stop slave1`

## Delete docker container
`docker rm master`

## Create docker network
`docker network create <network_name>`

## Check networks
`docker network ls`

## Run container from image
`docker run -d -P --name master mpi:v1`

## Check ports
`docker port master 22`

## Connect via SSH
`ssh root@0.0.0.0 -p 32768`

## Start image and add to network
`docker run -d --name slave1 --network mpinet mpi:v1`

## Check connected to docker network
`docker network inspect mpinet`

## Add running image to network
`docker network connect mpinet master`

## Add user with SSH
`useradd -ms /bin/bash mpiuser`

## Change to user with SSH
`su - mpiuser`