#include "stdio.h"
#include "pthread.h"
#include "semaphore.h"
#include "unistd.h"
#include "stdlib.h"
#include "Queue.h"

sem_t empty;
sem_t full;
pthread_mutex_t mutex;

void *ProducerFunc(void *arg){
    printf("%s\n",(char*)arg);
    int i;
    for(i=1;i<=10;i++)
    {


        sleep(1);
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        //CR
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
        printf("producer produced item %d\n",i);




    }
}

void * ConsumerFunc(void * arg)
{
    printf("%s\n",(char*)arg);
    int i;
    for(i=1;i<=10;i++)
    {


        sleep(1);


        sem_wait(&full);
        pthread_mutex_lock(&mutex);
        int item = i;
        //CR
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
        printf("consumer consumed item %d\n",item);



    }
}





int main(void)
{
    pthread_t thread1;
    pthread_t thread2;
    sem_init(&empty,0,10);
    sem_init(&full,0,0);
    pthread_mutex_init(&mutex,NULL);
    //init_semaphore();

    char * message1 = "i am producer";
    char * message2 = "i am consumer";

    pthread_create(&thread1,NULL,ProducerFunc,(void*)message1 );
    pthread_create(&thread2,NULL,ConsumerFunc,(void*)message2 );

    while(1);
    return 0;
}