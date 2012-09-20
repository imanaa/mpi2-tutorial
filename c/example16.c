/*######################################################################

 Exemple 16 : Use of Request

 Description:
   This example shows how to use Irecv and Wait in MPI for
   unsynchronized communication. The processor of rank 0 is
   called the master processor while the other processor are the slaves.
   Such an approach is useful when it is hard to assure a goad balance
   load. The total number of task have to be very large. As soon
   as a slave processor is done with a task, it contacts the master
   to receive a new task.
   
 Auteur: Steve Allen
         Centre de Calcul scientifique
         Universite de Sherbrooke

 Last update: October 2007

######################################################################*/

#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int main(int argc,char** argv)
{
   int         master = 0;
   int         finish_flag = -1;
   int         myrank, nprocs;
   int         nvol, nptc, totptc;
   int         i, nfinish, *buf_recv, nslaves, last;
   float       x, y, z;
   MPI_Request *request;
   MPI_Status  status;

   /* Initialisation */
   MPI_Init(&argc, &argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
   MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

   /* The number of cpus have to be at least 2 */
   if( nprocs < 2 )
   {
       printf("%d cpus is not enough for this example\n", nprocs);
       MPI_Finalize();
       exit( 1 );
    }

   nvol = 0;
   if (myrank == master){

      /* Operations for the master */
      if( argc > 1 )
      {
          nvol = atoi( argv[1] );
      }
      if( nvol <= 0 ) nvol = 6000;
      printf("Generating nvol=%d with %d procs\n", nvol, nprocs-1);

      nslaves = nprocs - 1;
      buf_recv = (int*) malloc( nslaves * sizeof(int) );
      request = (MPI_Request*) malloc( nslaves * sizeof(MPI_Request) );
      last = 0;
      nfinish = 0;

      /* Send a message indicating the start of work */
      for( i = 0; i < nslaves; i++ )
      {
         if( last < nvol )
         {
            last++;
            MPI_Send( &last, 1, MPI_INT, i+1, 0, MPI_COMM_WORLD );
            MPI_Irecv( &(buf_recv[i]), 1, MPI_INT, i+1, i+1, MPI_COMM_WORLD, &(request[i]) );
         }
         else
         {
            MPI_Send( &finish_flag, 1, MPI_INT, i+1, 0, MPI_COMM_WORLD );
            nfinish++;
         }
      }
      while( nfinish < nslaves )
      {
         /* Waiting for a message coming from the slave cpus */
         MPI_Waitany( nslaves, request, &i, &status );
         if( last < nvol )
         {
            /* If there is still tasks to be accomplished */
            last++;
            MPI_Send( &last, 1, MPI_INT, i+1, 0, MPI_COMM_WORLD );
            MPI_Irecv( &(buf_recv[i]), 1, MPI_INT, i+1, i+1, MPI_COMM_WORLD, &(request[i]) );
         }
         else
         {
            /* If every task has been accomplished */
            MPI_Send( &finish_flag, 1, MPI_INT, i+1, 0, MPI_COMM_WORLD );
            nfinish++;
         }
      }
      free( request );
      free( buf_recv );

   } else {

      /* Tasks for the slaves */
      totptc = 0;
      nvol = 0;
      buf_recv = (int*) malloc( sizeof(int) );
      MPI_Recv( buf_recv, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status );

      /* Until every task are not done */
      while( *buf_recv != finish_flag )
      {
         /* Accomplishing a task : Generating 3D positions */
         nptc = (int) ( ( (float) rand() ) / RAND_MAX * 2500 ) + 2500;
         for( i = 0; i < nptc; i++ )
         {
            x = ( (float) rand() ) / RAND_MAX;
            y = ( (float) rand() ) / RAND_MAX;
            z = ( (float) rand() ) / RAND_MAX;
         }
         totptc += nptc;
         nvol++;
         MPI_Send( &nptc, 1, MPI_INT, 0, myrank, MPI_COMM_WORLD );
         MPI_Recv( buf_recv, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status );
      }
      free( buf_recv );

      printf("Generated %d vol with a total of %d ptcs\n", nvol, totptc);
   }

   MPI_Barrier( MPI_COMM_WORLD );
   MPI_Finalize();
} 
