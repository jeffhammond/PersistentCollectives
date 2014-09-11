#ifndef PERSCOLL_IMPL_H
#define PERSCOLL_IMPL_H

#include "perscollconf.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <stddef.h>

#include <mpi.h>

#include "perscoll.h"

#include "likely.h"

void PersColl_Error_impl(const char *file, const int line, const char *func, const char *msg, ...);

#define PersColl_Error(...) PersColl_Error_impl(__FILE__,__LINE__,__func__,__VA_ARGS__)

typedef struct
{
    int num;
    /* Persistent Send-Recv implemtnation */
    MPI_Request * requests;
    /* RMA implementation */
    MPI_Win win;
}
PCMPI_Request_impl;

#endif // PERSCOLL_IMPL_H
