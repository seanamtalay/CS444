#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include "mt19937ar.h"

#define BUFFER_SIZE 32

struct Buffer buffer;
//indexes
int index_consumer = 0;
int index_producer = 0;

struct Item{
	int number;
	int wait;
};

struct Buffer{
	struct Item items[BUFFER_SIZE];
	pthread_mutex_t shareLock;
	pthread_cond_t con_cond;
	pthread_cond_t pro_cond;
};

int check_x86(){
	unsigned int eax = 0x01;
		unsigned int ebx;
		unsigned int ecx;
		unsigned int edx;
		__asm__ __volatile__(
	                     "cpuid;"
	                     : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
	                     : "a"(eax)
	                     );
		
		//check if bigger than 32 bits		
		if (ecx & 0x40000000){
			return 1;
		}
		else{
			return 0;
		}
		
}

int getRandNum(int min, int max){
	int num = 0;
	
	//if 32 bits use rdrand
	if(check_x86()==1){
        __asm__ __volatile__("rdrand %0":"=r"(num));
	}
	else{ // else if 64 bits use merelene twister
		num =  (int)genrand_int32();
	}
	
	num = abs(num % (max - min));
	if (num<min){
		return min;
	}
	else{
		return num;
	}
}

void *producer_function(void *foo){
	while(1){
		pthread_mutex_lock(&buffer.shareLock);
		
		struct Item new_item;
		new_item.number = getRandNum(1,100);
		new_item.wait = getRandNum(2,9);
		
		//print new item info
		printf("Producing item: number = %d\n", new_item.number);
		printf(", wait = %d\n", new_item.wait);
		
		//if consumer is at max
		if(index_consumer >= BUFFER_SIZE){
			printf("Buffer is at max\n");
			//signal comsumer
			pthread_cond_signal(&(buffer.con_cond));
			pthread_cond_wait(&(buffer.pro_cond), &buffer.shareLock);	
		}
		
		//then add item to buffer
		buffer.items[index_producer] = new_item;
		index_producer++;
		
		//tell consumer that item is ready
		pthread_cond_signal(&(buffer.con_cond));
		pthread_cond_wait(&(buffer.pro_cond), &buffer.shareLock);	
		
		//resize if buffer is at max
		if(index_producer>=BUFFER_SIZE){
			printf("At max\n");
			index_producer = 0;
		}
		
		pthread_mutex_unlock(&buffer.shareLock);
	}
}

void *consumer_function(void *foo){
	while(1){
		pthread_mutex_lock(&buffer.shareLock);
		struct Item consume_item;
		
		if(index_consumer>=BUFFER_SIZE){
			index_consumer = 0;
		}
		
		//tell producer to that comsumer is ready to consume
		pthread_cond_signal(&(buffer.pro_cond));
		pthread_cond_wait(&(buffer.con_cond), &buffer.shareLock);	
		
		//if the buffer is empty
		if(index_producer==0){
			printf("At max\n");
			pthread_cond_wait(&(buffer.con_cond), &buffer.shareLock);	
		}
		
		//CoNsUmEeeee
		consume_item = buffer.items[index_consumer];
		sleep(consume_item.wait);
		printf("Producing item: number = %d", consume_item.number);
		//printf(", wait = %d\n", consume_item.wait);
		
		index_consumer++;
		pthread_mutex_unlock(&buffer.shareLock);
		
	}
	
}

int main(int argc, char *argv[]){
	if(argc != 2){
		printf("Nope, try execute the program like: %s <number of threads>\n", argv[0]);
        exit(0);
	}
	
	int i;
	int num_threads = atoi(argv[1]);
	int total = 2*num_threads;
	pthread_t threads[total];
	
	//create
	for(i=0; i<num_threads; i++){
		pthread_create(&threads[i], NULL, consumer_function, NULL);
        pthread_create(&threads[i + 1], NULL, producer_function, NULL);
    
	}
	
	//join
	for(i=0;i<total;i++){
		pthread_join(threads[i], NULL);
	}
	return 0;
}