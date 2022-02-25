CC = gcc
LFLAGS = -lpthread

all: producer_consumer_problem.c
	$(CC) -o producer_consumer producer_consumer_problem.c $(LFLAGS)

clean:
	rm -rf producer_consumer
