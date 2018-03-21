/**
 *  \file parallel-mergesort.cc
 *
 *  \brief Implement your parallel mergesort in this file.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "sort.hh"

void Merge(keytype A[],keytype Temp[],int L,int R,int RightEnd)
{
    int LeftEnd=R-1;
    int p=L,i;
    int num=RightEnd-L+1;
    
    for(;L<=LeftEnd&&R<=RightEnd;Temp[p++]=(A[L]<A[R]?A[L++]:A[R++]));
    for(;L<=LeftEnd;Temp[p++]=A[L++]);
    for(;R<=RightEnd;Temp[p++]=A[R++]);
    for(i=0;i<num;i++,RightEnd--)
        A[RightEnd]=Temp[RightEnd];
}

void PMerge(keytype A[],keytype Temp[],int L,int R,int RightEnd,int threads)
{
    int LeftEnd=R-1;
    int p=L,i;
    int num=RightEnd-L+1;
    

    for(;L<=LeftEnd&&R<=RightEnd;Temp[p++]=(A[L]<A[R]?A[L++]:A[R++]));
    

   // #pragma omp parallel for shared(Temp,A) num_threads(threads) private(i) private(p)   
//#pragma omp parallel for  
    for(int i=L;i<=LeftEnd;i++){
	 Temp[p++]=A[i];}
   // #pragma omp parallel for  
    for(int j=R;j<=RightEnd;j++)
	Temp[p++]=A[j];

#pragma omp parallel for shared(RightEnd) private(i) 
    for(int m=RightEnd;m<num;m++){
        A[RightEnd]=Temp[RightEnd];
#pragma omp critical
	RightEnd--;}
}


void MSort(keytype A[],keytype Temp[],int L,int RightEnd)
{
    int center;
    if(L<RightEnd)
    {
        center=(L+RightEnd)/2;
        /* sort both halves in parallel */
        MSort(A,Temp,L,center);
        MSort(A,Temp,center+1,RightEnd);
        Merge(A,Temp,L,center+1,RightEnd);
    }
}

void PMSort(keytype A[],keytype Temp[],int L,int RightEnd, int threads)
{
    int center;
    if(threads==1)
        MSort(A,Temp,L,RightEnd);
    else if(threads>1)
    {
        if(L<RightEnd)
        {
            center=(L+RightEnd)/2;
            /* sort both halves in parallel */
            omp_set_num_threads(threads);
            #pragma omp parallel sections// private(threads) 
            {
                #pragma omp section
                {
                    PMSort(A,Temp,L,center,threads/2);
                }
                #pragma omp section
                {
                    PMSort(A,Temp,center+1,RightEnd,threads-threads/2);
                }
            }
            /* merge the two halves */
            PMerge(A,Temp,L,center+1,RightEnd,threads);
        }
    }
    

}
void Merge_sort(keytype A[],int N,int threads)
{
    keytype *Temp=(keytype *)malloc(N*sizeof(keytype));
    if(Temp)
    {
        PMSort(A,Temp,0,N-1,threads);
        free(Temp);
    }
    else
        printf("no space!\n");
}

 
void parallelSort (int N, keytype* A)
{
    //8 cores
    omp_set_nested(1);
    omp_set_num_threads(8);
    int threads =8;
    /* enable recursive parallel blocks */
    //omp_set_nested(1);

    Merge_sort(A,N,threads);

}
