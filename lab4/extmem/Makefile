
SRCPATH := .
INCPATH := .
SOURCE := $(wildcard $(SRCPATH)/*.c)
OBJS := $(patsubst %.c,%.o,$(SOURCE))

CC := gcc
CFLAGS := -Wall -g -I$(INCPATH) 

.PHONY : all clean rebuild

all : read select dataprep projection set join

select : select.o
	$(CC) $(CFLAGS) -o $@ $^
read : read.o
	$(CC) $(CFLAGS) -o $@ $^
dataprep : dataprep.o
	$(CC) $(CFLAGS) -o $@ $^
projection : projection.o
	$(CC) $(CFLAGS) -o $@ $^
set : set.o
	$(CC) $(CFLAGS) -o $@ $^
join : join.o
	$(CC) $(CFLAGS) -o $@ $^

clean :
	rm -f $(OBJS)

rebuild : clean all