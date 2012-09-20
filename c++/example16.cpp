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

#include "mpi2c++/mpi++.h"
#include <iostream>
#include <cstdlib>

int main( int argc,char* argv[] )
{
   int          master = 0;
   int          finish_flag = -1;
   int          myrank, nprocs;
   int          nvol, nptc, totptc;
   int          i, nfinish, *buf_recv, nslaves, last;
   float        x, y, z;
   MPI::Request *request;

   /* Initialisation */
   MPI::Init( argc, argv );
   myrank = MPI::COMM_WORLD.Get_rank( );
   nprocs = MPI::COMM_WORLD.Get_size( );

   /* The number of cpus must at least be two */
   if( nprocs < 2 )
   {
       std::cout << nprocs << " cpus is not enough for this example" << std::endl;
       MPI::Finalize();
       exit( 1 );
    }

   nvol = 0;
   if (myrank == master)
   {

      /* Tasks of the master */
      if( argc > 1 )
      {
          nvol = atoi( argv[1] );
      }
      if( nvol <= 0 ) nvol = 6000;
      nslaves = nprocs - 1;
      std::cout << "Generating nvol="<<nvol<<" with "<<nslaves<<" procs"<<std::endl;

      buf_recv = new int[ nslaves ];
      request = new MPI::Request[ nslaves ];
      last = 0;
      nfinish = 0;

      /* Sending the first tasks to each slaves */
      for( i = 0; i < nslaves; i++ )
      {
         if( last < nvol )
         {
            last++;
            MPI::COMM_WORLD.Send( &last, 1, MPI::INT, i+1, 0 );
            request[i] = MPI::COMM_WORLD.Irecv( &(buf_recv[i]), 1, MPI::INT, i+1, i+1 );
         }
         else
         {
            MPI::COMM_WORLD.Send( &finish_flag, 1, MPI::INT, i+1, 0 );
            nfinish++;
         }
      }
      while( nfinish < nslaves )
      {
         /* Waiting for any cpus to be done */
         i = MPI::Request::Waitany( nslaves, request );
         if( last < nvol )
         {
            /* If there still task to do */
            last++;
            MPI::COMM_WORLD.Send( &last, 1, MPI::INT, i+1, 0 );
            request[i] = MPI::COMM_WORLD.Irecv( &(buf_recv[i]), 1, MPI::INT, i+1, i+1 );
         }
         else
         {
            /* If all the tasks are completed */
            MPI::COMM_WORLD.Send( &finish_flag, 1, MPI::INT, i+1, 0 );
            nfinish++;
         }
      }
      delete [] request;
      delete [] buf_recv;

   } else {

      /* Tasks for slaves */
      totptc = 0;
      nvol = 0;
      buf_recv = new int;
      MPI::COMM_WORLD.Recv( buf_recv, 1, MPI::INT, 0, 0 );

      /* until all the tasks are note completed */
      while( *buf_recv != finish_flag )
      {
         /* Doing a task: Generating a list of particles positionss */
         nptc = (int) ( ( (float) rand() ) / RAND_MAX * 2500 ) + 2500;
         for( i = 0; i < nptc; i++ )
         {
            x = ( (float) rand() ) / RAND_MAX;
            y = ( (float) rand() ) / RAND_MAX;
            z = ( (float) rand() ) / RAND_MAX;
         }
         totptc += nptc;
         nvol++;
         MPI::COMM_WORLD.Send( &nptc, 1, MPI::INT, 0, myrank );
         MPI::COMM_WORLD.Recv( buf_recv, 1, MPI::INT, 0, 0 );
      }
      delete buf_recv;

      std::cout << "Generated "<<nvol<<" vol with a total of "<<totptc<<" ptcs"<<std::endl;
   }

   MPI::COMM_WORLD.Barrier( );
   MPI::Finalize();
}
