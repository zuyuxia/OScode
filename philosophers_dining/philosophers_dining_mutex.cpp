/*
	说明：该源码使用互斥锁解决哲学家就餐问题
	参数：5个线程对应5个哲学家；5把互斥锁对应5根筷子；第i个哲学家左右两边的筷子编号为i和(i+1)
	过程：每个哲学家都先拿自己左边的筷子，然后再尝试拿自己右边的筷子：
		  如果获取右边的筷子获取成功，则吃饭。
		  如果获取失败，则放下自己已经获得的左边筷子，重新进入思考状态
	注意：打印过程中，可能会出现问题，因为cout输出的前后没有加上互斥锁。所以g_print_mutex锁是用来输出完整信息
*/

#include <iostream>

#include <pthread.h>
#include <unistd.h>

using namespace std;

const int kNumOfPhilosophers = 5;

pthread_t g_threadID[kNumOfPhilosophers];

pthread_mutex_t g_mutex[kNumOfPhilosophers];

pthread_mutex_t g_print_mutex;

void *threadTask(void *arg);

int main(){
	//初始化5把互斥锁，代表5根筷子
	int i = 0;
	for(i = 0; i < 5; ++i)
		pthread_mutex_init(&g_mutex[i], NULL);

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

	//销毁5把互斥锁
	for(i = 0; i < 5; ++i)
		pthread_mutex_destroy(&g_mutex[i]);

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

	//i个哲学家的左右两把锁的index
	int left = i, right = (i + 1) % kNumOfPhilosophers;
	
	while(1){
		//先拿左边筷子
		pthread_mutex_lock(&g_mutex[left]);

		//再尝试拿右边筷子
		if(pthread_mutex_trylock(&g_mutex[right]) == 0){
			//成功拿到两根筷子，吃饭
			pthread_mutex_lock(&g_print_mutex);
			cout << "philosopher " << i << " is dining\n";
			pthread_mutex_unlock(&g_print_mutex);

			sleep(5);

			//释放两根筷子
			pthread_mutex_unlock(&g_mutex[right]);
			pthread_mutex_unlock(&g_mutex[left]);
		}
		else{
			//右边筷子被占用，释放已经拿到的左边筷子
			pthread_mutex_unlock(&g_mutex[left]);
		}
	}

	return nullptr;
}
