INCLUDE_DIR=$(shell echo ~)/local/include,/usr/local/include
LIBRARY_DIR=$(shell echo ~)/local/lib,/usr/local/lib
DESDTIR=/
PREFIX=/usr

CXX=g++
CXXFLAGS=-L${LIBRARY_DIR} -I${INCLUDE_DIR} -O2 -g -std=c++14 -fPIC -Wall -Wextra -march=native

all: ftsd fts

fts: src/fts.cc metadata.o archive.o index.o wire_protocol.o database.o remote_database.o
	${CXX} ${CXXFLAGS} -o fts src/fts.cc metadata.o archive.o index.o wire_protocol.o database.o remote_database.o -lboost_program_options -lrtosfs -lsodium -lsmplsocket -lprotobuf

ftsd: src/ftsd.cc database.o local_database.o metadata.o archive.o index.o wire_protocol.o metadata_cache.o manifest.o
	${CXX} ${CXXFLAGS} -o ftsd src/ftsd.cc database.o local_database.o metadata.o archive.o index.o wire_protocol.o metadata_cache.o manifest.o -lboost_program_options -lrtosfs -lsodium -lsmplsocket -lpthread -lprotobuf

database.o: src/database.cc src/database.h
	    ${CXX} ${CXXFLAGS} -c src/database.cc -o database.o

remote_database.o: src/remote_database.cc src/remote_database.h src/wire_protocol.pb.h
	    ${CXX} ${CXXFLAGS} -c src/remote_database.cc -o remote_database.o

local_database.o: src/local_database.cc src/local_database.h
	    ${CXX} ${CXXFLAGS} -c src/local_database.cc -o local_database.o

metadata.o: src/metadata.cc src/metadata.h
	    ${CXX} ${CXXFLAGS} -c src/metadata.cc -o metadata.o

metadata_cache.o: src/metadata_cache.cc src/metadata_cache.h
	    ${CXX} ${CXXFLAGS} -c src/metadata_cache.cc -o metadata_cache.o

manifest.o: src/manifest.cc src/manifest.h
	    ${CXX} ${CXXFLAGS} -c src/manifest.cc -o manifest.o

archive.o: src/archive.cc src/archive.h
	    ${CXX} ${CXXFLAGS} -c src/archive.cc -o archive.o

index.o: src/index.cc src/index.h
	    ${CXX} ${CXXFLAGS} -c src/index.cc -o index.o

wire_protocol.o: src/wire_protocol.pb.h
	    ${CXX} ${CXXFLAGS} -c src/wire_protocol.pb.cc -o wire_protocol.o

src/wire_protocol.pb.h: wire_protocol.proto
		protoc --cpp_out=src/ wire_protocol.proto

clean:
	rm -f ftsd
	rm -f fts
	rm -f *.o
	rm -f *.so
	rm -f *.a
	rm -f src/*.pb.h
	rm -f src/*.pb.cc
