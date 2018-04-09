.PHONY:clean
CC = g++ -std=c++11
BIN = reader_and_writer_semaphore reader_and_writer_mutex reader_and_writer_rwlock
all:$(BIN)
%.o:%.cpp
	$(CC) -c $< -o $@
reader_and_writer_semaphore:reader_and_writer_semaphore.o
	$(CC) $^ -o $@ -lpthread
reader_and_writer_mutex:reader_and_writer_mutex.o
	$(CC) $^ -o $@ -lpthread
reader_and_writer_rwlock:reader_and_writer_rwlock.o
	$(CC) $^ -o $@ -lpthread
clean:
	rm -f *.o $(BIN)
