/*######################################################################

 Example 11 : MPI_Alltoall

 Description:
   MPI_Alltoall allows each task to send specific data to all other
   tasks all at once.
   
             <--------- sendbuff ---------->
          ####################################
          #      #      #      #      #      #
        0 #  A0  #  A1  #  A2  #  A3  #  A4  #
          #      #      #      #      #      #
          ####################################
     T    #      #      #      #      #      #
        1 #  B0  #  B1  #  B2  #  B3  #  B4  #
     a    #      #      #      #      #      #
          ####################################
     c    #      #      #      #      #      #
        2 #  C0  #  C1  #  C2  #  C3  #  C4  #       BEFORE
     h    #      #      #      #      #      #
          ####################################
     e    #      #      #      #      #      #
        3 #  D0  #  D1  #  D2  #  D3  #  D4  #
     s    #      #      #      #      #      #
          ####################################
          #      #      #      #      #      #
        4 #  E0  #  E1  #  E2  #  E3  #  E4  #
          #      #      #      #      #      #
          ####################################
   
             <--------- recvbuff ---------->
          ####################################
          #      #      #      #      #      #
        0 #  A0  #  B0  #  C0  #  D0  #  E0  #
          #      #      #      #      #      #
          ####################################
     T    #      #      #      #      #      #
        1 #  A1  #  B1  #  C1  #  D1  #  E1  #
     a    #      #      #      #      #      #
          ####################################
     c    #      #      #      #      #      #
        2 #  A2  #  B2  #  C2  #  D2  #  E2  #       AFTER
     h    #      #      #      #      #      #
          ####################################
     e    #      #      #      #      #      #
        3 #  A3  #  B3  #  C3  #  D3  #  E3  #
     s    #      #      #      #      #      #
          ####################################
          #      #      #      #      #      #
        4 #  A4  #  B4  #  C4  #  D4  #  E4  #
          #      #      #      #      #      #
          ####################################
   
   Each task prepares a set of vectors of real random numbers 
   (one vector corresponding to each task) and stores them in the 
   matrix sendbuff. Then each task sends the corresponding
   vectors to all the available tasks, in one single call
   of MPI_Alltoall. The jth vector in sendbuff of task i is
   transfer to the ith vector in recvbuff of task j.
   
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
   int          ierr,i,j,itask,jtask;
   int	        buffsize;
   double       **sendbuff,**recvbuff,buffsum;
   double       inittime,totaltime,recvtime;
   
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
     printf(" Example 11 \n\n");
     printf(" Collective Communication : MPI_Alltoall \n\n");
     printf(" Vector size: %d\n",buffsize);
     printf(" Number of tasks: %d\n\n",ntasks);
     printf("##########################################################\n\n");
     printf("                --> BEFORE COMMUNICATION <--\n\n");
   }

   /*=============================================================*/
   /* Memory allocation.                                          */ 
   sendbuff=(double **)malloc(sizeof(double *)*ntasks);
   sendbuff[0]=(double *)malloc(sizeof(double)*ntasks*buffsize);
   for(i=1;i<ntasks;i++)sendbuff[i]=sendbuff[i-1]+buffsize;
   recvbuff=(double **)malloc(sizeof(double *)*ntasks);
   recvbuff[0]=(double *)malloc(sizeof(double)*ntasks*buffsize);
   for(i=1;i<ntasks;i++)recvbuff[i]=recvbuff[i-1]+buffsize;

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

   for(jtask=0;jtask<ntasks;jtask++){
     MPI_Barrier(MPI_COMM_WORLD);
     if ( taskid == jtask ){
       printf("\n");
       for(itask=0;itask<ntasks;itask++){
         buffsum=0.0;
         for(i=0;i<buffsize;i++){
           buffsum=buffsum+sendbuff[itask][i];
         }
         printf("Task %d : Sum of vector sent to %d -> %e \n",
                 taskid,itask,buffsum);
      
       }
     }
   }
     

   /*===============================================================*/
   /* Communication.                                                */

   inittime = MPI_Wtime();

   ierr=MPI_Alltoall(sendbuff[0],buffsize,MPI_DOUBLE,
                     recvbuff[0],buffsize,MPI_DOUBLE,
                     MPI_COMM_WORLD);
       
   totaltime = MPI_Wtime() - inittime;
        
   /*===============================================================*/
   /* Print out after communication.                                */

   MPI_Barrier(MPI_COMM_WORLD);
   if(taskid==0){
     printf("\n");
     printf("##########################################################\n\n");
     printf("                --> AFTER COMMUNICATION <-- \n\n");
   }
   for(jtask=0;jtask<ntasks;jtask++){
     MPI_Barrier(MPI_COMM_WORLD);
     if ( taskid == jtask ){
       printf("\n");
       for(itask=0;itask<ntasks;itask++){
         buffsum=0.0;
         for(i=0;i<buffsize;i++){
           buffsum=buffsum+recvbuff[itask][i];
         }
         printf("Task %d : Sum of vector received from %d -> %e \n",
                 taskid,itask,buffsum);
      
       }
     }
   }
        
   MPI_Barrier(MPI_COMM_WORLD);
   if(taskid==0){
     printf("\n");
     printf("##########################################################\n\n");
     printf(" Communication time : %f seconds\n\n",totaltime);  
     printf("##########################################################\n\n");
   }

   /*===============================================================*/
   /* Free the allocated memory.                                    */
   free(sendbuff[0]);
   free(sendbuff);
   free(recvbuff[0]);
   free(recvbuff);

   /*===============================================================*/
   /* MPI finalisation.                                             */
   MPI_Finalize();

}


