CC = gcc
CFLAGS = -Wall -g
DEPS = Prime.h Queue.h ActiveObject.h st_pipeline.h
OBJ = Prime.o Queue.o ActiveObject.o st_pipeline.o

all: st_pipeline

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

st_pipeline: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o st_pipeline