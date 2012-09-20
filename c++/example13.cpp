/*######################################################################

 Example 13 : Creation of process groups

 Description:
   This example shows how to use groups in MPI. To simplify,
   we fix the number of processors to 8. These processors are
   separated in two groups. The output gives the rank number
   of each processor before the separation (from 0 to 7) and
   after separation (from 0 to 3 for each group).

   This example has been taken from the URL address:
   http://www.msi.umn.edu/tutorial/MPI
   
 Auteur: Steve Allen
         Centre de Calcul scientifique
         Universite de Sherbrooke

 Modified by : François Guertin
               RQCHP, Université de Montréal

 Last update: August 2009

######################################################################*/

#include <mpi.h>
#include <iostream>

const int NPROCS=8;

int main(int argc,char** argv)
{
  int            rank, new_rank, sendbuf_rank, recvbuf_sum;
  int            ranks1[4]={0,1,2,3}, ranks2[4]={4,5,6,7};
  MPI::Group     orig_group, new_group1, new_group2;
  MPI::Intracomm new_comm, new_comm1, new_comm2;

  MPI::Init(argc, argv);
  rank = MPI::COMM_WORLD.Get_rank();
  sendbuf_rank = rank;

  //********** With MPI_Comm_create *********

  // Extract the original group handle 

  orig_group = MPI::COMM_WORLD.Get_group();

  // Divide tasks into two distinct groups
   
  new_group1 = orig_group.Incl(NPROCS/2, ranks1);
  new_group2 = orig_group.Incl(NPROCS/2, ranks2);

  // Create new communicators

  new_comm1 = MPI::COMM_WORLD.Create( new_group1 );
  new_comm2 = MPI::COMM_WORLD.Create( new_group2 );

  /* Keep communicatosr different than MPI_COMM_NULL based on the rank */
  if( rank < NPROCS/2 ){
    new_comm = new_comm1;
  } else {
    new_comm = new_comm2;
  }

  // Perform collective communications
  new_comm.Allreduce(&sendbuf_rank, &recvbuf_sum, 1, MPI_INT, MPI_SUM);
  new_rank = new_comm.Get_rank();

  std::cout << "Create: rank= "<<rank<<" newrank= "<<new_rank<<" recvbuf_sum="<<recvbuf_sum;
  std::cout << std::endl;

  new_comm.Free();
  new_group1.Free();
  new_group2.Free();


  // ********** With MPI_Comm_split *********

  // Choose the "color" of each process 
  int color;
  if( rank < NPROCS/2 ){
    color = 0;
  } else {
    color = 1;
  }

  /* Create new communicators */
  MPI::Intracomm split_comm;
  split_comm = MPI::COMM_WORLD.Split (color, rank);

  /* Perform collective communications */
  split_comm.Allreduce(&sendbuf_rank, &recvbuf_sum, 1, MPI_INT, MPI_SUM);
  new_rank = split_comm.Get_rank();
   
  std::cout << "Split: rank= "<<rank<<" newrank= "<<new_rank<<" recvbuf_sum="<<recvbuf_sum << '\n';

  split_comm.Free();


  MPI::Finalize();
} 
