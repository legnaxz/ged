#include  <stdio.h>
#include  <string.h>
#include  <errno.h>
#include  <unistd.h>
#include  <sys/types.h>
#include  <sys/socket.h>
#include  <netinet/in.h>
#include  <netinet/tcp.h>
#include  <arpa/inet.h>

#include  "tcpserver.h"
#include  "cfg.h"
#include  "vasifhandler.h"
#include  "log.h"


#define  CMD_SWITCH_BACK	0x0001
#define  CMD_READ_CONFIG	0x0002
#define  EVT_SWITCH_BACK	0x0101
#define  EVT_READ_CONFIG	0x0102


typedef struct
{
	unsigned short	size;
	unsigned short	cmd_evt;
	char			vasno;
	char			result[3];
} S_SWITCH_BACK;


extern char		*g_pConfigFile;


CTcpServer::CTcpServer()
{
	m_bStopFlag = false;
}

CTcpServer::~CTcpServer()
{
	m_bStopFlag = true;
}

int CTcpServer::CreateServer(unsigned short usPort)
{
	if(m_cSocket.CreateSocket(usPort) != 0) return -1;
	if(m_cSocket.Listen() != 0) return -1;

	return 0;
}

int CTcpServer::DestroyServer()
{
	m_cSocket.DestroySocket();
}

void CTcpServer::Run()
{
	int					i, sock, server_sock, nAllowed, index;
	struct sockaddr_in	cli_addr;
	pthread_t			tid;
	socklen_t			cli_len;
	char				szClientAddr[255], buf[255];
	S_SWITCH_BACK		rsp;

	LogPrintf(0, 0, TM_INFO, "TCP Server started successfully!!!\n");

	server_sock = m_cSocket.GetSocket();
	while(m_bStopFlag == false)
	{
		cli_len = sizeof(cli_addr);
		sock = accept(server_sock, (struct sockaddr *) &cli_addr, &cli_len);
		if(sock < 0)
		{
			if(errno != EAGAIN)
				LogPrintf(0, 0, TM_APIERROR, "server(accept): can't accept stream socket, %s\n", strerror(errno));
			else
				LogPrintf(0, 0, TM_APIERROR, "server(accept): EAGAIN\n");
			continue;
		}

		strcpy(szClientAddr, inet_ntoa(cli_addr.sin_addr));

		// Set a client socket option
		if(SetClientSockOpt(sock) == -1)
		{
			close(sock);
			LogPrintf(0, 0, TM_INFO, "close a socket(sock=%d) due to setting a socket option failure\n", sock);
			continue;
		}

		// Parsing the packet
		if(recv(sock, buf, sizeof(buf), 0) > 0)
		{
			S_SWITCH_BACK *p = (S_SWITCH_BACK *) buf;
			p->size = ntohs(p->size);
			p->cmd_evt= ntohs(p->cmd_evt);
			LogPrintf(0, 0, TM_INFO, "received (size=%d, cmd=0x%x)\n", p->size, p->cmd_evt);

			switch(p->cmd_evt)
			{
			case CMD_SWITCH_BACK:
				// send a changeback request to a VAS.
				index = p->vasno - '1';
				g_vasIfHandler[index]->SetVasChangeBack(1);

				rsp.size = htons(sizeof(S_SWITCH_BACK));
				rsp.cmd_evt = htons(EVT_SWITCH_BACK);
				rsp.vasno = p->vasno;
				strncpy(rsp.result, "SUC", 3);
				send(sock, (char*) &rsp, sizeof(S_SWITCH_BACK), 0);
				break;

			case CMD_READ_CONFIG:
				if(LoadParameters(g_pConfigFile) == -1)
				{
					LogPrintf(0, 0, TM_INFO, "Failed to read a config file, %s\n", g_pConfigFile);
					rsp.size = htons(sizeof(S_SWITCH_BACK));
					rsp.cmd_evt = htons(EVT_READ_CONFIG);
					rsp.vasno = '0';
					strncpy(rsp.result, "FAI", 3);
					send(sock, (char*) &rsp, sizeof(S_SWITCH_BACK), 0);
				}
				else
				{
					rsp.size = htons(sizeof(S_SWITCH_BACK));
					rsp.cmd_evt = htons(EVT_READ_CONFIG);
					rsp.vasno = '0';
					strncpy(rsp.result, "SUC", 3);
					send(sock, (char*) &rsp, sizeof(S_SWITCH_BACK), 0);
				}
				break;
			}
		}
		shutdown(sock, SHUT_RDWR);
		close(sock);
	}
}

int CTcpServer::SetClientSockOpt(int sockfd)
{
	int			v, sndbuf;
	socklen_t	len;
    struct timeval	tv;
	struct linger	sLinger;

	// set no delay for sending
	v = 1;
	if(setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &v, sizeof(v)) < 0)
	{
		LogPrintf(0, 0, TM_APIERROR, "setsockopt(sock=%d) TCP_NODELAY failure - %s\n", sockfd, strerror(errno));
		return -1;
	}

	len = sizeof(sndbuf);
	if(getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &sndbuf, &len) < 0)
	{
		LogPrintf(0, 0, TM_APIERROR, "getsockopt(sock=%d) SO_RCVBUF failure - %s\n", sockfd, strerror(errno));
		return -1;
	}
	sndbuf = 1000000;
	if(setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &sndbuf, sizeof(sndbuf)) < 0)
	{
		LogPrintf(0, 0, TM_APIERROR, "setsockopt(sock=%d) SO_RCVBUF failure - %s\n", sockfd, strerror(errno));
		return -1;
	}
	if(setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &sndbuf, sizeof(sndbuf)) < 0)
	{
		LogPrintf(0, 0, TM_APIERROR, "setsockopt(sock=%d) SO_SNDBUF failure - %s\n", sockfd, strerror(errno));
		return -1;
	}
	if(getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &sndbuf, &len) < 0)
	{
		LogPrintf(0, 0, TM_APIERROR, "getsockopt(sock=%d) SO_RCVBUF failure - %s\n", sockfd, strerror(errno));
		return -1;
	}

	// set socket recv/send timeout
	tv.tv_sec = 10;
	tv.tv_usec = 0;

	if(setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(tv)) < 0)
	{
		LogPrintf(0, 0, TM_APIERROR, "setsockopt(sock=%d) SO_RECVTIMEO failure - %s\n", sockfd, strerror(errno));
		return -1;
	}
	if(setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(tv)) < 0)
	{
		LogPrintf(0, 0, TM_APIERROR, "setsockopt(sock=%d) SO_SNDTIMEO failure - %s\n", sockfd, strerror(errno));
		return -1;
	}

	sLinger.l_onoff = 1;
	sLinger.l_linger = 0;
	if(setsockopt(sockfd, SOL_SOCKET, SO_LINGER, &sLinger, sizeof(sLinger)) < 0)
	{
		LogPrintf(0, 0, TM_APIERROR, "setsockopt(sock=%d) SO_SNDTIMEO failure - %s\n", strerror(errno));
		return -1;
	}

	return 0;
}
