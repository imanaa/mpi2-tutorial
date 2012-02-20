/*######################################################################

 Example 9 : MPI_Gather

 Description:
   This example uses MPI_Gather to put together on a single task (in this 
   case task 0) data that are distributed among all the tasks. It is the 
   reverse fonction of MPI_Scatter (see example 7).
      
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
   Then the function MPI_Gather put together the content of all
   these vectors into the matrix recvbuff on task 0.

   Before and after communication the sum for each vector is
   printed out for verification.
   
   The size of the vector (buffsize) is given as an argument to
   the program at run time.

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
   double       *sendbuff,**recvbuff,buffsum;
   double       inittime,totaltime;
   
   /*===============================================================*/
   /* MPI Initialisation. It's important to put this call at the    */
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
     printf(" Example 9 \n\n");
     printf(" Collective Communication : MPI_Gather \n\n");
     printf(" Vector size: %d\n",buffsize);
     printf(" Number of tasks: %d\n\n",ntasks);
     printf("##########################################################\n\n");
     printf("                --> BEFORE COMMUNICATION <--\n\n");
   }

   /*=============================================================*/
   /* Memory allocation.                                          */ 
   sendbuff=(double *)malloc(sizeof(double)*buffsize);
   if( taskid == 0 ){
     recvbuff=(double **)malloc(sizeof(double *)*ntasks);
     recvbuff[0]=(double *)malloc(sizeof(double)*ntasks*buffsize);
     for(i=1;i<ntasks;i++)recvbuff[i]=recvbuff[i-1]+buffsize;
   }
   else{
     recvbuff=(double **)malloc(sizeof(double *)*1);
     recvbuff[0]=(double *)malloc(sizeof(double)*1);
   }
   
   /*=============================================================*/
   /* Vectors and/or matrices initalisation.                      */
   srand((unsigned)time( NULL ) + taskid);
   for(i=0;i<buffsize;i++){
       sendbuff[i]=(double)rand()/RAND_MAX;
   }
   
   /*==============================================================*/
   /* Print out before communication.                              */
   
   MPI_Barrier(MPI_COMM_WORLD);
   
   buffsum=0.0;
   for(i=0;i<buffsize;i++){
     buffsum=buffsum+sendbuff[i];
   }
   printf("Task %d : Sum of vector elements = %e \n",taskid,buffsum);
     
   MPI_Barrier(MPI_COMM_WORLD);     
     
   /*===============================================================*/
   /* Communication.                                                */

   inittime = MPI_Wtime();

   ierr=MPI_Gather(sendbuff,buffsize,MPI_DOUBLE,
                   recvbuff[0],buffsize,MPI_DOUBLE,
                   0,MPI_COMM_WORLD);
       
   totaltime = MPI_Wtime() - inittime;
     
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
     free(recvbuff[0]);
     free(recvbuff);
   }
   else{
     free(recvbuff[0]);
     free(recvbuff);
     free(sendbuff);
   }

   /*===============================================================*/
   /* MPI finalisation.                                             */
   MPI_Finalize();

}


