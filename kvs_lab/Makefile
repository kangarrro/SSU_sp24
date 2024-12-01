SRCS = main.c
LIBSRCS = open.c close.c put.c get.c backup.c
TARG = custom
TARG_BASELINE = baseline

CC = gcc
OPTS = -g -fPIC

all: custom baseline

# Custom target
custom:
	$(CC) $(OPTS) $(SRCS) $(LIBSRCS) -o $(TARG)

# Baseline target
baseline:
	$(CC) $(OPTS) -DUSE_BASELINE $(SRCS) $(LIBSRCS) -o $(TARG_BASELINE)

# Clean target
clean:
	rm -f $(TARG) $(TARG_BASELINE)
