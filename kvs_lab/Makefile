SRCS = main.c
LIBSRCS = open.c close.c put.c get.c 
TARG = kvs_static
TARG_DYNAMIC = kvs_dynamic

CC = gcc
OPTS = -Wall -g -fPIC
LIBS_STATIC = -L. -lkvs
LIBS_DYNAMIC = -L. -lkvs -Wl,-rpath=.

OBJS = $(SRCS:.c=.o)
LOBJS = $(LIBSRCS:.c=.o)
LARS = libkvs.a
LIB_DYNAMIC = libkvs.so

all: static dynamic

# Static build target
static: $(TARG)

$(TARG): $(OBJS) $(LARS)
	$(CC) -o $(TARG) $(OBJS) $(LIBS_STATIC) -static

$(LARS): $(LOBJS)
	ar rs $(LARS) $(LOBJS)

# Dynamic library build target
$(LIB_DYNAMIC): $(LOBJS)
	$(CC) -shared -o $(LIB_DYNAMIC) $(LOBJS)

# Dynamic build target
dynamic: $(TARG_DYNAMIC)

$(TARG_DYNAMIC): $(OBJS) $(LIB_DYNAMIC)
	$(CC) -o $(TARG_DYNAMIC) $(OBJS) $(LIBS_DYNAMIC)

# Compile source files to object files
%.o: %.c
	$(CC) $(OPTS) -c $< -o $@

# Clean target
clean:
	rm -f $(OBJS) $(LOBJS) $(LARS) $(TARG) $(TARG_DYNAMIC) $(LIB_DYNAMIC)
