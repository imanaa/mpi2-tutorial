/*######################################################################

 Example 2 : My second Hello World!

 Description:
   This example shows how to use the call MPI_Barrier that allow
   to synchronize tasks. When a task encounters an MPI_Barrier
   call, it waits for all the other tasks of the given communicator to 
   reach the same point, before continuing its execution. The default 
   communicator used here (MPI_COMM_WORLD) includes all the tasks of the
   run. This example given is just an illustration, there are more 
   efficient ways to do the same job!
   
   This program must be run with 4 MPI tasks available.
   
 Author: Carol Gauthier
         Centre de Calcul scientifique
         Universite de Sherbrooke

 Last revision: Aout 2004

######################################################################*/

#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "math.h"
#include "mpi.h"

int main(int argc,char** argv)
{
   int          taskid, ntasks;
   int          ierr,i,j,itask;
   int	        buffsize;

   MPI_Init(&argc, &argv);

   MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
   MPI_Comm_size(MPI_COMM_WORLD,&ntasks);
   
   if ( taskid == 0 ){
     printf("\n\n\n\n\n");
   } 

   ierr=MPI_Barrier(MPI_COMM_WORLD);
   
   if ( taskid == 0 )printf("Hel");
   if ( taskid == 1 )printf("lo ");
   if ( taskid == 2 )printf("Wor");
   if ( taskid == 3 )printf("ld!");
   
   ierr=MPI_Barrier(MPI_COMM_WORLD);

   if ( taskid == 0 )printf(" (Unordered)\n");
   
   if ( taskid == 0 )printf("Hel");
   ierr=MPI_Barrier(MPI_COMM_WORLD);
   if ( taskid == 1 )printf("lo ");
   ierr=MPI_Barrier(MPI_COMM_WORLD);
   if ( taskid == 2 )printf("Wor");
   ierr=MPI_Barrier(MPI_COMM_WORLD);
   if ( taskid == 3 )printf("ld!");
   ierr=MPI_Barrier(MPI_COMM_WORLD);
   
   if ( taskid == 0 ){
     printf(" (Ordered)\n\n\n\n\n");
   }
   
   
   MPI_Finalize();
   
}
