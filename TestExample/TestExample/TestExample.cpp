#include "stdafx.h"

struct data
{
	int a;
	int b;
	int sum;
};
int main(int argc, char **argv)
{
	int sockfd;
	int clilen;
	int state;
	char buf[MAX_PACKET_SIZE];

	struct sockaddr_in serveraddr;
	struct data add_data;

	/// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return -1;
	memset(buf, 0x00, 255);
	clilen = sizeof(serveraddr);
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)
	{
		perror("socket error : ");
		exit(0);
	}

	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVER_IP);
	serveraddr.sin_port = htons(PORT);

	u_long iMode = 1;
	int result = ioctlsocket(sockfd, FIONBIO, &iMode);
	if (result != NO_ERROR){
		printf("ioctlsocket failed with error: %ld\n", result);
	}

	

	// TRY LOGIN
	while (1)
	{
		LoginPacket outPacket;
		outPacket.m_ChannelNumber = 1;
		outPacket.m_ID = 2;
		sendto(sockfd, (char*)&outPacket, sizeof(outPacket), 0, (struct sockaddr *)&serveraddr, clilen);

		Sleep(100);

		int n = recvfrom(sockfd, buf, MAX_PACKET_SIZE, 0, NULL, NULL);
		if (n == sizeof(LoginPacket) && memcmp(&outPacket, buf, n) == 0)
			break;
	}
	printf("Login Success!\n");

#ifdef RECV
	// recv data
	while (1)
	{
		int n = recvfrom(sockfd, buf, MAX_PACKET_SIZE, 0, NULL, NULL);
		if (n > 0)
		{
			printf("%s", buf);
		}
	}
#endif 

#ifdef SEND
	// send data
	while (1)
	{
		NormalPacketHeader outPacketHeader;
		outPacketHeader.m_ChannelNumber = 1;
		outPacketHeader.m_ID = 2;
		memcpy(buf, &outPacketHeader, sizeof(NormalPacketHeader));
		scanf("%s",buf + sizeof(NormalPacketHeader));
		sendto(sockfd, buf, sizeof(NormalPacketHeader) + strlen(buf + sizeof(NormalPacketHeader)), 0, (struct sockaddr *)&serveraddr, clilen);
	}
#endif



	closesocket(sockfd);
}