CC = gcc
EXE_FILE = main

SRC = main.c
OBJ = main.o
HEADERS = configs.h

all: $(EXE_FILE)

$(EXE_FILE): $(OBJ)
	$(CC) $(OBJ) -o $(EXE_FILE) -lssl -lcrypto

main.o: main.c $(HEADERS)
	$(CC) -c main.c -o main.o

clean:
	rm -f *.o $(EXE_FILE)