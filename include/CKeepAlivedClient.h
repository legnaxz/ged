/*
 * CKeepAlivedClient.h
 *
 *  Created on: 2018. 12. 6.
 *      Author: ukjukang
 */

#ifndef INCLUDE_CKEEPALIVEDCLIENT_H_
#define INCLUDE_CKEEPALIVEDCLIENT_H_

#include  "mysocket.h"

class CKeepAlivedClient {
public:
	CKeepAlivedClient(char* ip, unsigned short port);
	virtual ~CKeepAlivedClient();
	int CreateSocket();
	void DestroySocket();
	int ConnectServer();
	bool HealthCheck();

	int retry;// = 0;

private:
	char	m_szScsServerIp[20];
	unsigned short	m_usScsServerPort;
private:
//	bool		m_bStopFlag;
	CMySocket	m_cSocket;
	void GetTimestamp(char *pTimestamp);
};

#endif /* INCLUDE_CKEEPALIVEDCLIENT_H_ */
