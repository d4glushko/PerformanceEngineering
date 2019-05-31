# MPI

## Compile MPI 
`mpicc ./hello_world.c -o ./hello_world`

## Start MPI program
`mpirun -np 2 -hosts master,slave1 ./basic_message`

# Docker

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

`docker run -d -p 127.0.0.1:32768:22/tcp --name master mpi:v2`

## Check ports
`docker port master 22`

## Exec to docker container
`docker exec -it slave1 /bin/bash`

## Start image and add to network
`docker run -d --name slave1 --network mpinet mpi:v1`

## Check connected to docker network
`docker network inspect mpinet`

## Add running image to network
`docker network connect mpinet master`

# SSH

## Connect via SSH
`ssh root@0.0.0.0 -p 32768`

## Create SSH key
`ssh-keygen -t rsa`

## Send public key over SSH
`ssh-copy-id -i ./mpi.pub slave1`

## Create SSH agent process
``eval `ssh-agent` ``

## Add identity (private key) to agent process
`ssh-add ./mpi`

# Other

## Add user
`useradd -ms /bin/bash mpiuser`

## Change to user
`su - mpiuser`