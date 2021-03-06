#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdbool.h>

#define max 3

pthread_t joao;
pthread_t pedro;
pthread_t paulo;

pthread_mutex_t mutex;

sem_t pa;

int buraco = 0;
int vazios = 0;
int afechar = 0;

void *pjoao(){
	
	while(buraco < max){
	
		sem_wait(&pa);
		pthread_mutex_lock(&mutex);
		buraco++;
		afechar++;
		vazios++;
		pthread_mutex_unlock(&mutex);
		sem_post(&pa);
		
		exit(1);
	}
	pthread_exit(NULL);
}

void *ppedro(){
		
	while(buraco > 0){
	
		sem_wait(&pa);
		pthread_mutex_lock(&mutex);
		buraco++;
		afechar++;
		vazios++;
		pthread_mutex_unlock(&mutex);
		sem_post(&pa);
		
		exit(1);
	}
	pthread_exit(NULL);
}

void *ppaulo(){
	
	while(afechar > 0 && buraco < max){
	
		sem_wait(&pa);
		pthread_mutex_lock(&mutex);
		buraco--;
		afechar--;
		pthread_mutex_unlock(&mutex);
		sem_post(&pa);
		
		exit(1);
	}
}

int main(){

	sem_init(&pa,0,1);
	pthread_t t1,t2,t3;
	int id;
	pthread_attr_t attr; 
	
	pthread_attr_init(&attr);
    pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_JOINABLE);
	
	pthread_mutex_init(&mutex, NULL);
	
	id = 1;
	pthread_create(&t1, &attr, pjoao, NULL);
	
	id = 2;
	pthread_create(&t2, &attr, ppedro, NULL);
	
	id = 3;
	pthread_create(&t3, &attr, ppaulo, NULL);
	
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
    
    sem_destroy(&pa);
    
   	pthread_mutex_destroy(&mutex);
    
    printf("rodou");
    
    return 0;
}


