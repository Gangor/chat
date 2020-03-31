CC = c++
BINARY=chat.bin
CFLAGS = -pthread
SRC = chat.cpp

build: clean
	$(CC) $(SRC) -o $(BINARY) $(CFLAGS)

clean:
	rm -rf $(BINARY)

kill:
	kill -9 $$(ps -ef | grep -v grep | grep $(BINARY) | awk -F ' ' '{print $$2}')

run: build
	./$(BINARY) 16450