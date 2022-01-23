/*
 * CKeepAlivedClient.cpp
 *
 *  Created on: 2018. 12. 6.
 *      Author: ukjukang
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
#include  <sys/poll.h>

#include "log.h"
#include "packet.h"
#include "CKeepAlivedClient.h"

	char	buf[200];
	int		sockfd, timeout, nNumberOfFileDesriptors;
	int		ret, i;
	struct pollfd	poll_events;


CKeepAlivedClient::CKeepAlivedClient(char* ip, unsigned short port) {
	// TODO Auto-generated constructor stub
//	m_szVasServerIp = { 0 };
	m_usScsServerPort = 0;

	memset(&m_szScsServerIp, 0x00, sizeof(m_szScsServerIp));
	strcpy(m_szScsServerIp, ip);
	m_usScsServerPort = port;

	retry = 0;

	printf("parameter [%s]\n", ip);
	printf("Server IP = [%s]\n", m_szScsServerIp);
	printf("Server Port = [%d]\n", m_usScsServerPort);

}

CKeepAlivedClient::~CKeepAlivedClient() {
	// TODO Auto-generated destructor stub
}

int CKeepAlivedClient::CreateSocket() {

	if(m_cSocket.CreateSocket() == -1){
		return -1;
	}
	return 0;

}

int CKeepAlivedClient::ConnectServer() {


	if(m_cSocket.ConnectToServer(m_szScsServerIp, m_usScsServerPort) < 0){
		return -1;
	}

	return 0;
}

bool CKeepAlivedClient::HealthCheck() {

	PACKET_HEALTH_CHECK health_s;


	timeout = 5000;
	nNumberOfFileDesriptors = 1;

	char hPack[sizeof(PACKET_HEALTH_CHECK)];

	memset(hPack, 0x00, sizeof(PACKET_HEALTH_CHECK));

	memset(&health_s, 0x00, sizeof(PACKET_HEALTH_CHECK));

	memcpy(health_s.common_header.interface_id, 	"9999", 4);
	memcpy(health_s.common_header.body_size, 			"   19", 5);
	GetTimestamp(health_s.timestamp);
	memcpy(health_s.health_check, 								"H", 1);
	memcpy(health_s.resultcode, 								  " ", 1);

	memcpy(hPack, &health_s, sizeof(PACKET_HEALTH_CHECK));

	sockfd = m_cSocket.GetSocket();
	poll_events.fd      = sockfd;
	poll_events.events  = POLLIN | POLLERR | POLLHUP;
	poll_events.revents = 0;

	while(true){
		ret = poll((struct pollfd *) &poll_events, nNumberOfFileDesriptors, timeout);

		if (ret < 0){
			retry++;
			if(retry == 3){
				return false;
			}
			continue;
		}
		else if (ret == 0){
			if(m_cSocket.Send(hPack, sizeof(hPack)) < 0) {
				//retry++;
				LogPrintf(0, 0, TM_MAJOR, "SCS ACTIVE Send Fail!!!\n");
				//if(retry == 3){
				//	return false;
				//}
				return false;
			}
			else{
				LogPrintf(0, 0, TM_INFO, "SCS ACTIVE Health Chech OK !!! {%d}\n", retry);
				retry = 0;
			}
		}
		else {
			if(poll_events.revents & POLLIN)
			{
				if(m_cSocket.Receive(buf, sizeof(buf)) <= 0) {
					return false;
				}
			}
		}
	}
}

void CKeepAlivedClient::GetTimestamp(char *pTimestamp)
{
	struct timeval tv;
	struct tm	lt;
	int milli;

	gettimeofday(&tv, NULL);
	localtime_r(&tv.tv_sec, &lt);
	milli = tv.tv_usec / 1000;
	sprintf(pTimestamp, "%04d%02d%02d%02d%02d%02d%03d", lt.tm_year+1900, lt.tm_mon+1, lt.tm_mday, lt.tm_hour, lt.tm_min, lt.tm_sec, milli);

}

void CKeepAlivedClient::DestroySocket() {
	m_cSocket.DestroySocket();
}

