/*
	说明：该源码使用信号量解决哲学家就餐问题
	参数：5个线程对应5个哲学家；5个信号量对应5根筷子；第i个哲学家左右两边的筷子编号为i和(i+1)
	过程：奇数号哲学家都先拿自己左边的筷子，再拿自己右边的筷子：
		  偶数号哲学家都先拿自己右边的筷子，再拿自己左边的筷子
	注意：打印过程中，可能会出现问题，因为cout输出的前后没有加上互斥锁。所以g_print_mutex锁是用来输出完整信息
*/

#include <iostream>

#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

using namespace std;

const int kNumOfPhilosophers = 5;

pthread_t g_threadID[kNumOfPhilosophers];

sem_t g_semaphore[kNumOfPhilosophers];

pthread_mutex_t g_print_mutex;

void *threadTask(void *arg);

int main(){
	//初始化5个信号量，代表5根筷子
	int i = 0;
	for(i = 0; i < 5; ++i)
		sem_init(&g_semaphore[i], 0, 1);

	//初始化用于打印信息的互斥锁
	pthread_mutex_init(&g_print_mutex, NULL);

	//初始化5个线程，代表5个哲学家
	int *pthread_index = nullptr;
	for(i = 0; i < 5; ++i){
		pthread_index = new int(i);
		pthread_create(&g_threadID[i], NULL, threadTask, (void *)pthread_index);
	}

	//回收5个线程，避免出现僵尸线程
	for(i = 0; i < 5; ++i)
		pthread_join(g_threadID[i], NULL);

	//销毁打印用的互斥锁
	pthread_mutex_destroy(&g_print_mutex);

	//销毁5个信号量
	for(i = 0; i < 5; ++i)
		sem_destroy(&g_semaphore[i]);

	return 0;
}

void *threadTask(void *arg){
	//取出arg参数中的线程ID（不是pthread_self()函数返回的那个线程标识ID）
	//销毁分配的内存，并且置为空指针，避免野指针的出现
	int *temp = (int*)arg;
	int i = *temp;
	delete temp;
	temp = nullptr;
	arg = nullptr;

	//i个哲学家的左右两个信号量
	int left = i, right = (i + 1) % kNumOfPhilosophers;
	
	while(1){
		//奇数号哲学家先拿左边筷子，再拿右边筷子
		//偶数号哲学家先拿右边筷子，再拿左边筷子
		if(i % 2 == 1){
			sem_wait(&g_semaphore[left]);
			sem_wait(&g_semaphore[right]);

			//拿到两根筷子，吃饭
			pthread_mutex_lock(&g_print_mutex);
			cout << "philosopher " << i << " is dining\n";
			pthread_mutex_unlock(&g_print_mutex);

			sleep(5);

			//释放两根筷子
			sem_post(&g_semaphore[right]);
			sem_post(&g_semaphore[left]);
		}
		else{
			sem_wait(&g_semaphore[right]);
			sem_wait(&g_semaphore[left]);

			//拿到两根筷子，吃饭
			pthread_mutex_lock(&g_print_mutex);
			cout << "philosopher " << i << " is dining\n";
			pthread_mutex_unlock(&g_print_mutex);

			sleep(5);

			//释放两根筷子
			sem_post(&g_semaphore[left]);
			sem_post(&g_semaphore[right]);
		}

		//线程等待
		sleep(2);
	}

	return nullptr;
}
