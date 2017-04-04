/********************************************************
*Copyright (C)2016-2017 Wu Practise Project
*
*This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation
********************************************************/

/*
  This example program echo client(TCP).
*/
#include "socket_common.h"


/********************************************************
brief  :  Echo_WriteRead    --����д��    
input  :  argc              --��������
          argv              --����ָ������
output :  ��
return :  0 --�ɹ�    -1 --ʧ��
********************************************************/
int Echo_WriteRead(int iSocket, char *pBuf)
{
    char acSendBuf[MAX_BUF_SIZE]    = {"hello client"};
    char acRecvBuf[MAX_BUF_SIZE]    = {0};

    ECHO_EXPRE_RETURN(iSocket > 0, -1);

    write(iSocket, pBuf, strlen(pBuf));

    while((read(iSocket, acRecvBuf, MAX_BUF_SIZE-1)) > 0)
    {
        printf("echo:%s\n", acRecvBuf);
    }

    return 0;
}

/********************************************************
brief  :  Echo_WriteRead    --����д��    
input  :  argc              --��������
          argv              --����ָ������
output :  ��
return :  0 --�ɹ�    -1 --ʧ��
********************************************************/
int Echo_Fgets(FILE *fp, int iSocket)
{
    char acSendBuf[MAX_BUF_SIZE]    = {0};
    char acRecvBuf[MAX_BUF_SIZE]    = {0};

    ECHO_EXPRE_RETURN(iSocket > 0, -1);

    /**
    * �ӱ�׼�����ȡһ���ַ���,����
    **/
    while(fgets(acSendBuf, MAX_BUF_SIZE, fp))
    {
        write(iSocket, acSendBuf, strlen(acSendBuf));

        memset(acRecvBuf, 0, MAX_BUF_SIZE);
        if((read(iSocket, acRecvBuf, MAX_BUF_SIZE-1)) == 0)
        {
            printf("Echo_Fgets EOF\n");
            return 0;
        }

        /**
        *�������׼���
        **/
        fputs(acRecvBuf, stdout);
    }
    
    return 0;
}


/********************************************************
brief  :  ������
input  :  argc  --��������
          argv  --����ָ������
output :  ��
return :  0 --�ɹ�    -1 --ʧ��
********************************************************/
int main(int argc, char *argv[])
{
    int         iSockfd = 0;
    int         iStatus = -1;
    socklen_t   len     =0;
    struct sockaddr_in  stServaddr;
    struct sockaddr_in  stLocaladdr;
   
    ECHO_EXPRE_RETURN(argc > 1, -1);

    /**
    * ����socket
    **/
    iSockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == iSockfd)
    {
        printf("create socket failed\n");
        return -1;
    }

    /**
    *���ӷ�����
    **/
    memset(&stServaddr, 0, sizeof(stServaddr));
    memset(&stLocaladdr, 0, sizeof(stLocaladdr));

    stServaddr.sin_family = AF_INET;
    stServaddr.sin_port = htons(REMOTE_PORT);
    inet_pton(AF_INET, argv[1], &stServaddr.sin_addr);

    connect(iSockfd, (struct sockaddr *)&stServaddr, sizeof(stServaddr));

    /**
    * ��ȡ�����ں˷���ĵ�ַ�Ͷ˿�
    * @len�����ʼֵΪ�ڶ��������ĳ���
    **/
    len = sizeof(stLocaladdr);
    iStatus = getsockname(iSockfd, (struct sockaddr *)&stLocaladdr, &len);
    if(0 == iStatus)
    {
        printf("local addr:%s, port:%d\n", 
                inet_ntoa(stLocaladdr.sin_addr.s_addr), 
                ntohs(stLocaladdr.sin_port));
    }
    

    /**
    * �����ַ�����������
    **/
    Echo_Fgets(stdin, iSockfd);

    close(iSockfd);
    
    return 0;    
}



