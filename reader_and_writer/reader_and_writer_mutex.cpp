/*
	说明：该源码采用的是互斥锁解决读者写者问题，和信号量解决方案非常相似
	参数：5个线程对应5个读者，5个线程对应5个写者
		  互斥锁g_reader用来对全局变量g_readerIsReading进行保护
		  互斥锁g_writer对应写者是否能写入
	过程：读者每次先获得g_reader互斥锁，对g_readerIsReading进行改写
		  再根据当前读者数量判断是否需要获得g_writer防止写者此时写入
		  写者每次只需要获得g_writer就能进行写入，因为此时必定有一个读者获得了g_reader并且在等待g_writer
*/
#include <iostream>

#include <pthread.h>
#include <unistd.h>

using namespace std;

const int kReadersNumber = 5;
const int kWritersNumber = 5;

pthread_t g_threadID[kReadersNumber + kWritersNumber];

pthread_mutex_t g_reader;
pthread_mutex_t g_writer;

//正在对文件进行读操作的读者有多少个，初始值为0
int g_readersIsReading = 0;

void *readOperation(void *arg);
void *writeOperation(void *arg);

int main(){
	//初始化互斥锁
	pthread_mutex_init(&g_reader, NULL);
	pthread_mutex_init(&g_writer, NULL);

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
	
	//销毁互斥锁
	pthread_mutex_destroy(&g_reader);
	pthread_mutex_destroy(&g_writer);

	return 0;
}

void *readOperation(void *arg){
	int *temp = (int *)arg;
	int i = *temp;
	delete temp;
	temp = nullptr;
	arg = nullptr;

	while(1){
		//获取读者g_reader锁，改写当前读者数量，根据当前读者数量进行判断
		//如果只有1个读者，说明此时可能会有写者正在进行写入操作，因此需要尝试获得g_writer锁
		//如果有不只1个读者，即有多个读者，说明此时肯定没有写者进行写入操作（g_writer锁肯定被其中的某一个读者获取）
		pthread_mutex_lock(&g_reader);
		++g_readersIsReading;
		if(g_readersIsReading == 1){
			pthread_mutex_lock(&g_writer);
		}
		pthread_mutex_unlock(&g_reader);
		
		//进行读者操作
		cout << "reader " << i << " is reading" << endl;
		sleep(2);

		//读者i结束，解除g_reader锁，并根据情况释放g_writer锁
		pthread_mutex_lock(&g_reader);
		--g_readersIsReading;
		if(g_readersIsReading == 0){
			pthread_mutex_unlock(&g_writer);
		}
		cout << "reader " << i << " finish reading" << endl;
		pthread_mutex_unlock(&g_reader);
		
		sleep(2);
	}
	
	return nullptr;
}

void *writeOperation(void *arg){
	int *temp = (int *)arg;
	int i = *temp;
	delete temp;
	temp = nullptr;
	arg = nullptr;

	while(1){
		//获取g_writer锁，进行相应的写者操作
		pthread_mutex_lock(&g_writer);
		cout << "writer " << i << " is writing" << endl;
		sleep(1);
		
		cout << "writer " << i << " finish writing" << endl;
		pthread_mutex_unlock(&g_writer);

		sleep(2);
	}
	
	return nullptr;
}
