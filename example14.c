/*######################################################################

 Example 14 : Use of Pack

 Description:
   This example shows how to use packing to tranfer many variables
   in an unique communication. Here, we want to transfer two floating
   point variables and an integer. These are placed in a table
   variable which is used to pack the values. This table is tranfer
   from the processor 0 to all the other processors. Before the
   transmission, the processor 0 gives the value of the variable
   and after the communication, the other processes return their
   values.
   
   This example has been taken form :
               A User's Guide to MPI, Peter S. Pacheco (1998)

 Auteur: Steve Allen
         Centre de Calcul scientifique
         Universite de Sherbrooke

 Last update: September 2005

######################################################################*/

#include <stdio.h>
#include "mpi.h"

int main(int argc,char** argv)
{
   int     myrank;
   int     position;
   int     n;
   float   a, b;
   char    buffer[100];

   MPI_Init(&argc, &argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

   if (myrank == 0){
      n = 4;
      a = 1.;
      b = 2.;
      printf("Rank=%d, a= %f, b= %f, and n=%d\n", myrank, a, b, n);
      position = 0;

    // packing
      MPI_Pack(&a, 1, MPI_FLOAT, buffer, 100, &position, MPI_COMM_WORLD);
      MPI_Pack(&b, 1, MPI_FLOAT, buffer, 100, &position, MPI_COMM_WORLD);
      MPI_Pack(&n, 1, MPI_INT, buffer, 100, &position, MPI_COMM_WORLD);

    // communication
      MPI_Bcast(buffer, 100, MPI_PACKED, 0, MPI_COMM_WORLD);

   } else {
    // communication
      MPI_Bcast(buffer, 100, MPI_PACKED, 0, MPI_COMM_WORLD);
      position = 0;

    // unpacking
      MPI_Unpack(buffer, 100, &position, &a, 1, MPI_FLOAT, MPI_COMM_WORLD);
      MPI_Unpack(buffer, 100, &position, &b, 1, MPI_FLOAT, MPI_COMM_WORLD);
      MPI_Unpack(buffer, 100, &position, &n, 1, MPI_INT, MPI_COMM_WORLD);
      printf("Rank=%d, a= %f, b=%f, and n=%d\n", myrank, a, b, n);
   }

   MPI_Finalize();
} 
