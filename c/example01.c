/*######################################################################

 Example 1 : My first Hello World!

   Description:
     This simple program contains the minimum needed to use
     the MPI parallel environment.
     
   MPI_Init:
     Inititialisation of the MPI Environment. A this time the 
     executing program receives its unique task identification 
     number (taskid)

   MPI_Finalize:
    Clean closure of the MPI environment. Must be called before
    the end of the program (before an explicit exit for example), 
    otherwise The MPI environment might not be able to finish 
    cleanly. 
   
   MPI_Comm_Size:
    Returns the number of tasks available during the whole 
    duration of the execution.

   MPI_Comm_Rank:
    Returns the unique identification number of the task (taskid).

   Sortie du programme:
    In this example, each task print out the total number of
    task and its taskid, and of course "Hello World!".
    You shall notice that since no task has any priority over the
    other tasks, the print out is not done in order.
   
 Author: Carol Gauthier
         Centre de Calcul scientifique
         Universite de Sherbrooke

 Last revision: Aout 2004

######################################################################*/

#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include "mpi.h"

int main(int argc,char** argv)
{
   int          taskid, ntasks;
   int          ierr,i,j,itask;
   int	        buffsize;

   MPI_Init(&argc, &argv);

   MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
   MPI_Comm_size(MPI_COMM_WORLD,&ntasks);
   
   printf("ntasks= %d : taskid= %d  : Hello World!\n",ntasks,taskid);
   
   MPI_Finalize();
   
}
