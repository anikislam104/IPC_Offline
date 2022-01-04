//
// Created by aniki on 1/1/2022.
//
///should be named testing.c

//#include <stdio.h>
//#include "math.h"
//#include "time.h"
//#include "unistd.h"
//int factorial(int n)
//{
//    int s=1;
//    for(int i=n;i>0;i--){
//        s=s*i;
//    }
//    return s;
//}
//double poissonDist(double lambda,double x)
//{
//    //printf("%f  ",(double )lambda);
//    double a= exp(-1.0*(double )lambda);
//    //printf("%f   ",a);
//    a=a*pow((double )lambda,(double )x);
//    a=a/(double ) factorial((int)x);
//    //int ans=(int )a;
//    return a;
//}
//double poisson( int k, double mean ) {
//    double p = exp(-mean);
//    double f = 1;
//    for ( int i=0 ; i<k ; i++ ) f *= mean/(i+1);
//    return p*f;
//}
//
//int main(){
//    double lambda=2.0,x=1;
////    for(int i=1;i<17;i++) {
////        double s1 = poissonDist(lambda, (double )i);
////        printf("%f\n", s1);
////    }
//    printf("new\n");
//    for(int i=1;i<30;i++) {
//        double s1 = poisson(i, lambda);
//        printf("%f %d\n", s1,i);
////        if((int)(s1*100000)==0){
////            i=480;
////        }
//    }
//
//}

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "unistd.h"

int printRandoms(int lower, int upper)
{
    int i;
    for (i = 0; i < 1; i++) {
        int num = (rand() %
                   (upper - lower + 1)) + lower;
//        printf("%d ", num);
        return num;
    }
}

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

void main ()
{
    double u = U_Random ();
    int p = possion ();

    printf("%f\n", u);
    printf("%d\n", p);

    u = U_Random ();
    p = possion ();

    printf("%f\n", u);
    printf("%d\n", p);

    for (int i=0;i<10;i++){
        printf("%d\n",getPDR());
    }

}