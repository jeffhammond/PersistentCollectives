#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

MPI_Comm MPI_COMM_NODE;

int SMP_Bcast(void* buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm)
{
    /* Type_size only works for types without holes. */
    int ts = 0;
    MPI_Type_size(datatype, &ts);

    int nrank = -1;
    MPI_Comm_rank(MPI_COMM_WORLD, &nrank);

    MPI_Aint winsize = (nrank==0) ? count * ts : 0;
    void * temp = NULL;
    MPI_Win wintemp = MPI_WIN_NULL;
    MPI_Win_allocate_shared(winsize, ts, MPI_INFO_NULL, MPI_COMM_NODE, &temp, &wintemp);

    int disp; /* unused */
    MPI_Win_shared_query(wintemp, nrank, &winsize, &disp, &temp);
    printf("%d: temp=%p\n", nrank, temp);

    MPI_Win_lock_all(0, wintemp);

    if (nrank==0) {
        memcpy(temp, buffer, (size_t)count*ts);
    }
    MPI_Win_sync(wintemp);
    if (nrank!=0) {
        memcpy(buffer, temp, (size_t)count*ts);
    }

    MPI_Win_unlock_all(wintemp);
    MPI_Win_free(&wintemp);
    return MPI_SUCCESS;
}

int main(int argc, char* argv[])
{
    MPI_Init(&argc,&argv);
    MPI_Comm_split_type(MPI_COMM_WORLD, MPI_COMM_TYPE_SHARED, 0, MPI_INFO_NULL, &MPI_COMM_NODE);

    int n = (argc>1) ? atoi(argv[1]) : 1000;

    int wrank, wsize;
    MPI_Comm_rank(MPI_COMM_WORLD, &wrank);
    MPI_Comm_size(MPI_COMM_WORLD, &wsize);

    int nrank, nsize;
    MPI_Comm_rank(MPI_COMM_WORLD, &nrank);
    MPI_Comm_size(MPI_COMM_WORLD, &nsize);

    char *buf1 = NULL, *buf2 = NULL;
    MPI_Alloc_mem(n, MPI_INFO_NULL, &buf1);
    MPI_Alloc_mem(n, MPI_INFO_NULL, &buf2);

    double t0, t1, dt;

    for (int r=0; r<20; r++) {
        MPI_Barrier(MPI_COMM_WORLD);
        t0 = MPI_Wtime();
        MPI_Bcast(buf1, n, MPI_CHAR, 0, MPI_COMM_NODE);
        t1 = MPI_Wtime();
        dt = t1-t0;
        printf("%d: MPI_Bcast: %lf seconds, %lf MB/s \n", wrank, dt, n*(1.e-6/dt));
        fflush(stdout);

        MPI_Barrier(MPI_COMM_WORLD);
        t0 = MPI_Wtime();
        SMP_Bcast(buf2, n, MPI_CHAR, 0, MPI_COMM_NODE);
        t1 = MPI_Wtime();
        dt = t1-t0;
        printf("%d: SMP_Bcast: %lf seconds, %lf MB/s \n", wrank, dt, n*(1.e-6/dt));
        fflush(stdout);
    }

    MPI_Free_mem(&buf2);
    MPI_Free_mem(&buf1);

    MPI_Comm_free(&MPI_COMM_NODE);

    MPI_Finalize();
    return 0;
}
