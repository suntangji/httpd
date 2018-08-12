.PHONY: clean
CC = gcc
CFLAGS = -g -Wall 
TARGRT = httpd 
BIN_PATH = ./bin
BIN_TARGET = $(BIN_PATH)/$(TARGRT)
OBJ_PATH = ./obj
OBJ = $(OBJ_PATH)/httpd.o $(OBJ_PATH)/read_config.o $(OBJ_PATH)/handler.o $(OBJ_PATH)/response.o $(OBJ_PATH)/epoll.o


all: TARGRT

TARGRT: $(OBJ)	
	$(CC) $(CFLAGS) -lpthread -o $(BIN_TARGET) $^

$(OBJ_PATH)/%.o: %.c httpd.h
	$(CC) $(CFLAGS) -c -o $@ $<

read_config.o: read_config.h
response.o: response.h


clean:
	rm -f $(OBJ) $(BIN_TARGET)

