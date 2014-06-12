#!/bin/bash
gcc -o client_iterative client_iterative.c simon_socket.c -Wall
gcc -o server_iterative server_iterative.c simon_socket.c -Wall
gcc -o server_multiprocess server_multiprocess.c simon_socket.c -Wall
gcc -o server_multithread server_multithread.c simon_socket.c -Wall -lpthread
gcc -o server_select server_select.c simon_socket.c -Wall
gcc -o server_epoll server_epoll.c simon_socket.c -Wall
gcc -o pool_test pthread_pool.c pool_test.c -Wall -lpthread
gcc -o server_thrpool server_thrpool.c pthread_pool.c simon_socket.c -Wall -lpthread -g
