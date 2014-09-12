#include "internal.h"

void * safemalloc(size_t bytes)
{
    void * ptr = NULL;
    int rc = posix_memalign(&ptr, 8, bytes);
    if (rc!=0 || ptr==NULL) {
        PersColl_Error("Out-of-memory error for %zu bytes.\n", bytes);
    }
    return ptr;
}

/* Raise an internal fatal PersColl error.
 *
 * @param[in] file Current file name (__FILE__)
 * @param[in] line Current line numeber (__LINE__)
 * @param[in] func Current function name (__func__)
 * @param[in] msg  Message to be printed
 * @param[in] code Exit error code
 */
void PersColl_Error_impl(const char *file, const int line, const char *func, const char *msg, ...)
{
    va_list ap;
    int  disp;
    char string[500];

    disp  = 0;
    va_start(ap, msg);
    disp += vsnprintf(string, 500, msg, ap);
    va_end(ap);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    fprintf(stderr, "[%d] PersColl Internal error in %s (%s:%d)\n[%d] Message: %s\n",
            rank, func, file, line, rank, string);
    MPI_Abort(MPI_COMM_WORLD, 100);
}
