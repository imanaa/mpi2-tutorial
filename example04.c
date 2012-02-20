/*######################################################################

 Example 4 : MPI_Send and MPI_Recv

 Description:
   This program is a simple example of point-to-point communication
   using MPI_Send and MPI_Recv.
   
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
   gathered together on task 0 using an MPI_Gather call. Task 0
   than prints out these sums for comparison.

   This example is just an illustration of MPI_Send and MPI_Recv 
   usage. There is a more efficient ways to perform the same work
   by using the collective communication MPI_Scatter, as shown in 
   example 7 which does exacltly the same work as this one. It is 
   in fact strongly suggested to always use collective 
   communications whenever possible.
     
 Author: Carol Gauthier
         Centre de Calcul scientifique
         Universite de Sherbrooke

 Last revision: September 2005

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
   MPI_Status   status;
   int          ierr,i,j,itask;
   int	        buffsize;
   double       **sendbuff,*recvbuff,buffsum,buffsums[1024];
   double       inittime,totaltime,recvtime,recvtimes[1024];
   
   /*===============================================================*/
   /* MPI Initialisation. Its important to put this call at the     */
   /* begining of the program, after variable declarations.         */
   MPI_Init(&argc, &argv);

   /*===============================================================*/
   /* Get the number of MPI tasks and the taskid of this task.      */
   MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
   MPI_Comm_size(MPI_COMM_WORLD,&ntasks);

   /*===============================================================*/
   /* Get buffsize value from program arguments.                    */
   buffsize=atoi(argv[1]);

   /*===============================================================*/
   /* Printing out the description of the example.                  */
   if ( taskid == 0 ){
     printf("\n\n\n");
     printf("##########################################################\n\n");
     printf(" Example 4 \n\n");
     printf(" Point-to-point Communication: MPI_Send MPI_Recv \n\n");
     printf(" Vector size: %d\n",buffsize);
     printf(" Number of tasks: %d\n\n",ntasks);
     printf("##########################################################\n\n");
     printf("                --> BEFORE COMMUNICATION <--\n\n");
   }

  
   if ( taskid == 0 ){
     /*=============================================================*/
     /* Memory allocation.                                          */ 
     sendbuff=(double **)malloc(sizeof(double *)*ntasks);
     sendbuff[0]=(double *)malloc(sizeof(double)*ntasks*buffsize);
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
     recvbuff=(double *)malloc(sizeof(double)*buffsize);
     
   }

   /*===============================================================*/
   /* Communication.                                                */

   inittime = MPI_Wtime();

   if ( taskid == 0 ){

     for(itask=1 ; itask<ntasks ; itask++){
                  
       ierr = MPI_Send(sendbuff[itask],
                       buffsize,
                       MPI_DOUBLE,
	               itask,
	               0,
                       MPI_COMM_WORLD);
     } 

   }
   else{
   
     ierr = MPI_Recv(recvbuff,
                     buffsize,
                     MPI_DOUBLE,
                     0,
                     MPI_ANY_TAG,
                     MPI_COMM_WORLD,
                     &status);
     
     recvtime = MPI_Wtime();
     
     buffsum=0.0;
     for(i=0 ; i<buffsize ; i++){
       buffsum=buffsum+recvbuff[i];
     }
        
   }
   
   MPI_Barrier(MPI_COMM_WORLD);
   
   totaltime = MPI_Wtime() - inittime;

   /*===============================================================*/
   /* Print out after communication.                                */
   
   ierr=MPI_Gather(&recvtime,1,MPI_DOUBLE,
                   recvtimes,1, MPI_DOUBLE,
                   0,MPI_COMM_WORLD);
                   
   ierr=MPI_Gather(&buffsum,1,MPI_DOUBLE,
                   buffsums,1, MPI_DOUBLE,
                   0,MPI_COMM_WORLD);

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
     free(sendbuff[0]);
     free(sendbuff);
   }
   else{
     free(recvbuff);
   }

   /*===============================================================*/
   /* MPI finalisation.                                             */
   MPI_Finalize();

}


