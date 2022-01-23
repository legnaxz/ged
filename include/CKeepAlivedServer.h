/*
 * CKeepAlivedServer.h
 *
 *  Created on: 2018. 12. 4.
 *      Author: ukju
 */

#ifndef INCLUDE_CKEEPALIVEDSERVER_H_
#define INCLUDE_CKEEPALIVEDSERVER_H_


#include  "mythread.h"
#include  "mysocket.h"

class CKeepAlivedServer  : public CMyThread{
public:
	CKeepAlivedServer();
	virtual ~CKeepAlivedServer();
	int		CreateServer(unsigned short usPort);
	int		DestroyServer();
	void	Run();
	int		SetClientSockOpt(int sockfd);
	void GetTimestamp(char *pTimestamp);

private:
	bool		m_bStopFlag;
	CMySocket	m_cSocket;
};

#endif /* INCLUDE_CKEEPALIVEDSERVER_H_ */
