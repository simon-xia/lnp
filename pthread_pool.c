#include"pthread_pool.h"

void *pthread_process(void *arg)
{
	task_node *tmp = NULL;

//	printf("Now in the %lu thread\n", pthread_self());
	while (1) {
		pthread_mutex_lock(&pool->mutex);
//		printf("%lu thread get lock\n", pthread_self());

		while (!pool->head && !pool->if_destory/* && !pool->decrease*/) { //While !  用是否有任务来控制
//			printf("%lu thread will wait\n", pthread_self());
			pthread_cond_wait(&pool->cond, &pool->mutex);
		}

//		printf("%lu thread: signal is coming\n", pthread_self());
		if (pool->if_destory /*|| pool->decrease*/)
			break;

		tmp = pool->head;

		pool->head = pool->head->next;
	//		pool->working_num++;

		pthread_mutex_unlock(&pool->mutex);
//		printf("%lu thread pick task from queue\n", pthread_self());
		(tmp->func)(tmp->arg);
//		printf("%lu thread finish task\n", pthread_self());
		free(tmp);
		tmp = NULL; //mark

		/*
			pthread_mutex_lock(&pool->mutex);
			pool->working_num--;
			pthread_mutex_unlock(&pool->mutex);
			*/

	}
	pthread_mutex_unlock(&pool->mutex);//先解锁！！
	printf("%lu thread will exit\n", pthread_self());
	pthread_exit(NULL);
}
/*
void *pthread_main(void *arg)
{
	printf("This is main thread\n");
	int i;
	while (1)
	{
		usleep(50000);
		pthread_mutex_lock(&pool->mutex);
		if (pool->if_destory)
			break;
		if (pool->working_num == pool->current_num) {
			for (i = pool->current_num; i < 2 * pool->current_num; i++)
				pthread_create(&pool->p_tid[i], NULL, pthread_process, NULL);
			pool->current_num *= 2;
			printf("The number of thread has been enlarged to %d\n", pool->current_num);
		}
		else if (pool->working_num <= pool->current_num / 4){
			pool->decrease = 1;
			pthread_mutex_unlock(&pool->mutex);
			for (i = 0; i < pool->current_num / 2; i++)
				pthread_cond_signal(&pool->cond);
			pool->current_num /= 2;
			pool->decrease = 0;
			printf("The number of thread has been decrease to %d\n", pool->current_num);
		}
		pthread_mutex_unlock(&pool->mutex);
	}
	pthread_exit(NULL);
}
*/
void pool_init(int pthread_num)
{
	int i = 0;

	pool = (p_pool*)malloc(sizeof(p_pool));
	pthread_mutex_init(&pool->mutex, NULL);
	pthread_cond_init(&pool->cond, NULL);
	pool->head = pool->tail = NULL;
	pool->max_num = MAX_PTHREAD_NUM;
	pool->current_num = pthread_num;
	pool->working_num = 0;
	pool->if_destory = 0;
	pool->decrease = 0;
	pool->p_tid = (pthread_t*)malloc(pthread_num * sizeof(pthread_t));
	
//	pthread_create(&pool->p_tid[i], NULL, pthread_main, NULL);

	for (i = 0; i < pthread_num; i++)
		pthread_create(&pool->p_tid[i], NULL, pthread_process, NULL);

}

int add_task(void * (*func)(void *p), void *arg)
{
	task_node *tmp = (task_node*)malloc(sizeof(task_node));
	tmp->func = *func; //Mark
	tmp->arg = arg;
	tmp->next = NULL;

	pthread_mutex_lock(&pool->mutex);
	if (pool->head) {
		pool->tail = pool->tail->next = tmp;
	}
	else {
		pool->tail = pool->head = tmp;
	}

	pthread_mutex_unlock(&pool->mutex);
	//不加不行？
	//printf("Add task %d success!\n",*(int*)tmp->arg);
	//sleep(1);
	pthread_cond_signal(&pool->cond);
	
	tmp = NULL; //can't free
	return 0;
}


void pool_destory()
{
	int i;

//	pthread_mutex_lock(&pool->mutex);
	pool->if_destory = 1;
//	pthread_mutex_unlock(&pool->mutex);

	pthread_cond_broadcast(&pool->cond);

	for (i = 0; i < pool->current_num; i++)
	{
		if (!pthread_join(pool->p_tid[i], NULL))
			printf("Success to collect thread %lu\n", pool->p_tid[i]);
		else
			printf("Fail to collect thread %lu\n", pool->p_tid[i]);
	}

	free(pool->p_tid);
	free(pool->head);
	free(pool->tail);

	pthread_cond_destroy(&pool->cond);
	pthread_mutex_destroy(&pool->mutex);

	free(pool);
	pool = NULL;
}
