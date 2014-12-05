#include "stdafx.h"
#include "ChannelMananger.h"


bool IsLoginPacket(char* buf, int length);
bool IsRightPacket(char* buf, int length);

int main(int argc, char **argv)
{
	int sockfd;
	socklen_t clilen;
	int state;
	int n;
	struct sockaddr_in serveraddr, clientaddr;
	char buf[MAX_PACKET_SIZE];
	
	clilen = sizeof(clientaddr);

	ChannelMananger::GetInstance()->Init();

	
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)
	{
		perror("socket error : ");
		exit(0);
	}

	memset(&serveraddr, 0, sizeof(serveraddr));
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
            
            std::cout << "login! CI : " << packet.m_ChannelNumber << "/" << packet.m_ID << std::endl;

            
			bool ret = ChannelMananger::GetInstance()->Login(packet.m_ChannelNumber, packet.m_ID, clientaddr);

			if (ret == true)
            {
                ChannelMananger::GetInstance()->SetPacketMissingRate(packet.m_ChannelNumber, packet.m_RateOfMissing);
                std::cout<<"Packet Rate : Channel : "<< packet.m_ChannelNumber <<" - "<< ChannelMananger::GetInstance()->GetPacketMissingRate(packet.m_ChannelNumber) <<"%%"<< std::endl;
                
                std::cout<<"LOGIN : " << packet.m_ChannelNumber << "/" << packet.m_ID << std::endl;
				sendto(sockfd, buf, sizeof(LoginPacket), 0, (struct sockaddr *)&clientaddr, clilen);
            }
            else
            {
                std::cout<<"LOGIN FAIL : " << packet.m_ChannelNumber << "/" << packet.m_ID << std::endl;
            }
		}
		else
		{
			if (n < sizeof(ChannelNumber) + sizeof(ID))
				continue;

			NormalPacketHeader header;
			memcpy(&header, buf, sizeof(header));
			
            std::cout<<"DATA : CI : " << header.m_ChannelNumber <<" / " << header.m_ID <<" [" <<n<< "]\n" << std::endl;
            
            for (int i = 0; i < n; i++)
            {
                std::cout << (int)buf[i] << "/";
            }
            std::cout << std::endl;
            
			if (ChannelMananger::GetInstance()->IsLogin(header.m_ChannelNumber, header.m_ID, clientaddr) == false)
				continue;
            int randvalue = rand() % 100;
            if (ChannelMananger::GetInstance()->GetPacketMissingRate(header.m_ChannelNumber) < randvalue)
            {
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
	}


	close(sockfd);
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
