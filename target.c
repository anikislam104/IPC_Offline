//
// Created by aniki on 1/1/2022.
//
///should be named testing.c

#include <stdio.h>
#include "math.h"
int factorial(int n)
{
    int s=1;
    for(int i=n;i>0;i--){
        s=s*i;
    }
    return s;
}
double poissonDist(int lambda,int x)
{
    printf("%f  ",(double )lambda);
    double a= exp(-1.0*(double )lambda);
    printf("%f   ",a);
    a=a*pow((double )lambda,(double )x);
    a=a/(double ) factorial(x);
    int ans=(int )a;
    return a;
}
int main(){
    int lambda,x;
    lambda=1;
    x=1;
    printf("%f\n", poissonDist(lambda,x));
    lambda=1;
    x=2;
    printf("%f\n", poissonDist(lambda,x));
    lambda=1;
    x=3;
    printf("%f\n", poissonDist(lambda,x));

}