# makefile
# added capability to use makefile in various environments without modifications
UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
    cc = clang++
else ifeq ($(UNAME), Darwin)
    cc = g++
else
    cc = g++-4.7
endif

all: dataserver client

reqchannel.o: networkreqchannel.h networkreqchannel.cpp
	$(cc) -std=c++11 -c -g networkreqchannel.cpp

dataserver: dataserver.cpp networkreqchannel.o
	$(cc) -std=c++11 -g -o dataserver dataserver.cpp networkreqchannel.o -lpthread

client: client.cpp networkreqchannel.o
	$(cc) -std=c++11 -g -o client client.cpp networkreqchannel.o
