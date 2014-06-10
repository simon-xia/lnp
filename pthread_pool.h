#ifndef _PTHREAD_POOL_H_
#define _PTHREAD_POOL_H_

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>

#define MAX_PTHREAD_NUM 100

typedef struct task_node{
	void * (*func)(void * p);
	void * arg;
	struct task_node* next;
}task_node;

typedef struct p_pool{
	pthread_cond_t cond;
	pthread_mutex_t mutex;
	task_node *head, *tail;
	pthread_t *p_tid;			//mark , unsigned long
	int max_num;
	int current_num;
	int working_num;
	int if_destory;
	int decrease;
}p_pool;

p_pool * pool = NULL;

void pool_init(int pthread_num);
void *pthread_process(void *arg);
int add_task(void * (*func)(void *p), void *arg);
void pool_destory();

#endif
