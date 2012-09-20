/*######################################################################

 Example 9a : MPI::COMM_WORLD.Gather

 Description:

   Example 9a is identical to exemple 9 except that the send and
   receive buffer are placed in the same variable.

             <----------  buff ---------->
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
        2 #  CC  #      #      #      #      #        BEFORE
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

             <----------  buff ---------->
          ####################################
          #      #      #      #      #      #
        0 #  AA  #  BB  #  CC  #  DD  #  EE  #
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
   double       **buff,buffsum;
   double       inittime,totaltime;

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
     printf(" Example 9a \n\n");
     printf(" Collective Communication : MPI::COMM_WORLD.Gather \n\n");
     printf(" Vector size: %d\n",buffsize);
     printf(" Number of tasks: %d\n\n",ntasks);
     printf("##########################################################\n\n");
     printf("                --> BEFORE COMMUNICATION <--\n\n");
   }

   /*=============================================================*/
   /* Memory allocation.                                          */
   buff = new double*[ntasks];
   buff[0] = new double[ntasks*buffsize];
   for(i=1;i<ntasks;i++)buff[i]=buff[i-1]+buffsize;

   /*=============================================================*/
   /* Vectors and/or matrices initalisation.                      */
   srand((unsigned)time( NULL ) + taskid);
   for(i=0;i<buffsize;i++){
       buff[0][i]=(double)rand()/RAND_MAX;
   }

   /*==============================================================*/
   /* Print out before communication.                              */

   MPI::COMM_WORLD.Barrier();

   buffsum=0.0;
   for(i=0;i<buffsize;i++){
     buffsum=buffsum+buff[0][i];
   }
   printf("Task %d : Sum of vector elements= %e \n",taskid,buffsum);

   MPI::COMM_WORLD.Barrier();

   /*===============================================================*/
   /* Communication.                                                */

   inittime = MPI::Wtime();

   MPI::COMM_WORLD.Gather(buff[0],buffsize,MPI::DOUBLE,
                          buff[0],buffsize,MPI::DOUBLE,
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
         buffsum=buffsum+buff[itask][i];
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
   delete [] buff[0];
   delete [] buff;

   /*===============================================================*/
   /* MPI finalisation.                                             */
   MPI::Finalize();

}


