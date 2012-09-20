/*######################################################################

 Example 6 : MPI::COMM_WORLD.Isend/Irecv

 Description:
   Examples 5 and 6 demonstrate the difference between blocking
   and non-blocking point-to-point communication.
          Example 5: MPI::COMM_WORLD.Send/Recv (blocking)
          Example 6: MPI::COMM_WORLD.Isend/Irecv (non-blocking)

              sendbuff   recvbuff       sendbuff   recvbuff

              ########   ########       ########   ########
              #      #   #      #       #      #   #      #
          0   #  AA  #   #      #       #  AA  #   #  EE  #
              #      #   #      #       #      #   #      #
              ########   ########       ########   ########
     T        #      #   #      #       #      #   #      #
          1   #  BB  #   #      #       #  BB  #   #  AA  #
     a        #      #   #      #       #      #   #      #
              ########   ########       ########   ########
     s        #      #   #      #       #      #   #      #
          2   #  CC  #   #      #       #  CC  #   #  BB  #
     k        #      #   #      #       #      #   #      #
              ########   ########       ########   ########
     s        #      #   #      #       #      #   #      #
          3   #  DD  #   #      #       #  DD  #   #  CC  #
              #      #   #      #       #      #   #      #
              ########   ########       ########   ########
              #      #   #      #       #      #   #      #
          4   #  EE  #   #      #       #  EE  #   #  DD  #
              #      #   #      #       #      #   #      #
              ########   ########       ########   ########

                     BEFORE                    AFTER

   Each task transfers a vector of random numbers (sendbuff) to the
   next task (taskid+1). The last task transfers it to task 0.
   Consequently, each task receives a vector from the preceding task
   and puts it in recvbuff.

   This example shows that Isend and Irecv are much more
   appropriate to accomplish this work.

   Isend and Irecv are non-blocking, meaning that the function
   call returns before the communication is completed. Deadlock then
   becomes impossible with non-blocking communication, but other
   precautions must be taken when using them. In particular you will
   want to be sure at a certain point, that your data has effectively
   arrived! You will then place an Wait call for each send and/or
   receive you want to be completed before advancing in the program.

   It is clear that in using non-blocking call in this example, all
   the exchanges between the tasks occur at the same time.

   Before the communication, task 0 gather the sum of all the vectors
   to sent from each tasks, and prints them out. Similarly after
   the communication, task 0 gathers all the sum of the vectors received
   by each task and prints them out along with the communication times.

   Example 5 show how to use blocking communication (Send and Recv) to
   accomplish the same work much less efficiently.

   The size of the vecteur (buffsize) is given as an argument to
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
   MPI::Request send_request,recv_request;
   int          ierr,i,j,itask,recvtaskid;
   int          buffsize;
   double       *sendbuff,*recvbuff;
   double       sendbuffsum,recvbuffsum;
   double       sendbuffsums[1024],recvbuffsums[1024];
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
     printf(" Example 6 \n\n");
     printf(" Point-to-point Communication: MPI::COMM_WORLD.Isend/Irecv \n\n");
     printf(" Vector size: %d\n",buffsize);
     printf(" Number of tasks: %d\n\n",ntasks);
     printf("##########################################################\n\n");
     printf("                --> BEFORE COMMUNICATION <--\n\n");
   }

   /*=============================================================*/
   /* Memory allocation.                                          */
   sendbuff = new double[buffsize];
   recvbuff = new double[buffsize];

   /*=============================================================*/
   /* Vectors and/or matrices initalisation.                      */
   srand((unsigned)time( NULL ) + taskid);
   for(i=0;i<buffsize;i++){
     sendbuff[i]=(double)rand()/RAND_MAX;
   }

   /*==============================================================*/
   /* Print out before communication.                              */

   sendbuffsum=0.0;
   for(i=0;i<buffsize;i++){
     sendbuffsum += sendbuff[i];
   }
   MPI::COMM_WORLD.Gather(&sendbuffsum,1,MPI::DOUBLE,
                          sendbuffsums,1, MPI::DOUBLE,
                          0);
   if(taskid==0){
     for(itask=0;itask<ntasks;itask++){
       recvtaskid=itask+1;
       if(itask==(ntasks-1))recvtaskid=0;
       printf("Task %d : Sum of vector sent to %d = %e\n",
               itask,recvtaskid,sendbuffsums[itask]);
     }
   }

   /*===============================================================*/
   /* Communication.                                                */

   inittime = MPI::Wtime();

   if ( taskid == 0 ){
     send_request = MPI::COMM_WORLD.Isend(sendbuff,buffsize,MPI::DOUBLE,
                                          taskid+1,0);
     recv_request = MPI::COMM_WORLD.Irecv(recvbuff,buffsize,MPI::DOUBLE,
                                          ntasks-1,MPI::ANY_TAG);
     recvtime = MPI::Wtime();
   }
   else if( taskid == ntasks-1 ){
     send_request = MPI::COMM_WORLD.Isend(sendbuff,buffsize,MPI::DOUBLE,
                                         0,0);
     recv_request = MPI::COMM_WORLD.Irecv(recvbuff,buffsize,MPI::DOUBLE,
                                          taskid-1,MPI::ANY_TAG);
     recvtime = MPI::Wtime();
   }
   else{
     send_request = MPI::COMM_WORLD.Isend(sendbuff,buffsize,MPI::DOUBLE,
                                          taskid+1,0);
     recv_request = MPI::COMM_WORLD.Irecv(recvbuff,buffsize,MPI::DOUBLE,
                                          taskid-1,MPI::ANY_TAG);
     recvtime = MPI::Wtime();
   }
   send_request.Wait(status);
   recv_request.Wait(status);

   totaltime=MPI::Wtime() - inittime;

   /*===============================================================*/
   /* Print out after communication.                                */

   recvbuffsum=0.0;
   for(i=0;i<buffsize;i++){
     recvbuffsum += recvbuff[i];
   }

   MPI::COMM_WORLD.Gather(&recvbuffsum,1,MPI::DOUBLE,
                          recvbuffsums,1, MPI::DOUBLE,
                          0);

   MPI::COMM_WORLD.Gather(&recvtime,1,MPI::DOUBLE,
                          recvtimes,1, MPI::DOUBLE,
                          0);

   if(taskid==0){
     printf("##########################################################\n\n");
     printf("                --> AFTER COMMUNICATION <-- \n\n");
     for(itask=0;itask<ntasks;itask++){
       printf("Task %d : Sum of received vector= %e : Time=%f seconds\n",
               itask,recvbuffsums[itask],recvtimes[itask]);
     }
     printf("\n");
     printf("##########################################################\n\n");
     printf(" Communication time : %f seconds\n\n",totaltime);
     printf("##########################################################\n\n");
   }

   /*===============================================================*/
   /* Free the allocated memory.                                    */
   delete [] recvbuff;
   delete [] sendbuff;

   /*===============================================================*/
   /* MPI finalisation.                                             */
   MPI::Finalize();

}


