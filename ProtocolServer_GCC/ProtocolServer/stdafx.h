
#pragma once

#include "targetver.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <map>
#include <iostream>
#include <fstream>

#include <sys/socket.h>
#include <sys/types.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>

#define PORT 9999
#define MAX_PACKET_SIZE 1024
#define LOGIN_UNIQUE_KEY 0x10204080


typedef int ChannelNumber;
typedef int ID;

struct LoginPacket
{
	int m_UniqueKey;
	ChannelNumber m_ChannelNumber;
	ID m_ID;
	int m_RateOfMissing;
};
struct NormalPacketHeader
{
	ChannelNumber m_ChannelNumber;
	ID m_ID;
};
struct SessionInfo{
	ID m_ID;
	struct sockaddr_in m_SocketAddress;
};

typedef std::multimap<ChannelNumber, SessionInfo> SessionList;

typedef std::map<ID, ChannelNumber> IDCheckList;
typedef int MissingRate;
typedef std::map<ChannelNumber, MissingRate> PacketMissingRateList;