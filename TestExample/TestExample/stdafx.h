#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")

#define PORT 9999
#define MAX_PACKET_SIZE 1024
#define LOGIN_UNIQUE_KEY 0x10204080

#define SERVER_IP "10.73.42.50"

//#define RECV
#define SEND

typedef int ChannelNumber;
typedef int ID;

struct LoginPacket
{
	int m_UniqueKey;
	ChannelNumber m_ChannelNumber;
	ID m_ID;
	LoginPacket(){
		m_UniqueKey = LOGIN_UNIQUE_KEY;
	}
};
struct NormalPacketHeader
{
	ChannelNumber m_ChannelNumber;
	ID m_ID;
};
