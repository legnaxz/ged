#ifndef  __TCPSERVER_H__
#define  __TCPSERVER_H__


#include  "mythread.h"
#include  "mysocket.h"


class CTcpServer : public CMyThread
{
public:
	CTcpServer();
	~CTcpServer();
	int		CreateServer(unsigned short usPort);
	int		DestroyServer();
	void	Run();
	int		SetClientSockOpt(int sockfd);

private:
	bool		m_bStopFlag;
	CMySocket	m_cSocket;
};


#endif	// end of __TCPSERVER_H__
