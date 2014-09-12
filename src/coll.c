#include "internal.h"

int MPIX_Gather_init(const void *sendbuf, MPI_Count sendcount, MPI_Datatype sendtype,
                     void *recvbuf, MPI_Count recvcount, MPI_Datatype recvtype, int root,
                     int tag, MPI_Comm comm, PCMPI_Request *request)
{
    int is_intercomm;
    MPI_Comm_test_inter(comm, &is_intercomm);
    if (is_intercomm) {
        PersColl_Error("PersColl does not support intercommunicators yet.\n");
    }

    if (sendbuf==MPI_IN_PLACE) {
        PersColl_Error("PersColl does not support in-place collectives. \n");
    }

    int size, rank;
    MPI_Comm_size(comm, &size);
    MPI_Comm_rank(comm, &rank);

    PCMPI_Request_impl * impl = (PCMPI_Request_impl*)request;

    impl->num = (rank==root ? size+1 : 1);
    impl->requests = safemalloc(impl->num * sizeof(MPI_Request));
    if (rank==root) {
        for (int i=0; i<size; i++) {
            MPI_Aint lb /* unused */, recvextent;
            MPI_Type_get_extent(recvtype, &lb, &recvextent);
            /* Use tag=0 because there is perfect pair-wise matching without it. */
            MPI_Recv_init(recvbuf+i*recvextent, recvcount, recvtype,
                           i /* source */, tag /* tag */, comm, &(impl->requests[i+1]));
        }
    }
    MPI_Send_init(sendbuf, sendcount, sendtype,
                   root /* target */, tag /* tag */, comm, &(impl->requests[0]));

    return MPI_SUCCESS;
}

#if 0
{
    int nreqs = (rank==root ? size+1 : 1);
    MPI_Request * reqs = malloc(nreqs*sizeof(MPI_Request)); assert(reqs!=NULL);
    if (rank==root) {
        for (int i=0; i<size; i++) {
            MPI_Aint lb /* unused */, sendextent;
            MPI_Type_get_extent(sendtypes[i], &lb, &sendextent);
            /* Use tag=0 because there is perfect pair-wise matching without it. */
            MPIX_Isend(sendbuf+senddispls[i]*sendextent, sendcounts[i], sendtypes[i],
                       i /* target */, 0 /* tag */, comm, &reqs[i+1]);
        }
    }
    MPIX_Irecv(recvbuf, recvcount, recvtype,
               0 /* source */, 0 /* tag */, comm, &reqs[0]);
    MPI_Waitall(nreqs, reqs, MPI_STATUSES_IGNORE);
}

#endif
