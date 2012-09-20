/*######################################################################

 Example 3 : MPI_Bcast

 Description:
   This program is a simple example of MPI_Bcast usage. This call
   is used to broadcast data from one task to all the other all
   at once.
   
   The content of the vector "buff" (size=buffsize) on task 0, is 
   initialized with random values and then sent to all other tasks.
   Before and after the broadcast, each task calculates the sum of 
   all elements of buff and print it out on screen.
   
   The size of the vecteur (buffsize) is given as an argument to
   the program at run time.
   
    
                buff                buff
                                
              ########            ########
              #      #            #      #
          0   # ABCD #            # ABCD #
              #      #            #      #
              ########            ########
     T        #      #            #      #
          1   # 0000 #            # ABCD #
     a        #      #            #      #
              ########            ########
     s        #      #            #      #
          2   # 0000 #            # ABCD #
     k        #      #            #      #
              ########            ########
     s        #      #            #      #
          3   # 0000 #            # ABCD #
              #      #            #      #
              ########            ########
              #      #            #      #
          4   # 0000 #            # ABCD #
              #      #            #      #
              ########            ########
              
               BEFORE               AFTER
               

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

int main(int argc,char** argv){

   int          taskid, ntasks;
   int          ierr,i,j,itask;
   int	        buffsize;
   double       *buff,buffsum;
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
   
   /*=============================================================*/
   /* Memory allocation.                                          */ 
   buff=(double *)malloc(sizeof(double)*buffsize);

   /*===============================================================*/
   /* Printing out the description of the example.                  */
   if ( taskid == 0 ){
     printf("\n\n\n");
     printf("##########################################################\n\n");
     printf(" Example 3 \n\n");
     printf(" Collective Communication : MPI_Bcast \n\n");
     printf(" Vector size: %d\n",buffsize);
     printf(" Number of tasks: %d\n\n",ntasks);
     printf("##########################################################\n\n");
     printf("                --> BEFORE COMMUNICATION <--\n\n");
   }
   
   ierr=MPI_Barrier(MPI_COMM_WORLD);

   /*=============================================================*/
   /* Vectors and/or matrices initalisation.                      */
   if( taskid == 0 ){
     srand((unsigned)time( NULL ) + taskid);
     for(i=0;i<buffsize;i++)buff[i]=(double)rand()/RAND_MAX;
   }
   else{
     for(i=0;i<buffsize;i++)buff[i]=0.0;
   }
   
   /*==============================================================*/
   /* Print out before communication.                              */

   buffsum=0.0;
   for(i=0;i<buffsize;i++){
     buffsum += buff[i];
   }

   printf("Task %d : Sum of vector buff= %e\n",taskid,buffsum);  
   
   /*===============================================================*/
   /* Communication.                                                */

   inittime = MPI_Wtime();
   
   ierr=MPI_Bcast(buff,buffsize,MPI_DOUBLE,0,MPI_COMM_WORLD);
  
   totaltime = MPI_Wtime() - inittime;
   
   /*===============================================================*/
   /* Print out after communication.                                */

   buffsum=0.0;
   for(i=0;i<buffsize;i++){
     buffsum += buff[i];
   } 
     
   if ( taskid == 0 ){
     printf("##########################################################\n\n");
     printf("                --> AFTER COMMUNICATION <-- \n\n");
   }
   ierr=MPI_Barrier(MPI_COMM_WORLD);
   
   printf("Task %d : Sum of vector buff= %e\n",taskid,buffsum);
   
   ierr=MPI_Barrier(MPI_COMM_WORLD);

   if(taskid==0){
     printf("\n");
     printf("##########################################################\n\n");
     printf(" Communication time : %f seconds\n\n",totaltime);  
     printf("##########################################################\n\n");
   }

   /*===============================================================*/
   /* Free the allocated memory.                                    */
   free(buff);
   
   /*===============================================================*/
   /* MPI finalisation.                                             */
   MPI_Finalize();
   
}
