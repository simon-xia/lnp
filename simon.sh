#!/bin/bash
gcc -o client_iterative client_iterative.c simon_socket.c -Wall
gcc -o server_iterative server_iterative.c simon_socket.c -Wall
gcc -o server_mutiprocess server_mutiprocess.c simon_socket.c -Wall
gcc -o server_mutithread server_mutithread.c simon_socket.c -Wall -lpthread
#gcc -o pool_test pthread_pool.c pool_test.c -Wall -lpthread
gcc -o server_thrpool server_thrpool.c pthread_pool.c simon_socket.c -Wall -lpthread -g
