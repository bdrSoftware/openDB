#CC := g++
CC := clang++
CC_OPT := -ggdb3 -O0 -Wall -Wextra -fPIC -std=c++11
INCLUDE_LIBPQ := -I/usr/include/postgresql

OBJ_DIR = ./obj/
LIB_DEST = ./
BIN_DIR = ./bin/

.PHONY: openDB clean

SOURCES = common.cpp sqlType.cpp queryAttribute.cpp column.cpp record.cpp memory_storage.cpp file_storage.cpp table.cpp schema.cpp connection.cpp dbms.cpp database.cpp

OBJ = $(OBJ_DIR)common.o $(OBJ_DIR)sqlType.o $(OBJ_DIR)queryAttribute.o $(OBJ_DIR)column.o $(OBJ_DIR)record.o $(OBJ_DIR)memory_storage.o $(OBJ_DIR)file_storage.o $(OBJ_DIR)table.o $(OBJ_DIR)schema.o $(OBJ_DIR)connection.o $(OBJ_DIR)dbms.o $(OBJ_DIR)database.o

openDB : $(LIB_DEST)libopenDB.so $(LIB_DEST)libopenDB.a

clean :
	rm -f $(LIB_DEST)libopenDB.so $(LIB_DEST)libopenDB.a $(OBJ) $(BIN_DIR)unitTest.o $(BIN_DIR)unitTest

unitTest : unitTest.cpp openDB 
	$(CC) $(CC_OPT) -c -o $(BIN_DIR)unitTest.o $< $(INCLUDE_LIBPQ)
	$(CC) -o $(BIN_DIR)unitTest $(BIN_DIR)unitTest.o -Wl,-rpath,$(LIB_DEST) -L$(LIB_DEST) -lopenDB -lpq -lpthread

$(LIB_DEST)libopenDB.so : $(OBJ)
	$(CC) -shared -o $(LIB_DEST)libopenDB.so $(OBJ)

$(LIB_DEST)libopenDB.a : $(OBJ)
	ar rcs $(LIB_DEST)libopenDB.a $(OBJ)

$(OBJ_DIR)common.o : common.cpp common.hpp
	$(CC) $(CC_OPT) -c -o $@ $<
	
$(OBJ_DIR)sqlType.o : sqlType.cpp sqlType.hpp
	$(CC) $(CC_OPT) -c -o $@ $<
	
$(OBJ_DIR)queryAttribute.o :  queryAttribute.cpp queryAttribute.hpp
	$(CC) $(CC_OPT) -c -o $@ $<
	
$(OBJ_DIR)column.o : column.cpp column.hpp
	$(CC) $(CC_OPT) -c -o $@ $<

$(OBJ_DIR)record.o : record.cpp record.hpp
	$(CC) $(CC_OPT) -c -o $@ $<

$(OBJ_DIR)memory_storage.o : memory_storage.cpp memory_storage.hpp
	$(CC) $(CC_OPT) -c -o $@ $<

$(OBJ_DIR)file_storage.o : file_storage.cpp file_storage.hpp
	$(CC) $(CC_OPT) -c -o $@ $<

$(OBJ_DIR)table.o : table.cpp table.hpp
	$(CC) $(CC_OPT) -c -o $@ $<

$(OBJ_DIR)schema.o : schema.cpp schema.hpp
	$(CC) $(CC_OPT) -c -o $@ $<

$(OBJ_DIR)connection.o : connection.cpp connection.hpp
	$(CC) $(CC_OPT) -c -o $@ $< $(INCLUDE_LIBPQ)

$(OBJ_DIR)dbms.o : dbms.cpp dbms.hpp
	$(CC) $(CC_OPT) -c -o $@ $< $(INCLUDE_LIBPQ)

$(OBJ_DIR)database.o : database.cpp database.hpp
	$(CC) $(CC_OPT) -c -o $@ $< $(INCLUDE_LIBPQ)



