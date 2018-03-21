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

int BinarySearch(int x,keytype A[],int p,int r)
{
    int low=p,mid,high;
    if(p>=r+1)
        high=p;
    else
        high=r+1;
    while(low<high)
    {
        mid=(low+high)/2;
        if(x<=A[mid])
            high=mid;
        else
            low=mid+1;
    }
    return high;
}

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

void PMerge(keytype A[],keytype Temp[],int p1,int r1,int p2,int r2,int p3)
{
    int q1,q2,q3;
    int n1 = r1-p1+1;
    int n2 = r2-p2+1;
    int temp;
    if (n1<n2)
    {
        temp=n1; n1=n2; n2 = temp;
        temp=p1; p1=p2; p2 = temp;
        temp=r1; r1=r2; r2 = temp;
    }
    if (n1==0)
        return;
    else
    {
        q1 = (p1+r1)/2;
        q2 = BinarySearch(A[q1],A,p2,r2); //A is int or not
        q3 = p3+(q1-p1)+(q2-p2);
        Temp[q3]=A[q1];
        //#pragma omp parallel
        {
 //           #pragma omp task
            PMerge(A,Temp,p1,q1-1,p2,q2-1,p3);
            PMerge(A,Temp,q1+1,r1,q2,r2,q3+1);
   //         #pragma omp taskwait
        }
    }
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
            PMerge(A,Temp,L,center,center+1,RightEnd,L);
        }
    }
    

}
void Merge_sort(keytype A[],int N,int threads)
{
    keytype *Temp=(keytype *)malloc(N*sizeof(keytype));
    if(Temp)
    {
        PMSort(A,Temp,0,N-1,threads);
        for(int i=0;i<N;i++)
            A[i]=Temp[i];
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
