//
//  main.c
//  PacketLossTester
//
//  Created by echo on 2014. 12. 8..
//  Copyright (c) 2014년 echo. All rights reserved.
//

#include <iostream>
#include "util.h"
#include <stdlib.h>

int g_recv;

void tryRecv(int sock)
{
    int input = 0;
    
    ssize_t ret = Recv((char*)&input, sizeof(int), sock);
    
    if(ret > 0 )
    {
        std::cout << "recv : " << input << std::endl;
        
        g_recv ++;
    }
}

int main(int argc, const char * argv[]) {
    // insert code here...
    g_recv = 0;
    
    int w_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (w_socket < 0)
        return -1;
    
    
    int r_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (r_socket < 0)
        return -1;
    
    int loss = 0;
    scanf("%d",&loss);
    if ( Login(40, 80, loss, w_socket) < 0) return -1;
    if ( Login(40, 81, loss, r_socket) < 0) return -1;
    
    
    for( int i = 0 ; i < 100; i++)
    {			
        std::cout << "send packet : "<< i << std::endl;
        Send((char*)&i, sizeof(int), w_socket, 40, 80);
        tryRecv(r_socket);
    }
    for (int i = 0 ; i < 1000; i++)
    {
        tryRecv(r_socket);
    
        for ( int a = 0 ; a < 100000; a++)
        {
            int b = 0;
            b ++;
        }
    }
    
    printf("%d",g_recv);
    
    
    return 0;
}
