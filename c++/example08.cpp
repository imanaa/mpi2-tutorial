/*######################################################################

 Example 8 : MPI::COMM_WORLD.Scatterv

 Description:
   This example uses MPI::COMM_WORLD.Scatterv to ditribute data to all
   available tasks. This example is similar to example 7 except that
   only portions of the vectors are transmitted. This feature allows for
   a better control of the amount of data to be transfered, and so help
   reduce communication time.

                 <----- sendbuff ------>

          ####################################
          #      #      #      #      #      #
        0 # ABCD # EFGH # IJKL # MNOP # QRST #
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
        0 # AB   #
          #      #
          ########
     T    #      #
        1 # EFG  #
     a    #      #
          ########
     s    #      #
        2 # I    #                                   AFTER
     k    #      #
          ########
     s    #      #
        3 # MNOP #
          #      #
          ########
          #      #
        4 # QRS  #
          #      #
          ########


   Task 0 prepares one vector of real random numbers per task, and
   place them contiguously in memory in the vector sendbuff. Then
   portions of each of the vectors are sent to their corresponding
   task using Scatterv.

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
   int          buffsize,sendcounts[2048],displs[2048],recvcount;
   double       **sendbuff,*recvbuff,buffsum,buffsums[2048];
   double       inittime,totaltime,recvtime,recvtimes[2048];

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
     printf(" Example 8 \n\n");
     printf(" Collective Communication : MPI::COMM_WORLD.Scatterv \n\n");
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
       displs[itask]=itask*buffsize;
       sendcounts[itask]=buffsize/(itask+1);
       for(i=0;i<sendcounts[itask];i++){
         sendbuff[itask][i]=(double)rand()/RAND_MAX;
       }
     }

     /*==============================================================*/
     /* Print out before communication.                              */
     for(itask=0;itask<ntasks;itask++){
       buffsum=0.0;
       for(i=0;i<sendcounts[itask];i++){
         buffsum=buffsum+sendbuff[itask][i];
       }
       printf("Task %d: Vector sent to %d: sum=%e size= %d\n",
               taskid,itask,buffsum,sendcounts[itask]);

     }

   }

   /*=================================================================*/
   /* Size of received vector.                                        */
   recvcount=buffsize/(taskid+1);

   /*===============================================================*/
   /* Communication.                                                */

   inittime = MPI::Wtime();

   MPI::COMM_WORLD.Scatterv(sendbuff[0],sendcounts,displs,MPI::DOUBLE,
                            recvbuff,recvcount,MPI::DOUBLE,
                            0);

   totaltime=MPI::Wtime() - inittime;

   /*===============================================================*/
   /* Print out after communication.                                */

   buffsum=0.0;
   for(i=0 ; i<recvcount ; i++){
     buffsum=buffsum+recvbuff[i];
   }

   MPI::COMM_WORLD.Gather(&buffsum,1,MPI::DOUBLE,
                          buffsums,1, MPI::DOUBLE,
                          0);

   if(taskid==0){
     printf("\n");
     printf("##########################################################\n\n");
     printf("                --> AFTER COMMUNICATION <-- \n\n");
     for(itask=0;itask<ntasks;itask++){
       printf("Task %d : Sum of received vector: %e\n",
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


