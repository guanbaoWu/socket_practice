/********************************************************
*Copyright (C)2016-2017 Wu Practise Project
*
*This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation
********************************************************/

/**
* @brief:common head file.
**/

#ifndef SOCKET_COMMON_H_INCLUDED_
#define SOCKET_COMMON_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/time.h>


/**
* ∫Í∂®“Â
**/
#define MAX_BUF_SIZE                128
#define MAX_FD_SIZE                 1024

#define REMOTE_IP_ADDR              127.0.0.1
#define REMOTE_PORT                 5099

#define LOCAL_IP_ADDR               127.0.0.1
#define LISTEN_PORT                 5099
#define LISTENQ                     10


#define ECHO_EXPRE_RETURN(expr, retval)     \
    do{ \
    if(!(expr)){ return retval; } \
    }while(0)   \


#ifdef __cplusplus
}
#endif
#endif /* SOCKET_COMMON_H_INCLUDED_ */

