/********************************************************
*Copyright (C)2016-2017 Wu Practise Project
*
*This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation
********************************************************/

/*
  This example program echo client(TCP) by select.
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
    int         i                           = 0;
    int         n                           = 0;
    int         iListenfd                   = 0;
    int         iAcceptfd                   = 0;
    int         iSocket                     = 0;
    int         iMaxIndex                   = -1;
    int         iMaxFd                      = 0;
    int         iclient[MAX_FD_SIZE]        = {0};
    char        acRecvBuf[MAX_BUF_SIZE]     = {0};
    int         inReady                     = 0;
    socklen_t   len                         = 0;
    pid_t       pid                         = 0;
    struct sockaddr_in  stServaddr;
    struct sockaddr_in  stClientaddr;
    fd_set readfds, allsets;
    
   
    /**
    * 创建socket
    **/
    iListenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == iListenfd)
    {
        printf("create socket failed\n");
        return -1;
    }

    /**
    *绑定,监听
    **/
    memset(&stServaddr, 0, sizeof(stServaddr));

    stServaddr.sin_family = AF_INET;
    stServaddr.sin_port = htons(LISTEN_PORT);
    stServaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(iListenfd, (struct sockaddr *)&stServaddr, sizeof(stServaddr));

    listen(iListenfd, LISTENQ);

    /**
    * 初始化fd数组
    **/
    memset(iclient, -1, MAX_FD_SIZE);

    iMaxFd = iListenfd;


    /**
    * 初始化描述符集
    **/
    FD_ZERO(&allsets);
    FD_SET(iListenfd, &allsets);
    
    while(1)
    {

        /**
        *每次调用select前初始化描述符集
        **/
        readfds = allsets;

        printf("iMaxFd:%d\n", iMaxFd);
        
        inReady = select(iMaxFd+1, &readfds, NULL, NULL, NULL);

        printf("recv events:%d\n", inReady);

        if(FD_ISSET(iListenfd, &readfds))
        {
            memset(&stClientaddr, 0, sizeof(stClientaddr));
            len = sizeof(stClientaddr);
            iAcceptfd = accept(iListenfd, (struct sockaddr *)&stClientaddr, &len);

            printf("recv new client accept:%d\n", iAcceptfd);
            
            /**
            * 新的描述符加入描述符集
            **/
            for(i = 0; i < MAX_FD_SIZE; i++)
            {
                if(iclient[i] == -1)
                {
                    iclient[i] = iAcceptfd;
                    break;
                }
            }

            if(i == MAX_FD_SIZE)
            {
                printf("too many clients\n");
            }

            FD_SET(iAcceptfd, &allsets);

            iMaxFd = iMaxFd < iAcceptfd ? iAcceptfd : iMaxFd;
            iMaxIndex = i >= iMaxIndex ? i : iMaxIndex;

            if(--inReady <= 0)
                continue;
                
        }

        for(i = 0; i <= iMaxIndex; i++)
        {
            printf("recv iSockets:%d\n", iclient[i]);
            
            if(iclient[i] >= 0 && FD_ISSET(iclient[i], &readfds))
            {
                
                iSocket = iclient[i];
                memset(acRecvBuf, 0, MAX_BUF_SIZE);

                /**
                * 收对端FIN,从描述符集中删除此描述符
                **/
                if((n = read(iSocket, acRecvBuf, MAX_BUF_SIZE)) == 0)
                {
                     printf("remote terminal!!\n");
                     close(iSocket);
                     FD_CLR(iSocket, &allsets);
                     iclient[i] = -1;
                }
                else
                {
                    printf("Server_recv:%.*s\n", strlen(acRecvBuf), acRecvBuf);
                    write(iSocket, acRecvBuf, n);
                }

                if(--inReady <= 0)
                    break;
            }
            
        }

    }

}


