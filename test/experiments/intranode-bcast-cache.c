#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

MPI_Comm MPI_COMM_NODE;
MPI_Win wincache;
char * ptrcache;
int cachebytes;

int SMP_Setup_cache(int bytes)
{
    int nrank = -1;
    MPI_Comm_rank(MPI_COMM_WORLD, &nrank);

    cachebytes = bytes;

    MPI_Aint winsize = (nrank==0) ? bytes : 0;
    char * local = NULL;
    MPI_Win_allocate_shared(winsize, 1, MPI_INFO_NULL, MPI_COMM_NODE, &local, &wincache);

    int disp; /* unused */
    MPI_Win_shared_query(wincache, 0, &winsize, &disp, &ptrcache);

    MPI_Win_lock_all(0, wincache);
    return MPI_SUCCESS;
}

int SMP_Destroy_cache(void)
{
    MPI_Win_unlock_all(wincache);
    MPI_Win_free(&wincache);
    return MPI_SUCCESS;
}

int SMP_Bcast(char* buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm)
{
    int nrank = -1;
    MPI_Comm_rank(MPI_COMM_WORLD, &nrank);

    /* Type_size only works for types without holes. */
    int ts = 0;
    MPI_Type_size(datatype, &ts);

    size_t bytes = count*ts;
    if (bytes<=cachebytes) {
        if (nrank==0) {
            memcpy(ptrcache, buffer, bytes);
        }
        MPI_Win_sync(wincache);
        if (nrank!=0) {
            memcpy(buffer, ptrcache, bytes);
        }
    } else {
        size_t c = (size_t)(bytes/cachebytes);
        size_t r = (size_t)(bytes%cachebytes);
        for (size_t i=0; i<c; i++) {
            if (nrank==0) {
                memcpy(ptrcache, &(buffer[i*cachebytes]), cachebytes);
            }
            MPI_Win_sync(wincache);
            if (nrank!=0) {
                memcpy(&(buffer[i*cachebytes]), ptrcache, cachebytes);
            }
        }
        if (nrank==0) {
            memcpy(ptrcache, &(buffer[c*cachebytes]), bytes);
        }
        MPI_Win_sync(wincache);
        if (nrank!=0) {
            memcpy(&(buffer[c*cachebytes]), ptrcache, r);
        }
    }
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

    char * buf1 = NULL;
    char * buf2 = NULL;
    MPI_Alloc_mem(n, MPI_INFO_NULL, &buf1);
    MPI_Alloc_mem(n, MPI_INFO_NULL, &buf2);

    SMP_Setup_cache(8*1024*1024);
    MPI_Barrier(MPI_COMM_WORLD);

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

    SMP_Destroy_cache();

    MPI_Free_mem(buf1);
    MPI_Free_mem(buf2);

    MPI_Comm_free(&MPI_COMM_NODE);

    MPI_Finalize();
    return 0;
}
