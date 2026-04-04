CC = gcc
EXE = program
LDFLAGS = -lpthread -ldl
CFLAGS = -O2 -Wall
ifeq ($(OS), win)
    CC = x86_64-w64-mingw32-gcc
    EXE = program.exe
    LDFLAGS = -lpthread
    CFLAGS += -D__USE_MINGW_ANSI_STDIO=1
endif

OBJ = main.o sqlite3.o
all: $(EXE)
$(EXE): $(OBJ)
	$(CC) $(OBJ) -o $(EXE) $(LDFLAGS)
sqlite3.o: sqlite3.c
	$(CC) $(CFLAGS) -c sqlite3.c -o sqlite3.o
main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o
clean:
	rm -f *.o $(EXE)
