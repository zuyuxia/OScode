/*
	说明：该源码采用的是信号量解决读者写者问题
	参数：5个线程对应5个读者，5个线程对应5个写者
		  信号量g_reader用来对全局变量g_readerIsReading进行保护
		  信号量g_writer对应写者是否能写入
	过程：读者每次先获得g_reader信号量，对g_readerIsReading进行改写
		  再根据当前读者数量判断是否需要获得g_writer防止写者此时写入
		  写者每次只需要获得g_writer就能进行写入，因为此时必定有一个读者获得了g_reader并且在等待g_writer
*/
#include <iostream>

#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

using namespace std;

const int kReadersNumber = 5;
const int kWritersNumber = 5;

pthread_t g_threadID[kReadersNumber + kWritersNumber];

sem_t g_reader;
sem_t g_writer;

//正在对文件进行读操作的读者有多少个，初始值为0
int g_readersIsReading = 0;

void *readOperation(void *arg);
void *writeOperation(void *arg);

int main(){
	//初始化信号量
	sem_init(&g_reader, 0, 1);
	sem_init(&g_writer, 0, 1);

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
	
	//销毁信号量
	sem_destroy(&g_reader);
	sem_destroy(&g_writer);

	return 0;
}

void *readOperation(void *arg){
	int *temp = (int *)arg;
	int i = *temp;
	delete temp;
	temp = nullptr;
	arg = nullptr;

	while(1){
		//等待读者信号量，获取读者信号量，改写当前读者数量，根据当前读者数量进行判断
		//如果只有1个读者，说明此时可能会有写者正在进行写入操作，因此需要尝试获得g_writer信号量
		//如果有不只1个读者，即有多个读者，说明此时肯定没有写者进行写入操作（g_writer信号量肯定被其中的某一个读者已经获取量）
		sem_wait(&g_reader);
		++g_readersIsReading;
		if(g_readersIsReading == 1){
			sem_wait(&g_writer);
		}
		sem_post(&g_reader);
		
		//进行读者操作
		cout << "reader " << i << " is reading" << endl;
		sleep(2);

		//读者i结束，改写此时读者的数量，并根据情况释放g_writer信号量
		sem_wait(&g_reader);
		--g_readersIsReading;
		if(g_readersIsReading == 0){
			sem_post(&g_writer);
		}
		cout << "reader " << i << " finish reading" << endl;
		sem_post(&g_reader);
		
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
		//获取g_writer信号量，进行相应的写者操作
		sem_wait(&g_writer);
		cout << "writer " << i << " is writing" << endl;
		sleep(1);
		
		cout << "writer " << i << " finish writing" << endl;
		sem_post(&g_writer);

		sleep(2);
	}
	
	return nullptr;
}
