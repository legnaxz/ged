/*
 * CKeepAlivedServer.cpp
 *
 *  Created on: 2018. 12. 4.
 *      Author: ukju
 */

#include  <stdio.h>
#include  <string.h>
#include  <errno.h>
#include  <unistd.h>
#include  <sys/types.h>
#include  <sys/socket.h>
#include  <netinet/in.h>
#include  <netinet/tcp.h>
#include  <arpa/inet.h>
#include  <sys/time.h>
#include  <time.h>

#include "CKeepAlivedServer.h"
#include "log.h"
#include "packet.h"

CKeepAlivedServer::CKeepAlivedServer() {
	// TODO Auto-generated constructor stub
	m_bStopFlag = false;

}

CKeepAlivedServer::~CKeepAlivedServer() {
	// TODO Auto-generated destructor stub
	m_bStopFlag = true;
	DestroyServer();
}

int CKeepAlivedServer::CreateServer(unsigned short usPort){

	if(m_cSocket.CreateSocket(usPort) != 0) return -1;
	if(m_cSocket.Listen() != 0) return -1;

	return 0;
}

int CKeepAlivedServer::DestroyServer(){

	m_cSocket.DestroySocket();

	return 0;
}

void CKeepAlivedServer::Run(){
	int					sock, server_sock;
	struct sockaddr_in	cli_addr;

	PACKET_COMMON_HEADER head;
	PACKET_HEALTH_CHECK health, health_s;

	socklen_t			cli_len;
	char				szClientAddr[255], buf[255];

	LogPrintf(0, 0, TM_INFO, "SCS KeepAlived started successfully!!!\n");

	server_sock = m_cSocket.GetSocket();
	//if(m_bStopFlag == false)
	while(m_bStopFlag == false)
	{
		cli_len = sizeof(cli_addr);
		sock = accept(server_sock, (struct sockaddr *) &cli_addr, &cli_len);
		//while(m_bStopFlag == false)
		//{
			if(sock < 0)
			{
				if(errno != EAGAIN)
					LogPrintf(0, 0, TM_APIERROR, "SCS KeepAlived(accept): can't accept stream socket, %s\n", strerror(errno));
				else
					LogPrintf(0, 0, TM_APIERROR, "SCS KeepAlived(accept): EAGAIN\n");
				 //continue;
				return;
			}

			strcpy(szClientAddr, inet_ntoa(cli_addr.sin_addr));

			// Set a client socket option
			if(SetClientSockOpt(sock) == -1)
			{
				close(sock);
				LogPrintf(0, 0, TM_INFO, "close a SCS KeepAlived(sock=%d) due to setting a socket option failure\n", sock);
				//continue;
				return;
			}
			while(m_bStopFlag == false) {
			// Parsing the packet
			if(recv(sock, buf, sizeof(buf), 0) > 0)
			{
				char packetID[4];

				memset(&head, 0x00, sizeof(PACKET_COMMON_HEADER));
				memcpy(&head, buf, sizeof(PACKET_COMMON_HEADER));
				sprintf(packetID, "%-4.4s", head.interface_id);

				if(strncmp(packetID, "9999", 4)  == 0){
					char hPack[sizeof(PACKET_HEALTH_CHECK)];

					memset(hPack, 0x00, sizeof(PACKET_HEALTH_CHECK));

					memcpy(&health, buf, sizeof(PACKET_HEALTH_CHECK));
					printf("[%-4.4s]\n", 				health.common_header.interface_id);
					printf("[%-5.5s]\n", 				health.common_header.body_size);
					printf("[%-17.17s]\n", 			health.timestamp);
					printf("[%-1.1s]\n", 				health.health_check);
					printf("[%-1.1s]\n", 				health.resultcode);

					memset(&health_s, 0x00, sizeof(PACKET_HEALTH_CHECK));

					memcpy(health_s.common_header.interface_id, 	&health.common_header.interface_id, 4);
					memcpy(health_s.common_header.body_size, 			&health.common_header.body_size, 5);
					GetTimestamp(health_s.timestamp);
					memcpy(health_s.health_check, 								&health.health_check, 1);
					memcpy(health_s.resultcode, 								  "Y", 1);

					memcpy(hPack, &health_s, sizeof(PACKET_HEALTH_CHECK));
					send(sock, (char*)hPack, sizeof(hPack), 0);
				}
				//break;
			}
		}
	}
	shutdown(sock, SHUT_RDWR);
	close(sock);
}

int CKeepAlivedServer::SetClientSockOpt(int sockfd) {
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

void CKeepAlivedServer::GetTimestamp(char *pTimestamp)
{
	struct timeval tv;
	struct tm	lt;
	int milli;

	gettimeofday(&tv, NULL);
	localtime_r(&tv.tv_sec, &lt);
	milli = tv.tv_usec / 1000;
	sprintf(pTimestamp, "%04d%02d%02d%02d%02d%02d%03d", lt.tm_year+1900, lt.tm_mon+1, lt.tm_mday, lt.tm_hour, lt.tm_min, lt.tm_sec, milli);

}
