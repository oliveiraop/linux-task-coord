#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <semaphore.h>

typedef struct termios termios;
int on = 1;

// Buffers and mutexes

typedef struct {
  int number;
  pthread_mutex_t mutex;
  sem_t itens;
  sem_t vagas;
} buffer_t;

void insere(buffer_t* buffer, int item) {
  sem_wait(&(buffer->vagas));
  pthread_mutex_lock(&(buffer->mutex));
  sem_post(&(buffer->itens));
  printf("Item foi inserido do buffer %d\n", buffer->number);
  pthread_mutex_unlock(&(buffer->mutex));
}

void retira(buffer_t* buffer, int item) {
  sem_wait(&(buffer->itens));
  pthread_mutex_lock(&(buffer->mutex));
  sem_post(&(buffer->vagas));
  printf("Item foi retirado do buffer %d\n", buffer->number);
  pthread_mutex_unlock(&(buffer->mutex));
}

void *threadX (void *args){

  buffer_t *buffer1 = (buffer_t *) args;
  printf("XRecebi o buffer %d\n", buffer1->number);

  while (1){
      insere(buffer1, 1);
      sleep(3);
  }
  pthread_exit(NULL);
}

void *threadY (void *args){

  buffer_t **buffers;
  buffers = (buffer_t **) args;
  printf("YRecebi o buffer %d e o %d\n", buffers[0]->number, buffers[1]->number);

  while (1){
      retira(buffers[0], 1);
      sleep(1);
      insere(buffers[1], 1);
      sleep(3);
  }



  pthread_exit(NULL);

}

void *threadZ (void *args){
  buffer_t *buffer2 = (buffer_t *) args;
  printf("ZRecebi o buffer %d\n", buffer2->number);

  while (1){
      retira(buffer2, 1);
      sleep(3);
  }

  pthread_exit(NULL);

}

void *threadPrint(void *args) {
  while(1) {
    printf("Teste: %c\n", um);
    sleep(3);
  }
  
  pthread_exit(NULL);
}

int main (int argc, char *argv[]){
    /*
    X Y Z N1 N2
    */



    

    if (argc != 6) {
      perror("argument list incorrect");
      exit(EXIT_FAILURE);
    }
    printf("%s %s %s \n", argv[1], argv[2], argv[3]);
    int x_size = atoi(argv[1]);
    int y_size = atoi(argv[2]);
    int z_size = atoi(argv[3]);
    int b1_size = atoi(argv[4]);
    int b2_size = atoi(argv[5]);
    if (x_size <= 0 && x_size>=30) {
      perror("X length out of scope");
      exit(EXIT_FAILURE);
    }
    if (y_size <= 0 && y_size>=30) {
      perror("Y length out of scope");
      exit(EXIT_FAILURE);
    }
    if (z_size <= 0 && z_size>=30) {
      perror("Z length out of scope");
      exit(EXIT_FAILURE);
    }
    if (b1_size <= 0 && b1_size>=30) {
      perror("B1 length out of scope");
      exit(EXIT_FAILURE);
    }
    if (b2_size <= 0 && b2_size>=30) {
      perror("B2 length out of scope");
      exit(EXIT_FAILURE);
    }
    // array de threads
    pthread_t *X;
    pthread_t *Y;
    pthread_t *Z;
    X = (pthread_t *) malloc(x_size * sizeof(pthread_t));
    Y = (pthread_t *) malloc(y_size * sizeof(pthread_t));
    Z = (pthread_t *) malloc(z_size * sizeof(pthread_t));
    // buffers
    buffer_t buffer1;
    buffer_t buffer2;
    

      if (pthread_mutex_init(&buffer1.mutex, NULL) != 0)
      {
        perror("Error on mutex 1 creation");
        exit(EXIT_FAILURE);
      }
    if (pthread_mutex_init(&buffer2.mutex, NULL) != 0) {
      perror("Error on mutex 2 creation");
      exit(EXIT_FAILURE);
    }

    buffer1.number = 1;
    buffer2.number = 2;
    // itens init
    sem_init(&buffer1.itens, 0, 0);
    sem_init(&buffer2.itens, 0, 0);

    // vagas init
    sem_init(&buffer1.vagas, 0, b1_size);
    sem_init(&buffer2.vagas, 0, b2_size);

  
    // ###########################################
    // Threads initialization
    printf("Inicializando threads \n");

    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_JOINABLE);

    buffer_t* buffers[2];
    buffers[0] = &buffer1;
    buffers[1] = &buffer2;
    int status;
    int i;
    for (i = 0; i < x_size; i++) {
      status = pthread_create(&X[i], &attr, threadX, (void *) &buffer1);
        if(status){
            perror("pthread_create");
            exit(1);
        }
    }
    printf("Threads X criadas \n");
    for (i = 0; i < y_size; i++)
    {
      status = pthread_create(&Y[i], &attr, threadY, (void *) buffers);
        if(status){
            perror("pthread_create");
            exit(1);
        }
    }
    printf("Threads Y criadas \n");
    for (i = 0; i < z_size; i++) {
      status = pthread_create(&Z[i], &attr, threadZ, (void *) &buffer2);
        if(status){
            perror("pthread_create");
            exit(1);
        }
    }
    printf("Threads Z criadas \n");

    termios oldt;
    tcgetattr(STDIN_FILENO, &oldt);
    termios newt = oldt;
    newt.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    while (1) {
      um = getchar();
      sleep(0.1);
    }

    pthread_attr_destroy (&attr);
    pthread_exit(NULL);
}