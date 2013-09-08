CC = g++
OPZ = -ggdb3 -Wall -Wextra -fPIC -std=c++11

.PHONY: clean

openDB.a : common.o sqlType.o queryAttribute.o column.o table.o schema.o database.o
	ar rcs openDB.a common.o sqlType.o queryAttribute.o column.o table.o schema.o database.o

clean :
	rm -f openDB.a *.o unitTest

unitTest : openDB.a unitTest.cpp
	$(CC) $(OPZ) -c -o unitTest.o unitTest.cpp
	$(CC) -o unitTest unitTest.o openDB.a


common.o : common.hpp common.cpp
	$(CC) $(OPZ) -c -o common.o common.cpp

sqlType.o : sqlType.cpp sqlType.hpp
	$(CC) $(OPZ) -c -o sqlType.o sqlType.cpp

queryAttribute.o :  queryAttribute.cpp queryAttribute.hpp
	$(CC) $(OPZ) -c -o queryAttribute.o queryAttribute.cpp
	
column.o : column.cpp column.hpp
	$(CC) $(OPZ) -c -o column.o column.cpp

table.o : table.cpp table.hpp
	$(CC) $(OPZ) -c -o table.o table.cpp

schema.o : schema.cpp schema.hpp
	$(CC) $(OPZ) -c -o schema.o schema.cpp

database.o : database.cpp database.hpp
	$(CC) $(OPZ) -c -o database.o database.cpp



