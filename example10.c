/*######################################################################

 Example 10 : MPI_Allgather

 Description:
   MPI_Allgather efficiently combines an MPI_Gather and an MPI_Bcast
   in a single call. 
      
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
        2 #  CC  #                                   BEFORE
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
        1 #  AA  #  BB  #  CC  #  DD  #  EE  #
     a    #      #      #      #      #      #
          ####################################
     s    #      #      #      #      #      #
        2 #  AA  #  BB  #  CC  #  DD  #  EE  #       AFTER
     k    #      #      #      #      #      #
          ####################################
     s    #      #      #      #      #      #
        3 #  AA  #  BB  #  CC  #  DD  #  EE  #
          #      #      #      #      #      #
          ####################################
          #      #      #      #      #      #
        4 #  AA  #  BB  #  CC  #  DD  #  EE  #
          #      #      #      #      #      #
          ####################################
   
   Each task prepares a vector of real random numbers (sendbuff).
   Then the function MPI_Gather put together the content of all
   these vectors into the matrix recvbuff on all tasks.
   
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
   double       *sendbuff,**recvbuff,buffsum;
   double       inittime,totaltime;
   
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
     printf(" Example 10 \n\n");
     printf(" Collective Communication : MPI_Allgather \n\n");
     printf(" Vector size: %d\n",buffsize);
     printf(" Number of tasks: %d\n\n",ntasks);
     printf("##########################################################\n\n");
     printf("                --> BEFORE COMMUNICATION <--\n\n");
   }

   /*=============================================================*/
   /* Memory allocation.                                          */ 
   sendbuff=(double *)malloc(sizeof(double)*buffsize);
   recvbuff=(double **)malloc(sizeof(double *)*ntasks);
   recvbuff[0]=(double *)malloc(sizeof(double)*ntasks*buffsize);
   for(i=1;i<ntasks;i++)recvbuff[i]=recvbuff[i-1]+buffsize;
   
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
   printf("Task %d : Sum of vector = %e \n",taskid,buffsum);
   
   /*===============================================================*/
   /* Communication.                                                */
              
   inittime = MPI_Wtime();

   ierr=MPI_Allgather(sendbuff,buffsize,MPI_DOUBLE,
                      recvbuff[0],buffsize,MPI_DOUBLE,
                      MPI_COMM_WORLD);
       
   totaltime = MPI_Wtime() - inittime;
   
   /*===============================================================*/
   /* Print out after communication.                                */
   
   if ( taskid == 0 ){
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
     free(recvbuff[0]);
     free(recvbuff);
     free(sendbuff);

   /*===============================================================*/
   /* MPI finalisation.                                             */
   MPI_Finalize();

}


