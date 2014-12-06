//
// util.c
// Network for FSM sample code
//
// Created by Nam SeHyun, 2014.12.1.
// Copyright (c) 2014. Nam SeHyun All rights reserved.
// No License.

#include "util.h"

int sockfd;
struct sockaddr_in serveraddr;
ChannelNumber g_channel;
ID g_id;

int Login(unsigned int channel, unsigned int id, int lossRate)
{
    char buf[MAX_PACKET_SIZE];
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        return -1;
    
    // init serveraddr
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    serveraddr.sin_port = htons(PORT);
    
    // set non-block socket
    int flag;
    flag = fcntl( sockfd, F_GETFL, 0 );
    fcntl( sockfd, F_SETFL, flag | O_NONBLOCK );
    
    // prepare login packet
    LoginPacket outPacket;
    outPacket.m_UniqueKey = LOGIN_UNIQUE_KEY;
    outPacket.m_ChannelNumber = channel;
    outPacket.m_ID = id;
    outPacket.m_LossRate = lossRate;
    
    int tryCount = 0;
    while (tryCount < LOGIN_MAX_TRY_COUNT) {
        tryCount++;
        
        ssize_t sendLen = sendto(sockfd, (char*)&outPacket, sizeof(outPacket), 0, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
        if (sendLen < 0) {
            printf("Error for Send : %d / %s\n",errno, strerror(errno));
            printf("Maybe Server Is Down. Plz Ask to ServerManager - 010-4345-4341\n");
            return -1;
        }
        
        sleep(1);
        
	ssize_t n = recvfrom(sockfd, buf, MAX_PACKET_SIZE, 0, NULL, NULL);
		
        if ((n == sizeof(LoginPacket)) && (memcmp(&outPacket, buf, n) == 0)) {
            g_channel = channel;
            g_id = id;
            return 0;
        } else {
            if( n < 0)
                continue;
            else
                return -1;
        }
    }
    
    // tryCount over LOGIN_MAX_TRY_COUNT
    return -1;
}

int Send(char* buf, size_t length)
{
    NormalPacket outPacket;
    outPacket.m_ChannelNumber = g_channel;
    outPacket.m_ID = g_id;

    memcpy(outPacket.m_Data, buf, length);
    ssize_t n = sendto(sockfd, (char*)&outPacket, length + sizeof(ChannelNumber) + sizeof(ID), 0, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if ((n < 0) || (n != length))
        return -1;
    return 0;
}

ssize_t Recv(char* buf, int maxLength)
{
    ssize_t n = recvfrom(sockfd, buf, maxLength, 0, NULL, NULL);
    if ((n == -1) && ((errno == EWOULDBLOCK) || (errno == EAGAIN)))
        return 0;
    return n;
}

int kbhit(void)
{
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}

