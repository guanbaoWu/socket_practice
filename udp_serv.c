/********************************************************
*Copyright (C)2016-2017 Wu Practise Project
*
*This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation
********************************************************/

/*
  This example program server(UDP).
*/
#include "socket_common.h"

/********************************************************
brief  :  主函数
input  :  argc  --参数个数
          argv  --参数指针数组
output :  无
return :  0 --成功    -1 --失败
********************************************************/
int main(int argc, char *argv[])
{
    int iSocketId   = -1;
    int iReadByte   = 0;
    int iLen        = 0;
    
    char acSendBuf[MAX_BUF_SIZE]    = {0};
    char acRecvBuf[MAX_BUF_SIZE]    = {0};
    char acClient[MAX_BUF_SIZE]     = {0};
    
    struct sockaddr_in stCliaddr;
    struct sockaddr_in stServaddr;
    
    /**
    * 创建Socket(UDP)
    **/
    iSocketId = socket(AF_INET, SOCK_DGRAM, 0);
    if(-1 == iSocketId)
    {
        printf("create socket failed\n");
        return -1;
    }

    memset(&stServaddr, 0, sizeof(stServaddr));

    stServaddr.sin_family = AF_INET;
    stServaddr.sin_port = htons(LISTEN_PORT);
    stServaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(iSocketId, (struct sockaddr *)&stServaddr, sizeof(stServaddr));

    iLen = sizeof(stCliaddr);
    while(1)
    {

        memset(&stCliaddr, 0, sizeof(stCliaddr));
        
        /* arg6 and arg7 point */
        iReadByte = recvfrom(iSocketId, acRecvBuf, MAX_BUF_SIZE, 0, (struct sockaddr *)&stCliaddr, &iLen);
        if(0 > iReadByte)
        {
            if(errno == EINTR)
            {
                continue;
            }
            else
            {
                return -1;
            }
        }
        else if(0 == iReadByte)
        {
            printf("peer shutdown\n");
            return 0;
        }

        acRecvBuf[iReadByte] = '\0';
        
        /**
        *输出到标准输出
        **/
        fputs(acRecvBuf, stdout);

        sendto(iSocketId, acRecvBuf, iReadByte, 0, (struct sockaddr *)&stCliaddr, iLen);

        //(void)inet_ntop(AF_INET, &stCliaddr.sin_addr, acClient, iLen);

        printf("recv from client:%s:%d\n", inet_ntoa(stCliaddr.sin_addr.s_addr), ntohs(stCliaddr.sin_port));

    }

    return 0;
    
}

