#include <mpi.h>
#include <stdio.h>
#include <iostream>
#define K 10


struct mess {
    char position = 'L';
    int channel = 0;
    int status = 0;
    int T = 0;
    int rank = -1;
};

struct mess_channel_in {
    int rank;
};

struct mess_channel_out {
    int rank;
};

int main( int argc, char **argv )
{
	int rank, size;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int namelen;
	MPI_Init( &argc, &argv );
	MPI_Comm_rank( MPI_COMM_WORLD, &rank );
	MPI_Comm_size( MPI_COMM_WORLD, &size );
	MPI_Get_processor_name(processor_name,&namelen);
    char position = 'L';
    int channel = 0, status = 0, T = 0, responseCounter = 0;
    mess channelArray[K];
    



    std::cout << "Jestem " << rank << " z " << size << " na " << processor_name << std::endl;
	MPI_Finalize();
}