.PHONY:clean all
CC = g++ -std=c++11
BIN = producers_and_consumers producers_and_consumers_condition
all:$(BIN)
%.o:%.cpp
	$(CC) -c $< -o $@
producers_and_consumers:producers_and_consumers.o
	$(CC) $^ -o $@ -lpthread
producers_and_consumers_condition:producers_and_consumers_condition.o
	$(CC) $^ -o $@ -lpthread
clean:
	rm -f *.o $(BIN)
