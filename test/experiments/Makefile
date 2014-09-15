CC    := mpicc
CFLAGS = -g -O2 -Wall -std=c99

TESTS = intranode-bcast intranode-bcast-cache

all: $(TESTS)

%: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	-rm $(TESTS)

