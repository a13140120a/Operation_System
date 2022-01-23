#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int sum;
void* runner(void* param);

int main() {

	pthread_t tid;
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	
	pthread_create(&tid, &attr, runner, 100);

	pthread_join(tid, NULL);

	printf("sum = %d\n", sum);

	return 0;
}

void* runner(void* param)
{
	int i, upper = atoi((char*)param);
	sum = 0;

	for (i = 1; i <= upper; i++)
		sum += 1;

	pthread_exit(0);
}