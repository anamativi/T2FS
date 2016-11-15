CC=gcc
LIB_DIR=./lib
INC_DIR=./include
BIN_DIR=./bin
SRC_DIR=./src
TEST_DIR=./test

all: libt2fs.a test

libt2fs: libt2fs.a

libt2fs.a: $(BIN_DIR)/t2fs.o $(BIN_DIR)/_aux.o
	ar crs $(LIB_DIR)/libt2fs.a $(BIN_DIR)/t2fs.o $(BIN_DIR)/_aux.o $(LIB_DIR)/apidisk.o $(LIB_DIR)/bitmap2.o
	
$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c -o $@ $< -I$(INC_DIR)/ -Wall -m32

test : libt2fs.a $(TEST_DIR)/shell.c
	$(CC) $(TEST_DIR)/shell.c -L$(LIB_DIR) -I$(INC_DIR) -lt2fs -o $(TEST_DIR)/shell -Wall -m32 -std=c99

clean:
	rm -rf $(BIN_DIR)/* $(BIN_DIR)/shell $(LIB_DIR)/*.a $(BIN_DIR)/*.o $(SRC_DIR)/*~ $(INC_DIR)/*~ *~

# essa regra serve para ser rapido: limpa tudo, compila tudo, reseta o disco e inicia o terminal
#terminal: clean libt2fs.a
#	rm t2fs_disk.dat -f
#	cp disco_2K_64_C.dsk t2fs_disk.dat
#	$(BIN_DIR)/terminal
