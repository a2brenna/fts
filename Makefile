INCLUDE_DIR=$(shell echo ~)/local/include,/usr/local/include
LIBRARY_DIR=$(shell echo ~)/local/lib,/usr/local/lib
DESDTIR=/
PREFIX=/usr

CXX=g++
CXXFLAGS=-L${LIBRARY_DIR} -I${INCLUDE_DIR} -O2 -g -std=c++14 -fPIC -Wall -Wextra -march=native

all: ftsd fts

fts: src/fts.cc server.o metadata.o archive.o index.o wire_protocol.o
	${CXX} ${CXXFLAGS} -o fts src/fts.cc server.o metadata.o archive.o index.o wire_protocol.o -lboost_program_options -lrtosfs -lsodium -lcapnp -lkj -lsmplsocket

ftsd: src/ftsd.cc server.o metadata.o archive.o index.o wire_protocol.o internal.o metadata_cache.o manifest.o
	${CXX} ${CXXFLAGS} -o ftsd src/ftsd.cc server.o metadata.o archive.o index.o wire_protocol.o internal.o metadata_cache.o manifest.o -lboost_program_options -lrtosfs -lsodium -lcapnp -lkj -lsmplsocket -lpthread

server.o: src/server.cc src/server.h
	    ${CXX} ${CXXFLAGS} -c src/server.cc -o server.o

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

src/wire_protocol.capnp.c++: src/wire_protocol.capnp
	    capnp compile -oc++ src/wire_protocol.capnp

wire_protocol.o: src/wire_protocol.capnp.c++
	    ${CXX} ${CXXFLAGS} -c src/wire_protocol.capnp.c++ -o wire_protocol.o

src/internal.capnp.c++: src/internal.capnp
	    capnp compile -oc++ src/internal.capnp

internal.o: src/internal.capnp.c++
	    ${CXX} ${CXXFLAGS} -c src/internal.capnp.c++ -o internal.o

clean:
	rm -f ftsd
	rm -f *.o
	rm -f *.so
	rm -f *.a
	rm -f src/*.capnp.h
	rm -f src/*.capnp.c++
