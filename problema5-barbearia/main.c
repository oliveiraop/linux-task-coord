#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <termios.h>

#define THREADS_BARBERS 3
#define THREADS_CLIENTS 100

typedef struct termios termios;

int on = 1;

// semaforo da capacidade total de clientes
sem_t barbearia;

// semaforo para vagas no sofá
sem_t sofa;

// semaforo para vagas nas cadeiras dos barbeiros
sem_t cadeiras;

// semaforos do corte de cabelo
sem_t fila_corte;
sem_t aguarda_corte;

// semaforos do pagamento
sem_t mutex_fila_pagamento;
sem_t fila_pagamento;
sem_t recibo;

// semaforos relacionados ao funcionamento do caixa
sem_t caixa_mutex;
sem_t caixa_done;
sem_t caixa_init;


void *threadCliente(void* args) {

   long id = (long) args;
   if (!sem_trywait(&barbearia)) {
      printf("Cliente %ld chega na barbearia\n", id);
      sem_wait(&sofa);
      sem_wait(&cadeiras);
      sem_post(&sofa);
      sem_post(&fila_corte);
      sem_wait(&aguarda_corte);


      sem_post(&cadeiras);
      printf("Cliente %ld segue para pagamento\n", id);
      sem_wait(&mutex_fila_pagamento);
      sem_post(&fila_pagamento);
      sem_post(&mutex_fila_pagamento);
      sem_wait(&recibo);
   
      sem_post(&barbearia);
      printf("Cliente %ld vai embora\n", id);
   } else {
      printf("Barbearia cheia\n");
   }
   pthread_exit(NULL);
} 

void *threadBarbeiro(void* args){
   long id = (long) args;
   int qtd_fila_pagamento;
   while(on) {
      if (!sem_trywait(&fila_corte)) {
         printf("Barbeiro %ld fez um corte\n", id);
         sem_post(&aguarda_corte);
      } 
      if (!sem_trywait(&caixa_mutex)) {
         sem_wait(&mutex_fila_pagamento);
         sem_getvalue(&fila_pagamento, &qtd_fila_pagamento);
         sem_post(&mutex_fila_pagamento);
         if (qtd_fila_pagamento > 0) {
            sem_post(&caixa_init);
            printf("Barbeiro %ld está no caixa\n", id);
            sem_wait(&caixa_done);
         }
         sem_post(&caixa_mutex);
      }
      sleep(2);
   }
   pthread_exit(NULL);
} 

void *threadCaixa(){
   while(on) {
      sem_wait(&caixa_init);
      while (!sem_trywait(&fila_pagamento)) {
         sleep(1);

         sem_post(&recibo);
      }
      sem_post(&caixa_done);
   }

   pthread_exit(NULL);
   
} 

int main (int argc, char *argv[])
{


   printf("Botao 'c' cria thread cliente e botão 'x' encerra aplicação\n");

   pthread_t barbeiro[THREADS_BARBERS];
   pthread_t cliente[THREADS_CLIENTS];
   pthread_t caixa;
   pthread_attr_t attr ;
   long i;
   int status ;

   sem_init (&barbearia, 0, 20) ;
   sem_init (&sofa, 0, 4) ;
   sem_init (&cadeiras, 0, 3) ;

   sem_init (&fila_corte, 0, 0) ;
   sem_init (&aguarda_corte, 0, 0) ;

   sem_init (&mutex_fila_pagamento, 0, 1) ;
   sem_init (&fila_pagamento, 0, 0) ;
   sem_init (&recibo, 0, 0) ;

   sem_init (&caixa_mutex, 0, 1) ;
   sem_init (&caixa_init, 0, 0) ;
   sem_init (&caixa_done, 0, 0) ;

   pthread_attr_init (&attr) ;
   pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_JOINABLE);

   // create threads
   for(i=0; i < THREADS_BARBERS; i++)
   {
      status = pthread_create (&barbeiro[i], &attr, threadBarbeiro, (void *) i) ;
      if (status)
      {
         perror ("pthread_create barbeiro") ;
         exit (1) ;
      }
   }

   status = pthread_create (&caixa, &attr, threadCaixa, NULL) ;
      if (status)
      {
         perror ("pthread_create barbeiro") ;
         exit (1) ;
      }
   
   // wait all threads to finish   


   termios oldt;
    tcgetattr(STDIN_FILENO, &oldt);
    termios newt = oldt;
    newt.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    char um;
    i = 0;
    while (on)
    {
      um = getchar();
      if (um == 'c') {
        status = pthread_create(&cliente[i], &attr, threadCliente, (void *) i);
        if(status){
            perror("Cliente thread creation error");
            exit(1);
        }
        i++;
        if (i >= THREADS_CLIENTS -1) {
           i = 0;
        }
      } else if (um == 'x') {
        on = 0;
      }
    }
    printf("Programa barbearia finalizada\n");
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

   pthread_attr_destroy (&attr) ;
   exit(0);
}