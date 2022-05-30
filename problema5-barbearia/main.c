#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define THREADS_BARBERS 3
#define THREADS_CLIENTS 100
#define THREADS_CASHIERS 1

sem_t s ;

void * ThreadControlCouch(){

} 

void * ThreadControlCouch(){
    
} 

void * ThreadControlCouch(){
    
} 

int main (int argc, char *argv[])
{
   pthread_t threadBarbers [THREADS_BARBERS];
   pthread_t threadClients [THREADS_CLIENTS];
   pthread_t threadCashiers [THREADS_CASHIERS];
   pthread_attr_t attr ;
   long i, status ;

   sem_init (&s, 0, 1) ;

   pthread_attr_init (&attr) ;
   pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_JOINABLE);

   // create threads
   for(i=0; i < THREADS_BARBERS; i++)
   {
      status = pthread_create (&threadClients[i], &attr, ThreadControlCouch, (void *) i) ;
      if (status)
      {
         perror ("pthread_create") ;
         exit (1) ;
      }
   }
   
   // wait all threads to finish   
   for (i=0; i < THREADS_BARBERS; i++)
   {
      status = pthread_join (threadBarbers[i], NULL) ;
      if (status)
      {
         perror ("pthread_join") ;
         exit (1) ;
      }
   }

   pthread_attr_destroy (&attr) ;
   pthread_exit (NULL) ;
}