#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "unistd.h"
#include "semaphore.h"
#define NUM_THREADS	16
#define M 2
#define N 3
#define P 5
#define w 4
#define x 4
#define y 4
#define z 4
sem_t kioskFull;
pthread_mutex_t kiosk;
pthread_mutex_t belt;
int TIME=0;
struct Boarding{
    int passenger[M*N*P];
    int currentPassenger;
};
struct VIPchannel{
    int passengerGoing[M*N*P];
    int passengerWaiting[M*N*P];
    int PassengerComingBack[M*N*P];
    int going;
    int waiting;
    int comingBack;
};
struct passenger{
    int id;
    int isVIP;
    int time;
};
struct Belt{
    int passengers[P];
    int currentPassenger;
    int maximumPassenger;
    sem_t beltFull;

};
struct Kiosk{
    //struct Belt belts[N];
    int passengers[1];
    int currentPassenger;
    int maximumPassenger;
    sem_t kioskFull;
};

struct Airport{
    struct Kiosk kiosks[M];
    struct Belt belts[N];
    struct Boarding boarding;
    struct VIPchannel viPchannel;
};
struct Airport airport;
struct passenger passengerArray[NUM_THREADS];
//struct Kiosk kiosks[M];

int isKioskFull(int kioskIndex,int beltIndex){
    if(airport.kiosks[kioskIndex].currentPassenger==P){
        return 1;
    } else{
        return 0;
    }
}

void showKiosk(int kioskIndex){
    for(int i=0;i<airport.kiosks[kioskIndex].currentPassenger;i++){
        printf("%d\n",airport.kiosks[kioskIndex].passengers[i]);
    }
}

void showBelt(int beltIndex){
    for(int i=0;i<airport.belts[beltIndex].currentPassenger;i++){
        printf("%d\n",airport.belts[beltIndex].passengers[i]);
    }
}

void passKiosk(int kioskIndex,int passID){
    for(int i=0;i<airport.kiosks[kioskIndex].currentPassenger;i++){
        if(airport.kiosks[kioskIndex].passengers[i]==passID){
            for(int j=i;j<airport.kiosks[kioskIndex].currentPassenger-1;j++){
                airport.kiosks[kioskIndex].passengers[j]=airport.kiosks[kioskIndex].passengers[j+1];
            }
            airport.kiosks[kioskIndex].passengers[airport.kiosks[kioskIndex].currentPassenger-1]=-1;
            airport.kiosks[kioskIndex].currentPassenger--;
            break;
        }
    }
}

void passBelt(int beltIndex,int passID){
    for(int i=0;i<airport.belts[beltIndex].currentPassenger;i++){
        if(airport.belts[beltIndex].passengers[i]==passID){
            for(int j=i;j<airport.belts[beltIndex].currentPassenger-1;j++){
                airport.belts[beltIndex].passengers[j]=airport.belts[beltIndex].passengers[j+1];
            }
            airport.belts[beltIndex].passengers[airport.belts[beltIndex].currentPassenger-1]=-1;
            airport.belts[beltIndex].currentPassenger--;
            break;
        }
    }
}

int random=0;
void *Process(void *threadarg) {
    int passID,vip,ptime;
    struct passenger *passengerStatus;
    passengerStatus=(struct passenger_status *)threadarg;
    passID=passengerStatus->id;
    vip=passengerStatus->isVIP;
    ptime=passengerStatus->time;
    //printf("id=%d \nvip=%d \n",passID,vip);
    //pthread_mutex_lock(&mutex);
    int kioskIndex,beltIndex;
    if(vip==0) {
        ptime+=7;
        kioskIndex = (random++)%M;
        beltIndex =(random++)%N;
        if(random==55) random=0;
        sem_wait(&airport.kiosks[kioskIndex].kioskFull);
        printf("Passenger %d has started self check-in Kiosk %d at time %d\n", passID, kioskIndex,ptime);
        pthread_mutex_lock(&kiosk);
        airport.kiosks[kioskIndex].passengers[airport.kiosks[kioskIndex].currentPassenger] = passID;
        airport.kiosks[kioskIndex].currentPassenger++;
        //showKiosk(kioskIndex);
        sleep(w);
        ptime+=w;
        pthread_mutex_unlock(&kiosk);
        printf("Passenger %d has finished self check-in Kiosk %d at time %d\n", passID, kioskIndex,ptime);
        passKiosk(kioskIndex,passID);
        sem_post(&airport.kiosks[kioskIndex].kioskFull);
//        pthread_mutex_lock(&belt);
//        printf("Passenger %d is in the security check of Kiosk %d Belt %d\n", passID, kioskIndex, beltIndex);
//        sleep(x);
//        //passBelt(kioskIndex,beltIndex,passID);
//        printf("Passenger %d is going out from Kiosk %d Belt %d\n", passID, kioskIndex, beltIndex);
//        pthread_mutex_unlock(&belt);
        //sem_post(&airport.kiosks[kioskIndex].belts[beltIndex].kioskFull);

        //showBelt(kioskIndex,beltIndex);


    }
    //pthread_mutex_unlock(&mutex);
    //printf("%d\n",index);

    pthread_exit(NULL);

}
int main (int argc, char *argv[]) {
    pthread_t passengers[NUM_THREADS];
    int rc;
    pthread_mutex_init(&kiosk,NULL);
    pthread_mutex_init(&belt,NULL);

    for(int i=0;i<M;i++){
            airport.kiosks[i].currentPassenger=0;
            airport.kiosks[i].maximumPassenger=1;
            sem_init(&airport.kiosks[i].kioskFull,0,1);
            for(int k=0;k<airport.kiosks[i].maximumPassenger;k++){
                airport.kiosks[i].passengers[k]=-1;
                //printf("Kiosk %d belt %d passenger number %d\n",i,j,k);
            }
    }
    for(int i=0;i<N;i++){
        airport.belts[i].currentPassenger=0;
        airport.belts[i].maximumPassenger=P;
        sem_init(&airport.belts[i].beltFull,0,P);
        for(int k=0;k<P;k++){
            airport.belts[i].passengers[k]=-1;
            //printf("Kiosk %d belt %d passenger number %d\n",i,j,k);
        }
    }
    //sem_init(&kioskFull,0,0);
    for(int t=0; t<NUM_THREADS; t++){
        passengerArray[t].id=t;
        if(t%2==0){
            passengerArray[t].isVIP=1;
        } else{
            passengerArray[t].isVIP=0;
        }
        passengerArray[t].time=TIME;
        TIME+=2;
        printf("Passenger %d has arrived at the airport at time %d\n",t,passengerArray[t].time);
        rc= pthread_create(&passengers[t],NULL,Process,(void *)&passengerArray[t]);
        if(rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }
    pthread_exit(NULL);
    return  0;
}