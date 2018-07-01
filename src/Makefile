.PHONY: clean
CC = gcc
CFLAGS = -g -Wall -lpthread
TARGRT = httpd 
BIN_PATH = ./bin
BIN_TARGET = $(BIN_PATH)/$(TARGRT)
OBJ_PATH = ./obj
OBJ = $(OBJ_PATH)/httpd.o $(OBJ_PATH)/read_config.o $(OBJ_PATH)/handler.o


all: TARGRT

TARGRT: $(OBJ)	
	$(CC) $(CFLAGS) -o $(BIN_TARGET) $^

$(OBJ_PATH)/%.o: %.c httpd.h
	$(CC) $(CFLAGS) -c -o $@ $<

read_config.o: read_config.h


clean:
	rm -f $(OBJ) $(BIN_TARGET)

