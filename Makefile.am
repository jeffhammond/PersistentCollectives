#
# Copyright (C) 2014. See LICENSE in top-level directory.
#

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -I$(top_srcdir)/src

lib_LTLIBRARIES = libperscoll.la

noinst_LTLIBRARIES = libperscolli.la

libperscoll_la_SOURCES = 	src/coll.c \
							src/vcoll.c \
							src/ncoll.c \
							src/starttestwait.c \
							src/utils.c

libperscoll_la_LDFLAGS = -version-info $(libperscoll_abi_version)

libperscolli_la_SOURCES = $(libperscoll_la_SOURCES)
libperscolli_la_LDFLAGS = $(libperscoll_abi_version)

include_HEADERS = src/perscoll.h

bin_PROGRAMS =
check_PROGRAMS = 
TESTS = 
XFAIL_TESTS = 

MPIEXEC = mpiexec -n 2
TESTS_ENVIRONMENT = $(MPIEXEC)

include test/Makefile.mk

.PHONY: checkprogs
checkprogs: $(check_PROGRAMS)
