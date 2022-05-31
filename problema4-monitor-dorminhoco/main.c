#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <semaphore.h>

typedef struct termios termios;
int on = 1;

// Buffers and mutexes

sem_t cadeiras;
sem_t monitoria;
sem_t estudantes;

void *threadEstudante (void *args){
  if (!sem_trywait(&cadeiras))
  {
    sem_post(&estudantes);
    if (sem_trywait(&monitoria)) {
      printf("Estudante chega e senta para aguardar\n");
      sem_wait(&monitoria);
    }
    sem_post(&cadeiras);
    printf("Estudante entra para receber monitoria\n");
  }
  else
  {
    printf("Não tem cadeiras disponíveis\n");
  }

  pthread_exit(NULL);
}


void *threadMonitor(void *args) {
  while(on) {

    if (!sem_trywait(&estudantes)) {
      sleep(3);
      sem_post(&monitoria);
    } else {
      printf("Monitor está tirando uma sonequinha \n");
      sem_wait(&estudantes);
      sleep(3);
      sem_post(&monitoria);
    }
    printf("Aluno foi atendido e despensado \n");
  }

  pthread_exit(NULL);
}

int main (int argc, char *argv[]){
    /*
    n
    n = numero de cadeiras
    */



    

    if (argc != 2) {
      perror("argument list incorrect");
      exit(EXIT_FAILURE);
    }
    printf("%s %s %s \n", argv[1], argv[2], argv[3]);
    int n = atoi(argv[1]);
    if (n <= 2 && n>=30) {
      perror("n length out of scope");
      exit(EXIT_FAILURE);
    }
    printf("Botao 'e' cria thread estudante e botão 'x' encerra aplicação\n");
    // array de threads
    pthread_t estudante[100];
    pthread_t monitor;

    // cadeiras init
    sem_init(&cadeiras, 0, n);
    sem_init(&estudantes, 0, 0);

    // monitoria init
    sem_init(&monitoria, 0, 1);

  
    // ###########################################
    // Threads initialization
    printf("Inicializando threads \n");

    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_JOINABLE);

    int status;
    status = pthread_create(&monitor, &attr, threadMonitor, NULL);
    if(status){
        perror("Erro criação thread monitor");
        exit(1);
    }


    termios oldt;
    tcgetattr(STDIN_FILENO, &oldt);
    termios newt = oldt;
    newt.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    char um;
    int i = 0;
    while (on)
    {
      um = getchar();
      if (um == 'e') {
        status = pthread_create(&estudante[i], &attr, threadEstudante, NULL);
        if(status){
            perror("Erro criação thread estudante");
            exit(1);
        }
        if (i == 99) {
          i = 0;
        } else {
          i++;
        }
      } else if (um == 'x') {
        on = 0;
      }
    }
    printf("Acaba o momento de monitoria\n");
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    pthread_attr_destroy (&attr);
    exit(0);
}