CC=g++
CXXFLAGS=-g -std=c++11 -Wall -pedantic

all : server

server : ProtocolTestServer.o ChannelMananger.o
	g++ -std=c++11 ProtocolTestServer.o ChannelMananger.o -o server

ProtocolTestServer.o : ProtocolTestServer.cpp stdafx.h ChannelMananger.o
	g++ -std=c++11 -c ProtocolTestServer.cpp

ChannelMananger.o : ChannelMananger.h ChannelMananger.cpp
	g++ -std=c++11 -c ChannelMananger.cpp

clean:
	rm -rf *.o server
