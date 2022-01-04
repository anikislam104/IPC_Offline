#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "unistd.h"
#include "semaphore.h"
#include "time.h"
#include "math.h"
#define NUM_THREADS	15
//#define M 5
//#define N 3
//#define P 5
//#define w 2
//#define x 4
//#define y 4
//#define z 1
#define whomToPrint 1
clock_t start,end;
int M,N,P,w,x,y,z;
//sem_t kioskFull;
FILE *fp;
pthread_mutex_t kiosk;
pthread_mutex_t spkiosk;
pthread_mutex_t belt;
pthread_mutex_t boarding;
pthread_mutex_t vipCh;
int TIME=0;
struct Boarding{
    int passenger[10000];
    int currentPassenger;
    sem_t boarding;
    pthread_mutex_t mutex;
};
struct VIPchannel{
    int passengerGoing[10000];
    int passengerWaiting[10000];
    int passengerComingBack[10000];
    int going;
    int waiting;
    int comingBack;
    pthread_mutex_t mutex;
};
struct passenger{
    int id;
    int isVIP;
    int time;
    int lossID;
    //pthread_mutex_t mutex;
};
struct Belt{
    int passengers[10000];
    int currentPassenger;
    int maximumPassenger;
    sem_t beltFull;
    pthread_mutex_t mutex;

};
struct Kiosk{
    //struct Belt belts[N];
    int passengers[10000];
    int currentPassenger;
    int maximumPassenger;
    sem_t kioskFull;
    pthread_mutex_t mutex;
};

struct Airport{
    struct Kiosk *kiosks;
    struct Belt *belts;
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
        fprintf(fp,"%d\n\n",airport.kiosks[kioskIndex].passengers[i]);
    }
}

void showSpecialKiosk(){
    for(int i=0;i<airport.specialKiosk.currentPassenger;i++){
        fprintf(fp,"%d\n\n",airport.specialKiosk.passengers[i]);
    }
}

void showBelt(int beltIndex){
    for(int i=0;i<airport.belts[beltIndex].currentPassenger;i++){
        fprintf(fp,"%d\n\n",airport.belts[beltIndex].passengers[i]);
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
    airport.boarding.passenger[airport.boarding.currentPassenger]=passID;
    airport.boarding.currentPassenger++;
}

void passBoarding(int passID){
    for(int i=0;i<airport.boarding.currentPassenger;i++){
        if(airport.boarding.passenger[i]==passID){
            for(int j=i;j<airport.boarding.currentPassenger-1;j++){
                airport.boarding.passenger[j]=airport.boarding.passenger[j+1];
            }
            airport.boarding.passenger[airport.boarding.currentPassenger-1]=-1;
            //fprintf(fp,"what %d\n\n",airport.boarding.currentPassenger);
            airport.boarding.currentPassenger--;
            //fprintf(fp,"if %d\n\n",airport.boarding.currentPassenger);
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

int getTime(clock_t start,clock_t end){
    double t=((double )end-(double )start)/CLOCKS_PER_SEC;
    t= round(t);
    int t1=(int)t;
    //fprintf(fp,"time %d",t1);
    return t1;
}
int t;
void passSpecialKiosk(int pass){
    airport.specialKiosk.passengers[airport.specialKiosk.currentPassenger-1] = -1;
    //airport.viPchannel.comingBack=0;
    //fprintf(fp,"%d\n\n",airport.viPchannel.comingBack);
    airport.specialKiosk.currentPassenger--;
}
//int peopleKiosk;
int random=0;

double U_Random();

int possion()  /* 产生一个泊松分布的随机数，Lamda为总体平均数*/
{
    double Lambda = 2, k = 0;
    long double p = 1.0;
    long double l=exp(-Lambda);  /* 为了精度，才定义为long double的，exp(-Lambda)是接近0的小数*/
    while (p>=l)
    {
        double u = U_Random();
        p *= u;
        k++;
    }
    return k-1;
}

double U_Random()  /* 产生一个0~1之间的随机数 */
{
    static int done = 0;
    int number;
    if(!done)  /*srand种子只产生一次*/
    {
        srand((int)time(0));
        done = 1;
    }
    number=1+(int)(100.0*rand()/(RAND_MAX+1.0));
    return number/100.0;
}

int getPDR(){
    double u = U_Random ();
    int p = possion ();

    //printf("%f\n", u);
    //printf("%d\n", p);
    return p;
}


void *Process(void *threadarg) {
    int passID,vip,ptime,lid;
    struct passenger *passengerStatus;
    passengerStatus=(struct passenger_status *)threadarg;
    passID=passengerStatus->id;
    vip=passengerStatus->isVIP;
    ptime=passengerStatus->time;
    lid=passengerStatus->lossID;
    //fprintf(fp,"id=%d \n\nvip=%d \n\n",passID,vip);
    //pthread_mutex_lock(&mutex);
    int kioskIndex,beltIndex;





    if(vip==0) {
        //ptime+=7;
        kioskIndex = (random++)%M;
        beltIndex =(random++)%N;
        if(random==55) random=0;

        ///KIOSK CHECK-IN





        airport.kiosks[kioskIndex].passengers[airport.kiosks[kioskIndex].currentPassenger] = passID;
        airport.kiosks[kioskIndex].currentPassenger++;
        clock_t s=clock();
        sem_wait(&airport.kiosks[kioskIndex].kioskFull);
        pthread_mutex_lock(&airport.kiosks[kioskIndex].mutex);
        clock_t e=clock();
        double td=((double )e-(double )s)/CLOCKS_PER_SEC;
        td= round(td);
        t=(int)td;
        ptime+=t;
        fprintf(fp,"Passenger %d has started self check-in Kiosk %d at time %d\n\n", passID, kioskIndex,ptime);
        sleep(w);
        ptime+=w;
        pthread_mutex_unlock(&airport.kiosks[kioskIndex].mutex);
        fprintf(fp,"Passenger %d has finished self check-in Kiosk %d at time %d\n\n", passID, kioskIndex,ptime);
        passKiosk(kioskIndex,passID);
        sem_post(&airport.kiosks[kioskIndex].kioskFull);





        ///SECURITY CHECK(BELT)



        fprintf(fp,"Passenger %d has started waiting for security check in belt %d from time %d\n\n", passID, beltIndex, ptime);
        clock_t s1=clock();
        sem_wait(&airport.belts[beltIndex].beltFull);
        airport.belts[beltIndex].passengers[airport.belts[beltIndex].currentPassenger]=passID;
        airport.belts[beltIndex].currentPassenger++;
        clock_t e1=clock();
        double t1=((double )e1-(double )s1)/CLOCKS_PER_SEC;
        t1= round(t1);
        t=(int)t1;
        ptime+=t;
        fprintf(fp,"Passenger %d has started the security check at time %d\n\n",passID,ptime);
        ptime+=x;
        sleep(x);
        passBelt(beltIndex,passID);
        sem_post(&airport.belts[beltIndex].beltFull);
        fprintf(fp,"Passenger %d has crossed the security check at time %d\n\n",passID,ptime);




        ///BOARDING




        int hasLost=0;

                    ///loop

        while (hasLost==0) {
            fprintf(fp, "Passenger %d has started waiting to be boarded at time %d\n\n", passID, ptime);
            addPassengerInBoardingLine(passID);
            if (lid != 1) {
                clock_t s2 = clock();
                pthread_mutex_lock(&airport.boarding.mutex);
                clock_t e2 = clock();
                double t2 = ((double) e2 - (double) s2) / CLOCKS_PER_SEC;
                t2 = round(t2);
                t = (int) t2;
                ptime += t;
                fprintf(fp, "Passenger %d has started boarding the plane at time %d\n\n", passID, ptime);
                sleep(y);
                ptime += y;
                pthread_mutex_unlock(&airport.boarding.mutex);
                passBoarding(passID);
                fprintf(fp, "Passenger %d has boarded the plane at time %d\n\n", passID, ptime);
                hasLost++;
            } else {
                clock_t s2 = clock();
                pthread_mutex_lock(&airport.boarding.mutex);
                clock_t e2 = clock();
                double t2 = ((double) e2 - (double) s2) / CLOCKS_PER_SEC;
                t2 = round(t2);
                t = (int) t2;
                ptime += t;
                passBoarding(passID);
                pthread_mutex_unlock(&airport.boarding.mutex);
                fprintf(fp, "Passenger %d has started boarding the plane at time %d\n\n", passID, ptime);
                fprintf(fp,
                        "Passenger %d has lost his boarding pass.\n\nNow he has to go back to kiosk via VIP channel at time %d\n\n",
                        passID, ptime);
                int isComingBack = 0;
                clock_t s3 = clock();
                while (isComingBack == 0) {
                    if (addVIPChannelComingBack(passID) == 1) {
                        isComingBack = 1;
                    }
                }
                clock_t e3 = clock();
                double t3 = ((double) e3 - (double) s3) / CLOCKS_PER_SEC;
                t3 = round(t3);
                t = (int) t3;
                ptime += t;
                fprintf(fp, "Passenger %d is going back to special kiosk via VIP channel at time %d\n\n", passID,
                        ptime);





                ///VIP CHANNEL




                ptime += z;
                sleep(z);
                passComingBack(passID);



                ///SPECIAL KIOSK


                fprintf(fp, "Passenger %d has reached Special Kiosk  at time %d\n\n", passID, ptime);
                airport.specialKiosk.passengers[airport.specialKiosk.currentPassenger] = passID;
                airport.specialKiosk.currentPassenger++;
                clock_t s4 = clock();
                pthread_mutex_lock(&airport.specialKiosk.mutex);
                clock_t e4 = clock();
                double t4 = ((double) e4 - (double) s4) / CLOCKS_PER_SEC;
                t4 = round(t4);
                t = (int) t4;
                ptime += t;
                fprintf(fp, "Passenger %d has started self check-in Special Kiosk  at time %d\n\n", passID, ptime);
                sleep(w);
                ptime += w;
                pthread_mutex_unlock(&airport.specialKiosk.mutex);
                fprintf(fp, "Passenger %d has finished self check-in Special Kiosk  at time %d\n\n", passID, ptime);
                passSpecialKiosk(passID);



                ///VIP CHANNEL



                airport.viPchannel.waiting++;
                clock_t s5 = clock();
                while (1) {
                    if (addGoing(passID) == 1) {
                        fprintf(fp, "Passenger %d is going back to boarding via VIP channel at time %d\n\n", passID,
                                ptime);
                        airport.viPchannel.waiting--;
                        break;
                    }
                }
                clock_t e5 = clock();
                double t5 = ((double) e5 - (double) s5) / CLOCKS_PER_SEC;
                t5 = round(t5);
                t = (int) t5;
                ptime += t;
                sleep(z);
                ptime += z;
                passGoing(passID);
                lid=rand()%1000;
                //printf("%d\n",lid);
            }
        }



    }



        ///FOR VIPs  FOR VIPs  FOR VIPs  FOR VIPs  FOR VIPs  FOR VIPs



    else{


        kioskIndex = (random++)%M;
        beltIndex =(random++)%N;
        if(random==55) random=0;

        ///KIOSK CHECK-IN



        airport.kiosks[kioskIndex].passengers[airport.kiosks[kioskIndex].currentPassenger] = passID;
        airport.kiosks[kioskIndex].currentPassenger++;
        clock_t s=clock();
        sem_wait(&airport.kiosks[kioskIndex].kioskFull);
        pthread_mutex_lock(&airport.kiosks[kioskIndex].mutex);
        clock_t e=clock();
        double td=((double )e-(double )s)/CLOCKS_PER_SEC;
        td= round(td);
        t=(int)td;
        ptime+=t;
        fprintf(fp,"Passenger %d (VIP) has started self check-in Kiosk %d at time %d\n\n", passID, kioskIndex,ptime);
        sleep(w);
        ptime+=w;
        pthread_mutex_unlock(&airport.kiosks[kioskIndex].mutex);
        fprintf(fp,"Passenger %d (VIP) has finished self check-in Kiosk %d at time %d\n\n", passID, kioskIndex,ptime);
        passKiosk(kioskIndex,passID);
        sem_post(&airport.kiosks[kioskIndex].kioskFull);



        ///VIP CHANNEL


        airport.viPchannel.waiting++;
        clock_t s7=clock();
        while(1){
            if(addGoing(passID)==1){
                fprintf(fp,"Passenger %d (VIP) is going  to boarding via VIP channel at time %d\n\n",passID,ptime);
                airport.viPchannel.waiting--;
                break;
            }
        }
        clock_t e7=clock();
        double t7=((double )e7-(double )s7)/CLOCKS_PER_SEC;
        t7= round(t7);
        t=(int)t7;
        ptime+=t;
        sleep(z);
        ptime+=z;
        passGoing(passID);




        ///BOARDING




        int hasLost=0;
        while(hasLost==0) {
            fprintf(fp, "Passenger %d (VIP) has started waiting to be boarded at time %d\n\n", passID, ptime);
            addPassengerInBoardingLine(passID);
            if (lid!=0) {
                clock_t s2 = clock();
                pthread_mutex_lock(&airport.boarding.mutex);
                clock_t e2 = clock();
                double t2 = ((double) e2 - (double) s2) / CLOCKS_PER_SEC;
                t2 = round(t2);
                t = (int) t2;
                ptime += t;
                fprintf(fp, "Passenger %d (VIP) has started boarding the plane at time %d\n\n", passID, ptime);
                sleep(y);
                ptime += y;
                pthread_mutex_unlock(&airport.boarding.mutex);
                passBoarding(passID);
                fprintf(fp, "Passenger %d (VIP) has boarded the plane at time %d\n\n", passID, ptime);
                hasLost++;
            } else {
                clock_t s2 = clock();
                pthread_mutex_lock(&airport.boarding.mutex);
                clock_t e2 = clock();
                double t2 = ((double) e2 - (double) s2) / CLOCKS_PER_SEC;
                t2 = round(t2);
                t = (int) t2;
                ptime += t;
                passBoarding(passID);
                pthread_mutex_unlock(&airport.boarding.mutex);
                fprintf(fp, "Passenger %d (VIP) has started boarding the plane at time %d\n\n", passID, ptime);
                fprintf(fp,
                        "Passenger %d (VIP) has lost his boarding pass.\n\nNow he has to go back to kiosk via VIP channel at time %d\n\n",
                        passID, ptime);
                int isComingBack = 0;
                clock_t s3 = clock();
                while (isComingBack == 0) {
                    if (addVIPChannelComingBack(passID) == 1) {
                        isComingBack = 1;
                    }
                }
                clock_t e3 = clock();
                double t3 = ((double) e3 - (double) s3) / CLOCKS_PER_SEC;
                t3 = round(t3);
                t = (int) t3;
                ptime += t;
                //printf("%d\n",t);
                fprintf(fp, "Passenger %d (VIP) is going back to special kiosk via VIP channel at time %d\n\n", passID,
                        ptime);



                ///VIP CHANNEL





                ptime += z;
                sleep(z);
                passComingBack(passID);


                ///SPECIAL KIOSK




                fprintf(fp, "Passenger %d (VIP) has reached Special Kiosk  at time %d\n\n", passID, ptime);
                airport.specialKiosk.passengers[airport.specialKiosk.currentPassenger] = passID;
                airport.specialKiosk.currentPassenger++;
                clock_t s4 = clock();
                pthread_mutex_lock(&airport.specialKiosk.mutex);
                clock_t e4 = clock();
                double t4 = ((double) e4 - (double) s4) / CLOCKS_PER_SEC;
                t4 = round(t4);
                t = (int) t4;
                ptime += t;
                fprintf(fp, "Passenger %d (VIP) has started self check-in Special Kiosk  at time %d\n\n", passID, ptime);
                clock_t s8 = clock();
                sleep(w);
                clock_t e8 = clock();
                double t8 = ((double) e8 - (double) s8) / CLOCKS_PER_SEC;
                t8 = round(t8);
                t = (int) t8;
                ptime += w;
                pthread_mutex_unlock(&airport.specialKiosk.mutex);
                fprintf(fp, "Passenger %d (VIP) has finished self check-in Special Kiosk  at time %d\n\n", passID, ptime);
                passSpecialKiosk(passID);
//



                ///VIP CHANNEL



                airport.viPchannel.waiting++;
                clock_t s5 = clock();
                while (1) {
                    if (addGoing(passID) == 1) {
                        fprintf(fp, "Passenger %d (VIP) is going back to boarding via VIP channel at time %d\n\n", passID,
                                ptime);
                        airport.viPchannel.waiting--;
                        break;
                    }
                }
                clock_t e5 = clock();
                double t5 = ((double) e5 - (double) s5) / CLOCKS_PER_SEC;
                t5 = round(t5);
                t = (int) t5;
                ptime += t;
                sleep(z);
                ptime += z;
                passGoing(passID);
                lid=rand()%100;
                //printf("%d\n",lid);
            }
        }

    }


    pthread_exit(NULL);

}
int main()
{

    FILE *fptr;

    if ((fptr = fopen("D:/L3T2/Sessional/314 OS/Offline2/input.txt","r")) == NULL){
        printf("Error! opening file");
        exit(1);
    }

    fscanf(fptr,"%d", &M);
    fscanf(fptr,"%d", &N);
    fscanf(fptr,"%d", &P);
    fscanf(fptr,"%d", &w);
    fscanf(fptr,"%d", &x);
    fscanf(fptr,"%d", &y);
    fscanf(fptr,"%d", &z);
    fp = fopen("D:/L3T2/Sessional/314 OS/Offline2/output.txt", "w");



    fclose(fptr);
    pthread_t passengers[NUM_THREADS];
    int rc;
    pthread_mutex_init(&kiosk,NULL);
    pthread_mutex_init(&belt,NULL);
    pthread_mutex_init(&boarding,NULL);
    pthread_mutex_init(&spkiosk,NULL);
    pthread_mutex_init(&vipCh,NULL);
    airport.kiosks=(struct Kiosk *) malloc(M* sizeof(struct Kiosk));
    airport.belts=(struct Belt *) malloc(N* sizeof(struct Belt));
    sem_init(&airport.specialKiosk.kioskFull,0,1);
    pthread_mutex_init(&airport.specialKiosk.mutex,NULL);
    for(int i=0;i<M;i++){
        airport.kiosks[i].currentPassenger=0;
        airport.kiosks[i].maximumPassenger=10000;
        airport.specialKiosk.currentPassenger=0;
        airport.specialKiosk.maximumPassenger=10000;
        pthread_mutex_init(&airport.kiosks[i].mutex,NULL);
        sem_init(&airport.kiosks[i].kioskFull,0,1);
        for(int k=0;k<1;k++){
            airport.kiosks[i].passengers[k]=-1;
            airport.specialKiosk.passengers[k]=-1;
        }
    }
    for(int i=0;i<N;i++){
        airport.belts[i].currentPassenger=0;
        airport.belts[i].maximumPassenger=P;
        sem_init(&airport.belts[i].beltFull,0,P);
        pthread_mutex_init(&airport.belts[i].mutex,NULL);
        for(int k=0;k<P;k++){
            airport.belts[i].passengers[k]=-1;
        }
    }
    sem_init(&airport.boarding.boarding,0,1);
    airport.boarding.currentPassenger=0;
    pthread_mutex_init(&airport.boarding.mutex,NULL);
    for(int i=0;i<M*N*P;i++){
        airport.boarding.passenger[i]=-1;
        airport.viPchannel.passengerGoing[i]=-1;
        airport.viPchannel.passengerComingBack[i]=-1;
        airport.viPchannel.passengerWaiting[i]=-1;
    }
    pthread_mutex_init(&airport.viPchannel.mutex,NULL);
    airport.viPchannel.going=0;
    airport.viPchannel.waiting=0;
    airport.viPchannel.comingBack=0;
    for(int t0=0; t0<NUM_THREADS; t0++){
        passengerArray[t0].id=t0;
        passengerArray[t0].lossID=t0;
        passengerArray[t0].time=TIME;
        if(t0%2==0){
            passengerArray[t0].isVIP=1;
            fprintf(fp,"Passenger %d (VIP) has arrived at the airport at time %d\n\n",t0,passengerArray[t0].time);
        } else{
            passengerArray[t0].isVIP=0;
            fprintf(fp,"Passenger %d has arrived at the airport at time %d\n\n",t0,passengerArray[t0].time);
        }
        int pdr=getPDR();
        //printf("%d\n",pdr);
        TIME+=pdr;

        rc= pthread_create(&passengers[t0],NULL,Process,(void *)&passengerArray[t0]);
        if(rc){
            fprintf(fp,"ERROR; return code from pthread_create() is %d\n\n", rc);
            exit(-1);
        }
        sleep(pdr);
    }
    pthread_exit(NULL);
    return  0;
}