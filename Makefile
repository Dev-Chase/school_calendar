CC=clang
INCLUDE=include
CFLAGS=-g -Werror -Wall -I$(INCLUDE)
SRC=src
OBJ=obj
BIN=bin
EXE=scal
MAIN=main
OBJS=$(OBJ)/$(MAIN).o $(OBJ)/calendar.o $(OBJ)/list.o $(OBJ)/add.o $(OBJ)/delete.o $(OBJ)/submit.o $(OBJ)/change.o

all: $(BIN)/$(EXE)

run: all
	$(BIN)/$(EXE)

$(OBJ)/%.o: $(SRC)/%.c $(INCLUDE)/%.h
	$(CC) $(CFLAGS) -c -o $@ $<
	
$(OBJ)/$(MAIN).o: $(SRC)/$(MAIN).c
	$(CC) $(CFLAGS) -c -o $@ $^

$(BIN)/%: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -rf *.dSYM $(OBJ)/*.o $(BIN)/*

