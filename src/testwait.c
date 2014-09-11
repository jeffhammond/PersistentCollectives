#include "internal.h"

int PCMPI_Test(PCMPI_Request *request, int * flag, MPI_Status *status)
{
    PCMPI_Request_impl * impl = (PCMPI_Request_impl*)request;
    if ((impl->num > 1) && (impl->requests != NULL)) {
        MPI_Testall(impl->num, impl->requests, flag, MPI_STATUSES_IGNORE);
    } else if (impl->win != MPI_WIN_NULL) {
        PersColl_Error("RMA not implemented yet...\n");
    } else {
        PersColl_Error("Neither PSR nor RMA was setup!\n");
    }
    return MPI_SUCCESS;
}

int PCMPI_Wait(PCMPI_Request *request, MPI_Status *status)
{
    PCMPI_Request_impl * impl = (PCMPI_Request_impl*)request;
    if ((impl->num > 1) && (impl->requests != NULL)) {
        MPI_Waitall(impl->num, impl->requests, MPI_STATUSES_IGNORE);
    } else if (impl->win != MPI_WIN_NULL) {
        PersColl_Error("RMA not implemented yet...\n");
    } else {
        PersColl_Error("Neither PSR nor RMA was setup!\n");
    }
    return MPI_SUCCESS;
}

int PCMPI_Request_free(PCMPI_Request *request)
{
    PCMPI_Request_impl * impl = (PCMPI_Request_impl*)request;
    impl->num = 0;
    if (impl->requests != NULL) {
        free(impl->requests);
    }
    if (impl->win != MPI_WIN_NULL) {
        MPI_Win_free(&(impl->win));
    }
    free(request);
    return MPI_SUCCESS;
}

