#include "mpi2c++/mpi++.h"
#include <iostream>
#include <cstdlib>

using namespace std;

/* In this program we will communicate with MPI objects that are described
   by the following structure. It is important that the size memory of the
   elements of the strucure is fixed */

enum ParticuleSpecies { H2, O2 };

struct Particule
{
  public:
    int species;
    double x;
    double y;
    double z;
};

/* Declaration of the function that is used to build the derived datatype 
   for the preceding structure */
void BuildMPIParticuleType ( Particule& ptc, MPI::Datatype* MPIParticuleType );


int main( int argc, char **argv )
{
    const int     nombre_ptc = 100;
    int           myrank;
    Particule    *molecules, *ptc_ptr;
    MPI::Datatype MPIParticuleType;

    MPI::Init ( argc, argv );
    myrank = MPI::COMM_WORLD.Get_rank( );
    molecules = new Particule[ nombre_ptc ];

    /* Process rank 0 establishes the list of particles */
    if( myrank == 0 )
    {
        ptc_ptr = molecules;
        for ( int iter = 0; iter < nombre_ptc; iter++ )
        {
            ptc_ptr -> species = ( int ) H2;
            ptc_ptr -> x = ( (double) rand( ) ) / RAND_MAX;
            ptc_ptr -> y = ( (double) rand( ) ) / RAND_MAX;
            ptc_ptr -> z = ( (double) rand( ) ) / RAND_MAX;
            ptc_ptr++;
        }
    }

    /* List of particles is transmit to each processor */
    BuildMPIParticuleType ( *molecules, &MPIParticuleType );
    MPI::COMM_WORLD.Bcast ( molecules, nombre_ptc, MPIParticuleType, 0 );

    /* Check the result */
    if ( myrank == 1 )
    {
        ptc_ptr = molecules;
        cout << "type=" << ptc_ptr -> species << ", pos=(" << ptc_ptr -> x
                 << " " << ptc_ptr -> y << " " << ptc_ptr -> z << ")" << endl;
    }

    /* Clean memory */
    delete[] molecules;
    MPI::Finalize( );
}

// Definition of the function used to build the appropriate derived datatype
void BuildMPIParticuleType ( Particule& ptc, MPI::Datatype* MPIParticuleType )
{
    const int     nblock = 2;
    int           block_length[nblock] = {1, 3};
    MPI::Datatype typelist[nblock] = {MPI::INT, MPI::DOUBLE};
    MPI::Aint     displacement[nblock];
    MPI::Aint     origin, addresses[nblock];

    origin = MPI::Get_address ( &ptc );
    addresses[0] = MPI::Get_address ( &( ptc.species ) );
    addresses[1] = MPI::Get_address ( &( ptc.x ) );

    /* Note that we compute displacement fo the element x but not for the
       y and z elements. We can do so because y and z are of the same type
       as x and are contiguous in memory. We give a block length of 3 */

    for ( int i = 0; i < nblock; i++ )
    {
        displacement[i] = addresses[i] - origin;
    }

    *MPIParticuleType = MPI::Datatype::Create_struct ( nblock, block_length,
                                                       displacement, typelist );
    /* In this example we use Create_struct. But there is other functions that
       can be used to create a derived datatype.
       See book MPI - The Complete Reference */

    MPIParticuleType -> Commit( );
    /* It is important after creating a datatype to call the Commit function
       so you can use this datatype */
}
