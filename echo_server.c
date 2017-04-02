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
* 宏定义
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
    *调用系统函数wait回收子进程资源,避免子进程僵死
    *此函数只会调用一次,因此在收到多个SIGCHDL时会被忽略,且当未有子进程结束时会阻塞
    **/
    #if 0
    pid = wait(&stat);
    printf("child %d terminated\n", pid);
    #endif
    /**
    *用waitpid,设置选项WNOHANG为不阻塞
    **/
    while(waitpid(-1, &stat, WNOHANG) > 0)
    {
        printf("child %d terminated\n", pid);
    }
    
    return;    
}

/********************************************************
brief  :  Echo_WriteRead    --回显写读    
input  :  argc              --参数个数
          argv              --参数指针数组
output :  无
return :  0 --成功    -1 --失败
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
        * 发生中断,继续读
        **/
        if(n < 0 && errno == EINTR)
            goto again;
        else if(n < 0)
            printf("read failed");
            
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
    int         iListenfd = 0;
    int         iAcceptfd = 0;
    socklen_t   len       = 0;
    pid_t       pid       = 0;
    struct sockaddr_in  stServaddr;
    struct sockaddr_in  stClientaddr;
   
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
    stServaddr.sin_port = htons(LOCAL_PORT);
    stServaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(iListenfd, (struct sockaddr *)&stServaddr, sizeof(stServaddr));

    listen(iListenfd, LISTENQ);

    /**
    *调用信号处理函数捕捉子进程退出的信号:SIGCHLD
    **/
    signal(SIGCHLD, signalhandel);

    while(1)
    {
        memset(&stClientaddr, 0, sizeof(stClientaddr));
        len = sizeof(stClientaddr);
        
        /**
        * 阻塞直到有连接,当某些系统发生中断后,阻塞调用的函数不会重新启动
        * 需要避免此类情况
        **/
        if(0 > (iAcceptfd = accept(iListenfd, (struct sockaddr *)&stClientaddr, &len)))
        {
            /**
            *发生了中断,重新调用accept
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
        * 创建子进程处理连接
        **/
        if(0 == (pid = fork()))
        {
            /**
            * 父子进程共享fd,且计数自增1
            **/
            close(iListenfd);
            Server_EschoWriteRead(iAcceptfd);

            /**
            * 子进程处理完毕,退出子进程
            **/
            _exit(0);
        }

        close(iAcceptfd);
    }

}

