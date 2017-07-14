#include "socket_common.h"

int main()
{
	int listenfd, connectfd, acceptfd;
	struct sockaddr_in listen_addr;
	struct sockaddr_in connect_addr;
	socklen_t addrlen;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	
	memset(&listen_addr, 0, sizeof(listen_addr));
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	listen_addr.sin_port = 0;	/* kernel chooses port.	 */
	
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, NULL, 0);
	
	if (bind(listenfd, (struct sockaddr *) &listen_addr, sizeof (listen_addr))
		== -1)
	{
		printf("bind failed");
		return -1;
	}
	
	if(listen(listenfd, 1) == -1)
	{
		printf("listen failed[%s]\n", strerror(errno));
		return -1;
	}
	
	connectfd = socket(AF_INET, SOCK_STREAM, 0);
	
	memset(&connect_addr, 0, sizeof(connect_addr));
	addrlen = sizeof(connect_addr);
	if (getsockname(listenfd, (struct sockaddr *) &connect_addr, &addrlen) == -1)
	{
		printf("getsockname failed[%s]\n", strerror(errno));
		return -1;
	}
	
	connect(connectfd, (struct sockaddr *)&connect_addr, sizeof(connect_addr));
	
	addrlen = sizeof(listen_addr);
	acceptfd = accept(listenfd, (struct sockaddr *) &listen_addr, &addrlen);
	if (acceptfd < 0)
	{
		printf("acceptor failed");
		return -1;
	}

	if (getsockname(connectfd, (struct sockaddr *) &connect_addr, &addrlen) == -1)
	{
		printf("getsockname failed");
		return -1;
	}
	
	if (addrlen == sizeof (connect_addr)
		&& listen_addr.sin_family == connect_addr.sin_family
		&& listen_addr.sin_addr.s_addr == connect_addr.sin_addr.s_addr
		&& listen_addr.sin_port == connect_addr.sin_port)
	{
		printf("addr:%s, port:%d\n", inet_ntoa(listen_addr.sin_addr.s_addr), ntohs(listen_addr.sin_port));
	}
	
	return 0;
}
