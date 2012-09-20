/*######################################################################

 Example 9 : MPI::COMM_WORLD.Gather

 Description:
   This example uses MPI::COMM_WORLD.Gather to put together on a single
   task (in this case task 0) data that are distributed among all the
   tasks. It is the reverse fonction of Scatter (see example 7).

          sendbuff
          ########
          #      #
        0 #  AA  #
          #      #
          ########
     T    #      #
        1 #  BB  #
     a    #      #
          ########
     s    #      #
        2 #  CC  #                                    BEFORE
     k    #      #
          ########
     s    #      #
        3 #  DD  #
          #      #
          ########
          #      #
        4 #  EE  #
          #      #
          ########

            <---------- recvbuff ---------->
          ####################################
          #      #      #      #      #      #
        0 #  AA  #  BB  #  CC  #  DD  #  EE  #
          #      #      #      #      #      #
          ####################################
     T    #      #      #      #      #      #
        1 #      #      #      #      #      #
     a    #      #      #      #      #      #
          ####################################
     s    #      #      #      #      #      #
        2 #      #      #      #      #      #       AFTER
     k    #      #      #      #      #      #
          ####################################
     s    #      #      #      #      #      #
        3 #      #      #      #      #      #
          #      #      #      #      #      #
          ####################################
          #      #      #      #      #      #
        4 #      #      #      #      #      #
          #      #      #      #      #      #
          ####################################

   Each task prepares a vector of real random numbers (sendbuff).
   Then the function Gather put together the content of all
   these vectors into the matrix recvbuff on task 0.

   Before and after communication the sum for each vector is
   printed out for verification.

   The size of the vector (buffsize) is given as an argument to
   the program at run time.

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
   double       *sendbuff,**recvbuff,buffsum;
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
     printf(" Example 9 \n\n");
     printf(" Collective Communication : MPI::COMM_WORLD.Gather \n\n");
     printf(" Vector size: %d\n",buffsize);
     printf(" Number of tasks: %d\n\n",ntasks);
     printf("##########################################################\n\n");
     printf("                --> BEFORE COMMUNICATION <--\n\n");
   }

   /*=============================================================*/
   /* Memory allocation.                                          */
   sendbuff = new double[buffsize];
   if( taskid == 0 ){
     recvbuff = new double*[ntasks];
     recvbuff[0]=new double[ntasks*buffsize];
     for(i=1;i<ntasks;i++)recvbuff[i]=recvbuff[i-1]+buffsize;
   }
   else{
     recvbuff = new double*[1];
     recvbuff[0] = new double[1];
   }

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
   printf("Task %d : Sum of vector elements = %e \n",taskid,buffsum);

   MPI::COMM_WORLD.Barrier();

   /*===============================================================*/
   /* Communication.                                                */

   inittime = MPI::Wtime();

   MPI::COMM_WORLD.Gather(sendbuff,buffsize,MPI::DOUBLE,
                          recvbuff[0],buffsize,MPI::DOUBLE,
                          0);

   totaltime = MPI::Wtime() - inittime;

   /*===============================================================*/
   /* Print out after communication.                                */
   if ( taskid == 0 ){
     printf("\n");
     printf("##########################################################\n\n");
     printf("                --> AFTER COMMUNICATION <-- \n\n");
     for(itask=0;itask<ntasks;itask++){
       buffsum=0.0;
       for(i=0;i<buffsize;i++){
         buffsum=buffsum+recvbuff[itask][i];
       }
       printf("Task %d : Sum of vector received from %d -> %e \n",
               taskid,itask,buffsum);

     }
   }

   if(taskid==0){
     printf("\n");
     printf("##########################################################\n\n");
     printf(" Communication time : %f seconds\n\n",totaltime);
     printf("##########################################################\n\n");
   }

   /*===============================================================*/
   /* Free the allocated memory.                                    */
   if ( taskid == 0 ){
     delete [] recvbuff[0];
     delete [] recvbuff;
   }
   else{
     delete [] recvbuff[0];
     delete [] recvbuff;
     delete [] sendbuff;
   }

   /*===============================================================*/
   /* MPI finalisation.                                             */
   MPI::Finalize();

}


