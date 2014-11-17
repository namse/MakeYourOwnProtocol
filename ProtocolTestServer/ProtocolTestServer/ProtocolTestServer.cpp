#include "stdafx.h"
#include "ChannelMananger.h"


bool IsLoginPacket(char* buf, int length);
bool IsRightPacket(char* buf, int length);

int main(int argc, char **argv)
{
	int sockfd;
	int clilen;
	int state;
	int n;
	struct sockaddr_in serveraddr, clientaddr;
	char buf[MAX_PACKET_SIZE];
	
	clilen = sizeof(clientaddr);

	ChannelMananger::GetInstance()->Init();

	/// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return -1;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)
	{
		perror("socket error : ");
		exit(0);
	}

	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(PORT);

	state = bind(sockfd, (struct sockaddr *)&serveraddr,
		sizeof(serveraddr));
	if (state == -1)
	{
		perror("bind error : ");
		exit(0);
	}

	while (1)
	{
		n = recvfrom(sockfd, buf, MAX_PACKET_SIZE, 0, (struct sockaddr *)&clientaddr, &clilen);

		if (IsLoginPacket(buf, n))
		{
			LoginPacket packet;
			memcpy(&packet, buf, sizeof(LoginPacket));

			bool ret = ChannelMananger::GetInstance()->Login(packet.m_ChannelNumber, packet.m_ID, clientaddr);

			if (ret == true)
				sendto(sockfd, buf, sizeof(LoginPacket), 0, (struct sockaddr *)&clientaddr, clilen);
		}
		else
		{
			if (n < sizeof(ChannelNumber) + sizeof(ID))
				continue;

			NormalPacketHeader header;
			memcpy(&header, buf, sizeof(header));
			
			if (ChannelMananger::GetInstance()->IsLogin(header.m_ChannelNumber, header.m_ID, clientaddr) == false)
				continue;

			auto users = ChannelMananger::GetInstance()->GetChannelUsers(header.m_ChannelNumber);
			for (auto it = users.first; it != users.second; it++)
			{
				if (memcmp(&it->second.m_SocketAddress, &clientaddr, sizeof(struct sockaddr_in)) != 0)
				{
					sendto(sockfd, buf + sizeof(header), n - sizeof(header), 0, (struct sockaddr *)&it->second.m_SocketAddress, sizeof(it->second.m_SocketAddress));
				}
			}
		}
	}


	closesocket(sockfd);
	ChannelMananger::FreeInstance();
}

bool IsLoginPacket(char* buf, int length)
{
	if (length != sizeof(LoginPacket))
		return false;

	int first4Byte;
	memcpy(&first4Byte, buf, sizeof(first4Byte));

	if (first4Byte == LOGIN_UNIQUE_KEY)
		return true;
	return false;
}
