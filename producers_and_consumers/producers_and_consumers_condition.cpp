#include <iostream>

#include <pthread.h>
#include <unistd.h>

using namespace std;

const int kConsumers = 2;
const int kProducers = 2;

const int kMaxSize = 10;
int g_buffer[kMaxSize];
pthread_t g_threadID[kConsumers + kProducers];

pthread_cond_t g_empty;
pthread_cond_t g_full;
pthread_mutex_t g_mutex;

unsigned int in = 0;
unsigned int out = 0;
unsigned int product_id = 0;

void *Consume(void *arg);
void *Produce(void *arg);

int main(){
	int i = 0;
	//初始化缓冲区
	for(i = 0; i < kMaxSize; ++i)
		g_buffer[i] = -1;

	//初始化条件变量和互斥锁
	pthread_cond_init(&g_empty, NULL);
	pthread_cond_init(&g_full, NULL);
	pthread_mutex_init(&g_mutex, NULL);

	//初始化生产者线程进行生产
	for(i = 0; i < kProducers; ++i)
		pthread_create(&g_threadID[i], NULL, Produce, NULL);
	
	//初始化消费者线程进行消费
	for(i = 0; i < kConsumers; ++i)
		pthread_create(&g_threadID[kProducers + i], NULL, Consume, NULL);

	//等待所有线程的返回，避免出现僵尸子线程
	for(i = 0; i < (kProducers + kConsumers); ++i)
		pthread_join(g_threadID[i], NULL);

	//销毁条件变量和互斥锁
	pthread_cond_destroy(&g_empty);
	pthread_cond_destroy(&g_full);
	pthread_mutex_destroy(&g_mutex);
	return 0;
}

void *Consume(void *arg){
	pthread_t threadID = pthread_self();
	int i = 0;

	while(1){
		pthread_mutex_lock(&g_mutex);

		while((out % kMaxSize) == in){
			cout << "buffer is empty ! consumer is waiting\n";
			pthread_cond_wait(&g_empty, &g_mutex);
		}

		//消费产品前，打印当前缓冲区的情况
		cout << "before consuming, buffer is:\n";
		for(i = 0; i < kMaxSize; ++i)
			cout << g_buffer[i] << "\t";
		cout << endl;

		//取出产品进行消费
		cout << threadID << "	:is consuming product " << g_buffer[out] << endl;
		g_buffer[out] = -1;
		out = (out + 1) % kMaxSize;


		//消费产品后，打印当前缓冲区的情况
		cout << "after consuming, buffer is:\n";
		for(i = 0; i < kMaxSize; ++i)
			cout << g_buffer[i] << "\t";
		cout << endl;

		pthread_cond_signal(&g_full);
		pthread_mutex_unlock(&g_mutex);

		sleep(1);
	}

	return nullptr;
}

void *Produce(void *arg){
	pthread_t threadID = pthread_self();
	int i = 0;

	while(1){
		pthread_mutex_lock(&g_mutex);

		while((in + 1) % kMaxSize == out){
			cout << "buffer is full ! producer is waiting\n";
			pthread_cond_wait(&g_full, &g_mutex);
		}

		//生产产品前，打印当前缓冲区的情况
		cout << "before producing, buffer is:\n";
		for(i = 0; i < kMaxSize; ++i)
			cout << g_buffer[i] << "\t";
		cout << endl;

		//生产产品
		cout << threadID << "	:is producing product " << product_id << " in buffer[" << in << "]\n";
		g_buffer[in] = product_id;
		in = (in + 1) % kMaxSize;
		++product_id;

		//生产产品后，打印当前缓冲区的情况
		cout << "after producing, buffer is:\n";
		for(i = 0; i < kMaxSize; ++i)
			cout << g_buffer[i] << "\t";
		cout << endl;

		pthread_cond_signal(&g_empty);
		pthread_mutex_unlock(&g_mutex);

		sleep(5);
	}

	return nullptr;
}
