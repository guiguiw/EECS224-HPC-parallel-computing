#include <stdio.h>
#include <omp.h>
int lol()
{
omp_set_num_threads (4);
#pragma omp parallel
{
int ID = omp_get_thread_num();
 printf ("hello, world!%d\n",ID); 
}
return 0;
}
int main(){
lol();
return 0;
}
