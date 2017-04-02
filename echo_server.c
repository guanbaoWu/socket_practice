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
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>



/**
* �궨��
**/
#define MAX_BUF_SIZE                128
#define LOCAL_IP_ADDR               127.0.0.1
#define LOCAL_PORT                  5099
#define LISTENQ                     10


#define ECHO_EXPRE_RETURN(expr, retval)     \
    do{ \
    if(!(expr)){ return retval; } \
    }while(0)   \

/**
*@brief:signal(SIGCHLD) handle func
**/
void signalhandel(int signal)
{
    pid_t   pid;
    int     stat;

    /**
    *����ϵͳ����wait�����ӽ�����Դ,�����ӽ��̽���
    *�˺���ֻ�����һ��,������յ����SIGCHDLʱ�ᱻ����,�ҵ�δ���ӽ��̽���ʱ������
    **/
    #if 0
    pid = wait(&stat);
    printf("child %d terminated\n", pid);
    #endif
    /**
    *��waitpid,����ѡ��WNOHANGΪ������
    **/
    while(waitpid(-1, &stat, WNOHANG) > 0)
    {
        printf("child %d terminated\n", pid);
    }
    
    return;    
}

/********************************************************
brief  :  Echo_WriteRead    --����д��    
input  :  argc              --��������
          argv              --����ָ������
output :  ��
return :  0 --�ɹ�    -1 --ʧ��
********************************************************/
int Server_EschoWriteRead(int iSocket)
{
    ssize_t n = 0;
    char acRecvBuf[MAX_BUF_SIZE]    = {0};

    ECHO_EXPRE_RETURN(iSocket > 0, -1);

    again:
    
        while((n = (read(iSocket, acRecvBuf, MAX_BUF_SIZE-1))) > 0)
        {
            printf("Server_recv:%s\n", acRecvBuf);
            write(iSocket, acRecvBuf, n);
            memset(acRecvBuf, 0, MAX_BUF_SIZE);
        }

        /**
        * �����ж�,������
        **/
        if(n < 0 && errno == EINTR)
            goto again;
        else if(n < 0)
            printf("read failed");
            
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
    int         iListenfd = 0;
    int         iAcceptfd = 0;
    socklen_t   len       = 0;
    pid_t       pid       = 0;
    struct sockaddr_in  stServaddr;
    struct sockaddr_in  stClientaddr;
   
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
    stServaddr.sin_port = htons(LOCAL_PORT);
    stServaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(iListenfd, (struct sockaddr *)&stServaddr, sizeof(stServaddr));

    listen(iListenfd, LISTENQ);

    /**
    *�����źŴ�������׽�ӽ����˳����ź�:SIGCHLD
    **/
    signal(SIGCHLD, signalhandel);

    while(1)
    {
        memset(&stClientaddr, 0, sizeof(stClientaddr));
        len = sizeof(stClientaddr);
        
        /**
        * ����ֱ��������,��ĳЩϵͳ�����жϺ�,�������õĺ���������������
        * ��Ҫ����������
        **/
        if(0 > (iAcceptfd = accept(iListenfd, (struct sockaddr *)&stClientaddr, &len)))
        {
            /**
            *�������ж�,���µ���accept
            **/
            if(errno == EINTR)
                continue;
            else
            {
                printf("accept socket failed\n");
                return -1;
            }
        }

        /**
        * �����ӽ��̴�������
        **/
        if(0 == (pid = fork()))
        {
            /**
            * ���ӽ��̹���fd,�Ҽ�������1
            **/
            close(iListenfd);
            Server_EschoWriteRead(iAcceptfd);

            /**
            * �ӽ��̴������,�˳��ӽ���
            **/
            _exit(0);
        }

        close(iAcceptfd);
    }

}

