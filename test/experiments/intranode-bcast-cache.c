#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define DEBUG

MPI_Comm MPI_COMM_NODE;
MPI_Win wincache;
char * ptrcache;
int cachebytes;

const int reps = 200;

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
    MPI_Comm_rank(comm, &nrank);

#ifndef DEBUG
    int nsize = 0;
    MPI_Comm_size(comm, &nsize);
    /* fast path for trivial case */
    if (nsize==1) return MPI_SUCCESS;
#endif

    /* Type_size only works for types without holes. */
    int ts = 0;
    MPI_Type_size(datatype, &ts);

    MPI_Aint flag = (MPI_Aint)buffer;

    size_t bytes = count*ts;
    if (bytes<=cachebytes) {
        if (nrank==0) {
            memcpy(ptrcache, buffer, bytes);
        }
        MPI_Bcast(&flag, 1, MPI_AINT, 0, comm);
        MPI_Win_sync(wincache);
        if (nrank!=0) {
            memcpy(buffer, ptrcache, bytes);
        }
    } else {
        int c = (int)(bytes/cachebytes);
        int r = (int)(bytes%cachebytes);
        MPI_Barrier(comm);
        for (int i=0; i<c; i++) {
            if (nrank==0) {
                memcpy(ptrcache, &(buffer[i*cachebytes]), cachebytes);
            }
            MPI_Bcast(&flag, 1, MPI_AINT, 0, comm); /* Faster than Barrier? */
            MPI_Win_sync(wincache);
            if (nrank!=0) {
                memcpy(&(buffer[i*cachebytes]), ptrcache, cachebytes);
            }
            MPI_Barrier(comm); /* Should be faster than MPI_Reduce... */
        }
        if (nrank==0) {
            memcpy(ptrcache, &(buffer[c*cachebytes]), r);
        }
        MPI_Bcast(&flag, 1, MPI_AINT, 0, comm); /* Faster than Barrier? */
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
    int n = (argc>1) ? atoi(argv[1]) : 1000;

    int wrank, wsize;
    MPI_Comm_rank(MPI_COMM_WORLD, &wrank);
    MPI_Comm_size(MPI_COMM_WORLD, &wsize);

    MPI_Comm_split_type(MPI_COMM_WORLD, MPI_COMM_TYPE_SHARED, 0, MPI_INFO_NULL, &MPI_COMM_NODE);

    int nrank, nsize;
    MPI_Comm_rank(MPI_COMM_NODE, &nrank);
    MPI_Comm_size(MPI_COMM_NODE, &nsize);

    char * vartmp = getenv("SMP_CACHE_SIZE");
    int inttmp = (vartmp==NULL) ? 8*1024*1024 : atoi(vartmp);
    SMP_Setup_cache(inttmp);
    MPI_Barrier(MPI_COMM_WORLD);

#ifdef DEBUG
    if (wrank==0) {
        printf("MPI vs. SMP bcast of %d bytes (cache size = %d bytes)\n", n, inttmp);
        printf("world size = %d, node size = %d\n", wsize, nsize);
    }
    char procname[MPI_MAX_PROCESSOR_NAME];
    int len;
    MPI_Get_processor_name(procname, &len);
    printf("rank %d is %s\n", wrank, procname);
    MPI_Barrier(MPI_COMM_WORLD);
    fflush(stdout);
#endif

    char * buf1 = NULL;
    char * buf2 = NULL;
    MPI_Alloc_mem(n, MPI_INFO_NULL, &buf1);
    MPI_Alloc_mem(n, MPI_INFO_NULL, &buf2);

    memset(buf1, nrank==0 ? 'Z' : 'A', n);
    memset(buf2, nrank==0 ? 'Z' : 'A', n);

    double t0, t1, dtmpi = 0.0, dtsmp = 0.0;
    for (int r=0; r<reps; r++) {

        MPI_Barrier(MPI_COMM_WORLD);
        t0 = MPI_Wtime();
        MPI_Bcast(buf1, n, MPI_CHAR, 0, MPI_COMM_NODE);
        t1 = MPI_Wtime();
        if (r>1) dtmpi += (t1-t0);

        MPI_Barrier(MPI_COMM_WORLD);
        t0 = MPI_Wtime();
        SMP_Bcast(buf2, n, MPI_CHAR, 0, MPI_COMM_NODE);
        t1 = MPI_Wtime();
        if (r>1) dtsmp += (t1-t0);

        if (r==0) {
            char * tmp = malloc(n);
            memset(tmp, 'Z', n);
            int err1 = memcmp(tmp, buf1, n);
            int err2 = memcmp(tmp, buf2, n);
            if (err1>0 || err2>0) {
                printf("%d: errors: MPI (%d), SMP (%d) \n", wrank, err1, err2);
                for (int i=0; i<n; i++) {
                    printf("%d: %d \'%c\' \'%c\' (\'%c\')\n", wrank, i, buf1[i], buf2[i], tmp[i]);
                }
                SMP_Destroy_cache();
                MPI_Finalize();
                return 1;
            }
            free(tmp);
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    double dtavg, dtmin, dtmax;
    MPI_Reduce(&dtmpi, &dtavg, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_NODE);
    MPI_Reduce(&dtmpi, &dtmin, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_NODE);
    MPI_Reduce(&dtmpi, &dtmax, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_NODE);
    if (nrank==0) {
        dtavg /= nsize;
        printf("%d: %s (%lf,%lf,%lf) seconds (%lf,%lf,%lf) MB/s (min,avg,max) \n",
                wrank, "MPI", dtmin, dtavg, dtmax,
                (reps-2)*n*(1.e-6/dtmax), (reps-2)*n*(1.e-6/dtavg), (reps-2)*n*(1.e-6/dtmin));
        fflush(stdout);
    }
    MPI_Reduce(&dtsmp, &dtavg, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_NODE);
    MPI_Reduce(&dtsmp, &dtmin, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_NODE);
    MPI_Reduce(&dtsmp, &dtmax, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_NODE);
    if (nrank==0) {
        dtavg /= nsize;
        printf("%d: %s (%lf,%lf,%lf) seconds (%lf,%lf,%lf) MB/s (min,avg,max) \n",
                wrank, "SMP", dtmin, dtavg, dtmax,
                (reps-2)*n*(1.e-6/dtmax), (reps-2)*n*(1.e-6/dtavg), (reps-2)*n*(1.e-6/dtmin));
        fflush(stdout);
    }

    SMP_Destroy_cache();

    MPI_Free_mem(buf1);
    MPI_Free_mem(buf2);

    MPI_Comm_free(&MPI_COMM_NODE);

    MPI_Finalize();
    return 0;
}
