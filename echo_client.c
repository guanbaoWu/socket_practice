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
brief  :  Echo_WriteRead    --回显写读    
input  :  argc              --参数个数
          argv              --参数指针数组
output :  无
return :  0 --成功    -1 --失败
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
brief  :  Echo_WriteRead    --回显写读    
input  :  argc              --参数个数
          argv              --参数指针数组
output :  无
return :  0 --成功    -1 --失败
********************************************************/
int Echo_Fgets(FILE *fp, int iSocket)
{
    char acSendBuf[MAX_BUF_SIZE]    = {0};
    char acRecvBuf[MAX_BUF_SIZE]    = {0};

    ECHO_EXPRE_RETURN(iSocket > 0, -1);

    /**
    * 从标准输入读取一行字符串,发送
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
        *输出到标准输出
        **/
        fputs(acRecvBuf, stdout);
    }
    
    return 0;
}


/********************************************************
brief  :  主函数
input  :  argc  --参数个数
          argv  --参数指针数组
output :  无
return :  0 --成功    -1 --失败
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
    * 创建socket
    **/
    iSockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == iSockfd)
    {
        printf("create socket failed\n");
        return -1;
    }

    /**
    *连接服务器
    **/
    memset(&stServaddr, 0, sizeof(stServaddr));
    memset(&stLocaladdr, 0, sizeof(stLocaladdr));

    stServaddr.sin_family = AF_INET;
    stServaddr.sin_port = htons(REMOTE_PORT);
    inet_pton(AF_INET, argv[1], &stServaddr.sin_addr);

    connect(iSockfd, (struct sockaddr *)&stServaddr, sizeof(stServaddr));

    /**
    * 获取本地内核分配的地址和端口
    * @len必须初始值为第二个参数的长度
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
    * 回显字符串到服务器
    **/
    Echo_Fgets(stdin, iSockfd);

    close(iSockfd);
    
    return 0;    
}



