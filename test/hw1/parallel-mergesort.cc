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
void merge_sort(keytype arr[] ,keytype begin ,keytype end);
void merge_core (keytype arr[], keytype begin, keytype mid, keytype end);
static int compare (const void* a, const void* b)
{ 
  keytype ka = *(const keytype *)a;
  keytype kb = *(const keytype *)b;
  if (ka < kb)
    return -1;
  else if (ka == kb)
    return 0;
  else
    return 1;
}

void
parallelSort (int N, keytype* A)
{
omp_set_num_threads (8);
#pragma omp parallel
//qsort (A, N, sizeof (keytype), compare);
merge_sort(A,0,N);
  /* Lucky you, you get to start from scratch */
}

void merge_sort(keytype arr[] ,keytype begin ,keytype end)
{

if(end-begin < 2) return;
keytype mid = (begin+end)/2;
merge_sort(arr,begin,mid);
merge_sort(arr,mid,end);
merge_core(arr,begin,mid,end);
}
void merge_core (keytype arr[], keytype begin, keytype mid, keytype end)
{
    int i=begin, j=mid, k=0;
    int *tmp = (int*)malloc(sizeof(int)*(end-begin));
    for(; i<mid && j<end; tmp[k++]=(arr[i]<arr[j]?arr[i++]:arr[j++]));
    for(; i<mid; tmp[k++]=arr[i++]);
    for(; j<end; tmp[k++]=arr[j++]);
    for(i=begin, k=0; i<end; arr[i++]=tmp[k++]);
    free(tmp);
}
/* eof */
