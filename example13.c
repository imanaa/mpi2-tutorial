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

#include <stdio.h>
#include <mpi.h>
#define NPROCS 8

int main(int argc,char** argv)
{
   int        rank, new_rank, sendbuf_rank, recvbuf_sum;
   int        ranks1[4]={0,1,2,3},ranks2[4]={4,5,6,7};
   MPI_Group  orig_group, new_group1, new_group2;
   MPI_Comm   new_comm, new_comm1, new_comm2;
   MPI_Comm   split_comm;
   int        color;

   MPI_Init(&argc, &argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   sendbuf_rank = rank;

   /*********** With MPI_Comm_create *********/

   /* Extract the original group handle */

   MPI_Comm_group(MPI_COMM_WORLD, &orig_group);

   /* Divide tasks into two distinct groups based upon rank */

   MPI_Group_incl(orig_group, NPROCS/2, ranks1, &new_group1);
   MPI_Group_incl(orig_group, NPROCS/2, ranks2, &new_group2);

   /* Create new communicators */

   MPI_Comm_create(MPI_COMM_WORLD, new_group1, &new_comm1);
   MPI_Comm_create(MPI_COMM_WORLD, new_group2, &new_comm2);

   /* Keep communicators different than MPI_COMM_NULL based on the rank */
   if( rank < NPROCS/2 ){
     new_comm = new_comm1;
   } else {
     new_comm = new_comm2;
   }
   
   /* Perform collective communications */
   MPI_Allreduce(&sendbuf_rank, &recvbuf_sum, 1, MPI_INT, MPI_SUM, new_comm);
   MPI_Comm_rank(new_comm, &new_rank);

   printf("Create: rank= %d newrank= %d recvbuf_sum= %d\n",rank,new_rank,recvbuf_sum);

   MPI_Comm_free(&new_comm);
   MPI_Group_free(&new_group1);
   MPI_Group_free(&new_group2);

   /********** With MPI_Comm_split *********/
   
   /* Choose the "color" of each process */
   if( rank < NPROCS/2 ){
     color = 0;
   } else {
     color = 1;
   }

   /* Create new communicators */
   MPI_Comm_split (MPI_COMM_WORLD, color, rank, &split_comm);

   /* Perform collective communications */
   MPI_Allreduce(&sendbuf_rank, &recvbuf_sum, 1, MPI_INT, MPI_SUM, split_comm);
   MPI_Comm_rank(split_comm, &new_rank);
   
   printf("Split:   rank= %d newrank= %d recvbuf_sum= %d\n",rank,new_rank,recvbuf_sum);

   MPI_Comm_free(&split_comm);
 

   MPI_Finalize();
} 
