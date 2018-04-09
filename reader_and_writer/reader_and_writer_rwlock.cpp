/*
	说明：该源码采用的是读写锁解决读者写者问题
	参数：5个线程对应5个读者，1个线程对应1个写者
		  读写锁g_rwlock
	过程：读者每次先获得读出锁再进行操作
		  写者每次先获得写入锁再进行操作
*/

#include <iostream>

#include <pthread.h>
#include <unistd.h>

using namespace std;

const int kReadersNumber = 5;
const int kWritersNumber = 1;

pthread_t g_threadID[kReadersNumber + kWritersNumber];

pthread_rwlock_t g_rwlock; 

void *readOperation(void *arg);
void *writeOperation(void *arg);

int main(){
	//初始化读写锁
	pthread_rwlock_init(&g_rwlock, NULL);

	//初始化5个线程，代表5个读者
	int *pthread_id = nullptr;

	for(int i = 0; i < kReadersNumber; ++i){
		pthread_id = new int(i);
		pthread_create(&g_threadID[i], NULL, readOperation, (void *)pthread_id);
	}

	//初始化5个线程，代表5个写者
	for(int i = 0; i < kWritersNumber; ++i){
		pthread_id = new int(i);
		pthread_create(&g_threadID[i + kReadersNumber], NULL, writeOperation, (void *)pthread_id);
	}

	//回收读者线程和写者线程
	for(int i = 0; i < kReadersNumber + kWritersNumber; ++i)
		pthread_join(g_threadID[i], NULL);
	
	//销毁读写锁
	pthread_rwlock_destroy(&g_rwlock);

	return 0;
}

void *readOperation(void *arg){
	int *temp = (int *)arg;
	int i = *temp;
	delete temp;
	temp = nullptr;
	arg = nullptr;

	while(1){
		//获取g_rwlock的读出锁
		pthread_rwlock_rdlock(&g_rwlock);
		
		//进行读者操作
		cout << "reader " << i << " is reading" << endl;
		sleep(1);
		cout << "reader " << i << " finish reading" << endl;

		pthread_rwlock_unlock(&g_rwlock);
		
		sleep(1);
	}
}

void *writeOperation(void *arg){
	int *temp = (int *)arg;
	int i = *temp;
	delete temp;
	temp = nullptr;
	arg = nullptr;

	while(1){
		//获取g_rwlock的写入锁，进行相应的写者操作
		pthread_rwlock_wrlock(&g_rwlock);

		cout << "writer " << i << " is writing" << endl;
		sleep(1);
		cout << "writer " << i << " finish writing" << endl;

		pthread_rwlock_unlock(&g_rwlock);

		sleep(2);
	}
}
