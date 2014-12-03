
#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <WinSock2.h>
#include <map>
#include <iostream>
#include <fstream>
#pragma comment(lib,"ws2_32.lib")

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