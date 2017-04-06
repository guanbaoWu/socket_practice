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
#include <poll.h>
#include "socket_common.h"


/********************************************************
brief  :  ������
input  :  argc  --��������
          argv  --����ָ������
output :  ��
return :  0 --�ɹ�    -1 --ʧ��
********************************************************/
int main(int argc, char *argv[])
{
    int         i                           = 0;
    int         n                           = 0;
    int         iMaxIndex                   = 0;
    int         inReady                     = 0;
    int         iListenfd                   = 0;
    int         iAcceptfd                   = 0;
    int         iSocket                     = 0;
    char        acRecvBuf[MAX_BUF_SIZE]     = {0};
    socklen_t   len                         = 0;
    struct sockaddr_in  stServaddr;
    struct sockaddr_in  stClientaddr;

    struct pollfd stClientArry[MAX_FD_SIZE];

    /**
    * ����socket
    **/
    iListenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == iListenfd)
    {
        printf("create socket failed\n");
        return -1;
    }

    /**
    *��,����
    **/
    memset(&stServaddr, 0, sizeof(stServaddr));

    stServaddr.sin_family = AF_INET;
    stServaddr.sin_port = htons(LISTEN_PORT);
    stServaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(iListenfd, (struct sockaddr *)&stServaddr, sizeof(stServaddr));

    listen(iListenfd, LISTENQ);

    /**
    * ��ʼ��poll�ṹ������
    **/
    for(i = 1; i < MAX_FD_SIZE; i++)
    {
        stClientArry[i].fd = -1;
    }

    /**
    * �õ�һ���¼�
    **/
    stClientArry[0].fd = iListenfd;
    stClientArry[0].events = POLLRDNORM;

    while(1)
    {
        inReady = poll(stClientArry, iMaxIndex + 1, -1);

        if(stClientArry[0].revents & POLLRDNORM)
        {
            memset(&stClientaddr, 0, sizeof(stClientaddr));
            len = sizeof(stClientaddr);
            
            iAcceptfd = accept(iListenfd, (struct sockaddr *)&stClientaddr, &len);

            /**
            * �������ӵ��׽���
            **/
            for(i = 1; i < MAX_FD_SIZE; i++)
            {
                if(stClientArry[i].fd == -1)
                {
                    stClientArry[i].fd = iAcceptfd;
                    stClientArry[i].events = POLLRDNORM;
                    break;
                }
            }

            if(i == MAX_FD_SIZE)
            {
                printf("too many connect\n");
                return -1;
            }

            iMaxIndex = i >= iMaxIndex ? i :iMaxIndex;
            
            if(--inReady <= 0)
                continue;
        }

        for(i = 1; i <= iMaxIndex; i++)
        {
            if((stClientArry[i].fd >=0) && (stClientArry[i].revents & (POLLRDNORM | POLLERR)))
            {
                iSocket = stClientArry[i].fd;
                if((n = read(iSocket, acRecvBuf, MAX_BUF_SIZE)) < 0)
                {
                    if(errno == ECONNRESET)
                    {
                        close(iSocket);
                        stClientArry[i].fd = -1;              
                    }
                    else
                    {
                        printf("read error\n");
                        return -1;
                    }
                }
                else if(n == 0)
                {
                    close(iSocket);
                    stClientArry[i].fd = -1;  
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


