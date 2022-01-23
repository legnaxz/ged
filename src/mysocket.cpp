#include  <stdio.h>
#ifdef  WIN32
#include  <winsock.h>
#define	  _MT
#include  <process.h>
#else
#include  <sys/types.h>
#include  <sys/socket.h>
#include  <sys/poll.h>
#include  <netinet/in.h>
#include  <arpa/inet.h>
#include  <netinet/tcp.h>
#include  <unistd.h>
#include  <fcntl.h>
#endif

#include  <errno.h>
#include  <string.h>
#include  "mysocket.h"
#include  "log.h"


CMySocket::CMySocket()
{
	m_nSock = -1;
}

CMySocket::~CMySocket()
{
}

int CMySocket::CreateSocket(unsigned short usPort)
{
	int					v, sndrcvbuf, reuse;
	struct linger		sLinger;
	struct sockaddr_in	serv_addr;
    struct timeval		tv;

	if(m_nSock != -1)
	{
		LogPrintf(0, 0, TM_INFO, "Socket is already initialized!\n");
		return -1;
	}

	if((m_nSock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		LogPrintf(0, 0, TM_APIERROR, "Can't open socket, %s\n", strerror(errno));
		return -1;
	}

	v = 1;
	if(setsockopt(m_nSock, IPPROTO_TCP, TCP_NODELAY, &v, sizeof(v)) < 0)
	{
		LogPrintf(0, 0, TM_APIERROR, "Failed to set TCP_NODELAY, %s\n", strerror(errno));
		close(m_nSock);
		m_nSock = -1;
		return -1;
	}

	sndrcvbuf = 1000000;
	if(setsockopt(m_nSock, SOL_SOCKET, SO_RCVBUF, &sndrcvbuf, sizeof(sndrcvbuf)) < 0)
	{
		LogPrintf(0, 0, TM_APIERROR, "Failed to set SO_RCVBUF, %s\n", strerror(errno));
		close(m_nSock);
		m_nSock = -1;
		return -1;
	}
	if(setsockopt(m_nSock, SOL_SOCKET, SO_SNDBUF, &sndrcvbuf, sizeof(sndrcvbuf)) < 0)
	{
		LogPrintf(0, 0, TM_APIERROR, "Failed to set SO_SNDBUF, %s\n", strerror(errno));
		close(m_nSock);
		m_nSock = -1;
		return -1;
	}

	// set socket recv/send timeout
	tv.tv_sec = 10;
	tv.tv_usec = 0;
	if(setsockopt(m_nSock, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(tv)) < 0)
	{
		LogPrintf(0, 0, TM_APIERROR, "Failed to set SO_RECVTIMEO, %s\n", strerror(errno));
		close(m_nSock);
		m_nSock = -1;
		return -1;
	}
	if(setsockopt(m_nSock, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(tv)) < 0)
	{
		LogPrintf(0, 0, TM_APIERROR, "Failed to set SO_SNDTIMEO, %s\n", strerror(errno));
		close(m_nSock);
		m_nSock = -1;
		return -1;
	}

	reuse = 1;
	if(setsockopt(m_nSock, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0)
	{
		LogPrintf(0, 0, TM_APIERROR, "Failed to set SO_REUSEADDR, %s\n", strerror(errno));
		close(m_nSock);
		m_nSock = -1;
		return -1;
	}

	sLinger.l_onoff = 1;
	sLinger.l_linger = 0;
	if(setsockopt(m_nSock, SOL_SOCKET, SO_LINGER, &sLinger, sizeof(sLinger)) < 0)
	{
		LogPrintf(0, 0, TM_APIERROR, "Failed to set SO_LINGER, %s\n", strerror(errno));
		close(m_nSock);
		m_nSock = -1;
		return -1;
	}

	if(usPort != 0)
	{
		bzero((char*) &serv_addr, sizeof(serv_addr));
		serv_addr.sin_family		= AF_INET;
		serv_addr.sin_addr.s_addr	= htonl(INADDR_ANY);
		serv_addr.sin_port			= htons(usPort);
		if(bind(m_nSock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		{
			LogPrintf(0, 0, TM_APIERROR, "Failed to bind a local address, %s\n", strerror(errno));
			close(m_nSock);
			m_nSock = -1;
			return -1;
		}
	}

	return 0;
}

int CMySocket::Listen()
{
	if(listen(m_nSock, 1024) < 0)
	{
		LogPrintf(0, 0, TM_APIERROR, "server: listen, %s\n", strerror(errno));
		close(m_nSock);
		m_nSock = -1;
		return -1;
	}

	return 0;
}

int CMySocket::ConnectToServer(char *pIpAddr, unsigned short usPort)
{
	int					ret;
	struct sockaddr_in	serv_addr;

#ifdef WIN32
	memset(&serv_addr, 0x0, sizeof(serv_addr));
#else
	bzero((char*) &serv_addr, sizeof(serv_addr));
#endif
	serv_addr.sin_family      = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(pIpAddr);
	serv_addr.sin_port        = htons(usPort);
	if((ret = connect(m_nSock, (struct sockaddr *) &serv_addr, sizeof(serv_addr))) < 0)
	{
		LogPrintf(0, 0, TM_APIERROR, "Failed to connect to a server, %s:%d-%s.\n", pIpAddr, usPort, strerror(errno));
		close(m_nSock);
		m_nSock = -1;
		return -1;
	}

	return 0;
}

int CMySocket::GetSocket()
{
	return m_nSock;
}

void CMySocket::DestroySocket()
{
	if(m_nSock != -1)
	{
		shutdown(m_nSock, SHUT_RDWR);
		if(close(m_nSock) < 0)
		{
			LogPrintf(0, 0, TM_INFO, "socket(%d) close error, %s\n", m_nSock, strerror(errno));
		}
	}

	m_nSock = -1;
}

int	CMySocket::Send(char *pSendPkt, size_t length)
{
	int		ret;

	ret = send(m_nSock, pSendPkt, length, 0);
	if(ret <= 0)
	{
		LogPrintf(0, 0, TM_INFO, "%d bytes sent.\n", ret);
		LogPrintf(0, 0, TM_APIERROR, "send(fd=%d): Error when sending TCP packets, %s\n", m_nSock, strerror(errno));
		return -1;
	}
//	LogPrintf(0, 0, TM_INFO, "%d bytes sent.\n", ret);

	return ret;
}

int	CMySocket::Receive(char *pRcvdPkt, size_t length)
{
	int		rcvd_len;

	rcvd_len = recv(m_nSock, pRcvdPkt, length, 0);
	if(rcvd_len <= 0)
	{
		if(rcvd_len != 0)
			LogPrintf(0, 0, TM_APIERROR, "recv(fd=%d): Error when receiving TCP packets, %s\n", m_nSock, strerror(errno));
		return rcvd_len;
	}
//	LogPrintf(0, 0, TM_INFO, "%d bytes received.\n", rcvd_len);

	return rcvd_len;
}
