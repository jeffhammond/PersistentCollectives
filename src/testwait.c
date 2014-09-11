#include "internal.h"

typedef PCMPI_Request void*;

typedef struct
{
    int num;
    MPI_Request * requests;
}
PCMPI_Request_impl;

int PCMPI_Test(PCMPI_Request *request, int * flag, MPI_Status *status)
{

    return MPI_SUCCESS;
}

int PCMPI_Wait(PCMPI_Request *request, MPI_Status *status)
{

    return MPI_SUCCESS;
}

int PCMPI_Request_free(PCMPI_Request *request)
{

    return MPI_SUCCESS;
}

