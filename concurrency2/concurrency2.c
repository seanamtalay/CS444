/**
CS 444 Concurrency assignment 2
Namtalay Laorattanavech
Dakota Zaengle
**/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
pthread_mutex_t forks[5];

struct Philosopher{
	char name[255];
	int id;
};

void think(struct Philosopher *philosopher){

	int time = rand()%20+1;
	printf("%s is thinking. ", philosopher->name);
	printf("[for %d seconds]\n", time);
	sleep(time);
	
	
}

void eat(struct Philosopher *philosopher){
	int time = rand()%8+2;
	printf("%s is eating. ", philosopher->name);
	printf("[for %d seconds]\n", time);
	sleep(time);
	printf("%s finished eating. ", philosopher->name);
}

void put_down_forks(struct Philosopher *philosopher){
	int fork_right = (int)MIN(philosopher->id,((philosopher->id)+1)%5);
	int fork_left = (int)MAX(philosopher->id,((philosopher->id)+1)%5);
	
	printf("%s puts down the forks.\n", philosopher->name);
	pthread_mutex_unlock(&forks[fork_right]);
	pthread_mutex_unlock(&forks[fork_left]);
}

void take_up_forks(struct Philosopher *philosopher){
	int fork_right = (int)MIN(philosopher->id,((philosopher->id)+1)%5);
	int fork_left = (int)MAX(philosopher->id,((philosopher->id)+1)%5);
	
	printf("%s is trying to get the right fork [fork id: %d]\n", philosopher->name, fork_right);
	pthread_mutex_lock(&forks[fork_right]);
	printf("%s successfully got the right fork [fork id: %d]\n", philosopher->name, fork_right);
	
	printf("%s is trying to get the left fork [fork id: %d]\n", philosopher->name, fork_left);
	pthread_mutex_lock(&forks[fork_left]);
	printf("%s successfully got the right fork [fork id: %d]\n", philosopher->name, fork_left);
	
	printf("%s is now having two forks\n",philosopher->name );

}

void *endless_life_of_philosophers(void *ph){
	struct Philosopher *philosopher = (struct Philosopher *)ph;
	printf("%s has joined the table\n", philosopher->name);
	
	while(1){
		think(philosopher);
		take_up_forks(philosopher);
		eat(philosopher);
		put_down_forks(philosopher);
	}
}	

int main(){
	pthread_t threads[5];
	struct Philosopher *philosopher;
	philosopher = malloc(sizeof(struct Philosopher)*5);
	//char names[] = {"Plato", "Aristotle", "Immanuel Kant", "René Descartes", "Socrates"};
	strcpy(philosopher[0].name, "Plato");
	strcpy(philosopher[1].name, "Aristotle");
	strcpy(philosopher[2].name, "Immanuel Kant");
	strcpy(philosopher[3].name, "René Descartes");
	strcpy(philosopher[4].name, "Socrates");
	int i;
	printf("start loop\n");
	
	for(i=0; i<5; i++){
		philosopher[i].id = i;
		pthread_create(&threads[i], NULL, endless_life_of_philosophers, &philosopher[i]);
	}

	pthread_exit(NULL);

	return 0;
}