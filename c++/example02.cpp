/*######################################################################

 Example 2 : My second Hello World!

 Description:
   This example shows how to use the call MPI::COMM_WORLD.Barrier that
   allow to synchronize tasks. When a task encounters a Barrier call,
   it waits for all the other tasks of the given communicator to
   reach the same point, before continuing its execution. The default
   communicator used here (MPI::COMM_WORLD) includes all the tasks of
   the run. This example given is just an illustration, there are more
   efficient ways to do the same job!

   This program must be run with 4 MPI tasks available.

 Author: Carol Gauthier
         Francis Jackson (C++ translation)
         Centre de Calcul scientifique
         Universite de Sherbrooke

 Last revision: August 2005

######################################################################*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <mpi.h>

int main(int argc,char** argv)
{
   int          taskid, ntasks;
   int          ierr,i,j,itask;
   int          buffsize;

   MPI::Init(argc, argv);

   taskid = MPI::COMM_WORLD.Get_rank();
   ntasks = MPI::COMM_WORLD.Get_size();

   if ( taskid == 0 ){
     printf("\n\n\n\n\n");
   }

   MPI::COMM_WORLD.Barrier();

   if ( taskid == 0 )printf("Hel");
   if ( taskid == 1 )printf("lo ");
   if ( taskid == 2 )printf("Wor");
   if ( taskid == 3 )printf("ld!");

   MPI::COMM_WORLD.Barrier();

   if ( taskid == 0 )printf(" (Unordered)\n");

   if ( taskid == 0 )printf("Hel");
   MPI::COMM_WORLD.Barrier();
   if ( taskid == 1 )printf("lo ");
   MPI::COMM_WORLD.Barrier();
   if ( taskid == 2 )printf("Wor");
   MPI::COMM_WORLD.Barrier();
   if ( taskid == 3 )printf("ld!");
   MPI::COMM_WORLD.Barrier();

   if ( taskid == 0 ){
     printf(" (Ordered)\n\n\n\n\n");
   }


   MPI::Finalize();

}
