/*######################################################################

 Example 1 : My first Hello World!

Description:
     This simple program contains the minimum needed to use
     the MPI parallel environment.

   MPI::Init:
     Inititialisation of the MPI Environment. A this time the
     executing program receives its unique task identification
     number (taskid)

   MPI::Finalize:
    Clean closure of the MPI environment. Must be called before
    the end of the program (before an explicit exit for example),
    otherwise The MPI environment might not be able to finish
    cleanly.

   MPI::COMM_WORLD.Get_size:
    Returns the number of tasks available during the whole
    duration of the execution.

   MPI::COMM_WORLD.Get_rank:
    Returns the unique identification number of the task (taskid).

   Program output:
    In this example, each task print out the total number of
    task and its taskid, and of course "Hello World!".
    You shall notice that since no task has any priority over the
    other tasks, the print out is not done in order.

 Author: Carol Gauthier
         Francis Jackson (C++ translation)
         Centre de Calcul scientifique
         Universite de Sherbrooke

 Last revision: August 2005

######################################################################*/

#include <stdlib.h>
#include <stdio.h>
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

   printf("ntasks= %d : taskid= %d  : Hello World!\n",ntasks,taskid);

   MPI::Finalize();

}
