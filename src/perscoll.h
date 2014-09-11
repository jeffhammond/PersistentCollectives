#ifndef PERSCOLL_H_INCLUDED
#define PERSCOLL_H_INCLUDED

#include <mpi.h>

typedef void* PCMPI_Request;

int PCMPI_Test(PCMPI_Request *request, int * flag, MPI_Status *status);
int PCMPI_Wait(PCMPI_Request *request, MPI_Status *status);

/* TODO PCMPI_{Test,Wait}{all,any,some} */

int PCMPI_Request_free(PCMPI_Request *request);

/* Collectives */

int MPIX_Gather_init(const void *sendbuf, MPI_Count sendcount, MPI_Datatype sendtype,
                     void *recvbuf, MPI_Count recvcount, MPI_Datatype recvtype, int root,
                     int tag, MPI_Comm comm, PCMPI_Request *request);
int MPIX_Scatter_init(const void *sendbuf, MPI_Count sendcount, MPI_Datatype sendtype,
                      void *recvbuf, MPI_Count recvcount, MPI_Datatype recvtype, int root,
                      int tag, MPI_Comm comm, PCMPI_Request *request);
int MPIX_Allgather_init(const void *sendbuf, MPI_Count sendcount, MPI_Datatype sendtype,
                        void *recvbuf, MPI_Count recvcount, MPI_Datatype recvtype,
                        int tag, MPI_Comm comm, PCMPI_Request *request);
int MPIX_Alltoall_init(const void *sendbuf, MPI_Count sendcount, MPI_Datatype sendtype,
                       void *recvbuf, MPI_Count recvcount, MPI_Datatype recvtype,
                       int tag, MPI_Comm comm, PCMPI_Request *request);

int MPIX_Igather_init(const void *sendbuf, MPI_Count sendcount, MPI_Datatype sendtype,
                      void *recvbuf, MPI_Count recvcount, MPI_Datatype recvtype, int root,
                      int tag, MPI_Comm comm, PCMPI_Request *request);
int MPIX_Iscatter_init(const void *sendbuf, MPI_Count sendcount, MPI_Datatype sendtype,
                       void *recvbuf, MPI_Count recvcount, MPI_Datatype recvtype, int root,
                       int tag, MPI_Comm comm, PCMPI_Request *request);
int MPIX_Iallgather_init(const void *sendbuf, MPI_Count sendcount, MPI_Datatype sendtype,
                         void *recvbuf, MPI_Count recvcount, MPI_Datatype recvtype,
                         int tag, MPI_Comm comm, PCMPI_Request *request);
int MPIX_Ialltoall_init(const void *sendbuf, MPI_Count sendcount, MPI_Datatype sendtype,
                        void *recvbuf, MPI_Count recvcount, MPI_Datatype recvtype,
                        int tag, MPI_Comm comm, PCMPI_Request *request);

#if 0

int MPIX_Bcast_init(void *buffer, MPI_Count count, MPI_Datatype datatype, int root, int tag, MPI_Comm comm, PCMPI_Request *request);
int MPIX_Ibcast_init(void *buffer, MPI_Count count, MPI_Datatype datatype, int root, int tag, MPI_Comm comm, PCMPI_Request *request);

/* Neighborhood collectives */

int MPIX_Neighbor_allgather_init(const void *sendbuf, MPI_Count sendcount, MPI_Datatype sendtype,
                              void *recvbuf, MPI_Count recvcount, MPI_Datatype recvtype, int tag, MPI_Comm comm, PCMPI_Request *request);
int MPIX_Neighbor_alltoall_init(const void *sendbuf, MPI_Count sendcount, MPI_Datatype sendtype,
                             void *recvbuf, MPI_Count recvcount, MPI_Datatype recvtype, int tag, MPI_Comm comm, PCMPI_Request *request);
int MPIX_Ineighbor_allgather_init(const void *sendbuf, MPI_Count sendcount, MPI_Datatype sendtype,
                               void *recvbuf, MPI_Count recvcount, MPI_Datatype recvtype, int tag, MPI_Comm comm,
                               PCMPI_Request *request);
int MPIX_Ineighbor_alltoall_init(const void *sendbuf, MPI_Count sendcount, MPI_Datatype sendtype,
                             void *recvbuf, MPI_Count recvcount, MPI_Datatype recvtype, int tag, MPI_Comm comm,
                             PCMPI_Request *request);

/* Reductions */

int MPIX_Reduce_init(const void *sendbuf, void *recvbuf, MPI_Count count,
                  MPI_Datatype datatype, MPI_Op op, int root, int tag, MPI_Comm comm, PCMPI_Request *request);
int MPIX_Allreduce_init(const void *sendbuf, void *recvbuf, MPI_Count count,
                     MPI_Datatype datatype, MPI_Op op, int tag, MPI_Comm comm, PCMPI_Request *request);
int MPIX_Reduce_scatter_block_init(const void *sendbuf, void *recvbuf, MPI_Count recvcount,
                                MPI_Datatype datatype, MPI_Op op, int tag, MPI_Comm comm, PCMPI_Request *request);
int MPIX_Ireduce_init(const void *sendbuf, void *recvbuf, MPI_Count count,
                   MPI_Datatype datatype, MPI_Op op, int root, int tag, MPI_Comm comm, PCMPI_Request *request);
int MPIX_Iallreduce_init(const void *sendbuf, void *recvbuf, MPI_Count count,
                      MPI_Datatype datatype, MPI_Op op, int tag, MPI_Comm comm, PCMPI_Request *request);
int MPIX_Ireduce_scatter_block_init(const void *sendbuf, void *recvbuf, MPI_Count recvcount,
                                 MPI_Datatype datatype, MPI_Op op, int tag, MPI_Comm comm, PCMPI_Request *request);

int MPIX_Reduce_scatter_init(const void *sendbuf, void *recvbuf, const MPI_Count recvcounts[],
                          MPI_Datatype datatype, MPI_Op op, int tag, MPI_Comm comm, PCMPI_Request *request);
int MPIX_Ireduce_scatter_init(const void *sendbuf, void *recvbuf, const MPI_Count recvcounts[],
                           MPI_Datatype datatype, MPI_Op op, int tag, MPI_Comm comm, PCMPI_Request *request);

int MPIX_Scan_init(const void *sendbuf, void *recvbuf, MPI_Count count, MPI_Datatype datatype, MPI_Op op, int tag, MPI_Comm comm, PCMPI_Request *request);
int MPIX_Exscan_init(const void *sendbuf, void *recvbuf, MPI_Count count, MPI_Datatype datatype, MPI_Op op, int tag, MPI_Comm comm, PCMPI_Request *request);
int MPIX_Iscan_init(const void *sendbuf, void *recvbuf, MPI_Count count, MPI_Datatype datatype, MPI_Op op, int tag, MPI_Comm comm,
                 PCMPI_Request *request);
int MPIX_Iexscan_init(const void *sendbuf, void *recvbuf, MPI_Count count, MPI_Datatype datatype, MPI_Op op, int tag, MPI_Comm comm,
                   PCMPI_Request *request);

/* V-collectives */

int MPIX_Gatherv_init(const void *sendbuf, MPI_Count sendcount, MPI_Datatype sendtype,
                   void *recvbuf, const MPI_Count recvcounts[], const MPI_Aint displs[], MPI_Datatype recvtype,
                   int root, int tag, MPI_Comm comm, PCMPI_Request *request);
int MPIX_Scatterv_init(const void *sendbuf, const MPI_Count sendcounts[], const MPI_Aint displs[], MPI_Datatype sendtype,
                    void *recvbuf, MPI_Count recvcount, MPI_Datatype recvtype,
                    int root, int tag, MPI_Comm comm, PCMPI_Request *request);
int MPIX_Allgatherv_init(const void *sendbuf, MPI_Count sendcount, MPI_Datatype sendtype,
                      void *recvbuf, const MPI_Count recvcounts[], const MPI_Aint displs[], MPI_Datatype recvtype,
                      int tag, MPI_Comm comm, PCMPI_Request *request);
int MPIX_Alltoallv_init(const void *sendbuf, const MPI_Count sendcounts[], const MPI_Aint sdispls[], MPI_Datatype sendtype,
                     void *recvbuf, const MPI_Count recvcounts[], const MPI_Aint rdispls[], MPI_Datatype recvtype,
                     int tag, MPI_Comm comm, PCMPI_Request *request);
int MPIX_Alltoallw_init(const void *sendbuf, const MPI_Count sendcounts[], const MPI_Aint sdispls[], const MPI_Datatype sendtypes[],
                     void *recvbuf, const MPI_Count recvcounts[], const MPI_Aint rdispls[], const MPI_Datatype recvtypes[],
                     int tag, MPI_Comm comm, PCMPI_Request *request);

int MPIX_Neighbor_allgatherv_init(const void *sendbuf, MPI_Count sendcount, MPI_Datatype sendtype,
                               void *recvbuf, const MPI_Count recvcounts[], const MPI_Aint displs[],
                               MPI_Datatype recvtype, int tag, MPI_Comm comm, PCMPI_Request *request);
int MPIX_Neighbor_alltoallv_init(const void *sendbuf, const MPI_Count sendcounts[], const MPI_Aint sdispls[],
                              MPI_Datatype sendtype, void *recvbuf, const MPI_Count recvcounts[],
                              const MPI_Aint rdispls[], MPI_Datatype recvtype, int tag, MPI_Comm comm, PCMPI_Request *request);
int MPIX_Neighbor_alltoallw_init(const void *sendbuf, const MPI_Count sendcounts[], const MPI_Aint sdispls[],
                              const MPI_Datatype sendtypes[], void *recvbuf, const MPI_Count recvcounts[],
                              const MPI_Aint rdispls[], const MPI_Datatype recvtypes[], int tag, MPI_Comm comm, PCMPI_Request *request);

/* Nonblocking V-collectives */

int MPIX_Igatherv_init(const void *sendbuf, MPI_Count sendcount, MPI_Datatype sendtype,
                    void *recvbuf, const MPI_Count recvcounts[], const MPI_Aint displs[], MPI_Datatype recvtype,
                    int root, int tag, MPI_Comm comm, PCMPI_Request *request);
int MPIX_Iscatterv_init(const void *sendbuf, const MPI_Count sendcounts[], const MPI_Aint displs[], MPI_Datatype sendtype,
                     void *recvbuf, MPI_Count recvcount, MPI_Datatype recvtype, int root, int tag, MPI_Comm comm, PCMPI_Request *request);
int MPIX_Iallgatherv_init(const void *sendbuf, MPI_Count sendcount, MPI_Datatype sendtype,
                       void *recvbuf, const MPI_Count recvcounts[], const MPI_Aint displs[], MPI_Datatype recvtype,
                       int tag, MPI_Comm comm, PCMPI_Request *request);
int MPIX_Ialltoallv_init(const void *sendbuf, const MPI_Count sendcounts[], const MPI_Aint sdispls[], MPI_Datatype sendtype,
                      void *recvbuf, const MPI_Count recvcounts[], const MPI_Aint rdispls[], MPI_Datatype recvtype,
                      int tag, MPI_Comm comm, PCMPI_Request *request);

int MPIX_Ialltoallw_init(const void *sendbuf, const MPI_Count sendcounts[], const MPI_Aint sdispls[], const MPI_Datatype sendtypes[],
                      void *recvbuf, const MPI_Count recvcounts[], const MPI_Aint rdispls[], const MPI_Datatype recvtypes[],
                      int tag, MPI_Comm comm, PCMPI_Request *request);

int MPIX_Ineighbor_allgatherv_init(const void *sendbuf, MPI_Count sendcount, MPI_Datatype sendtype,
                                void *recvbuf, const MPI_Count recvcounts[], const MPI_Aint displs[],
                                MPI_Datatype recvtype, int tag, MPI_Comm comm, PCMPI_Request *request);
int MPIX_Ineighbor_alltoallv_init(const void *sendbuf, const MPI_Count sendcounts[], const MPI_Aint sdispls[],
                               MPI_Datatype sendtype, void *recvbuf, const MPI_Count recvcounts[],
                               const MPI_Aint rdispls[], MPI_Datatype recvtype, int tag, MPI_Comm comm,
int MPIX_Ineighbor_alltoallw_init(const void *sendbuf, const MPI_Count sendcounts[],
                               const MPI_Aint sdispls[], const MPI_Datatype sendtypes[],
                               void *recvbuf, const MPI_Count recvcounts[], const MPI_Aint rdispls[],
                               const MPI_Datatype recvtypes[], int tag, MPI_Comm comm, PCMPI_Request *request);

#endif

#endif // PERSCOLL_H_INCLUDED
