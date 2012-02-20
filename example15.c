#include "mpi.h"
#include <stdlib.h>

/* In this program we will communicate with MPI objects that are described
   by the following structure. It is important that the size memory of the
   elements of the strucure is fixed */

enum ParticuleSpecies { H2, O2 };

typedef struct particule
{
    int    species;
    double x;
    double y;
    double z;
} Particule;

int main( int argc, char **argv )
{
#define nombre_ptc 100
    int          myrank;
    int          iter;
    Particule   *molecules, *ptc_ptr;
    MPI_Datatype MPIParticuleType;
    void BuildMPIParticuleType( Particule ptc, MPI_Datatype *MPIParticuleType );

    MPI_Init (&argc, &argv);
    MPI_Comm_rank ( MPI_COMM_WORLD, &myrank );
    molecules = ( Particule * ) malloc( sizeof( Particule ) * nombre_ptc );

    /* Process rank 0 establishes the list of particles */
    if ( myrank == 0 )
    {
        ptc_ptr = molecules;
        for ( iter = 0; iter < nombre_ptc; iter++ )
        {
            ptc_ptr -> species = ( int ) H2;
            ptc_ptr -> x = ( (double) rand( ) ) / RAND_MAX;
            ptc_ptr -> y = ( (double) rand( ) ) / RAND_MAX;
            ptc_ptr -> z = ( (double) rand( ) ) / RAND_MAX;
            ptc_ptr++;
        }
    }

    /* List of particles is transmit to each processor */
    BuildMPIParticuleType( molecules[0], &MPIParticuleType );
    MPI_Bcast( molecules, nombre_ptc, MPIParticuleType, 0, MPI_COMM_WORLD );

    /* Check the result */
    if ( myrank == 1 )
    {
        ptc_ptr = molecules;
        printf( "type=%d, pos=(%f,%f,%f)\n", ptc_ptr -> species, ptc_ptr -> x,
                 ptc_ptr -> y, ptc_ptr -> z );
    }

    /* Clean memory */
    free ( molecules );
    MPI_Finalize( );
}

// Definition of the function used to build the appropriate derived datatype
void BuildMPIParticuleType( Particule ptc, MPI_Datatype *MPIParticuleType )
{
#define nblock 2
    int          i;
    int          block_length[nblock] = {1, 3};
    MPI_Datatype typelist[nblock] = {MPI_INT, MPI_DOUBLE};
    MPI_Aint     displacement[nblock];
    MPI_Aint     origin, addresses[nblock];

    MPI_Address( &ptc, &origin );
    MPI_Address( &( ptc.species ), &( addresses[0] ) );
    MPI_Address( &( ptc.x ), &( addresses[1] ) );

    /* Note that we compute displacement fo the element x but not for the
       y and z elements. We can do so because y and z are of the same type
       as x and are contiguous in memory. We give a block length of 3 */

    for( i = 0; i < nblock; i++ )
    {
        displacement[i] = addresses[i] - origin;
    }

    MPI_Type_struct( nblock, block_length, displacement, typelist,
                     MPIParticuleType );
    /* In this example we use Type_struct. But there is other functions that
       can be used to create a derived datatype.
       See book MPI - The Complete Reference */

    MPI_Type_commit( MPIParticuleType );
    /* It is important after creating a datatype to call the Commit function
       so you can use this datatype */
}

