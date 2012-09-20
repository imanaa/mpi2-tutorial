/*######################################################################

 Example 4 : MPI::COMM_WORLD.Send/Recv

 Description:
   This program is a simple example of point-to-point communication
   using MPI::COMM_WORLD.Send and MPI::COMM_WORLD.Recv.

             <---------- Data ----------->

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

             <---------- Data ----------->

          ####################################
          #      #      #      #      #      #
        0 #  AA  #      #      #      #      #
          #      #      #      #      #      #
          ####################################
     T    #      #      #      #      #      #
        1 #  BB  #      #      #      #      #
     a    #      #      #      #      #      #
          ####################################
     s    #      #      #      #      #      #
        2 #  CC  #      #      #      #      #       AFTER
     k    #      #      #      #      #      #
          ####################################
     s    #      #      #      #      #      #
        3 #  DD  #      #      #      #      #
          #      #      #      #      #      #
          ####################################
          #      #      #      #      #      #
        4 #  EE  #      #      #      #      #
          #      #      #      #      #      #
          ####################################

   Task 0 first prepares (initialize with random numbers) one vector
   of real values for each available task in the run. Then it sends
   each vector to its corresponding task, one after the other.

   Before the communication, task 0 prints out the sum of all the
   vectors to be sent. After communication is completed, each task
   calculate the sum of the received vectors and the results are
   gathered together on task 0 using an MPI::COMM_WORLD.Gather call.
   Task 0 than prints out these sums for comparison.

   This example is just an illustration of Send/Recv usage. There is
   a more efficient ways to perform the same work by using the collective
   communication MPI::COMM_WORLD.Scatter, as shown in example 7 which
   does exacltly the same work as this one. It is in fact strongly
   suggested to always use collective communications whenever possible.

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
   /* MPI Initialisation. Its important to put this call at the     */
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
     printf(" Example 4 \n\n");
     printf(" Point-to-point Communication: MPI::COMM_WORLD.Send/Recv \n\n");
     printf(" Vector size: %d\n",buffsize);
     printf(" Number of tasks: %d\n\n",ntasks);
     printf("##########################################################\n\n");
     printf("                --> BEFORE COMMUNICATION <--\n\n");
   }


   if ( taskid == 0 ){
     /*=============================================================*/
     /* Memory allocation.                                          */
     sendbuff = new double*[ntasks];
     sendbuff[0] = new double[ntasks*buffsize];
     for(i=1;i<ntasks;i++)sendbuff[i]=sendbuff[i-1]+buffsize;

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

   }
   else{

     /*=============================================================*/
     /* Memory allocation.                                          */
     recvbuff = new double[buffsize];

   }

   /*===============================================================*/
   /* Communication.                                                */

   inittime = MPI::Wtime();

   if ( taskid == 0 ){

     for(itask=1 ; itask<ntasks ; itask++){

       MPI::COMM_WORLD.Send(sendbuff[itask],
                            buffsize,
                            MPI::DOUBLE,
                            itask,
                            0);
     }

   }
   else{

     MPI::COMM_WORLD.Recv(recvbuff,
                          buffsize,
                          MPI::DOUBLE,
                          0,
                          MPI::ANY_TAG,
                          status);

     recvtime = MPI::Wtime();

     buffsum=0.0;
     for(i=0 ; i<buffsize ; i++){
       buffsum=buffsum+recvbuff[i];
     }

   }

   MPI::COMM_WORLD.Barrier();

   totaltime=MPI::Wtime() - inittime;

   /*===============================================================*/
   /* Print out after communication.                                */

   MPI::COMM_WORLD.Gather(&recvtime,1,MPI::DOUBLE,
                          recvtimes,1, MPI::DOUBLE,
                          0);

   MPI::COMM_WORLD.Gather(&buffsum,1,MPI::DOUBLE,
                          buffsums,1, MPI::DOUBLE,
                          0);

   if(taskid==0){
     printf("\n");
     printf("##########################################################\n\n");
     printf("                --> AFTER COMMUNICATION <-- \n\n");
     for(itask=1;itask<ntasks;itask++){
       printf("Task %d : Vector received at %f seconds : Sum= %e\n",
               itask,recvtimes[itask],buffsums[itask]);
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
     delete [] recvbuff;
   }

   /*===============================================================*/
   /* MPI finalisation.                                             */
   MPI::Finalize();

}


