/*######################################################################

 Example 7 : MPI::COMM_WORLD.Scatter

 Description:
   This example uses MPI::COMM_WORLD.Scatter to ditribute data to all
   available tasks.

                 <----- sendbuff ------>

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
        2 #      #      #      #      #      #       BEFORE
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

          recvbuff

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
        2 #  CC  #                                    AFTER
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

   Task 0 prepares one vector of real random numbers per task, and
   place them contiguously in memory in the vector sendbuff. Then
   each of the vectors are sent to their corresponding task using
   Scatter.

   Before and after communication the sum for each vector is
   printed out for verification.

   The size of the vector (buffsize) is given as an argument to
   the program at run time.

   This example does exactly the same work as example 4 but
   does it using collective communication. You can compare
   the performance! You'll see that it's much more efficient to
   use collective communication when ever possible.

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
   double       **sendbuff,*recvbuff,buffsum,buffsums[1024];
   double       inittime,totaltime,recvtime,recvtimes[1024];

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
     printf(" Example 7 \n\n");
     printf(" Collective Communication : MPI::COMM_WORLD.Scatter \n\n");
     printf(" Vector size: %d\n",buffsize);
     printf(" Number of tasks: %d\n\n",ntasks);
     printf("##########################################################\n\n");
     printf("                --> BEFORE COMMUNICATION <--\n\n");
   }

   /*=============================================================*/
   /* Memory allocation.                                          */
   recvbuff = new double[buffsize];
   if ( taskid == 0 ){
     sendbuff = new double*[ntasks];
     sendbuff[0] = new double[ntasks*buffsize];
     for(i=1;i<ntasks;i++)sendbuff[i]=sendbuff[i-1]+buffsize;
   }
   else{
     sendbuff = new double*[1];
     sendbuff[0] = new double[1];
   }


   if ( taskid == 0 ){

     /*=============================================================*/
     /* Vectors and/or matrices initalisation.                      */
     srand((unsigned)time( NULL ) + taskid);
     for(itask=0;itask<ntasks;itask++){
       for(i=0;i<buffsize;i++){
         sendbuff[itask][i]=(double)rand()/RAND_MAX;
       }
     }

     /*==============================================================*/
     /* Print out before communication.                              */
     for(itask=1;itask<ntasks;itask++){
       buffsum=0.0;
       for(i=0;i<buffsize;i++){
         buffsum=buffsum+sendbuff[itask][i];
       }
       printf("Task %d : Sum of vector sent to %d -> %e \n",
               taskid,itask,buffsum);

     }
     printf("\n");

   }

   /*===============================================================*/
   /* Communication.                                                */

   inittime = MPI::Wtime();

   MPI::COMM_WORLD.Scatter(sendbuff[0],buffsize,MPI::DOUBLE,
                           recvbuff,buffsize,MPI::DOUBLE,
                           0);

   totaltime = MPI::Wtime() - inittime;

   /*===============================================================*/
   /* Print out after communication.                                */

   buffsum=0.0;
   for(i=0 ; i<buffsize ; i++){
     buffsum=buffsum+recvbuff[i];
   }

   MPI::COMM_WORLD.Gather(&buffsum,1,MPI::DOUBLE,
                          buffsums,1, MPI::DOUBLE,
                          0);

   if(taskid==0){
     printf("##########################################################\n\n");
     printf("                --> AFTER COMMUNICATION <-- \n\n");
     for(itask=1;itask<ntasks;itask++){
       printf("Task %d : Sum of received vector= %e\n",
               itask,buffsums[itask]);
     }
     printf("\n");
     printf("##########################################################\n\n");
     printf(" Communication time : %f seconds\n\n",totaltime);
     printf("##########################################################\n\n");
   }

   /*===============================================================*/
   /* Free the allocated memory.                                    */
   if ( taskid == 0 ){
     delete [] sendbuff[0];
     delete [] sendbuff;
   }
   else{
     delete [] sendbuff[0];
     delete [] sendbuff;
     delete [] recvbuff;
   }

   /*===============================================================*/
   /* MPI finalisation.                                             */
   MPI::Finalize();

}


