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

#include "mpi2c++/mpi++.h"
#include <iostream>

int main(int argc,char** argv)
{
   int            myrank;
   int            position;
   int            n, fl_size, int_size;
   float          a, b;
   char           buffer[100];
   MPI::Datatype  datapack = MPI_PACKED;

   MPI::Init(argc, argv);
   myrank = MPI::COMM_WORLD.Get_rank();
   fl_size = sizeof(a);
   int_size = sizeof(n);

   if (myrank == 0){
      n = 4;
      a = 1.;
      b = 2.;
      std::cout << "Rank="<<myrank<<", a="<<a<<", b="<<b<<", and n="<<n<<std::endl;
      position = 0;

     // packing
      datapack.Pack(&a, fl_size, buffer, 100, position, MPI::COMM_WORLD);
      datapack.Pack(&b, fl_size, buffer, 100, position, MPI::COMM_WORLD);
      datapack.Pack(&n, int_size, buffer, 100, position, MPI::COMM_WORLD);

     // Communication
      MPI::COMM_WORLD.Bcast(buffer, 100, datapack, 0);

   } else {

     // Communication
      MPI::COMM_WORLD.Bcast(buffer, 100, datapack, 0);
      position = 0;

     // Unpacking
      datapack.Unpack(buffer, 100, &a, fl_size, position, MPI::COMM_WORLD);
      datapack.Unpack(buffer, 100, &b, fl_size, position, MPI::COMM_WORLD);
      datapack.Unpack(buffer, 100, &n, int_size, position, MPI::COMM_WORLD);

      std::cout << "Rank="<<myrank<<", a="<<a<<", b="<<b<<", and n="<<n<<std::endl;
   }

   MPI::Finalize();
}
