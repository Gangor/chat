CC 		:= gcc
CFLAGS 	:= -pthread

BIN     := bin
INCLUDE	:= include
SRC 	:= src

LIBRARIES   :=
EXECUTABLE  := chat

all: build

build: $(SRC)/*.c
	clear
	$(CC) $(CFLAGS) -I $(INCLUDE) $^ -o $(BIN)/$(EXECUTABLE)

clean:
	> ${EXECUTABLE}_log

kill:
	kill -9 $$(ps -ef | grep -v grep | grep $(EXECUTABLE) | awk -F ' ' '{print $$2}')

run: build
	./$(BIN)/$(EXECUTABLE) 1234

memcheck: build
	valgrind ./$(BIN)/$(EXECUTABLE) 1234 --leak-check=full

client:
	clear
	telnet 127.0.0.1 1234

watch:
	tail -f ./$(EXECUTABLE)_log