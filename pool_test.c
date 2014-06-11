 /* 
 *   test the thread pool with a simple example 
 *				Mon Jun 9 21:50:36 CST 2014
 *              by  Simon Xia
 */

#include<stdio.h>
#include<stdlib.h>

extern void pool_init(int num);

void *process(void *p)
{
//	usleep(50000);   加上后 core dumped
	printf("This is pthread %lu, the result is %d\n", pthread_self(), *(int*)p);
}

int main()
{
	int i, *p;
	pool_init(4);

	p = (int*)malloc(10*sizeof(int));
	for (i = 0; i < 10; i++)
	{
		*p[i] = i; 
		add_task(process, (void *)p);
	}

	free(p);
    sleep(5);
	pool_destory();
	return 0;
}

