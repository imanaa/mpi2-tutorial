/*######################################################################

 Example 12 : MPI::COMM_WORLD.Reduce

 Description:
   MPI::COMM_WORLD.Reduce allows to apply a reduction operation to the
   vector sendbuff over all the available tasks.

            <--- sendbuff ---->

     T     *********************
         0 * A0 * B0 * .. * Z0 *
     a     *********************
         1 * A1 * B1 * .. * Z1 *
     s     *********************
         2 * A2 * B2 * .. * Z2 *
     k     *********************
         3 * A3 * B3 * .. * Z3 *
     s     *********************
         4 * A4 * B4 * .. * Z4 *
           *********************

            <--- recvbuff ---->

     T     **************************************************
         0 * A0+A1+..+A4 * B0+B1+..+B4 *  ..  * Z0+Z1+..+Z4 *
     a     **************************************************
         1 *             *             *  ..  *             *
     s     **************************************************
         2 *             *             *  ..  *             *
     k     **************************************************
         3 *             *             *  ..  *             *
     s     **************************************************
         4 *             *             *  ..  *             *
           **************************************************

   Each task prepares one vector of real random numbers. Reduce
   applies a mathematical operation (addition in this case) on the
   jth element of sendbuff of all the available tasks, and places the
   results in the root task (task 0 in this case). See the
   documentation for the other mathematical operations available.

   Before and after communication the sum for each vector is
   printed out for verification.

   The size of the vector (buffsize) is given as an argument to
   the program at run time.

   Similarly, the function Allreduce does the same work and
   broadcast the reulting vector to all the tasks.

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
   MPI::Status  status;
   int          ierr,i,j,itask;
   int          buffsize;
   double       *sendbuff,*recvbuff,buffsum,totalsum;
   double       inittime,totaltime;

   /*===============================================================*/
   /* MPI Initialisation. It's important to put this call at the    */
   /* begining of the program, after variable declarations.         */
   MPI::Init(argc, argv);

   /*===============================================================*/
   /* Get the number of MPI tasks and the taskid of this task.      */
   taskid = MPI::COMM_WORLD.Get_rank();
   ntasks = MPI::COMM_WORLD.Get_size();

   /*===============================================================*/
   /* Get buffsize value from program arguments.                    */
   buffsize=atoi(argv[1]);

   /*===============================================================*/
   /* Printing out the description of the example.                  */
   if ( taskid == 0 ){
     printf("\n\n\n");
     printf("##########################################################\n\n");
     printf(" Example 12 \n\n");
     printf(" Collective Communication : MPI::COMM_WORLD.Reduce \n\n");
     printf(" Vector size: %d\n",buffsize);
     printf(" Number of tasks: %d\n\n",ntasks);
     printf("##########################################################\n\n");
     printf("                --> BEFORE COMMUNICATION <--\n\n");
   }

   /*=============================================================*/
   /* Memory allocation.                                          */
   sendbuff = new double[buffsize];
   recvbuff = new double[buffsize];

   /*=============================================================*/
   /* Vectors and/or matrices initalisation.                      */
   srand((unsigned)time( NULL ) + taskid);
   for(i=0;i<buffsize;i++){
       sendbuff[i]=(double)rand()/RAND_MAX;
   }

   /*==============================================================*/
   /* Print out before communication.                              */

   MPI::COMM_WORLD.Barrier();

   buffsum=0.0;
   for(i=0;i<buffsize;i++){
     buffsum=buffsum+sendbuff[i];
   }
   printf(" Task %d : Sum of sendbuff elements = %e \n",taskid,buffsum);

   MPI::COMM_WORLD.Barrier();

   MPI::COMM_WORLD.Reduce(&buffsum,&totalsum,1,MPI::DOUBLE,MPI::SUM,0);
   if(taskid==0){
     printf("                                     =============\n");
     printf("                              TOTAL: %e \n\n",totalsum);
   }

   /*===============================================================*/
   /* Communication.                                                */

   inittime = MPI::Wtime();

   MPI::COMM_WORLD.Reduce(sendbuff,recvbuff,buffsize,MPI::DOUBLE,MPI::SUM,0);

   totaltime = MPI::Wtime() - inittime;

   /*===============================================================*/
   /* Print out after communication.                                */
   if ( taskid == 0 ){
     printf("##########################################################\n\n");
     printf("                --> AFTER COMMUNICATION <-- \n\n");
     buffsum=0.0;
     for(i=0;i<buffsize;i++){
       buffsum=buffsum+recvbuff[i];
     }
     printf(" Task %d : Sum of recvbuff elements -> %e \n",taskid,buffsum);
     printf("\n");
     printf("##########################################################\n\n");
     printf(" Communication time : %f seconds\n\n",totaltime);
     printf("##########################################################\n\n");
   }

   /*===============================================================*/
   /* Free the allocated memory.                                    */
   delete [] recvbuff;
   delete [] sendbuff;

   /*===============================================================*/
   /* MPI finalisation.                                             */
   MPI::Finalize();

}


