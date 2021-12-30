#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "unistd.h"
#include "semaphore.h"
#define NUM_THREADS	2
#define M 5
#define N 3
#define P 5
#define w 2
#define x 4
#define y 4
#define z 1
sem_t kioskFull;
pthread_mutex_t kiosk;
pthread_mutex_t spkiosk;
pthread_mutex_t belt;
pthread_mutex_t boarding;
pthread_mutex_t vipCh;
int TIME=0;
struct Boarding{
    int passenger[M*N*P];
    int currentPassenger;
    sem_t boarding;
};
struct VIPchannel{
    int passengerGoing[M*N*P];
    int passengerWaiting[M*N*P];
    int passengerComingBack[M*N*P];
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
    struct Kiosk specialKiosk;
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
int myBeltPosition(int beltIndex,int passID){
    for(int i=0;i<airport.belts[beltIndex].currentPassenger;i++){
        if(airport.belts[beltIndex].passengers[i]==passID){
            return i;
        }
    }
}

void addPassengerInBoardingLine(int passID){
    airport.boarding.passenger[airport.boarding.currentPassenger]=1;
    airport.boarding.currentPassenger++;
}

void passBoarding(int passID){
    for(int i=0;i<airport.boarding.currentPassenger;i++){
        if(airport.boarding.passenger[i]==passID){
            for(int j=i;j<airport.boarding.currentPassenger-1;j++){
                airport.boarding.passenger[j]=airport.boarding.passenger[j+1];
            }
            airport.boarding.passenger[airport.boarding.currentPassenger-1]=-1;
            airport.boarding.currentPassenger--;
            break;
        }
    }
}

int addVIPChannelComingBack(int passID){
    if(airport.viPchannel.going==0 && airport.viPchannel.waiting==0){
        airport.viPchannel.passengerComingBack[airport.viPchannel.comingBack]=passID;
        airport.viPchannel.comingBack++;
        return 1;
    }
    else{
        return 0;
    }

}

void passComingBack(int passID){
    for(int i=0;i<airport.viPchannel.comingBack;i++){
        if(airport.viPchannel.passengerComingBack[i]==passID){
            for(int j=i;j<airport.viPchannel.comingBack-1;j++){
                airport.viPchannel.passengerComingBack[j]=airport.viPchannel.passengerComingBack[j+1];
            }
            airport.viPchannel.passengerComingBack[airport.viPchannel.comingBack-1]=-1;
            airport.viPchannel.comingBack--;
            break;
        }
    }
}

int addGoing(int passID){
    if(airport.viPchannel.comingBack==0){
        airport.viPchannel.passengerGoing[airport.viPchannel.going]=passID;
        airport.viPchannel.going++;
        return 1;
    }
    else{
        return 0;
    }
}

void passGoing(int passID){
    for(int i=0;i<airport.viPchannel.going;i++){
        if(airport.viPchannel.passengerGoing[i]==passID){
            for(int j=i;j<airport.viPchannel.going-1;j++){
                airport.viPchannel.passengerGoing[j]=airport.viPchannel.passengerGoing[j+1];
            }
            airport.viPchannel.passengerGoing[airport.viPchannel.going-1]=-1;
            airport.viPchannel.going--;
            break;
        }
    }
}

void passSpecialKiosk(int pass){
    airport.specialKiosk.passengers[airport.specialKiosk.currentPassenger-1] = -1;
    //airport.viPchannel.comingBack=0;
    printf("%d\n",airport.viPchannel.comingBack);
    airport.specialKiosk.currentPassenger--;
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

        //KIOSK CHECK-IN

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
        ptime++;


        //SECURITY CHECK(BELT)


        //pthread_mutex_lock(&belt);
        sem_wait(&airport.belts[beltIndex].beltFull);
        printf("Passenger %d has started waiting for security check in belt %d from time %d\n", passID, beltIndex, ptime);
        pthread_mutex_lock(&belt);
        airport.belts[beltIndex].passengers[airport.belts[beltIndex].currentPassenger]=passID;
        ptime+=airport.belts[beltIndex].currentPassenger*x;
        airport.belts[beltIndex].currentPassenger++;

        printf("Passenger %d has started the security check at time %d\n",passID,ptime);
        ptime+=x;

        sleep(x);
        passBelt(beltIndex,passID);

//        printf("Passenger %d is going out from Kiosk %d Belt %d\n", passID, kioskIndex, beltIndex);
        pthread_mutex_unlock(&belt);
        sem_post(&airport.belts[beltIndex].beltFull);
        printf("Passenger %d has crossed the security check at time %d\n",passID,ptime);
//
        //showBelt(kioskIndex,beltIndex);

        //BOARDING
        //sem_wait(&airport.boarding.boarding);
        ptime++;
        printf("Passenger %d has started waiting to be boarded at time %d\n",passID,ptime);
        addPassengerInBoardingLine(passID);
        ptime+=(airport.boarding.currentPassenger-1)*y;
        //pthread_mutex_lock(&boarding);
        if(passID==0) {
            printf("Passenger %d has started boarding the plane at time %d\n", passID, ptime);
            ptime += y;
            //sem_wait(&airport.boarding.boarding);
            pthread_mutex_lock(&boarding);
            passBoarding(passID);
            pthread_mutex_unlock(&boarding);
            //sem_post(&airport.boarding.boarding);
            printf("Passenger %d has boarded the plane at time %d\n", passID, ptime);
        }
        else{
            //sem_wait(&airport.boarding.boarding);
//            pthread_mutex_lock(&boarding);
            printf("Passenger %d has started boarding the plane at time %d\n", passID, ptime);
            printf("Passenger %d has lost his boarding pass.\nNow he has to go back to kiosk via VIP channel at time %d\n",passID,ptime);
            int isComingBack=0;
            while (isComingBack==0){
                if(addVIPChannelComingBack(passID)==1){
                    ptime++;
                    isComingBack=1;
                    printf("Passenger %d is going back to special kiosk via VIP channel at time %d\n",passID,ptime);

                    //sem_post(&airport.boarding.boarding);
                }

            }
           //pthread_mutex_unlock(&boarding);

            //SPECIAL KIOSK
            pthread_mutex_lock(&vipCh);
            passComingBack(passID);
            //printf("%d",airport.viPchannel.comingBack);
            ptime+=z;
            pthread_mutex_unlock(&vipCh);

            sem_wait(&airport.specialKiosk.kioskFull);
            printf("Passenger %d has started self check-in Special Kiosk  at time %d\n", passID,ptime);
            pthread_mutex_lock(&spkiosk);
            airport.specialKiosk.passengers[airport.specialKiosk.currentPassenger] = passID;
            airport.specialKiosk.currentPassenger++;
            //showKiosk(kioskIndex);
            sleep(w);
            ptime+=w;
            pthread_mutex_unlock(&spkiosk);

            printf("Passenger %d has finished self check-in Special Kiosk  at time %d\n", passID,ptime);
//            printf("%d\n",airport.viPchannel.comingBack);
            passSpecialKiosk(passID);
            sem_post(&airport.specialKiosk.kioskFull);
            ptime++;


            //VIP CHANNEL
            airport.viPchannel.waiting++;

            while(1){
                ptime++;
                if(addGoing(passID)==1){
                    printf("Passenger %d is going back to boarding via VIP channel at time %d\n",passID,ptime);
                    airport.viPchannel.waiting--;
                    break;
                }
                //printf("looped");
            }
            passGoing(passID);
            ptime+=z;
            printf("Passenger %d has started waiting to be boarded at time %d\n",passID,ptime);
            addPassengerInBoardingLine(passID);
            ptime+=(airport.boarding.currentPassenger-1)*y;
            printf("%d\n",airport.boarding.currentPassenger);
            //pthread_mutex_lock(&boarding);
                printf("Passenger %d has started boarding the plane at time %d\n", passID, ptime);
                ptime += y;
                //sem_wait(&airport.boarding.boarding);
                pthread_mutex_lock(&boarding);
                passBoarding(passID);
                pthread_mutex_unlock(&boarding);
                //sem_post(&airport.boarding.boarding);
                printf("Passenger %d has boarded the plane at time %d\n", passID, ptime);

        }
//        pthread_mutex_unlock(&boarding);
//        sem_post(&airport.boarding.boarding);

    }



    //FOR VIPs
    else{
        ptime+=7;
        kioskIndex = (random++)%M;
        beltIndex =(random++)%N;
        if(random==55) random=0;

        //KIOSK CHECK-IN

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
        ptime++;

        //VIP CHANNEL


        //BOARDING
        //sem_wait(&airport.boarding.boarding);
        ptime++;
        printf("Passenger %d has started waiting to be boarded at time %d\n",passID,ptime);
        addPassengerInBoardingLine(passID);
        ptime+=(airport.boarding.currentPassenger-1)*y;
        pthread_mutex_lock(&boarding);
        printf("Passenger %d has started boarding the plane at time %d\n",passID,ptime);
        ptime+=y;
        passBoarding(passID);
        printf("Passenger %d has boarded the plane at time %d\n",passID,ptime);
        pthread_mutex_unlock(&boarding);

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
    pthread_mutex_init(&boarding,NULL);
    pthread_mutex_init(&spkiosk,NULL);
    pthread_mutex_init(&vipCh,NULL);
    sem_init(&airport.specialKiosk.kioskFull,0,1);
    for(int i=0;i<M;i++){
            airport.kiosks[i].currentPassenger=0;
            airport.kiosks[i].maximumPassenger=1;
            airport.specialKiosk.currentPassenger=0;
            airport.specialKiosk.maximumPassenger=1;
            sem_init(&airport.kiosks[i].kioskFull,0,1);
            for(int k=0;k<airport.kiosks[i].maximumPassenger;k++){
                airport.kiosks[i].passengers[k]=-1;
                airport.specialKiosk.passengers[k]=-1;
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
    sem_init(&airport.boarding.boarding,0,1);
    airport.boarding.currentPassenger=0;
    for(int i=0;i<M*N*P;i++){
        airport.boarding.passenger[i]=-1;
        airport.viPchannel.passengerGoing[i]=-1;
        airport.viPchannel.passengerComingBack[i]=-1;
        airport.viPchannel.passengerWaiting[i]=-1;
    }
    airport.viPchannel.going=0;
    airport.viPchannel.waiting=0;
    airport.viPchannel.comingBack=0;
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