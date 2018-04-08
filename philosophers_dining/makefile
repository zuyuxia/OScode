.PHONY:clean all
CC = g++ -std=c++11
BIN = philosophers_dining_mutex philosophers_dining_semaphore
all:$(BIN)
%.o:%.cpp
	$(CC) -c $< -o $@
philosophers_dining_mutex:philosophers_dining_mutex.o
	$(CC) $^ -o $@ -lpthread
philosophers_dining_semaphore:philosophers_dining_semaphore.o
	$(CC) $^ -o $@ -lpthread
clean:
	rm -f *.o $(BIN)
