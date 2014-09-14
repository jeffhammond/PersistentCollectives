CC     = mpicc
CFLAGS = -g -O2 -Wall -std=c99

TESTS = intranode-bcast

all: $(TESTS)

%: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	-rm $(TESTS)

