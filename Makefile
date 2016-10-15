INCLUDE_DIR=$(shell echo ~)/local/include,/usr/local/include
LIBRARY_DIR=$(shell echo ~)/local/lib,/usr/local/lib
DESDTIR=/
PREFIX=/usr

CXX=g++
CXXFLAGS=-L${LIBRARY_DIR} -I${INCLUDE_DIR} -O2 -g -std=c++14 -fPIC -Wall -Wextra -march=native

all: ftsd fts

fts: src/fts.cc server.o metadata.o archive.o index.o
	${CXX} ${CXXFLAGS} -o fts src/fts.cc server.o metadata.o archive.o index.o -lboost_program_options -lrtosfs -lsodium

ftsd: src/ftsd.cc server.o metadata.o archive.o index.o
	${CXX} ${CXXFLAGS} -o ftsd src/ftsd.cc server.o metadata.o archive.o index.o -lboost_program_options -lrtosfs -lsodium

server.o: src/server.cc src/server.h
	    ${CXX} ${CXXFLAGS} -c src/server.cc -o server.o

metadata.o: src/metadata.cc src/metadata.h
	    ${CXX} ${CXXFLAGS} -c src/metadata.cc -o metadata.o

archive.o: src/archive.cc src/archive.h
	    ${CXX} ${CXXFLAGS} -c src/archive.cc -o archive.o

index.o: src/index.cc src/index.h
	    ${CXX} ${CXXFLAGS} -c src/index.cc -o index.o

clean:
	rm -f ftsd
	rm -f *.o
	rm -f *.so
	rm -f *.a
