CC := g++
#CC := clang++
CC_OPT := -ggdb3 -O0 -Wall -Wextra -fPIC -std=c++11
INCLUDE_LIBPQ := -I/usr/include/postgresql

OBJ_DIR = ./obj/
SRC_DIR = ./src/
LIB_DEST = ./
BIN_DIR = ./bin/
HEADER_DIR = /usr/include/openDB/
LIBRARY_DIR = /usr/lib/

.PHONY: openDB clean install uninstall

OBJ = $(OBJ_DIR)common.o $(OBJ_DIR)sqlType.o $(OBJ_DIR)queryAttribute.o $(OBJ_DIR)column.o $(OBJ_DIR)record.o $(OBJ_DIR)memory_storage.o $(OBJ_DIR)file_storage.o $(OBJ_DIR)table.o $(OBJ_DIR)schema.o $(OBJ_DIR)connection.o $(OBJ_DIR)dbms.o $(OBJ_DIR)database.o

openDB : $(LIB_DEST)libopenDB.so $(LIB_DEST)libopenDB.a

clean :
	rm -f $(LIB_DEST)libopenDB.so $(LIB_DEST)libopenDB.a $(OBJ) $(BIN_DIR)unitTest.o $(BIN_DIR)unitTest

install: openDB
	mkdir -p $(HEADER_DIR)
	mkdir -p $(HEADER_DIR)src/
	cp -f openDB.hpp $(HEADER_DIR)
	cp -f SRC_DIR*.hpp $(HEADER_DIR)src/
	cp -t $(LIBRARY_DIR) $(LIB_DEST)libopenDB.so $(LIB_DEST)libopenDB.a
	
uninstall:
	rm -rf $(HEADER_DIR) $(LIBRARY_DIR)libopenDB.so $(LIBRARY_DIR)libopenDB.a

unitTest : $(SRC_DIR)unitTest.cpp openDB 
	$(CC) $(CC_OPT) -c -o $(BIN_DIR)unitTest.o $< $(INCLUDE_LIBPQ)
	$(CC) -o $(BIN_DIR)unitTest $(BIN_DIR)unitTest.o -Wl,-rpath,$(LIB_DEST) -L$(LIB_DEST) -lopenDB -lpq -lpthread

$(LIB_DEST)libopenDB.so : $(OBJ)
	$(CC) -shared -o $(LIB_DEST)libopenDB.so $(OBJ)

$(LIB_DEST)libopenDB.a : $(OBJ)
	ar rcs $(LIB_DEST)libopenDB.a $(OBJ)

$(OBJ_DIR)common.o : $(SRC_DIR)common.cpp $(SRC_DIR)common.hpp
	$(CC) $(CC_OPT) -c -o $@ $<
	
$(OBJ_DIR)sqlType.o : $(SRC_DIR)sqlType.cpp $(SRC_DIR)sqlType.hpp
	$(CC) $(CC_OPT) -c -o $@ $<
	
$(OBJ_DIR)queryAttribute.o :  $(SRC_DIR)queryAttribute.cpp $(SRC_DIR)queryAttribute.hpp
	$(CC) $(CC_OPT) -c -o $@ $<
	
$(OBJ_DIR)column.o : $(SRC_DIR)column.cpp $(SRC_DIR)column.hpp
	$(CC) $(CC_OPT) -c -o $@ $<

$(OBJ_DIR)record.o : $(SRC_DIR)record.cpp $(SRC_DIR)record.hpp
	$(CC) $(CC_OPT) -c -o $@ $<

$(OBJ_DIR)memory_storage.o : $(SRC_DIR)memory_storage.cpp $(SRC_DIR)memory_storage.hpp
	$(CC) $(CC_OPT) -c -o $@ $<

$(OBJ_DIR)file_storage.o : $(SRC_DIR)file_storage.cpp $(SRC_DIR)file_storage.hpp
	$(CC) $(CC_OPT) -c -o $@ $<

$(OBJ_DIR)table.o : $(SRC_DIR)table.cpp $(SRC_DIR)table.hpp
	$(CC) $(CC_OPT) -c -o $@ $<

$(OBJ_DIR)schema.o : $(SRC_DIR)schema.cpp $(SRC_DIR)schema.hpp
	$(CC) $(CC_OPT) -c -o $@ $<

$(OBJ_DIR)connection.o : $(SRC_DIR)connection.cpp $(SRC_DIR)connection.hpp
	$(CC) $(CC_OPT) -c -o $@ $< $(INCLUDE_LIBPQ)

$(OBJ_DIR)dbms.o : $(SRC_DIR)dbms.cpp $(SRC_DIR)dbms.hpp
	$(CC) $(CC_OPT) -c -o $@ $< $(INCLUDE_LIBPQ)

$(OBJ_DIR)database.o : $(SRC_DIR)database.cpp $(SRC_DIR)database.hpp
	$(CC) $(CC_OPT) -c -o $@ $< $(INCLUDE_LIBPQ)



