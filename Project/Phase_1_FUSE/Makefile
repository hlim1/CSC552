CXX=g++
# CXXFLAGS=-Wall -I.
# CXXFLAGS = -Wall -g -I.
CXXFLAGS = -Wall -g -I. -lfuse

DEPS = flash.h log.h directory.h file.h fuse_implement.h inode.h
OBJ = flash.o log.o lfs.o mklfs.o directory.o file.o fuse_implement.o inode.o
LFS_OBJ = flash.o log.o directory.o file.o fuse_implement.o inode.o lfs.o 
MKLFS_OBJ = flash.o log.o directory.o file.o fuse_implement.o inode.o mklfs.o

%.o: %.c $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

all: lfs mklfs

lfs: $(LFS_OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS)

mklfs: $(MKLFS_OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS)

.PHONY: clean

clean:
	rm -f *.o lfs mklfs